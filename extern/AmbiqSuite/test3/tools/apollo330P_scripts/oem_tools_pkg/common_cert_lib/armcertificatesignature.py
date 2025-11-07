# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

from common_utils import cryptolayer

class ArmCertificateSignature:

    def __init__(self, cert_data_to_sign, signer_keypair_filename, signer_keypair_passphrase_filename):
        self._signature = cryptolayer.Common.rsa_sign(cert_data_to_sign,
                                                      signer_keypair_filename,
                                                      signer_keypair_passphrase_filename)
        self._signature = self._signature[::-1]  # reversing the signature for compatibility reasons

    def serialize_to_bytes(self):
        return self._signature
