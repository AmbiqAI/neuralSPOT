# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import logging
import os
import re
import struct

from common_cert_lib.armcertificate import ArmCertificate
from common_cert_lib.armcertificatebody import ArmCertificateBody
from common_cert_lib.armcertificateheader import ArmCertificateHeader
from common_cert_lib.armcertificatesignature import ArmCertificateSignature
from common_utils import global_defines, cryptolayer

import plugin_manager
import binascii

class ContentCertCreationError(Exception):
    """Raised when certificate creation fails"""
    pass

class ContentArmCertificateHeader(ArmCertificateHeader):
    NUM_OF_SW_COMPS_BIT_POS = 16
    CODE_ENCRYPTION_SUPPORT_BIT_POS = 4
    LOAD_VERIFY_SCHEME_BIT_POS = 8
    CRYPTO_TYPE_BIT_POS = 12
    CERT_TOKEN = 0x53426363

    def __init__(self, cert_version, code_enc_id, load_verify_scheme, crypto_type, num_of_comps):
        self._cert_version = (cert_version[0] << 16) + cert_version[1]
        sizeof_sw_records_field = (global_defines.NONCE_SIZE_IN_WORDS
                                   + num_of_comps
                                   * (global_defines.HASH_ALGORITHM_SHA256_SIZE_IN_WORDS
                                      + global_defines.SW_REC_ADDR32_SIGNED_DATA_SIZE_IN_WORDS))
        self._signed_content_size = (self.HEADER_SIZE_IN_WORDS
                                     + global_defines.SW_VERSION_OBJ_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.SB_CERT_RSA_KEY_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.NP_SIZE_IN_WORDS
                                     + sizeof_sw_records_field)

        self._flags = (0xf
                       | (code_enc_id << self.CODE_ENCRYPTION_SUPPORT_BIT_POS)
                       | (load_verify_scheme << self.LOAD_VERIFY_SCHEME_BIT_POS)
                       | (crypto_type << self.CRYPTO_TYPE_BIT_POS)
                       | (num_of_comps << self.NUM_OF_SW_COMPS_BIT_POS))

    def magic_number(self):
        return self.CERT_TOKEN

    def cert_version_number(self):
        return self._cert_version

    def signed_content_size(self):
        return self._signed_content_size

    def flags(self):
        return self._flags

    def serialize_to_bytes(self):
        data = (struct.pack('<I', self.CERT_TOKEN)
                + struct.pack('<I', self._cert_version)
                + struct.pack('<I', self._signed_content_size)
                + struct.pack('<I', self._flags))
        return data

class ContentArmCertificateBody(ArmCertificateBody):
    NUM_OF_BYTES_IN_ADDRESS = 4
    MEM_ADDRESS_UNLOAD_FLAG = 0xFFFFFFFF

    def __init__(self, sw_version, signer_pubkey_filename,
                 code_enc_id, images_table, load_verify_scheme, aes_enc_key, crypto_type):
        self._sw_version = sw_version
        self._images_table_filename = images_table
        self._load_verify_scheme = load_verify_scheme
        self._enc_key_filename = aes_enc_key
        self._crypto_type = crypto_type
        self.logger = logging.getLogger()
        self._signer_rsa_public_key = cryptolayer.Common.get_n_and_np_from_public_key(signer_pubkey_filename)

        if code_enc_id == global_defines.USE_AES_CE_ID_NONE:
            self._key_nonce = bytes(8)
        else:
            self._key_nonce = os.urandom(8)

        self._sw_records = []
        self._none_signed_info = []
        # process images-table
        with open(self._images_table_filename, "r") as images_file:
            image_list = images_file.readlines()

        self._num_images = sum(1 for line in image_list if not re.match(r'^\s*(#|$)', line))

        if not 0 < self._num_images < 17:
            raise ContentCertCreationError("number of images images-table is not supported")
        # analyze image-tables lines
        parsed_images = []
        for line in image_list:
            if not re.match(r'^\s*#|^\s*$', line):   # ignore commented out and empty lines
                line_elements = line.split()

                image_filename = line_elements[0]
                mem_load_address = int(line_elements[1], 16)
                if mem_load_address == 0:
                    raise ContentCertCreationError("Invalid load address in images-table: 0 is not allowed as an address")
                mem_load_address_byte_list = list(mem_load_address.to_bytes(self.NUM_OF_BYTES_IN_ADDRESS,
                                                                            byteorder="big"))
                flash_store_address = int(line_elements[2], 16)
                image_max_size = int(line_elements[3], 16)
                is_aes_code_enc_used = int(line_elements[4], 16)
                write_protect = int(line_elements[5], 16)
                copy_protect = int(line_elements[6], 16)
                extended_format = int(line_elements[7], 16) # Not Supported. Must be set to 0

                # perform some basic input checks:
                if (self._load_verify_scheme == global_defines.VERIFY_IMAGE_IN_FLASH
                    and mem_load_address != self.MEM_ADDRESS_UNLOAD_FLAG) \
                        or (self._load_verify_scheme != global_defines.VERIFY_IMAGE_IN_FLASH
                            and mem_load_address == self.MEM_ADDRESS_UNLOAD_FLAG):
                    raise ContentCertCreationError("invalid load address defined in images-table: "
                                            "mem_load_address can be 0xffffffff only in load_verify_scheme==1 case")
                if (self._load_verify_scheme == global_defines.VERIFY_IMAGE_IN_MEM
                    and flash_store_address != self.MEM_ADDRESS_UNLOAD_FLAG) \
                        or (self._load_verify_scheme != global_defines.VERIFY_IMAGE_IN_MEM
                            and flash_store_address == self.MEM_ADDRESS_UNLOAD_FLAG):
                    raise ContentCertCreationError("invalid flash address defines in images-table: "
                                            "flash_store_address can be 0xffffffff only in load_verify_scheme==2 case")

                parsed_images.append({"image_filename": image_filename,
                                      "mem_load_address": mem_load_address,
                                      "mem_load_address_byte_list": mem_load_address_byte_list,
                                      "flash_store_address": flash_store_address,
                                      "image_max_size": image_max_size,
                                      "is_aes_code_enc_used": is_aes_code_enc_used,
                                      "write_protect": write_protect,
                                      "copy_protect": copy_protect,
                                      "extended_format": extended_format})

        # image_hash_and_size = []
        for item in parsed_images:
            if item["is_aes_code_enc_used"] == 1:
                # do Aes and Hash
                if code_enc_id != global_defines.USE_AES_CE_ID_NONE:
                    with open(self._enc_key_filename, "r") as aes_key_file:
                        str_key_data = aes_key_file.read()
                    str_key_data_bytes = str_key_data.split(",")
                    if len(str_key_data_bytes) < global_defines.AES_DECRYPT_KEY_SIZE_IN_BYTES:
                        message = "key size in aes-enc-key file is too small - must be at least 128 bits!"
                        if self.logger is not None:
                            self.logger.error(message)
                        raise ContentCertCreationError(message)
                    if len(str_key_data_bytes) > global_defines.AES_DECRYPT_KEY_SIZE_IN_BYTES:
                        message = "key size in aes-enc-key file is too big - truncating parameter"
                        if self.logger is not None:
                            self.logger.warning(message)
                        else:
                            print(message)
                    key_data_list = [int(item, 16) for item in str_key_data_bytes]
                    key_data_list = key_data_list[:global_defines.AES_DECRYPT_KEY_SIZE_IN_BYTES]

                    # combine nonce and address into iv (8 bytes nonce + 4 bytes address + 4 bytes of zeros)
                    aes_iv = self._key_nonce + bytes(item["mem_load_address_byte_list"]) + bytes(4)
                    # encrypt image
                    new_image_filename = item["image_filename"][:-4] + global_defines.SW_COMP_FILE_NAME_POSTFIX
                    hash_of_image, actual_image_size = cryptolayer.Common.encrypt_file_with_aes_ctr(item["image_filename"],
                                                                                                    new_image_filename,
                                                                                                    key_data_list,
                                                                                                    aes_iv,
                                                                                                    self._crypto_type)
                else:
                    raise ContentCertCreationError("invalid aes-ce-id for image encryption flag in image of images-table - "
                                            "must have aes-enc-key for images to be encrypted")
            else:
                # do only hash
                with open(item["image_filename"], "rb") as input_file:
                    plaintext = input_file.read()
                actual_image_size = len(plaintext)
                hash_of_image = cryptolayer.HashCrypto.calculate_sha256_hash(plaintext)
            # image_hash_and_size.append([hash_of_image, actual_image_size])
            self._sw_records.append([hash_of_image, item["mem_load_address"], item["image_max_size"], item["is_aes_code_enc_used"]])
            self._none_signed_info.append([item["flash_store_address"], actual_image_size])

        if write_protect:
            is_aes_code_enc_used = is_aes_code_enc_used | 0x2
        if copy_protect:
            is_aes_code_enc_used = is_aes_code_enc_used | 0x4
        if extended_format:
            is_aes_code_enc_used = is_aes_code_enc_used | 0x8

    @property
    def num_images(self):
        return self._num_images

    @property
    def sw_version(self):
        return self._sw_version

    @property
    def signer_rsa_public_key(self):
        return self._signer_rsa_public_key

    @property
    def key_nonce(self):
        return self._key_nonce

    def serialize_to_bytes(self):
        data = (self._signer_rsa_public_key
                + struct.pack('<I', self._sw_version)
                + self._key_nonce
                + b''.join([struct.pack('<I', item) if type(item) is not bytes else item for sublist in self._sw_records for item in sublist])
                )
        return data

    @property
    def x509_body_extension_data(self):
        data = (struct.pack('<I', self._sw_version)
                + self._key_nonce
                + b''.join(
                    [struct.pack('<I', item) if type(item) is not bytes else item for sublist in self._sw_records for
                     item in sublist])
                )
        return data

    @property
    def none_signed_info_serialized(self):
        return b''.join([struct.pack('<I', item) for sublist in self._none_signed_info for item in sublist])

class ContentArmCertificate(ArmCertificate):

    def __init__(self, certificate_config, cert_version):
        self._cert_config = certificate_config
        self._cert_version = cert_version
        self._num_of_comps = None

        self._cert_config.cert_keypair = "../../am_oem_key_gen_util/oemRSAKeys/oemContentCertKeyPair.pem"
        self._cert_config.cert_keypair_pwd = "../../am_oem_key_gen_util/oemRSAKeys/pwdOemContentCertKey_Rsa.txt"
        self._cert_body = ContentArmCertificateBody(self._cert_config.nvcounter_val,
                                                    self._cert_config.auth_pubkey,
                                                    self._cert_config.aes_ce_id,
                                                    self._cert_config.images_table,
                                                    self._cert_config.load_verify_scheme,
                                                    self._cert_config.aes_enc_key,
                                                    self._cert_config.crypto_type)
        self._num_of_comps = self._cert_body.num_images
        self._cert_header = ContentArmCertificateHeader(self._cert_version,
                                                        self._cert_config.aes_ce_id,
                                                        self._cert_config.load_verify_scheme,
                                                        self._cert_config.crypto_type,
                                                        self._num_of_comps)
        self._certificate_data = self._cert_header.serialize_to_bytes() + self._cert_body.serialize_to_bytes()

        plugin = plugin_manager.load_plugin(self._cert_config.signature_plugin_cfg, self._cert_config.signature_plugin_path)
        self._cert_signature = plugin.get_signature(self._cert_config.auth_key, self._certificate_data)

        self._certificate_data += self._cert_signature
        self._certificate_data += self._cert_body.none_signed_info_serialized

    @property
    def header(self):
        return self._cert_header

    @property
    def body(self):
        return self._cert_body

    @property
    def signature(self):
        return self._cert_signature

    @property
    def certificate_data(self):
        return self._certificate_data
