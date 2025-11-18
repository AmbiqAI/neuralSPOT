# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import abc

class ArmCertificateBody(abc.ABC):
    @property
    @abc.abstractmethod
    def signer_rsa_public_key(self):
        pass

    @abc.abstractmethod
    def serialize_to_bytes(self):
        pass
