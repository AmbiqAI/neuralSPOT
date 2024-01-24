#!/usr/bin/env python3

# *****************************************************************************
#
#    create_cust_wireupdate_blob.py
#
#    Utility to generate image blobs for Apollo3 Bootloader
#    assisted wired updates
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
#from keys_info import keyTblAes, keyTblHmac, minAesKeyIdx, maxAesKeyIdx, minHmacKeyIdx, maxHmacKeyIdx , INFO_KEY, FLASH_KEY

#******************************************************************************
#
# Generate the image blob as per command line parameters
#
#******************************************************************************
def process(appFile, imagetype, loadaddress, authalgo, encalgo, authKeyIdx, encKeyIdx, optionsVal, maxSize, output, keyFile):

    app_binarray = bytearray()
    # Open the file, and read it into an array of integers.
    with appFile as f_app:
        app_binarray.extend(f_app.read())
        f_app.close()

    # Make sure it is page multiple
    if ((maxSize & (FLASH_PAGE_SIZE - 1)) != 0):
        am_print ("split needs to be multiple of flash page size", level=AM_PRINT_LEVEL_ERROR)
        return

    filenames = keyFile.split('.')
    keys = importlib.import_module(filenames[0])

    if (encalgo != 0):
        if ((encKeyIdx < keys.minAesKeyIdx) or (encKeyIdx > keys.maxAesKeyIdx)):
            am_print("Invalid encKey Idx ", encKeyIdx, level=AM_PRINT_LEVEL_ERROR)
            return
        if (encalgo == 2):
            if (encKeyIdx & 0x1):
                am_print("Invalid encKey Idx ", encKeyIdx, level=AM_PRINT_LEVEL_ERROR);
                return
            keySize = 32
        else:
            keySize = 16
    if (authalgo != 0):
        if ((authKeyIdx < keys.minHmacKeyIdx) or (authKeyIdx > keys.maxHmacKeyIdx) or (authKeyIdx & 0x1)):
            am_print("Invalid authKey Idx ", authKeyIdx, level=AM_PRINT_LEVEL_ERROR);
            return
    hdr_length  = AM_WU_IMAGEHDR_SIZE;   #fixed header length
    am_print("Header Size = ", hex(hdr_length))

    orig_app_length  = (len(app_binarray))

    if (imagetype == AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA):
        if (orig_app_length & 0x3):
            am_print("INFO0 blob length needs to be multiple of 4", level=AM_PRINT_LEVEL_ERROR)
            return
        if ((loadaddress + orig_app_length) > INFO_SIZE_BYTES):
            am_print("INFO0 Offset and length exceed size", level=AM_PRINT_LEVEL_ERROR)
            return

    if (encalgo != 0):
        block_size = keySize
        app_binarray = pad_to_block_size(app_binarray, block_size, 0)
    else:
        # Add Padding
        app_binarray = pad_to_block_size(app_binarray, 4, 0)

    app_length  = (len(app_binarray))
    am_print("app_size ",hex(app_length), "(",app_length,")")
    pad_length = app_length - orig_app_length

    if (app_length + hdr_length > maxSize):
        am_print("Image size bigger than max - Creating Split image")

    start = 0
    # now output all three binary arrays in the proper order
    output = output + '.bin'
    out = open(output, mode = 'wb')

    while (start < app_length):
        #generate mutable byte array for the header
        hdr_binarray = bytearray([0x00]*hdr_length);

        if (app_length - start > maxSize):
            end = start + maxSize
            # update size
            fill_word(hdr_binarray, AM_WU_IMAGEHDR_OFFSET_SIZE, maxSize)
        else:
            end = app_length
            # update size
            fill_word(hdr_binarray, AM_WU_IMAGEHDR_OFFSET_SIZE, orig_app_length-start)

        # Create imageType & options
        hdr_binarray[AM_WU_IMAGEHDR_OFFSET_IMAGETYPE] = imagetype
        hdr_binarray[AM_WU_IMAGEHDR_OFFSET_OPTIONS] = 0

        if (imagetype == AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA):
            if (end != app_length):
                am_print("Insufficient split size ", maxSize, level=AM_PRINT_LEVEL_ERROR)
                return
            key = keys.INFO_KEY
            # word offset
            fill_word(hdr_binarray, AM_WU_IMAGEHDR_OFFSET_ADDR, loadaddress>>2)
        else:
            key = keys.FLASH_KEY
            # load address
            fill_word(hdr_binarray, AM_WU_IMAGEHDR_OFFSET_ADDR, loadaddress)
            if (loadaddress & (FLASH_PAGE_SIZE - 1)):
                am_print("WARNING!!! - load address is not page aligned", level=AM_PRINT_LEVEL_ERROR)
            # Set the options only for the first block
            if (start == 0):
                hdr_binarray[AM_WU_IMAGEHDR_OFFSET_OPTIONS] = optionsVal

        # Create Info0 Update Blob for wired update
        fill_word(hdr_binarray, AM_WU_IMAGEHDR_OFFSET_KEY, key)

        w0 = ((authalgo & 0xf) | ((authKeyIdx << 8) & 0xf00) | ((encalgo << 16) & 0xf0000) | ((encKeyIdx << 24) & 0x0f000000))

        fill_word(hdr_binarray, 0, w0)

        if (encalgo != 0):
            keyIdx = encKeyIdx - keys.minAesKeyIdx
            ivValAes = os.urandom(AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES)
            am_print("Initialization Vector")
            am_print([hex(n) for n in ivValAes])
            keyAes = os.urandom(keySize)
            am_print("AES Key used for encryption")
            am_print([hex(keyAes[n]) for n in range (0, keySize)])
            # Encrypted Part - after security header
            enc_binarray = encrypt_app_aes((hdr_binarray[AM_WU_IMAGEHDR_START_ENCRYPT:hdr_length] + app_binarray[start:end]), keyAes, ivValAes)
#            am_print("Key used for encrypting AES Key")
#            am_print([hex(keys.keyTblAes[keyIdx*AM_SECBOOT_KEYIDX_BYTES + n]) for n in range (0, keySize)])
            # Encrypted Key
            enc_key = encrypt_app_aes(keyAes, keys.keyTblAes[keyIdx*AM_SECBOOT_KEYIDX_BYTES:(keyIdx*AM_SECBOOT_KEYIDX_BYTES + keySize)], ivVal0)
            am_print("Encrypted Key")
            am_print([hex(enc_key[n]) for n in range (0, keySize)])
            # Fill up the IV
            for x in range(0, AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES):
                hdr_binarray[AM_WU_IMAGEHDR_OFFSET_IV + x]  = ivValAes[x]
            # Fill up the Encrypted Key
            for x in range(0, keySize):
                hdr_binarray[AM_WU_IMAGEHDR_OFFSET_KEK + x]  = enc_key[x]
        else:
            enc_binarray = hdr_binarray[AM_WU_IMAGEHDR_START_ENCRYPT:hdr_length] + app_binarray[start:end]


        if (authalgo != 0): # Authentication needed
            keyIdx = authKeyIdx - keys.minHmacKeyIdx
#            am_print("Key used for HMAC")
#            am_print([hex(keys.keyTblHmac[keyIdx*AM_SECBOOT_KEYIDX_BYTES + n]) for n in range (0, AM_HMAC_SIG_SIZE)])
            # Initialize the HMAC - Sign is computed on image following the signature
            sig = compute_hmac(keys.keyTblHmac[keyIdx*AM_SECBOOT_KEYIDX_BYTES:(keyIdx*AM_SECBOOT_KEYIDX_BYTES+AM_HMAC_SIG_SIZE)], hdr_binarray[AM_WU_IMAGEHDR_START_HMAC:AM_WU_IMAGEHDR_START_ENCRYPT] + enc_binarray)
            am_print("HMAC")
            am_print([hex(n) for n in sig])
            # Fill up the HMAC
            for x in range(0, AM_HMAC_SIG_SIZE):
                hdr_binarray[AM_WU_IMAGEHDR_OFFSET_SIG + x]  = sig[x]

        am_print("Writing to file ", output)
        am_print("Image from ", str(hex(start)), " to ", str(hex(end)), " will be loaded at", str(hex(loadaddress)))
        out.write(hdr_binarray[0:AM_WU_IMAGEHDR_START_ENCRYPT])
        out.write(enc_binarray)

        # Reset start for next chunk
        start = end
        loadaddress = loadaddress + maxSize

def parse_arguments():
    parser = argparse.ArgumentParser(description =
                     'Generate Apollo3 Wired Update Blob')

    parser.add_argument('--load-address', dest='loadaddress', type=auto_int, default=hex(0x60000),
                        help='Load address of the binary - Where in flash the blob will be stored (could be different than install address of binary within).')

    parser.add_argument('--bin', dest='appFile', type=argparse.FileType('rb'),
                        help='binary file (blah.bin)')

    parser.add_argument('-i', dest = 'imagetype', default=AM_SECBOOT_WIRED_IMAGETYPE_MAIN, type=int,
                        choices = [
                                (AM_SECBOOT_WIRED_IMAGETYPE_SBL),
                                (AM_SECBOOT_WIRED_IMAGETYPE_AM3P),
                                (AM_SECBOOT_WIRED_IMAGETYPE_PATCH),
                                (AM_SECBOOT_WIRED_IMAGETYPE_MAIN),
                                (AM_SECBOOT_WIRED_IMAGETYPE_CHILD),
                                (AM_SECBOOT_WIRED_IMAGETYPE_CUSTPATCH),
                                (AM_SECBOOT_WIRED_IMAGETYPE_NONSECURE),
                                (AM_SECBOOT_WIRED_IMAGETYPE_INFO0),
                                (AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA)
                                ],
                        help = 'ImageType ('
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_SBL) + ': SBL, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_AM3P) + ': AM3P, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_PATCH) + ': Patch, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_MAIN) + ': Main, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_CHILD) + ': Child, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_CUSTPATCH) + ': CustOTA, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_NONSECURE) + ': NonSecure, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_INFO0) + ': Info0, '
                                + str(AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA) + ': Info0-NoOTA) '
                                '- default[Main]')

    parser.add_argument('--options', dest = 'options', type=auto_int, default=0x1,
                        help = 'Options (16b hex value) - bit0 instructs to perform OTA of the image after wired download (set to 0 if only downloading & skipping OTA flow)')

    parser.add_argument('-o', dest = 'output', default='wuimage',
                        help = 'Output filename (without the extension)')

    parser.add_argument('--authkey', dest = 'authkey', type=auto_int, default=(AM_SECBOOT_MIN_KEYIDX_INFO0), choices = range(AM_SECBOOT_MIN_KEYIDX_INFO0, AM_SECBOOT_MAX_KEYIDX_INFO0 + 1),
                        help = 'Authentication Key Idx? (' + str(AM_SECBOOT_MIN_KEYIDX_INFO0) + ' to ' + str(AM_SECBOOT_MAX_KEYIDX_INFO0) + ')')

    parser.add_argument('--kek', dest = 'kek', type=auto_int, default=(AM_SECBOOT_MIN_KEYIDX_INFO0), choices = range(AM_SECBOOT_MIN_KEYIDX_INFO0, AM_SECBOOT_MAX_KEYIDX_INFO0 + 1),
                        help = 'KEK Index? (' + str(AM_SECBOOT_MIN_KEYIDX_INFO0) + ' to ' + str(AM_SECBOOT_MAX_KEYIDX_INFO0) + ')')

    parser.add_argument('--authalgo', dest = 'authalgo', type=auto_int, default=0, choices=range(0, AM_SECBOOT_AUTH_ALGO_MAX+1),
                        help = helpAuthAlgo)

    parser.add_argument('--encalgo', dest = 'encalgo', type=auto_int, default=0, choices = range(0, AM_SECBOOT_ENC_ALGO_MAX+1),
                        help = helpEncAlgo)

    parser.add_argument('--split', dest='split', type=auto_int, default=hex(MAX_DOWNLOAD_SIZE),
                        help='Specify the max block size if the image will be downloaded in pieces')

    parser.add_argument('-k', type=str, dest='keyFile', nargs='?', default='keys_info.py',
                        help='key file in specified format [default = keys_info.py]')

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
    process(args.appFile, args.imagetype, args.loadaddress, args.authalgo, args.encalgo, args.authkey, args.kek, args.options, args.split, args.output, args.keyFile)

if __name__ == '__main__':
    main()
