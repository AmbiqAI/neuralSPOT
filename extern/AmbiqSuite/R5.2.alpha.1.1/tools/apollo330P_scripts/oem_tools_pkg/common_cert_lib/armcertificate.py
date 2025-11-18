# Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause OR Arm’s non-OSI source license
#

import abc

class ArmCertificate(abc.ABC):
    @property
    @abc.abstractmethod
    def header(self):
        pass

    @property
    @abc.abstractmethod
    def body(self):
        pass

    @property
    @abc.abstractmethod
    def signature(self):
        pass

    @property
    @abc.abstractmethod
    def certificate_data(self):
        pass
