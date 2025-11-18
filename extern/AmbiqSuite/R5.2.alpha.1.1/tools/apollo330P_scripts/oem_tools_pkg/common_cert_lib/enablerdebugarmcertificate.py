# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import struct

from common_cert_lib.armcertificate import ArmCertificate
from common_cert_lib.armcertificatebody import ArmCertificateBody
from common_cert_lib.armcertificateheader import ArmCertificateHeader
from common_cert_lib.armcertificatesignature import ArmCertificateSignature
from common_utils import cryptolayer, global_defines

import plugin_manager

class EnablerDebugArmCertificateHeader(ArmCertificateHeader):
    HBK_ID_FLAG_BIT_OFFSET = 0
    LCS_ID_FLAG_BIT_OFFSET = 4
    RMA_CERT_FLAG_BIT_OFFSET = 8
    CERT_TOKEN = 0x5364656E

    def __init__(self, cert_version, rma_mode, hkb_id, lcs):
        self._cert_version = (cert_version[0] << 16) + cert_version[1]
        self._signed_content_size = (self.HEADER_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.SB_CERT_RSA_KEY_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.NP_SIZE_IN_WORDS
                                     + 8  # size of DCU lock & mask values
                                     + global_defines.HASH_ALGORITHM_SHA256_SIZE_IN_WORDS)

        self._flags = (hkb_id << self.HBK_ID_FLAG_BIT_OFFSET
                       | (lcs << self.LCS_ID_FLAG_BIT_OFFSET)
                       | (rma_mode << self.RMA_CERT_FLAG_BIT_OFFSET))

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

class EnablerDebugArmCertificateBody(ArmCertificateBody):

    def __init__(self, signer_pubkey_filename,
                 debug_mask_values, debug_lock_values, next_cert_pubkey_filename):
        self._signer_rsa_public_key = cryptolayer.Common.get_n_and_np_from_public_key(signer_pubkey_filename)
        self._debug_mask_values = debug_mask_values
        self._debug_lock_values = debug_lock_values
        self._hashed_pubkey_next_cert = cryptolayer.Common.get_hashed_n_and_np_from_public_key(next_cert_pubkey_filename)

    @property
    def signer_rsa_public_key(self):
        return self._signer_rsa_public_key

    @property
    def hashed_pubkey_next_cert(self):
        return self._hashed_pubkey_next_cert

    def serialize_to_bytes(self):
        data = (self._signer_rsa_public_key
                + b''.join([struct.pack('<I', i) for i in self._debug_mask_values])
                + b''.join([struct.pack('<I', i) for i in self._debug_lock_values])
                + self._hashed_pubkey_next_cert)
        return data

class EnablerDebugArmCertificate(ArmCertificate):

    def __init__(self, certificate_config, cert_version):
        self._cert_config = certificate_config
        self._cert_version = cert_version
        self._cert_header = EnablerDebugArmCertificateHeader(cert_version,
                                                             self._cert_config.rma_mode,
                                                             self._cert_config.hbk_id,
                                                             self._cert_config.lcs)
        self._cert_body = EnablerDebugArmCertificateBody(self._cert_config.auth_pubkey,
                                                         self._cert_config.debug_masks,
                                                         self._cert_config.debug_locks,
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
