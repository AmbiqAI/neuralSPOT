#!/usr/bin/env python3

# *****************************************************************************
#
#    create_info_keys.py
#
#    Create Apollo3 keys
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2023, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
#
# *****************************************************************************

import argparse
import sys
from Crypto.Cipher import AES
import array
import os
import binascii
import importlib

from am_defines import *
#from keys_info import keyTblAes, keyTblHmac, wrapKey, minWrapMode


#******************************************************************************
#
# Generate the device keys for a given chipId based on well known keys and key Wrap mode
#
#******************************************************************************
def process(wrapMode, chipId0, chipId1, inFile, keyFile):
    filenames = keyFile.split('.')
    keys = importlib.import_module(filenames[0])
    if (inFile is not None):
        for line in inFile:
            linecontent = line.rstrip()
            if linecontent:
                chipids = line.split(':')
                process_chip(wrapMode, int(chipids[0], 16), int(chipids[1], 16), keys)
    else:
        process_chip(wrapMode, chipId0, chipId1, keys)

def process_chip(wrapMode, chipId0, chipId1, keys):

    output = str(hex(chipId0)) + '_' + str(hex(chipId1))
    chipId = int_to_bytes(chipId0) + int_to_bytes(chipId1)
    while (len(chipId) != AM_SECBOOT_KEYIDX_BYTES):
        chipId = chipId + chipId
    am_print("ChipID: ", [hex(n) for n in chipId], level=AM_PRINT_LEVEL_VERBOSE)

    if (wrapMode < keys.minWrapMode):
        am_print("Invalid Wrap Mode", level=AM_PRINT_LEVEL_ERROR)
        return
    if (wrapMode == AM_SECBOOT_KEYWRAP_NONE): ## None
        am_print("wrap mode None")
        with open(output + '_info_kek.bin', mode = 'wb') as out_info_kek:
            am_print("writing to " + output + '_info_kek.bin')
            for i in range (0, INFO_MAX_ENC_KEYS):
                am_print([hex(n) for n in keys.keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]], level=AM_PRINT_LEVEL_DEBUG)
                out_info_kek.write(bytearray(keys.keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]))
        with open(output + '_info_authkey.bin', mode = 'wb') as out_info_authkey:
            am_print("writing to " + output + '_info_authkey.bin')
            for i in range (0, INFO_MAX_AUTH_KEYS):
                am_print([hex(n) for n in keys.keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]], level=AM_PRINT_LEVEL_DEBUG)
                out_info_authkey.write(bytearray(keys.keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]))
    elif (wrapMode == AM_SECBOOT_KEYWRAP_XOR): ## XOR
        am_print("wrap mode XOR")
        with open(output + '_info_kek.bin', mode = 'wb') as out_info_kek:
            am_print("writing to " + output + '_info_kek.bin')
            for i in range (0, INFO_MAX_ENC_KEYS):
                scrambledKey = keys.keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]
                for j in range (0, AM_SECBOOT_KEYIDX_BYTES):
                    scrambledKey[j] = scrambledKey[j] ^ keys.wrapKey[j] ^ chipId[j]
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_kek.write(bytearray(scrambledKey))
        with open(output + '_info_authkey.bin', mode = 'wb') as out_info_authkey:
            am_print("writing to " + output + '_info_authkey.bin')
            for i in range (0, INFO_MAX_AUTH_KEYS):
                scrambledKey = keys.keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)]
                for j in range (0, AM_SECBOOT_KEYIDX_BYTES):
                    scrambledKey[j] = scrambledKey[j] ^ keys.wrapKey[j] ^ chipId[j]
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_authkey.write(bytearray(scrambledKey))
    elif (wrapMode == AM_SECBOOT_KEYWRAP_AES128): ## AES128
        am_print("wrap mode AES128")
        with open(output + '_info_kek.bin', mode = 'wb') as out_info_kek:
            am_print("writing to " + output + '_info_kek.bin')
            for i in range (0, INFO_MAX_ENC_KEYS):
                scrambledKey = encrypt_app_aes(keys.keyTblAes[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], keys.wrapKey, chipId)
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_kek.write(bytearray(scrambledKey))
        with open(output + '_info_authkey.bin', mode = 'wb') as out_info_authkey:
            am_print("writing to " + output + '_info_authkey.bin')
            for i in range (0, INFO_MAX_AUTH_KEYS):
                scrambledKey = encrypt_app_aes(keys.keyTblHmac[i*AM_SECBOOT_KEYIDX_BYTES:(i*AM_SECBOOT_KEYIDX_BYTES + AM_SECBOOT_KEYIDX_BYTES)], keys.wrapKey, chipId)
                am_print([hex(n) for n in scrambledKey], level=AM_PRINT_LEVEL_DEBUG)
                out_info_authkey.write(bytearray(scrambledKey))
    else:
        am_print("Unknown wrap mode", level=AM_PRINT_LEVEL_ERROR)
        return


def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Generate Apollo3 Device keys')

    parser.add_argument('--wrap', dest='wrap', type=auto_int, default=AM_SECBOOT_KEYWRAP_NONE, choices = range(AM_SECBOOT_KEYWRAP_NONE, AM_SECBOOT_KEYWRAP_MAX+1),
                        help='wrap mode (0 = None, 1 = XOR, 2 = AES128) [default ' + str(AM_SECBOOT_KEYWRAP_NONE) +'] ')

    parser.add_argument('--chipid0', dest='chipId0', type=auto_int, default=0,
                        help='CHIPID0 for the device')

    parser.add_argument('--chipid1', dest='chipId1', type=auto_int, default=0,
                        help='CHIPID1 for the device')

    parser.add_argument('-k', type=str, dest='keyFile', nargs='?', default='keys_info.py',
                        help='key file in specified format [default = keys_info.py]')

    parser.add_argument('-f', type=argparse.FileType('r'), dest='inFile', nargs='?',
                        help='Input text file with chip ID Tuples (chipID0:chipID1 in each line)')

    parser.add_argument('--loglevel', dest='loglevel', type=auto_int, default=AM_PRINT_LEVEL_INFO,
                        choices = range(AM_PRINT_LEVEL_MIN, AM_PRINT_LEVEL_MAX+1),
                        help=helpPrintLevel)

    args = parser.parse_args()

    return args


#******************************************************************************
#
# Main function.
#
#******************************************************************************
def main():
    # Read the arguments.
    args = parse_arguments()

    am_set_print_level(args.loglevel)
    process(args.wrap, args.chipId0, args.chipId1, args.inFile, args.keyFile)

if __name__ == '__main__':
    main()
