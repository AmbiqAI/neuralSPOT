# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import configparser
import logging

class ConfigParsingError(Exception):
    """Raised when trying to load a misconfigured CFG file"""
    pass

class EnablerDebugCertificateConfig:
    CFG_SECTION_NAME = "ENABLER-DBG-CFG"
    CC_MNG_CHIP_MANUFACTURE_LCS = 0x0
    CC_MNG_DEVICE_MANUFACTURE_LCS = 0x1
    CC_MNG_SECURE_LCS = 0x5
    CC_MNG_RMA_LCS = 0x7

    def __init__(self, config_filename):
        """
        Parses the ENABLER-DBG-CFG enabler debug certificate cfg file.
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
        self._lcs = None
        self._rma_mode = None
        self._debug_masks = [0, 0, 0, 0]
        self._debug_locks = [0, 0, 0, 0]
        self._hbk_id = None
        self._key_cert_pkg = None
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
    def lcs(self):
        return self._lcs

    @lcs.setter
    def lcs(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter lcs must be an integer")
        elif value not in [self.CC_MNG_CHIP_MANUFACTURE_LCS,
                           self.CC_MNG_DEVICE_MANUFACTURE_LCS,
                           self.CC_MNG_SECURE_LCS,
                           self.CC_MNG_RMA_LCS]:
            raise ValueError("invalid input value for config parameter lcs")
        else:
            self._lcs = value

    @property
    def rma_mode(self):
        return self._rma_mode

    @rma_mode.setter
    def rma_mode(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter rma_mode must be an integer")
        elif value not in [0, 1]:
            raise ValueError("invalid input value for config parameter rma_mode")
        else:
            self._rma_mode = value

    @property
    def debug_masks(self):
        return self._debug_masks

    @debug_masks.setter
    def debug_masks(self, value):
        if isinstance(value, list) is False:
            raise TypeError("Config parameter debug_masks must be a list")
        elif len(value) != 4 or isinstance(value[0], int) is False or isinstance(value[1], int) is False or \
                isinstance(value[2], int) is False or isinstance(value[3], int) is False:
            raise TypeError("Config parameter debug_masks must be a list of 4 integers")
        else:
            for item in value:
                if not 0 <= item <= 0xFFFFFFFF:
                    raise ValueError("invalid input value for config parameter debug_masks")
            self._debug_masks = value

    @property
    def debug_locks(self):
        return self._debug_locks

    @debug_locks.setter
    def debug_locks(self, value):
        if isinstance(value, list) is False:
            raise TypeError("Config parameter debug_locks must be a list")
        elif len(value) != 4 or isinstance(value[0], int) is False or isinstance(value[1], int) is False or \
                isinstance(value[2], int) is False or isinstance(value[3], int) is False:
            raise TypeError("Config parameter debug_locks must be a list of 4 integers")
        else:
            for item in value:
                if not 0 <= item <= 0xFFFFFFFF:
                    raise ValueError("invalid input value for config parameter debug_locks")
            self._debug_locks = value

    @property
    def hbk_id(self):
        return self._hbk_id

    @hbk_id.setter
    def hbk_id(self, value):
        if isinstance(value, int) is False:
            raise TypeError("Config parameter hbk_id must be an integer")
        elif value not in [0, 1, 2, 0xF]:
            raise ValueError("invalid input value for config parameter hbk_id")
        else:
            self._hbk_id = value

    @property
    def key_cert_pkg(self):
        return self._key_cert_pkg

    @key_cert_pkg.setter
    def key_cert_pkg(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter key_cert_pkg must be a string")
        else:
            self._key_cert_pkg = value

    @property
    def next_cert_pubkey(self):
        return self._next_cert_pubkey

    @next_cert_pubkey.setter
    def next_cert_pubkey(self, value):
        if isinstance(value, str) is False:
            raise TypeError("Config parameter next_cert_pubkey must be a string")
        elif value == "":
            raise ValueError("Config parameter next_cert_pubkey cannot be an empty string!")
        else:
            self._next_cert_pubkey = value

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

        is_debug_exist = 0
        if (self._parser.has_option(self.section_name, 'debug-mask[0-31]')
                and self._parser.has_option(self.section_name, 'debug-mask[32-63]')
                and self._parser.has_option(self.section_name, 'debug-mask[64-95]')
                and self._parser.has_option(self.section_name, 'debug-mask[96-127]')
                and self._parser.has_option(self.section_name, 'debug-lock[0-31]')
                and self._parser.has_option(self.section_name, 'debug-lock[32-63]')
                and self._parser.has_option(self.section_name, 'debug-lock[64-95]')
                and self._parser.has_option(self.section_name, 'debug-lock[96-127]')):
            debug_masks = [int(self._parser.get(self.section_name, 'debug-mask[0-31]'), 16),
                           int(self._parser.get(self.section_name, 'debug-mask[32-63]'), 16),
                           int(self._parser.get(self.section_name, 'debug-mask[64-95]'), 16),
                           int(self._parser.get(self.section_name, 'debug-mask[96-127]'), 16)]
            self.debug_masks = debug_masks

            debug_locks = [int(self._parser.get(self.section_name, 'debug-lock[0-31]'), 16),
                           int(self._parser.get(self.section_name, 'debug-lock[32-63]'), 16),
                           int(self._parser.get(self.section_name, 'debug-lock[64-95]'), 16),
                           int(self._parser.get(self.section_name, 'debug-lock[96-127]'), 16)]
            self.debug_locks = debug_locks
            is_debug_exist = 1

        if self._parser.has_option(self.section_name, 'rma-mode'):
            self.rma_mode = self._parser.getint(self.section_name, 'rma-mode')
        else:
            self.rma_mode = 0

        is_hbk_exist = self._parser.has_option(self.section_name, 'hbk-id')
        if is_hbk_exist:
            self.hbk_id = self._parser.getint(self.section_name, 'hbk-id')
        else:
            self.hbk_id = 0xF  # don't care value for mandatory certificate header flag

        is_key_pkg_exist = 0
        if self._parser.has_option(self.section_name, 'key-cert-pkg'):
            self.key_cert_pkg = self._parser.get(self.section_name, 'key-cert-pkg')
            is_key_pkg_exist = 1

        if self.rma_mode == 1 and is_debug_exist == 1:
            message = "Both RMA and debug mode are defined"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        if self.rma_mode == 0 and is_debug_exist == 0:
            message = "Neither RMA nor Debug mode is defined"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        if is_key_pkg_exist == 0 and not is_hbk_exist:
            message = "Neither hbk-id nor key-cert-pkg is defined"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        if not is_hbk_exist and self.rma_mode == 1:
            message = "hbk-id must exist if RMA mode is defined"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)

        if not self._parser.has_option(self.section_name, 'lcs'):
            message = "no lcs parameter found"
            if self._logger is not None:
                self._logger.error(message)
            raise ConfigParsingError(message)
        else:
            self.lcs = self._parser.getint(self.section_name, 'lcs')

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
