# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import configparser
import logging

from common_utils import global_defines

class ConfigParsingError(Exception):
    """Raised when trying to load a misconfigured CFG file"""
    pass

class KeyCertificateConfig:
    CFG_SECTION_NAME = "KEY-CFG"

    def __init__(self, config_filename):
        """
        Parses the KEY-CFG key certificate cfg file.
        Raises ConfigParsingError when cfg has been written incorrectly.
        Raises TypeError or ValueError when a cfg value is incorrect on its own.

        :param str config_filename: name of cfg file to parse
        """
        self._config_filename = config_filename
        self._logger = logging.getLogger()
        self._parser = configparser.ConfigParser()

        self._auth_key = None
        self._signature_plugin_path = None
        self._signature_plugin_cfg = None
        self._auth_pubkey = None
        self._cert_keypair = None      #TODO Ensure this can be removed
        self._cert_keypair_pwd = None  #TODO Ensure this can be removed
        self._hbk_id = None
        self._nvcounter_val = None
        self._next_cert_pubkey = None
        self._cert_pkg = None
        self._parse_config()

    @property
    def section_name(self):
        return self.CFG_SECTION_NAME

    @property
    def auth_key(self):
        return self._auth_key

    @auth_key.setter
    def auth_key(self, value):
        if isinstance(value, int) is False:
            raise ValueError("Config parameter auth-key must be an integer!")
        elif value not in [0x0, 0x1, 0x2, 0x10, 0x11]:
            raise ValueError("invalid input value for config parameter auth-key")
        else:
            self._auth_key = value

    @property
    def signature_plugin_path(self):
        return self._signature_plugin_path

    @signature_plugin_path.setter
    def signature_plugin_path(self, value):
        if value == "":
            raise ValueError("Config parameter signature-plugin-path cannot be an empty string!")
        elif isinstance(value, str) is False:
            raise ValueError("Config parameter signature-plugin-path must be a string")
        else:
            self._signature_plugin_path = value

    @property
    def signature_plugin_cfg(self):
        return self._signature_plugin_cfg

    @signature_plugin_cfg.setter
    def signature_plugin_cfg(self, value):
        if value == "":
            raise ValueError("Config parameter signature-plugin-cfg cannot be an empty string!")
        elif isinstance(value, str) is False:
            raise ValueError("Config parameter signature-plugin-cfg must be a string")
        else:
            self._signature_plugin_cfg = value

    @property
    def auth_pubkey(self):
        return self._auth_pubkey

    @auth_pubkey.setter
    def auth_pubkey(self, value):
        if value == "":
            raise ValueError("Config parameter auth-pubkey cannot be an empty string!")
        elif isinstance(value, str) is False:
            raise ValueError("Config parameter auth-pubkey must be a string")
        else:
            self._auth_pubkey = value

    @property
    def cert_keypair(self):
        return self._cert_keypair

    @cert_keypair.setter
    def cert_keypair(self, value):
        if value == "":
            raise ValueError("Config parameter cert-keypair cannot be an empty string!")
        elif isinstance(value, str) is False:
            raise TypeError("Config parameter cert-keypair must be a string")
        else:
            self._cert_keypair = value

    #TODO Ensure this can be removed
    @property
    def cert_keypair_pwd(self):
        return self._cert_keypair_pwd

    @cert_keypair_pwd.setter
    def cert_keypair_pwd(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter cert-keypair-pwd must be a string")
        else:
            self._cert_keypair_pwd = value

    @property
    def hbk_id(self):
        return self._hbk_id

    @hbk_id.setter
    def hbk_id(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter hbk-id must be an integer")
        elif value not in [0, 1, 2]:
            raise ValueError("invalid input value for config parameter hbk-id")
        else:
            self._hbk_id = value

    @property
    def nvcounter_val(self):
        return self._nvcounter_val

    @nvcounter_val.setter
    def nvcounter_val(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter nvcounter-val must be an integer")
        elif value not in range(global_defines.SW_REVOCATION_MAX_NUM_OF_BITS_HBK2 + 1):
            raise ValueError("invalid input value for config parameter nvcounter-val")
        else:
            self._nvcounter_val = value

    @property
    def next_cert_pubkey(self):
        return self._next_cert_pubkey

    @next_cert_pubkey.setter
    def next_cert_pubkey(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter next-cert-pubkey must be a string")
        elif value == "":
            raise ValueError("Config parameter next-cert-pubkey cannot be an empty string!")
        else:
            self._next_cert_pubkey = value

    @property
    def cert_pkg(self):
        return self._cert_pkg

    @cert_pkg.setter
    def cert_pkg(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter cert-pkg must be a string")
        elif value == "":
            raise ValueError("Config parameter cert-pkg cannot be an empty string!")
        else:
            self._cert_pkg = value

    def _parse_config(self):
        if self._logger is not None:
            self._logger.info("Parsing config file: " + self._config_filename)
        parsed_list = self._parser.read(self._config_filename)
        if len(parsed_list) == 1 and self._logger is not None:
            self._logger.info(
                "\n".join(["Parsed config items:"]
                          + [":\t".join([item[0], item[1]]) for item in self._parser.items(self.CFG_SECTION_NAME)])
            )
        else:
            message = "File " + self._config_filename + " could not be parsed. The file may not exist."
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        if not self._parser.has_section(self.section_name):
            message = "section [" + self.section_name + "] wasn't found in cfg file"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)

        if not self._parser.has_option(self.section_name, 'auth-key'):
            message = "auth-key not found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.auth_key = int(self._parser.get(self.section_name, 'auth-key'), 0)

        if not self._parser.has_option(self.section_name, 'signature-plugin-path'):
            message = "signature-plugin-path not found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.signature_plugin_path = self._parser.get(self.section_name, 'signature-plugin-path')

        if not self._parser.has_option(self.section_name, 'signature-plugin-cfg'):
            message = "signature-plugin-cfg not found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.signature_plugin_cfg = self._parser.get(self.section_name, 'signature-plugin-cfg')

        if not self._parser.has_option(self.section_name, 'auth-pubkey'):
            message = "auth-pubkey not found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.auth_pubkey = self._parser.get(self.section_name, 'auth-pubkey')

        if not self._parser.has_option(self.section_name, 'hbk-id'):
            message = "hbk-id not found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.hbk_id = self._parser.getint(self.section_name, 'hbk-id')

        if not self._parser.has_option(self.section_name, 'nvcounter-val'):
            message = "nvcounter-val not found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.nvcounter_val = self._parser.getint(self.section_name, 'nvcounter-val')
        # verify nvcounter_val according to HBK
        if self.hbk_id == 0:
            max_value_nvcounter = global_defines.SW_REVOCATION_MAX_NUM_OF_BITS_HBK0
        elif self.hbk_id == 1:
            max_value_nvcounter = global_defines.SW_REVOCATION_MAX_NUM_OF_BITS_HBK1
        else:
            max_value_nvcounter = global_defines.SW_REVOCATION_MAX_NUM_OF_BITS_HBK2
        if self.nvcounter_val > max_value_nvcounter:
            message = "Invalid nvcounter-val based on given hbk-id"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)

        if not self._parser.has_option(self.section_name, 'next-cert-pubkey'):
            message = "no next-cert-pubkey parameter found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.next_cert_pubkey = self._parser.get(self.section_name, 'next-cert-pubkey')

        if not self._parser.has_option(self.section_name, 'cert-pkg'):
            message = "no cert-pkg parameter found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.cert_pkg = self._parser.get(self.section_name, 'cert-pkg')
