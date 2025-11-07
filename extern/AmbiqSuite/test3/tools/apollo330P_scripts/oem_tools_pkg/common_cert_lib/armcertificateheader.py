# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import abc

class ArmCertificateHeader(abc.ABC):
    HEADER_SIZE_IN_WORDS = 4

    @property
    @abc.abstractmethod
    def magic_number(self):
        pass

    @property
    @abc.abstractmethod
    def cert_version_number(self):
        pass

    @property
    @abc.abstractmethod
    def signed_content_size(self):
        pass

    @property
    @abc.abstractmethod
    def flags(self):
        pass

    @abc.abstractmethod
    def serialize_to_bytes(self):
        pass
