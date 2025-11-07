#!/usr/bin/env python3
#
# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import optparse
import configparser
import sys
import struct
import os
import logging
sys.path.append(os.path.join(sys.path[0], "..", ".."))

from common_utils import loggerinitializer
from common_utils import cryptolayer

# Util's log file
LOG_FILENAME = "oem_asset_pkg.log"

# This utility builds production asset  package:
# the package format is:
#                       token, version, asset length, user data (20 bytes)
#                       nonce(12 bytes)
#                       encrypted asset (up to 512 bytes - multiple of 16 bytes)
#                       asset tag (16 bytes)

class ArgumentParser:
    def __init__(self):
        self.cfg_filename = None
        self.log_filename = LOG_FILENAME
        self.parser = optparse.OptionParser(usage="usage: %prog cfg_file [log_filename]")

    def parse_arguments(self):
        (options, args) = self.parser.parse_args()
        if len(args) > 2 or len(args) < 1:
            self.parser.error("incorrect number of positional arguments")
        elif len(args) == 2:
            self.log_filename = args[1]
        self.cfg_filename = args[0]

ASSET_TYPE_OEM_DATA = 3
class AssetPackageConfig:
    CFG_SECTION_NAME = "DMPU-OEM-ASSET-CFG"

    def __init__(self):
        self._asset_type = None
        self._icv_enc_oem_key = None
        self._oem_enc_keypair = None
        self._oem_enc_keypwd = ""
        self._asset_filename = None
        self._pkg_filename = None

    @property
    def section_name(self):
        return self.CFG_SECTION_NAME

    @property
    def asset_type(self):
        return self._asset_type

    @asset_type.setter
    def asset_type(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter asset-type must be a string")
        elif value not in ["kcp", "kce", "oem_data"]:
            raise ValueError("Config parameter asset-type has invalid input value")
        else:
            self._asset_type = value

    @property
    def icv_enc_oem_key(self):
        return self._icv_enc_oem_key

    @icv_enc_oem_key.setter
    def icv_enc_oem_key(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter icv-enc-oem-key must be a string")
        elif value == "":
            raise ValueError("Config parameter icv-enc-oem-key cannot be an empty string!")
        else:
            self._icv_enc_oem_key = value

    @property
    def oem_enc_keypair(self):
        return self._oem_enc_keypair

    @oem_enc_keypair.setter
    def oem_enc_keypair(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-enc-keypair must be a string")
        elif value == "":
            raise ValueError("Config parameter oem-enc-keypair cannot be an empty string!")
        else:
            self._oem_enc_keypair = value

    @property
    def oem_enc_keypwd(self):
        return self._oem_enc_keypwd

    @oem_enc_keypwd.setter
    def oem_enc_keypwd(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-enc-keypwd must be a string")
        else:
            self._oem_enc_keypwd = value

    @property
    def asset_filename(self):
        return self._asset_filename

    @asset_filename.setter
    def asset_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter asset-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter asset-filename cannot be an empty string!")
        else:
            self._asset_filename = value

    @property
    def pkg_filename(self):
        return self._pkg_filename

    @pkg_filename.setter
    def pkg_filename(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter pkg-filename must be a string")
        elif value == "":
            raise ValueError("Config parameter pkg-filename cannot be an empty string!")
        else:
            self._pkg_filename = value

class AssetPackageConfigParser:

    def __init__(self, config_filename):
        """
        Parses the DMPU-OEM-ASSET-CFG cfg file.
        Raises ConfigParsingError when cfg has been written incorrectly.
        Raises TypeError or ValueError when a cfg value is incorrect on its own.

        :param str config_filename: name of cfg file to parse
        """
        self.config_filename = config_filename
        self.config = configparser.ConfigParser()
        self.logger = logging.getLogger()
        self._config_holder = AssetPackageConfig()

    def get_config(self):
        return self._config_holder

    def parse_config(self):
        self.logger.info("Parsing config file: " + self.config_filename)
        self.config.read(self.config_filename)

        if not self.config.has_section(self._config_holder.section_name):
            self.logger.warning("section [" + self._config_holder.section_name + "] wasn't found in cfg file")
            return False

        if not self.config.has_option(self._config_holder.section_name, 'asset-type'):
            self.logger.warning("asset-type not found")
            return False
        else:
            self._config_holder.asset_type = self.config.get(self._config_holder.section_name, 'asset-type')

        if not self.config.has_option(self._config_holder.section_name, 'icv-enc-oem-key'):
            self.logger.warning("icv-enc-oem-key not found")
            return False
        else:
            self._config_holder.icv_enc_oem_key = self.config.get(self._config_holder.section_name, 'icv-enc-oem-key')

        if not self.config.has_option(self._config_holder.section_name, 'oem-enc-keypair'):
            self.logger.warning("oem-enc-keypair not found")
            return False
        else:
            self._config_holder.oem_enc_keypair = self.config.get(self._config_holder.section_name, 'oem-enc-keypair')

        if self.config.has_option(self._config_holder.section_name, 'oem-enc-keypwd'):
            self._config_holder.oem_enc_keypwd = self.config.get(self._config_holder.section_name, 'oem-enc-keypwd')

        if not self.config.has_option(self._config_holder.section_name, 'asset-filename'):
            self.logger.warning("asset-filename not found")
            return False
        else:
            self._config_holder.asset_filename = self.config.get(self._config_holder.section_name, 'asset-filename')

        if not self.config.has_option(self._config_holder.section_name, 'pkg-filename'):
            self.logger.warning("pkg-filename not found")
            return False
        else:
            self._config_holder.pkg_filename = self.config.get(self._config_holder.section_name, 'pkg-filename')

        return True

class AssetPackager:
    PROD_ASSET_SIZE = 16
    PUBKEY_SIZE_BYTES = 384     # 3072 bits
    PROD_ASSET_PROV_TOKEN = 0x50726F64
    PROD_ASSET_PROV_VERSION = 0x10000
    PROD_ASSET_RESERVED1_VAL = 0x52657631
    PROD_ASSET_RESERVED2_VAL = 0x52657632
    PROD_ASSET_NONCE_SIZE = 12
    PROD_OEM_ENC_CONTEXT = "Kce "
    PROD_OEM_PROV_CONTEXT = "Kcp "
    PROD_OEM_PROP_DATA_CONTEXT = "OEMP"

    def __init__(self, key_cfg):
        self.config = key_cfg
        self.logger = logging.getLogger()

    def generate_package(self):
        self.logger.info("OEM Asset provisioning Utility started")
        with open(self.config.asset_filename, "rb") as asset_file:
            asset_data = asset_file.read()
        asset_data_size = len(asset_data)

        if self.config.asset_type == "kce" or self.config.asset_type == "kcp" :
            if asset_data_size != self.PROD_ASSET_SIZE:
                self.logger.error("Invalid asset size: " + str(asset_data_size))
                sys.exit(-1)
        elif self.config.asset_type == "oem_data":
            if asset_data_size != 2048:
                self.logger.error("Invalid asset size: " + str(asset_data_size))
                sys.exit(-1)

        with open(self.config.icv_enc_oem_key, "rb") as koem_key_file:
            koem_key = koem_key_file.read()
        koem_key_size = len(koem_key)
        if koem_key_size != self.PUBKEY_SIZE_BYTES:
            self.logger.error("Invalid key size: " + str(koem_key_size))
            sys.exit(-1)

        self.logger.info("**** Generate DMPU Asset package ****")
        # build package header
        nonce = os.urandom(self.PROD_ASSET_NONCE_SIZE)
        asset_package = (struct.pack('<I', self.PROD_ASSET_PROV_TOKEN)
                         + struct.pack('<I', self.PROD_ASSET_PROV_VERSION)
                         + struct.pack('<I', asset_data_size)
                         + struct.pack('<I', self.PROD_ASSET_RESERVED1_VAL)
                         + struct.pack('<I', self.PROD_ASSET_RESERVED2_VAL))

        # decrypt temporary key created by icv
        # TODO: Add the plugin interface here
        key_tmp = cryptolayer.Common.decrypt_data_with_rsa_keypair(self.config.oem_enc_keypair, self.config.oem_enc_keypwd, koem_key)

        if self.config.asset_type == "kcp":
            key_prov_context = self.PROD_OEM_PROV_CONTEXT
        elif self.config.asset_type == "kce":
            key_prov_context = self.PROD_OEM_ENC_CONTEXT
        elif self.config.asset_type == "oem_data":
            key_prov_context = self.PROD_OEM_PROP_DATA_CONTEXT

        # calculate Kprov= cmac(Ktmp, 0x01 || "P"  || 0x0 || key_prov_context || 0x80)
        input_data = (struct.pack('B', 0x01)
                      + struct.pack('B', 0x50) # ASCII val for "P"
                      + struct.pack('B', 0x0)
                      + key_prov_context.encode('utf-8')
                      + struct.pack('B', 0x80))  # outkeysize in bits
        prov_key = cryptolayer.AesCrypto.calc_aes_cmac(input_data, key_tmp)

        # encrypt and authenticate the asset
        encrypted_data_and_tag = cryptolayer.AesCrypto.encrypt_aes_ccm(prov_key, nonce, asset_package, asset_data)
        # attach encrypted data to asset blob and nonce
        asset_package += nonce
        asset_package += encrypted_data_and_tag
        # write asset blob to output file
        with open(self.config.pkg_filename, "wb") as encrypted_outfile:
            encrypted_outfile.write(asset_package)

if __name__ == "__main__":
    if not (sys.version_info.major == 3 and sys.version_info.minor == 10 and sys.version_info.micro == 12):
        print("The script has been tested with Python 3.10.12!")
    if (sys.version_info.major == 3 and sys.version_info.minor == 9):
        print("*** Ambiq recommends against using Python3.9 ***")
        exit(1)
    # parse arguments
    the_argument_parser = ArgumentParser()
    the_argument_parser.parse_arguments()
    # get logging up and running
    logger_config = loggerinitializer.LoggerInitializer(the_argument_parser.log_filename)
    logger = logging.getLogger()
    # get util configuration parameters

    config_parser = AssetPackageConfigParser(the_argument_parser.cfg_filename)
    if config_parser.parse_config() is False:
        logger.critical("Config file parsing is not successful")
        sys.exit(-1)
    # create secure asset package
    asset_provisioner = AssetPackager(config_parser.get_config())
    asset_provisioner.generate_package()
    logger.info("**** Generate OEM asset package completed successfully ****")
