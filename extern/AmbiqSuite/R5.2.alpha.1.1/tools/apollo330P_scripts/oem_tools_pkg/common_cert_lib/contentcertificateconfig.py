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

class ContentCertificateConfig:
    CFG_SECTION_NAME = "CNT-CFG"

    def __init__(self, config_filename):
        """
        Parses the CNT-CFG content certificate cfg file.
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
        self._load_verify_scheme = None
        self._crypto_type = None
        self._aes_ce_id = None
        self._aes_enc_key = None
        self._images_table = None
        self._nvcounter_val = None
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
    def load_verify_scheme(self):
        return self._load_verify_scheme

    @load_verify_scheme.setter
    def load_verify_scheme(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter load_verify_scheme must be an integer")
        elif value not in [0, 1, 2, 3]:
            raise ValueError("invalid input value for config parameter load_verify_scheme")
        else:
            self._load_verify_scheme = value

    @property
    def crypto_type(self):
        return self._crypto_type

    @crypto_type.setter
    def crypto_type(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter crypto_type must be an integer")
        elif value not in [0, 1]:
            raise ValueError("invalid input value for config parameter crypto_type")
        else:
            self._crypto_type = value

    @property
    def aes_ce_id(self):
        return self._aes_ce_id

    @aes_ce_id.setter
    def aes_ce_id(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter aes_ce_id must be an integer")
        elif value not in [0, 1, 2]:
            raise ValueError("invalid input value for config parameter aes_ce_id")
        else:
            self._aes_ce_id = value

    @property
    def aes_enc_key(self):
        return self._aes_enc_key

    @aes_enc_key.setter
    def aes_enc_key(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter aes_enc_key must be a string")
        else:
            self._aes_enc_key = value

    @property
    def images_table(self):
        return self._images_table

    @images_table.setter
    def images_table(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter images_table must be a string")
        elif value == "":
            raise ValueError("Config parameter images_table cannot be an empty string!")
        else:
            self._images_table = value

    @property
    def nvcounter_val(self):
        return self._nvcounter_val

    @nvcounter_val.setter
    def nvcounter_val(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter nvcounter_val must be an integer")
        elif value not in range(global_defines.SW_REVOCATION_MAX_NUM_OF_BITS_HBK2 + 1):
            raise ValueError("invalid input value for config parameter nvcounter_val")
        else:
            self._nvcounter_val = value

    @property
    def cert_pkg(self):
        return self._cert_pkg

    @cert_pkg.setter
    def cert_pkg(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter cert_pkg must be a string")
        elif value == "":
            raise ValueError("Config parameter cert_pkg cannot be an empty string!")
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

        if not self._parser.has_option(self.section_name, 'load-verify-scheme'):
            message = "parameter load-verify-scheme not found in cfg file"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.load_verify_scheme = self._parser.getint(self.section_name, 'load-verify-scheme')

        if not self._parser.has_option(self.section_name, 'crypto-type'):
            message = "parameter crypto-type not found in cfg file"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.crypto_type = self._parser.getint(self.section_name, 'crypto-type')

        if not self._parser.has_option(self.section_name, 'aes-ce-id'):
            if self._logger is not None:
                self._logger.error("parameter aes-ce-id not found in cfg file")
            raise ConfigParsingError
        else:
            self.aes_ce_id = self._parser.getint(self.section_name, 'aes-ce-id')

        if self._parser.has_option(self.section_name, 'aes-enc-key'):
            self.aes_enc_key = self._parser.get(self.section_name, 'aes-enc-key')
        else:
            self.aes_enc_key = ""

        if not self._parser.has_option(self.section_name, 'images-table'):
            if self._logger is not None:
                self._logger.error("parameter images-table not found in cfg file")
            raise ConfigParsingError
        else:
            self.images_table = self._parser.get(self.section_name, 'images-table')

        if not self._parser.has_option(self.section_name, 'nvcounter-val'):
            if self._logger is not None:
                self._logger.error("parameter nvcounter-val not found in cfg file")
            raise ConfigParsingError
        else:
            self.nvcounter_val = self._parser.getint(self.section_name, 'nvcounter-val')

        if not self._parser.has_option(self.section_name, 'cert-pkg'):
            if self._logger is not None:
                self._logger.error("parameter cert_pkg not found in cfg file")
            raise ConfigParsingError
        else:
            self.cert_pkg = self._parser.get(self.section_name, 'cert-pkg')

        # SB does not support encrypted images in case of "loading only" and "verify only in flash" modes
        if self.aes_ce_id != global_defines.USE_AES_CE_ID_NONE and (
                self.load_verify_scheme == global_defines.VERIFY_IMAGE_IN_FLASH or
                self.load_verify_scheme == global_defines.LOADING_ONLY_IMAGE):
            if self._logger is not None:
                self._logger.error("invalid parameter combination for aes-ce-id and load-verify-scheme")
            raise ConfigParsingError
