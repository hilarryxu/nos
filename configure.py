#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import sys
import os
import pipes
from optparse import OptionParser

sourcedir = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(sourcedir, 'misc'))
import ninja_syntax  # noqa

KERNEL_NAME = 'nos'


class Platform(object):
    def __init__(self, platform=None):
        self._platform = platform
        if self._platform is not None:
            return
        self._platform = sys.platform
        if self._platform.startswith('linux'):
            self._platform = 'linux'
        elif self._platform.startswith('win'):
            self._platform = 'win'
        elif self._platform.startswith('darwin'):
            self._platform = 'mac'

    @staticmethod
    def known_platforms():
        return ['linux', 'win']

    def platform(self):
        return self._platform

    def is_linux(self):
        return self._platform == 'linux'

    def is_windows(self):
        return self._platform == 'win'

    def is_mac(self):
        return self._platform == 'mac'


parser = OptionParser()
parser.add_option('--verbose', action='store_true', help='enable verbose build')
parser.add_option('--arch', help='target arch', default='i386')
parser.add_option(
    '--host',
    help='host platform (' + '/'.join(Platform.known_platforms()) + ')',
    choices=Platform.known_platforms()
)

(options, args) = parser.parse_args()
if args:
    print('ERROR: extra unparsed command-line arguments:', args)
    sys.exit(1)

platform = Platform()

BUILD_FILENAME = 'build.ninja'
ninja_writer = ninja_syntax.Writer(open(BUILD_FILENAME, 'w'))
n = ninja_writer

n.comment('It is generated by ' + os.path.basename(__file__) + '.')
n.newline()

n.variable('ninja_required_version', '1.3')
n.newline()

configure_args = sys.argv[1:]
n.variable('configure_args', ' '.join(configure_args))
env_keys = set(['CC', 'AR', 'CFLAGS', 'LDFLAGS'])
configure_env = dict((k, os.environ[k]) for k in os.environ if k in env_keys)
if configure_env:
    config_str = ' '.join(
        [k + '=' + pipes.quote(configure_env[k]) for k in configure_env]
    )
    n.variable('configure_env', config_str + '$ ')
n.newline()

CC = configure_env.get('CC', 'gcc')
objext = '.o'


def src(filename):
    return os.path.join('$root', filename)


def built(filename):
    return os.path.join('$builddir', filename)


def binary(name):
    if platform.is_windows():
        exe = name + '.exe'
        n.build(name, 'phony', exe)
        return exe
    return name


def shell_escape(str):
    if platform.is_windows():
        return str
    if '"' in str:
        return "'%s'" % str.replace("'", "\\'")
    return str


root = sourcedir
if root == os.getcwd():
    root = '.'

n.variable('root', root)
n.variable('builddir', 'build')
n.variable('arch', options.arch)
n.newline()

default_cross_gcc_prefix = ''
if platform.is_windows():
    default_cross_gcc_prefix = './Crosstools/bin/i586-elf-'
elif platform.is_mac():
    default_cross_gcc_prefix = 'i686-elf-'

n.variable(
    'cross_gcc_prefix',
    configure_env.get('CROSS_GCC_PREFIX', default_cross_gcc_prefix)
)
n.variable('cc', CC)
n.variable('ar', configure_env.get('AR', 'ar'))
n.newline()

cflags = []
ldflags = []

if 'CFLAGS' in configure_env:
    cflags.append(configure_env['CFLAGS'])
    ldflags.append(configure_env['CFLAGS'])

n.variable('cflags', ' '.join(shell_escape(flag) for flag in cflags))
if 'LDFLAGS' in configure_env:
    ldflags.append(configure_env['LDFLAGS'])
n.variable('ldflags', ' '.join(shell_escape(flag) for flag in ldflags))
n.newline()

all_targets = []

#: kernel and boot
n.variable('kernel_cc', '${cross_gcc_prefix}$cc')
n.variable('kernel_ld', '${cross_gcc_prefix}ld')
n.variable('kernel_ar', '${cross_gcc_prefix}$ar')
n.variable('kernel_asm', '${kernel_cc}')
n.variable('kernel_objcopy', '${cross_gcc_prefix}objcopy')
n.variable('kernel_objdump', '${cross_gcc_prefix}objdump')
n.newline()

kernel_cflags = [
    '-m32',
    '-std=gnu99',
    '-ffreestanding',
    '-g -ggdb -gstabs+',
    '-Wall',
    '-Wextra',
    '-Werror',
    '-fno-pic',
    '-DNOS_ASSERT_PANIC',
    '-DNOS_DEBUG_LOG',
    '-I$root/libk/include',
    '-I$root/kernel',
    '-I$root/include',
]
kernel_ldflags = [
    '-melf_i386',
]
kernel_asmflags = [
    '-m32',
    '-I$root/kernel',
    '-I$root/include',
]
n.variable(
    'kernel_cflags', ' '.join(shell_escape(flag) for flag in kernel_cflags)
)
n.variable(
    'kernel_ldflags', ' '.join(shell_escape(flag) for flag in kernel_ldflags)
)
n.variable(
    'kernel_asmflags', ' '.join(shell_escape(flag) for flag in kernel_asmflags)
)
n.newline()

n.rule(
    'kernel_cc',
    command=
    '$kernel_cc -MMD -MT $out -MF $out.d $cflags $kernel_cflags -c $in -o $out',
    depfile='$out.d',
    deps='gcc',
    description='KERNEL_CC $out'
)
n.newline()

n.rule(
    'kernel_asm',
    command='$kernel_asm $kernel_asmflags -c -o $out $in',
    description='KERNEL_ASM $out'
)
n.newline()

n.rule(
    'kernel_ar',
    command='$kernel_ar crs $out $in',
    description='KERNEL_AR $out'
)
n.newline()

n.rule(
    'kernel_link',
    command='$kernel_ld $ldflags $kernel_ldflags -o $out $in $libs',
    description='KERNEL_LINK $out'
)
n.newline()


def kernel_built(filename):
    return os.path.join('$builddir', 'kernel', KERNEL_NAME, filename)


def kernel_src(filename):
    return os.path.join('$root', 'kernel', KERNEL_NAME, filename)


def kernel_cc(name, **kwargs):
    return n.build(
        kernel_built(name + objext), 'kernel_cc', kernel_src(name + '.c'),
        **kwargs
    )


#: boot
boot_objs = []
boot_objs += n.build(
    built('boot/boot' + objext),
    'kernel_asm',
    src('boot/boot.S')
)
boot_objs += n.build(
    built('boot/main' + objext),
    'kernel_cc',
    src('boot/main.c'),
    variables=dict(kernel_cflags='$kernel_cflags -Os')
)
boot_out = n.build(
    built('boot/boot.out'),
    'kernel_link',
    boot_objs,
    variables=dict(kernel_ldflags='$kernel_ldflags -N -e start -Ttext 0x7C00')
)

n.rule('r_boot', command='$kernel_objcopy -S -O binary -j .text $in $out')
boot_bin = n.build(built('boot/boot.bin'), 'r_boot', boot_out)

all_targets += boot_bin
n.newline()

#: libk
libk_objs = []
for name in [
    'libk/src/stdlib',
    'libk/src/string',
    'libk/src/ctype',
]:
    libk_objs += n.build(
        built(name + objext),
        'kernel_cc',
        src(name + '.c'),
        variables=dict(
            cflags=
            '$cflags -D_KERNEL_ -I$root/libk/include'
        )
    )

libk = n.build(built('lib/libk.a'), 'kernel_ar', libk_objs)

all_targets += libk
n.newline()

#: kernel
kernel_objs = []

kernel_objs += n.build(
    kernel_built('start' + objext), 'kernel_asm',
    kernel_src('start.S')
)
kernel_objs += n.build(
    kernel_built('trap_stub' + objext), 'kernel_asm',
    kernel_src('trap_stub.S')
)

for name in [
    'kernel',
    'drvs/cga',
    'printk',
    'gdt',
    'idt',
    'exception',
    'pic',
    'pit',
    'task',
    'mm/paging',
    'mm/pmm',
    'mm/vmm',
    'drvs/serial',
    'debug',
]:
    kernel_objs += kernel_cc(name)

kernel_elf = n.build(
    kernel_built('kernel.elf'),
    'kernel_link',
    kernel_objs,
    variables=dict(
        kernel_ldflags='$kernel_ldflags -T{0} -Map {1}'.format(
            kernel_src('kernel.ld'),
            built('kernel.map')
        ),
        libs='-L$builddir/lib -lk'
    ),
    implicit=[] + libk
)

all_targets += kernel_elf
n.newline()

#: user apps
user_test_objs = []
user_test_objs += n.build(
    built('user/test/test' + objext),
    'kernel_cc',
    src('user/test/test.c')
)
user_test_bin = n.build(
    built('user/test/test.bin'),
    'kernel_link',
    user_test_objs,
    variables=dict(
        kernel_ldflags='$kernel_ldflags -T{0}'.format(
            src('user/test/test.ld'),
        )
    )
)

all_targets += user_test_bin
n.newline()

#: nos.img
if platform.is_windows():
    n.rule(
        'r_nos_img',
        command='fasm $in $out',
        description='BUILD_IMG $out'
    )
    nos_img = n.build(
        'nos.img', 'r_nos_img', 'nos.asm',
        implicit=[] + boot_bin + kernel_elf
    )

    all_targets += nos_img
    n.newline()

#: DONE
if platform.is_windows():
    n.default(nos_img)
else:
    n.default(kernel_elf)
n.newline()
n.build('all', 'phony', all_targets)

n.close()
print('wrote %s.' % BUILD_FILENAME)
