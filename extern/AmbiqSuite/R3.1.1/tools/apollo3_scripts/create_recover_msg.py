#!/usr/bin/env python3

# *****************************************************************************
#
#    create_info0.py
#
#    Utility to generate Recover Message for Apollo3 Bootloader
#    assisted Wired updates
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
import hashlib
import hmac
import os
import binascii
import importlib

from am_defines import *
#from keys_info import recoveryKey

#******************************************************************************
#
# Generate the image blob as per command line parameters
#
#******************************************************************************
def process(binfile, n0, n1, n2, n3, custId, output, keyFile):

    if (binfile != ''):
        # Read the binary file from the command line.
        with open(binfile, mode='rb') as binfile:
            blob = binfile.read()
        # Gather the important binary metadata.
        blobLen = len(blob)
    else:
        blob = bytearray()
        blobLen = 0

    filenames = keyFile.split('.')
    keys = importlib.import_module(filenames[0])

    # Send Recover command
    print('Building Recover Command.')

    hdr_binarray = bytearray([0x00]*(AM_WU_RECOVERY_HDR_SIZE))
    fill_word(hdr_binarray, 4, (((blobLen + AM_WU_RECOVERY_HDR_SIZE) << 16) | AM_SECBOOT_WIRED_MSGTYPE_RECOVER))
    # CustId
    fill_word(hdr_binarray, AM_WU_RECOVERY_HDR_OFFSET_CUSTID, custId)
    # Recovery key
    for x in range(0, AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES):
        hdr_binarray[AM_WU_RECOVERY_HDR_OFFSET_RECKEY + x]  = keys.recoveryKey[x]
    # Nonce
    fill_word(hdr_binarray, AM_WU_RECOVERY_HDR_OFFSET_NONCE + 0, n0)
    fill_word(hdr_binarray, AM_WU_RECOVERY_HDR_OFFSET_NONCE + 4, n1)
    fill_word(hdr_binarray, AM_WU_RECOVERY_HDR_OFFSET_NONCE + 8, n2)
    fill_word(hdr_binarray, AM_WU_RECOVERY_HDR_OFFSET_NONCE + 12, n3)

    # Compute crc
    crc = crc32(hdr_binarray[4:AM_WU_RECOVERY_HDR_SIZE] + blob)
    fill_word(hdr_binarray, 0, crc)

    # Write the message as a raw message
    with open(output + '.msg', mode = 'wb') as out:
        print("Writing to file ", output + '.msg')
        out.write(hdr_binarray + blob)

def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Generate Apollo3 Recovery Message')

    parser.add_argument('-f', dest='binfile', default='',
                        help = 'Binary file representing the raw Recovery Blob provided by Ambiq')

    parser.add_argument('-o', dest = 'output',
                        help = 'Output filename (without the extension)')

    parser.add_argument('--n0', dest = 'n0', type=auto_int, default=0xFFFFFFFF,
                        help = 'Nonce 0 - should correspond to the value provided to Ambiq (default = 0xFFFFFFFF)')

    parser.add_argument('--n1', dest = 'n1', type=auto_int, default=0xFFFFFFFF,
                        help = 'Nonce 1 - should correspond to the value provided to Ambiq (default = 0xFFFFFFFF)')

    parser.add_argument('--n2', dest = 'n2', type=auto_int, default=0xFFFFFFFF,
                        help = 'Nonce 2 - should correspond to the value provided to Ambiq (default = 0xFFFFFFFF)')

    parser.add_argument('--n3', dest = 'n3', type=auto_int, default=0xFFFFFFFF,
                        help = 'Nonce 3 - should correspond to the value provided to Ambiq (default = 0xFFFFFFFF)')

    parser.add_argument('--custId', dest='custId', type=auto_int, default=0xFFFFFFFF,
                        help='Customer ID - should correspond to the value provided to Ambiq (default = 0xFFFFFFFF)')

    parser.add_argument('-k', type=str, dest='keyFile', nargs='?', default='keys_info.py',
                        help='key file in specified format [default = keys_info.py]')

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


    process(args.binfile, args.n0, args.n1, args.n2, args.n3, args.custId, args.output, args.keyFile)

if __name__ == '__main__':
    main()
