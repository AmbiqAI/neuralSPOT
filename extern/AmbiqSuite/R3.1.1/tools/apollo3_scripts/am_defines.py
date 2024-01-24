#!/usr/bin/env python3

# *****************************************************************************
#
#    am_defines.py
#
#    Utility functions.
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

import sys
from Crypto.Cipher import AES
from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_v1_5
from Crypto.Hash import SHA256
import array
import hashlib
import hmac
import os
import binascii


ivVal0 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

FLASH_PAGE_SIZE             = 0x2000                # 8K
MAX_DOWNLOAD_SIZE           = 0x48000               # 288K
AM_SECBOOT_DEFAULT_NONSECURE_MAIN   = 0xC000

AM_SECBOOT_AESCBC_BLOCK_SIZE_WORDS  = 4
AM_SECBOOT_AESCBC_BLOCK_SIZE_BYTES  = 4*AM_SECBOOT_AESCBC_BLOCK_SIZE_WORDS

AM_SECBOOT_MIN_KEYIDX_INFO0    = 8 ## KeyIdx 8 - 15
AM_SECBOOT_MAX_KEYIDX_INFO0    = 15
AM_SECBOOT_MIN_KEYIDX_INFO1    = 0 ## KeyIdx 0 - 7
AM_SECBOOT_MAX_KEYIDX_INFO1    = 7
AM_SECBOOT_KEYIDX_BYTES        = 16

# Encryption Algorithm
AM_SECBOOT_ENC_ALGO_NONE = 0
AM_SECBOOT_ENC_ALGO_AES128 = 1
AM_SECBOOT_ENC_ALGO_MAX  = AM_SECBOOT_ENC_ALGO_AES128
# String constants
helpEncAlgo = 'Encryption Algo? (0(default) = none, 1 = AES128)'

# Authentication Algorithm
AM_SECBOOT_AUTH_ALGO_NONE = 0
AM_SECBOOT_AUTH_ALGO_SHA256HMAC = 1
AM_SECBOOT_AUTH_ALGO_MAX = AM_SECBOOT_AUTH_ALGO_SHA256HMAC
# String constants
helpAuthAlgo = 'Authentication Algo? (0(default) = none, 1 = SHA256)'


FLASH_INVALID               = 0xFFFFFFFF

# KeyWrap Mode
AM_SECBOOT_KEYWRAP_NONE     = 0
AM_SECBOOT_KEYWRAP_XOR      = 1
AM_SECBOOT_KEYWRAP_AES128   = 2
AM_SECBOOT_KEYWRAP_MAX      = AM_SECBOOT_KEYWRAP_AES128

#******************************************************************************
#
# Magic Numbers
#
#******************************************************************************
AM_IMAGE_MAGIC_MAIN       = 0xC0
AM_IMAGE_MAGIC_CHILD      = 0xCC
AM_IMAGE_MAGIC_NONSECURE  = 0xCB
AM_IMAGE_MAGIC_INFO0      = 0xCF

# Dummy for creating images for customer - not understood by SBL
# This could be any value from the definition:
# #define AM_IMAGE_MAGIC_CUST(x)   ((((x) & 0xF0) == 0xC0) && ((x) != 0xC0) && ((x) != 0xCC) && ((x) != 0xCB) && ((x) != 0xCF))
AM_IMAGE_MAGIC_CUSTPATCH  = 0xC1

#******************************************************************************
#
# Image Types
#
#******************************************************************************
AM_SECBOOT_WIRED_IMAGETYPE_SBL                  = 0
AM_SECBOOT_WIRED_IMAGETYPE_AM3P                 = 1
AM_SECBOOT_WIRED_IMAGETYPE_PATCH                = 2
AM_SECBOOT_WIRED_IMAGETYPE_MAIN                 = 3
AM_SECBOOT_WIRED_IMAGETYPE_CHILD                = 4
AM_SECBOOT_WIRED_IMAGETYPE_CUSTPATCH            = 5
AM_SECBOOT_WIRED_IMAGETYPE_NONSECURE            = 6
AM_SECBOOT_WIRED_IMAGETYPE_INFO0                = 7
AM_SECBOOT_WIRED_IMAGETYPE_INFO0_NOOTA          = 32
AM_SECBOOT_WIRED_IMAGETYPE_INVALID              = 0xFF


#******************************************************************************
#
# Wired Message Types
#
#******************************************************************************
AM_SECBOOT_WIRED_MSGTYPE_HELLO          = 0
AM_SECBOOT_WIRED_MSGTYPE_STATUS         = 1
AM_SECBOOT_WIRED_MSGTYPE_OTADESC        = 2
AM_SECBOOT_WIRED_MSGTYPE_UPDATE         = 3
AM_SECBOOT_WIRED_MSGTYPE_ABORT          = 4
AM_SECBOOT_WIRED_MSGTYPE_RECOVER        = 5
AM_SECBOOT_WIRED_MSGTYPE_RESET          = 6
AM_SECBOOT_WIRED_MSGTYPE_ACK            = 7
AM_SECBOOT_WIRED_MSGTYPE_DATA           = 8


#******************************************************************************
#
# Wired Message ACK Status
#
#******************************************************************************
AM_SECBOOT_WIRED_ACK_STATUS_SUCCESS              = 0
AM_SECBOOT_WIRED_ACK_STATUS_FAILURE              = 1
AM_SECBOOT_WIRED_ACK_STATUS_INVALID_INFO0        = 2
AM_SECBOOT_WIRED_ACK_STATUS_CRC                  = 3
AM_SECBOOT_WIRED_ACK_STATUS_SEC                  = 4
AM_SECBOOT_WIRED_ACK_STATUS_MSG_TOO_BIG          = 5
AM_SECBOOT_WIRED_ACK_STATUS_UNKNOWN_MSGTYPE      = 6
AM_SECBOOT_WIRED_ACK_STATUS_INVALID_ADDR         = 7
AM_SECBOOT_WIRED_ACK_STATUS_INVALID_OPERATION    = 8
AM_SECBOOT_WIRED_ACK_STATUS_INVALID_PARAM        = 9
AM_SECBOOT_WIRED_ACK_STATUS_SEQ                  = 10
AM_SECBOOT_WIRED_ACK_STATUS_TOO_MUCH_DATA        = 11

#******************************************************************************
#
# Definitions related to Image Headers
#
#******************************************************************************
AM_HMAC_SIG_SIZE                = 32
AM_KEK_SIZE                     = 16
AM_CRC_SIZE                     = 4

AM_MAX_UART_MSG_SIZE            = 8192  # 8K buffer in SBL

# Wiredupdate Image Header
AM_WU_IMAGEHDR_OFFSET_SIG       = 16
AM_WU_IMAGEHDR_OFFSET_IV        = 48
AM_WU_IMAGEHDR_OFFSET_KEK       = 64
AM_WU_IMAGEHDR_OFFSET_IMAGETYPE = (AM_WU_IMAGEHDR_OFFSET_KEK + AM_KEK_SIZE)
AM_WU_IMAGEHDR_OFFSET_OPTIONS   = (AM_WU_IMAGEHDR_OFFSET_IMAGETYPE + 1)
AM_WU_IMAGEHDR_OFFSET_KEY       = (AM_WU_IMAGEHDR_OFFSET_IMAGETYPE + 4)
AM_WU_IMAGEHDR_OFFSET_ADDR      = (AM_WU_IMAGEHDR_OFFSET_KEY + 4)
AM_WU_IMAGEHDR_OFFSET_SIZE      = (AM_WU_IMAGEHDR_OFFSET_ADDR + 4)

AM_WU_IMAGEHDR_START_HMAC       = (AM_WU_IMAGEHDR_OFFSET_SIG + AM_HMAC_SIG_SIZE)
AM_WU_IMAGEHDR_START_ENCRYPT    = (AM_WU_IMAGEHDR_OFFSET_KEK + AM_KEK_SIZE)
AM_WU_IMAGEHDR_SIZE             = (AM_WU_IMAGEHDR_OFFSET_KEK + AM_KEK_SIZE + 16)


# Image Header
AM_IMAGEHDR_SIZE_MAIN           = 256
AM_IMAGEHDR_SIZE_AUX            = (112 + AM_KEK_SIZE)

AM_IMAGEHDR_OFFSET_CRC          = 4
AM_IMAGEHDR_OFFSET_SIG          = 16
AM_IMAGEHDR_OFFSET_IV           = 48
AM_IMAGEHDR_OFFSET_KEK          = 64
AM_IMAGEHDR_OFFSET_SIGCLR       = (AM_IMAGEHDR_OFFSET_KEK + AM_KEK_SIZE)
AM_IMAGEHDR_START_CRC           = (AM_IMAGEHDR_OFFSET_CRC + AM_CRC_SIZE)
AM_IMAGEHDR_START_HMAC_INST     = (AM_IMAGEHDR_OFFSET_SIG + AM_HMAC_SIG_SIZE)
AM_IMAGEHDR_START_ENCRYPT       = (AM_IMAGEHDR_OFFSET_KEK + AM_KEK_SIZE)
AM_IMAGEHDR_START_HMAC          = (AM_IMAGEHDR_OFFSET_SIGCLR + AM_HMAC_SIG_SIZE)
AM_IMAGEHDR_OFFSET_ADDR         = AM_IMAGEHDR_START_HMAC
AM_IMAGEHDR_OFFSET_VERKEY       = (AM_IMAGEHDR_OFFSET_ADDR + 4)
AM_IMAGEHDR_OFFSET_CHILDPTR     = (AM_IMAGEHDR_OFFSET_VERKEY + 4)

# Recover message
AM_WU_RECOVERY_HDR_SIZE             = 44
AM_WU_RECOVERY_HDR_OFFSET_CUSTID    = 8
AM_WU_RECOVERY_HDR_OFFSET_RECKEY    = (AM_WU_RECOVERY_HDR_OFFSET_CUSTID + 4)
AM_WU_RECOVERY_HDR_OFFSET_NONCE     = (AM_WU_RECOVERY_HDR_OFFSET_RECKEY + 16)
AM_WU_RECOVERY_HDR_OFFSET_RECBLOB   = (AM_WU_RECOVERY_HDR_OFFSET_NONCE + 16)


#******************************************************************************
#
# INFOSPACE related definitions
#
#******************************************************************************
AM_SECBOOT_INFO0_SIGN_PROGRAMMED0   = 0x48EAAD88
AM_SECBOOT_INFO0_SIGN_PROGRAMMED1   = 0xC9705737
AM_SECBOOT_INFO0_SIGN_PROGRAMMED2   = 0x0A6B8458
AM_SECBOOT_INFO0_SIGN_PROGRAMMED3   = 0xE41A9D74

AM_SECBOOT_INFO0_SIGN_UINIT0        = 0x5B75A5FA
AM_SECBOOT_INFO0_SIGN_UINIT1        = 0x7B9C8674
AM_SECBOOT_INFO0_SIGN_UINIT2        = 0x869A96FE
AM_SECBOOT_INFO0_SIGN_UINIT3        = 0xAEC90860

INFO_SIZE_BYTES                     = (8 * 1024)
INFO_MAX_AUTH_KEY_WORDS             = 32
INFO_MAX_ENC_KEY_WORDS              = 32

INFO_MAX_AUTH_KEYS   = (INFO_MAX_AUTH_KEY_WORDS*4//AM_SECBOOT_KEYIDX_BYTES)
INFO_MAX_ENC_KEYS    = (INFO_MAX_ENC_KEY_WORDS*4//AM_SECBOOT_KEYIDX_BYTES)


#******************************************************************************
#
# CRC using ethernet poly, as used by Apollo3 hardware for validation
#
#******************************************************************************
def crc32(L):
    return (binascii.crc32(L) & 0xFFFFFFFF)

#******************************************************************************
#
# Pad the text to the block_size. bZeroPad determines how to handle text which
# is already multiple of block_size
#
#******************************************************************************
def pad_to_block_size(text, block_size, bZeroPad):
    text_length = len(text)
    amount_to_pad = block_size - (text_length % block_size)
    if (amount_to_pad == block_size):
        if (bZeroPad == 0):
            amount_to_pad = 0
    for i in range(0, amount_to_pad, 1):
        text += bytes(chr(amount_to_pad), 'ascii')
    return text


#******************************************************************************
#
# AES CBC encryption
#
#******************************************************************************
def encrypt_app_aes(cleartext, encKey, iv):
    key = array.array('B', encKey).tostring()
    ivVal = array.array('B', iv).tostring()
    plaintext = array.array('B', cleartext).tostring()

    encryption_suite = AES.new(key, AES.MODE_CBC, ivVal)
    cipher_text = encryption_suite.encrypt(plaintext)

    return cipher_text

#******************************************************************************
#
# AES 128 CBC encryption
#
#******************************************************************************
def encrypt_app_aes128(cleartext, encKey, iv):
    key = array.array('B', encKey).tostring()
    ivVal = array.array('B', iv).tostring()
    plaintext = array.array('B', cleartext).tostring()

    encryption_suite = AES.new(key, AES.MODE_CBC, ivVal)
    cipher_text = encryption_suite.encrypt(plaintext)

    return cipher_text

#******************************************************************************
#
# SHA256 HMAC
#
#******************************************************************************
def compute_hmac(key, data):
    sig = hmac.new(array.array('B', key).tostring(), array.array('B', data).tostring(), hashlib.sha256).digest()
    return sig

#******************************************************************************
#
# RSA PKCS1_v1_5 sign
#
#******************************************************************************
def compute_rsa_sign(prvKeyFile, data):
    key = open(prvKeyFile, "r").read()
    rsakey = RSA.importKey(key)
    signer = PKCS1_v1_5.new(rsakey)
    digest = SHA256.new()
    digest.update(bytes(data))
    sign = signer.sign(digest)
    return sign

#******************************************************************************
#
# RSA PKCS1_v1_5 sign verification
#
#******************************************************************************
def verify_rsa_sign(pubKeyFile, data, sign):
    key = open(pubKeyFile, "r").read()
    rsakey = RSA.importKey(key)
    #print(hex(rsakey.n))
    verifier = PKCS1_v1_5.new(rsakey)
    digest = SHA256.new()
    digest.update(bytes(data))
    return verifier.verify(digest, sign)

#******************************************************************************
#
# Fill one word in bytearray
#
#******************************************************************************
def fill_word(barray, offset, w):
    barray[offset + 0]  = (w >>  0) & 0x000000ff;
    barray[offset + 1]  = (w >>  8) & 0x000000ff;
    barray[offset + 2]  = (w >> 16) & 0x000000ff;
    barray[offset + 3]  = (w >> 24) & 0x000000ff;


#******************************************************************************
#
# Turn a 32-bit number into a series of bytes for transmission.
#
# This command will split a 32-bit integer into an array of bytes, ordered
# LSB-first for transmission over the UART.
#
#******************************************************************************
def int_to_bytes(n):
    A = [n & 0xFF,
         (n >> 8) & 0xFF,
         (n >> 16) & 0xFF,
         (n >> 24) & 0xFF]

    return A

#******************************************************************************
#
# Extract a word from a byte array
#
#******************************************************************************
def word_from_bytes(B, n):
    return (B[n] + (B[n + 1] << 8) + (B[n + 2] << 16) + (B[n + 3] << 24))


#******************************************************************************
#
# automatically figure out the integer format (base 10 or 16)
#
#******************************************************************************
def auto_int(x):
    return int(x, 0)

#******************************************************************************
#
# User controllable Prints control
#
#******************************************************************************
# Defined print levels
AM_PRINT_LEVEL_MIN     = 0
AM_PRINT_LEVEL_NONE    = AM_PRINT_LEVEL_MIN
AM_PRINT_LEVEL_ERROR   = 1
AM_PRINT_LEVEL_INFO    = 2
AM_PRINT_LEVEL_VERBOSE = 4
AM_PRINT_LEVEL_DEBUG   = 5
AM_PRINT_LEVEL_MAX     = AM_PRINT_LEVEL_DEBUG

# Global variable to control the prints
AM_PRINT_VERBOSITY = AM_PRINT_LEVEL_INFO

helpPrintLevel = 'Set Log Level (0: None), (1: Error), (2: INFO), (4: Verbose), (5: Debug) [Default = Info]'

def am_set_print_level(level):
    global AM_PRINT_VERBOSITY
    AM_PRINT_VERBOSITY = level

def am_print(*args, level=AM_PRINT_LEVEL_INFO, **kwargs):
    global AM_PRINT_VERBOSITY
    if (AM_PRINT_VERBOSITY >= level):
        print(*args, **kwargs)
