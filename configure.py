#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import sys
import os
import string
import pipes
from optparse import OptionParser
# import subprocess

sourcedir = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(sourcedir, 'misc'))
import ninja_syntax


class Platform(object):
    def __init__(self, platform):
        self._platform = platform
        if self._platform is not None:
            return
        self._platform = sys.platform
        if self._platform.startswith('linux'):
            self._platform = 'linux'
        elif self._platform.startswith('win'):
            self._platform = 'win'

    @staticmethod
    def known_platforms():
      return ['linux', 'win']

    def platform(self):
        return self._platform

    def is_linux(self):
        return self._platform == 'linux'

    def is_windows(self):
        return self._platform == 'win'

    def can_rebuild_in_place(self):
        return not (self.is_windows())


parser = OptionParser()
parser.add_option('--verbose', action='store_true',
                  help='enable verbose build')
parser.add_option('--platform',
                  help='target platform (' +
                       '/'.join(Platform.known_platforms()) + ')',
                  choices=Platform.known_platforms())
parser.add_option('--host',
                  help='host platform (' +
                       '/'.join(Platform.known_platforms()) + ')',
                  choices=Platform.known_platforms())

(options, args) = parser.parse_args()
if args:
    print('ERROR: extra unparsed command-line arguments:', args)
    sys.exit(1)

platform = Platform(options.platform)
if options.host:
    host = Platform(options.host)
else:
    host = platform

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
    config_str = ' '.join([k + '=' + pipes.quote(configure_env[k])
                           for k in configure_env])
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
n.newline()

n.variable('cross_gcc_prefix', configure_env.get('CROSS_GCC_PREFIX', ''))
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
n.variable('kernel_objcopy', '${cross_gcc_prefix}objcopy')
n.variable('kernel_objdump', '${cross_gcc_prefix}objdump')
n.newline()

kernel_cflags = [
    '-m32', '-ffreestanding',
    '-Wall', '-Wextra',
    '-I./include',
]
kernel_ldflags = [
    '-melf_i386'
]
n.variable('kernel_cflags', ' '.join(shell_escape(flag) for flag in kernel_cflags))
n.variable('kernel_ldflags', ' '.join(shell_escape(flag) for flag in kernel_ldflags))
n.newline()

n.rule('kernel_cc',
    command='$kernel_cc -MMD -MT $out -MF $out.d $cflags $kernel_cflags -c $in -o $out',
    depfile='$out.d',
    deps='gcc',
    description='KERNEL_CC $out')
n.newline()

n.rule('kernel_ar',
       command='rm -f $out && $kernel_ar crs $out $in',
       description='KERNEL_AR $out')
n.newline()

n.rule('kernel_link',
    command='$kernel_ld $ldflags $kernel_ldflags -o $out $in $libs',
    description='KERNEL_LINK $out')
n.newline()


def kernel_built(filename):
    return os.path.join('$builddir', 'kernel', filename)


def kernel_src(filename):
    return os.path.join('$root', 'kernel', filename)


def kernel_cc(name, **kwargs):
    return n.build(kernel_built(name + objext), 'kernel_cc', kernel_src(name + '.c'), **kwargs)

#: boot
boot_objs = []
boot_objs += n.build(
    built('boot/boot' + objext),
    'kernel_cc',
    'boot/boot.S'
)
boot_objs += n.build(
    built('boot/main' + objext),
    'kernel_cc',
    'boot/main.c',
    variables=dict(
        kernel_cflags='$kernel_cflags -Os'
    )
)
boot_out = n.build(
    built('boot/boot.out'),
    'kernel_link',
    boot_objs,
    variables=dict(
        kernel_ldflags='$kernel_ldflags -N -e start -Ttext 0x7C00'
    )
)

n.rule('r_boot',
       command='$kernel_objcopy -S -O binary -j .text $in $out')
boot_binary = n.build(built('boot/boot'), 'r_boot', boot_out)
all_targets += boot_binary
n.newline()

#: kernel
kernel_objs = []
for name in ['kmain']:
    kernel_objs += kernel_cc(name)

kernel_elf = n.build(
    kernel_built('kernel'),
    'kernel_link',
    kernel_objs,
    variables=dict(
        kernel_ldflags='$kernel_ldflags -T {0} -nostdlib'.format(
            kernel_src('kernel.ld')
        )
    )
)
all_targets += kernel_elf
n.newline()

#: nos.img
n.rule('r_nos_img',
       command='fasm $in',
       description='BUILD_IMG $out')
nos_img = n.build('nos.img', 'r_nos_img', 'nos.asm',
    implicit=[] + boot_binary + kernel_elf
)
all_targets += nos_img
n.newline()

#: DONE
n.default(nos_img)
n.newline()
n.build('all', 'phony', all_targets)

n.close()
print('wrote %s.' % BUILD_FILENAME)
