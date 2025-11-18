# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import struct

from common_cert_lib.armcertificate import ArmCertificate
from common_cert_lib.armcertificatebody import ArmCertificateBody
from common_cert_lib.armcertificateheader import ArmCertificateHeader
from common_cert_lib.armcertificatesignature import ArmCertificateSignature
from common_utils import global_defines, cryptolayer

import plugin_manager

class KeyArmCertificateHeader(ArmCertificateHeader):
    CERT_TOKEN = 0x53426b63

    def __init__(self, cert_version, hbk_id):
        self._cert_version = (cert_version[0] << 16) + cert_version[1]
        self._hbk_id = hbk_id
        self._signed_content_size = (self.HEADER_SIZE_IN_WORDS
                                     + global_defines.SW_VERSION_OBJ_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.SB_CERT_RSA_KEY_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.NP_SIZE_IN_WORDS
                                     + global_defines.HASH_ALGORITHM_SHA256_SIZE_IN_WORDS)
        self._flags = hbk_id

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

class KeyArmCertificateBody(ArmCertificateBody):

    def __init__(self, sw_version, signer_pubkey_filename,
                 next_cert_pubkey_filename):

        self._sw_version = sw_version
        self._signer_rsa_public_key = cryptolayer.Common.get_n_and_np_from_public_key(signer_pubkey_filename)
        self._hashed_pubkey_next_cert = cryptolayer.Common.get_hashed_n_and_np_from_public_key(next_cert_pubkey_filename)

    @property
    def sw_version(self):
        return self._sw_version

    @property
    def signer_rsa_public_key(self):
        return self._signer_rsa_public_key

    @property
    def hashed_pubkey_next_cert(self):
        return self._hashed_pubkey_next_cert

    def serialize_to_bytes(self):
        data = (self._signer_rsa_public_key
                + struct.pack('<I', self._sw_version)
                + self._hashed_pubkey_next_cert)
        return data

class KeyArmCertificate(ArmCertificate):

    def __init__(self, certificate_config, cert_version):
        self._cert_config = certificate_config
        self._cert_version = cert_version
        self._cert_header = KeyArmCertificateHeader(self._cert_version,
                                                    self._cert_config.hbk_id)

        self._cert_body = KeyArmCertificateBody(self._cert_config.nvcounter_val,
                                                self._cert_config.auth_pubkey,
                                                self._cert_config.next_cert_pubkey)
        self._certificate_data = self._cert_header.serialize_to_bytes() + self._cert_body.serialize_to_bytes()

        plugin = plugin_manager.load_plugin(self._cert_config.signature_plugin_cfg, self._cert_config.signature_plugin_path)
        self._cert_signature = plugin.get_signature(self._cert_config.auth_key, self._certificate_data)

        self._certificate_data += self._cert_signature

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
