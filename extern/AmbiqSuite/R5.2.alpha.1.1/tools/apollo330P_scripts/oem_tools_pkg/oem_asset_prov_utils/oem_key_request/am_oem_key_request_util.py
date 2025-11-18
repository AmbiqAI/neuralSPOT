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
LOG_FILENAME = "key_request_cert.log"

# This utility builds key request certifiacte sent from OEM to ICV:
# the package Header format is:
#                       token, version, length,
#                       certifiacte main PubKey (pub key)
#                       certifiacte encryption PubKey (pub key)
#                       certSign

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

class OemKeyRequestConfig:
    CFG_SECTION_NAME = "DMPU-OEM-KEY-REQ-CFG"

    def __init__(self):
        self._oem_main_keypair = None
        self._oem_main_keypwd = ""
        self._oem_enc_pubkey = None
        self._oem_cert_pkg = None

    @property
    def section_name(self):
        return self.CFG_SECTION_NAME

    @property
    def oem_main_keypair(self):
        return self._oem_main_keypair

    @oem_main_keypair.setter
    def oem_main_keypair(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-main-keypair must be a string")
        elif value == "":
            raise ValueError("Config parameter oem-main-keypair cannot be an empty string!")
        else:
            self._oem_main_keypair = value

    @property
    def oem_main_keypwd(self):
        return self._oem_main_keypwd

    @oem_main_keypwd.setter
    def oem_main_keypwd(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-main-keypwd must be a string")
        else:
            self._oem_main_keypwd = value

    @property
    def oem_enc_pubkey(self):
        return self._oem_enc_pubkey

    @oem_enc_pubkey.setter
    def oem_enc_pubkey(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-enc-pubkey must be a string")
        elif value == "":
            raise ValueError("Config parameter oem-enc-pubkey cannot be an empty string!")
        else:
            self._oem_enc_pubkey = value

    @property
    def oem_cert_pkg(self):
        return self._oem_cert_pkg

    @oem_cert_pkg.setter
    def oem_cert_pkg(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter oem-cert-pkg must be a string")
        elif value == "":
            raise ValueError("Config parameter oem-cert-pkg cannot be an empty string!")
        else:
            self._oem_cert_pkg = value

class OemKeyRequestConfigParser:

    def __init__(self, config_filename):
        self.config_filename = config_filename
        self.config = configparser.ConfigParser()
        self.logger = logging.getLogger()
        self._config_holder = OemKeyRequestConfig()

    def get_config(self):
        return self._config_holder

    def parse_config(self):
        self.logger.info("Parsing config file: " + self.config_filename)
        self.config.read(self.config_filename)

        if not self.config.has_section(self._config_holder.section_name):
            self.logger.warning("section [" + self._config_holder.section_name + "] wasn't found in cfg file")
            return False

        if not self.config.has_option(self._config_holder.section_name, 'oem-main-keypair'):
            self.logger.warning("oem-main-keypair not found")
            return False
        else:
            self._config_holder.oem_main_keypair = self.config.get(self._config_holder.section_name, 'oem-main-keypair')

        if self.config.has_option(self._config_holder.section_name, 'oem-main-keypwd'):
            self._config_holder.oem_main_keypwd = self.config.get(self._config_holder.section_name, 'oem-main-keypwd')

        if not self.config.has_option(self._config_holder.section_name, 'oem-enc-pubkey'):
            self.logger.warning("oem-enc-pubkey not found")
            return False
        else:
            self._config_holder.oem_enc_pubkey = self.config.get(self._config_holder.section_name, 'oem-enc-pubkey')

        if not self.config.has_option(self._config_holder.section_name, 'oem-cert-pkg'):
            self.logger.warning("oem-cert-pkg not found")
            return False
        else:
            self._config_holder.oem_cert_pkg = self.config.get(self._config_holder.section_name, 'oem-cert-pkg')

        return True

class KeyRequestCertificateCreator:
    # oem key request certificate struct is:
    #  token || version || size || main N+Np || enc N + Np || signature
    #   4           4              4       384+20            384+20           384
    OEM_KEY_REQ_CERT_SIZE = 1204
    DMPU_CERT_HEADER_SIZE_IN_BYTES = 12  # token || version || size
    DMPU_OEM_KEY_REQ_TOKEN = 0x52455144
    DMPU_OEM_KEY_REQ_VERSION = 0x01

    PUBKEY_SIZE_BYTES = 384  # 3072 bits
    NP_SIZE_IN_BYTES = 20
    KRTL_SIZE = 16

    def __init__(self, key_request_cfg):
        self.config = key_request_cfg
        self.logger = logging.getLogger()

    def create_certificate(self):
        self.logger.info("**** Generate OEM key requesting certificate ****")
        # token || version || size || main public key + Np || enc public key + Np
        certificate_length = self.DMPU_CERT_HEADER_SIZE_IN_BYTES + 2 * (self.PUBKEY_SIZE_BYTES + self.NP_SIZE_IN_BYTES)
        certificate_header = (struct.pack('<I', self.DMPU_OEM_KEY_REQ_TOKEN)
                              + struct.pack('<I', self.DMPU_OEM_KEY_REQ_VERSION)
                              + struct.pack('<I', certificate_length))

        # get the enabler certificate public key + Np
        oem_rsa_public_key_params = cryptolayer.Common.get_n_and_np_from_keypair(self.config.oem_main_keypair,
                                                                                 self.config.oem_main_keypwd)
        # get encrypted  public key  (from public key)
        oem_enc_pubkey_params = cryptolayer.Common.get_n_and_np_from_public_key(self.config.oem_enc_pubkey)
        # Do RSA signature
        signed_data = certificate_header + oem_rsa_public_key_params + oem_enc_pubkey_params
        signature = cryptolayer.Common.rsa_sign(signed_data, self.config.oem_main_keypair, self.config.oem_main_keypwd)
        # Build the end of the certificate - add the signature
        full_certificate_data = signed_data + signature

        # write certificate to output file
        with open(self.config.oem_cert_pkg, "wb") as certificate_outfile:
            certificate_outfile.write(full_certificate_data)

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
    config_parser = OemKeyRequestConfigParser(the_argument_parser.cfg_filename)
    if config_parser.parse_config() is False:
        logger.critical("Config file parsing is not successful")
        sys.exit(-1)
    # create secure asset package
    asset_provisioner = KeyRequestCertificateCreator(config_parser.get_config())
    asset_provisioner.create_certificate()
    logger.info("**** Generate OEM key requesting successfully ****")
