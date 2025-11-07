#!/usr/bin/env python3
#
# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

####################################################################
# Filename - hbk_gen_util.py
# Description - This file is responsible for creation of public key
#               HASH and its corresponding number of Zeroes
#               as it should be saved in the OTP/NVM
####################################################################

import argparse
import sys
import os
import logging
from pathlib import Path
sys.path.append(os.path.join(sys.path[0], "..", ".."))

from common_utils import loggerinitializer
from common_utils import cryptolayer

# Util's default log filename
LOG_FILENAME = "gen_hbk_log.log"

OUTPUT_SIZE_SHA_256 = 32
OUTPUT_SIZE_SHA_256_TRUNC = int(OUTPUT_SIZE_SHA_256/2)
BITS_WITHIN_WORD = 32
BYTES_WITHIN_WORD = 4

def create_word_list(byte_array, byte_array_word_count, endianness):
    """
    Creates a hexadecimal (32 bits) word list out of a byte array representation of an input number
    :param byte_array: input number represented as a list of the bytes it consists of
    :param byte_array_word_count: number of (32 bit) words in input array
    :param endianness: output byte order -> little endian 'L' or big endian 'B'
    :return: 32 bit word list
    """
    word_list = []
    if not isinstance(byte_array, list) or not isinstance(byte_array_word_count, int):
        raise TypeError("input parameter byte_array or byte_array_word_count has wrong type")
    if endianness != 'L' and endianness != 'B':
        raise ValueError("input parameter endianness has wrong value")
    else:
        for i in range(byte_array_word_count):
            if endianness == 'L':
                current_word = byte_array[i * 4]
                current_word = current_word + (byte_array[i * 4 + 1] << 8)
                current_word = current_word + (byte_array[i * 4 + 2] << 16)
                current_word = current_word + (byte_array[i * 4 + 3] << 24)
            else:
                current_word = byte_array[i * 4 + 3]
                current_word = current_word + (byte_array[i * 4 + 2] << 8)
                current_word = current_word + (byte_array[i * 4 + 1] << 16)
                current_word = current_word + (byte_array[i * 4] << 24)
            word_list.append(current_word)
        return word_list

def count_zero_bits_in_bytes(bytes_input, bit_size):
    """
    Calculate the number of zero bits in a number given in a bytes representation.

    :param bytes_input: (bytes) input number
    :param bit_size: size of bytes_input in bits
    :return: (int) number of zero bits

    Returns TypeError if parameter bytes_input is not "bytes"
    Return TypeError if parameter bit_size is not int
    """
    # converting to str in binary representation - byteorder/endianness is irrelevant for this function
    binary_input = format(int.from_bytes(bytes_input, byteorder="big"), 'b').zfill(bit_size)
    return binary_input.count("0")

class ArgumentParser:
    # Adding the utility python scripts to the python PATH
    CURRENT_PATH = sys.path[0]
    OUTPUT_DIR_NAME = CURRENT_PATH + "/hbk_gen_util_outPut"
    PUBLIC_KEY_HASH_FILE_NAME =  OUTPUT_DIR_NAME + "/hbkPubKeyHash.txt"
    PUBLIC_KEY_ZEROS_FILE_NAME = OUTPUT_DIR_NAME + "/hbkPubKeyHashZeros.txt"
    PUBLIC_KEY_HASH_BIN_FILE_NAME = OUTPUT_DIR_NAME + "/hbkPubKeyHash_bin.bin"

    def __init__(self):
        desc_message = "Calculates Hbk that is used for certificate verification and provisioning"
        usage_message = "%(prog)s [-h] -key <key_filename> [-endian <B|L>] [-hash_format <SHA256|SHA256_TRUNC>] " \
                        "[-hash_out FILE] [-zeros_out FILE] [-l FILE]"
        self.parser = argparse.ArgumentParser(description=desc_message, usage=usage_message)
        self.parser.add_argument("-key", help="!Mandatory! The name of the PEM file holding the RSA public key")
        self.parser.add_argument("-endian", default="L",
                                 help="Sets the output endianness: B for Big Endian or L for Little Endian ["
                                      "default: %(default)s]")
        self.parser.add_argument("-hash_format", default="SHA256",
                                 help="Sets the hash format: SHA256 for full SHA-256 hash (Hbk) or SHA256_TRUNC "
                                      "for truncated SHA-256 hash (Hbk0 or Hbk1) [default: %(default)s]")
        # TODO: Verify that we want theses removed
        self.parser.add_argument("-hash_out", default=self.PUBLIC_KEY_HASH_FILE_NAME, metavar="FILE",
                                 help="Writes hash result to FILE [default: %(default)s]")
        self.parser.add_argument("-zeros_out", default=self.PUBLIC_KEY_ZEROS_FILE_NAME, metavar="FILE",
                                 help="Writes number of zero bits in the hash result to FILE [default: %(default)s]")
        self.parser.add_argument("-l", "--log", default=LOG_FILENAME, metavar="FILE",
                                 help="Writes event log to FILE [default: %(default)s]")

        args = self.parser.parse_args()
        if args.key is None:
            self.parser.error("Option -key is a mandatory parameter, use option -h for help!")
        self.key_filename = args.key
        if not Path(args.key).is_file():
            self.parser.error("The path given for RSA public key appears to be non-existing!")
        self.output_endianness = args.endian
        if self.output_endianness != "B" and self.output_endianness != "L":
            self.parser.error("Invalid value for argument output endianness (-endian)!")
        self.hash_format = args.hash_format
        if self.hash_format != "SHA256" and self.hash_format != "SHA256_TRUNC":
            self.parser.error("Invalid value for argument hash format (-hash_format)!")

        self.result_hash_filename = args.hash_out
        self.result_zeros_filename = args.zeros_out
        self.log_filename = args.log

class HbkGenerator:
    def __init__(self, argument_parser):
        self.argument_parser = argument_parser
        self.logger = logging.getLogger()
        self.hash_digest = None
        self.hash_word_list = None
        self.zero_bits = None

        # decide hash output size
        if self.argument_parser.hash_format == "SHA256":
            self.outputSize = OUTPUT_SIZE_SHA_256
        else:
            self.outputSize = OUTPUT_SIZE_SHA_256_TRUNC
        self.output_word_count = self.outputSize // BYTES_WITHIN_WORD
        self.output_bit_count = self.output_word_count * BITS_WITHIN_WORD

    def generate_hbk(self):
        self.logger.info("Step 1 calculate hash")
        self.hash_digest = cryptolayer.Common.get_hashed_n_and_np_from_public_key(self.argument_parser.key_filename)
        self.hash_digest = self.hash_digest[:self.outputSize]
        self.hash_word_list = create_word_list(list(self.hash_digest), self.output_word_count,
                                               self.argument_parser.output_endianness)
        self.logger.info("Step 2 - Calculate num of zero bits over the HASH")
        self.zero_bits = count_zero_bits_in_bytes(self.hash_digest, self.output_bit_count)

        with open(self.argument_parser.PUBLIC_KEY_HASH_BIN_FILE_NAME, 'wb') as publicKeyHash:
            publicKeyHash.write(self.hash_digest)

    def write_output_files(self):
        hex_list = [format(item, "#08x") for item in self.hash_word_list]
        hash_output_string = ", ".join(hex_list)
        with open(self.argument_parser.result_hash_filename, 'w') as hash_output_file:
            hash_output_file.write(hash_output_string)
        with open(self.argument_parser.result_zeros_filename, 'w') as zeros_output_file:
            zeros_output_file.write(hex(self.zero_bits))

if __name__ == "__main__":
    if not (sys.version_info.major == 3 and sys.version_info.minor == 10 and sys.version_info.micro == 12):
        print("The script has been tested with Python 3.10.12!")
    if (sys.version_info.major == 3 and sys.version_info.minor == 9):
        print("*** Ambiq recommends against using Python3.9 ***")
        exit(1)
    # parse arguments
    hbk_gen_argument_parser = ArgumentParser()
    # get logging up and running
    logger_config = loggerinitializer.LoggerInitializer(hbk_gen_argument_parser.log_filename)
    logger = logging.getLogger()
    # perform main task
    generator = HbkGenerator(hbk_gen_argument_parser)
    generator.generate_hbk()
    generator.write_output_files()
    logger.info("Function completed successfully")
