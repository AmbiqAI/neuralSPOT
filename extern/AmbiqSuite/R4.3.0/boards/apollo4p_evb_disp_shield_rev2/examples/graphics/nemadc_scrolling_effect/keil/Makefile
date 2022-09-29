#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2022, Ambiq Micro, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# Third party software included in this distribution is subject to the
# additional license terms as defined in the /docs/licenses directory.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
#
#******************************************************************************
TARGET := nemadc_scrolling_effect
COMPILERNAME := Keil
PROJECT := nemadc_scrolling_effect_Keil
CONFIG := bin
AM_SoftwareRoot ?= ../../../../../..

SHELL:=/bin/bash
#### Required Executables ####
K := $(shell type -p UV4.exe)
RM := $(shell which rm 2>/dev/null)

ifeq ($(K),)
all clean:
	$(info Tools w/$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/keil/bin/lib_nema_apollo4p_nemagfx.lib
LIBS+= ../../../../../../mcu/apollo4p/hal/mcu/keil/bin/libam_hal.lib
LIBS+= ../../../../bsp/keil/bin/libam_bsp.lib
INCS = ../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaGFX
INCS+= ../../../../bsp
INCS+= ../../../../../../utils
INCS+= ../../../../../../CMSIS/ARM/Include
INCS+= ../../../../../../devices
INCS+= ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaDC
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaDC
INCS+= ../../../../../../mcu/apollo4p
INCS+= ../../../../../../mcu/apollo4p/hal
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/common
INCS+= ../src
INCS+= ../../../../../..
INCS+= ../../../../../../CMSIS/AmbiqMicro/Include
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaGFX
INCS+= ../../../../../../third_party/ThinkSi/NemaGFX_SDK/common/mem

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_dc_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_event.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_utils.c
SRCS += ../src/am_resources.c
SRCS += ../src/main.c
SRCS += ../src/nemadc_scrolling_effect.c
SRCS += ../../../../../../devices/am_devices_dsi_rm67162.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_stdio.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	UV4.exe -b -t "nemadc_scrolling_effect" nemadc_scrolling_effect.uvprojx -j0 || [ $$? -eq 1 ]

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning... ;\
	$(RM) -rf $(CONFIG)

endif
.PHONY: all clean directories

