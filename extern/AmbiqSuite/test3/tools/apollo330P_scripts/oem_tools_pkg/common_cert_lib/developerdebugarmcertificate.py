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

class DeveloperDebugArmCertificateHeader(ArmCertificateHeader):
    CERT_TOKEN = 0x53646465

    def __init__(self, cert_version):
        self._cert_version = (cert_version[0] << 16) + cert_version[1]
        self._signed_content_size = (self.HEADER_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.SB_CERT_RSA_KEY_SIZE_IN_WORDS
                                     + cryptolayer.RsaCrypto.NP_SIZE_IN_WORDS
                                     + 4  # size of DCU mask values
                                     + global_defines.SOC_ID_SIZE_IN_WORDS)

        self._flags = 0

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

class DeveloperDebugArmCertificateBody(ArmCertificateBody):

    def __init__(self, signer_publickey_filename,
                 debug_mask_values, soc_id_filename):
        self._soc_id_filename = soc_id_filename
        self._signer_rsa_public_key = cryptolayer.Common.get_n_and_np_from_public_key(signer_publickey_filename)
        self._debug_mask_values = debug_mask_values
        with open(self._soc_id_filename, "rb") as soc_id_holder_file:
            self._soc_id = soc_id_holder_file.read()
            if len(self._soc_id) != global_defines.SOC_ID_SIZE_IN_BYTES:
                raise ValueError("Invalid SoC_ID size in input file " + self._soc_id_filename)

    @property
    def signer_rsa_public_key(self):
        return self._signer_rsa_public_key

    @property
    def soc_id(self):
        return self._soc_id

    def serialize_to_bytes(self):
        data = (self._signer_rsa_public_key
                + b''.join([struct.pack('<I', i) for i in self._debug_mask_values])
                + self._soc_id)
        return data

class DeveloperDebugArmCertificate(ArmCertificate):

    def __init__(self, certificate_config, cert_version):
        self._cert_config = certificate_config
        self._cert_version = cert_version
        self._cert_header = DeveloperDebugArmCertificateHeader(cert_version)
        self._cert_body = DeveloperDebugArmCertificateBody(self._cert_config.auth_pubkey,
                                                           self._cert_config.debug_masks,
                                                           self._cert_config.soc_id)
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