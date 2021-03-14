#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import os
import sys
from os.path import join
import shutil
from struct import pack

PY3 = sys.version_info >= (3,)
PY2 = int(sys.version[0]) == 2

if PY2:
    text_type = unicode  # noqa
    binary_type = str
    string_types = (str, unicode)  # noqa
    integer_types = (int, long)  # noqa
    unicode = unicode  # noqa
    basestring = basestring  # noqa
else:
    text_type = str
    binary_type = bytes
    string_types = (str,)
    integer_types = (int,)
    unicode = str
    basestring = (str, bytes)


def to_bytes(s):
    if PY2:
        return s
    else:
        return binary_type(s, 'utf-8')


initrd_dir = './initrd'

shutil.copyfile('./build/user/test/test.bin',
                join(initrd_dir, 'test.bin'))

filenames = []
for filename in os.listdir(initrd_dir):
    filenames.append(filename)

nfiles = len(filenames)
offset = 0
magic = 0xABCD1234

header_sz = 4
file_header_sz = 76


def get_file_content(filepath):
    with open(filepath, 'rb') as fobj:
        content = fobj.read()
    return content


with open('./build/initrd', 'wb+') as fobj:
    fobj.write(pack('<I', nfiles))
    offset += header_sz
    offset += file_header_sz * nfiles

    file_headers = []
    file_contents = []
    for filename in filenames:
        content = get_file_content(join(initrd_dir, filename))
        bin_data = b''
        bin_data += pack('<I', magic)
        bin_data += pack('<64s', to_bytes(filename))
        bin_data += pack('<I', offset)
        bin_data += pack('<I', len(content))
        fobj.write(bin_data)

        offset += len(content)
        file_contents.append(content)
    for content in file_contents:
        fobj.write(content)
