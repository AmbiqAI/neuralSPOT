#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2024, Ambiq Micro, Inc.
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
# This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
#
#******************************************************************************
TARGET := libam_hal
COMPILERNAME := iar
PROJECT := libam_hal_iar
CONFIG := bin
AM_SoftwareRoot ?= ../../../..

SHELL:=/bin/bash
#### Required Executables ####
K := $(shell type -p IarBuild.exe)
RM = $(shell which rm 2>/dev/null)

ifeq ($(K),)
all clean:
	$(info Tools w/$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ${libraries}
INCS = ${incs}

all: directories $(CONFIG)/$(TARGET).a

# Source Dependencies must be defined before they are used.
SRCS = ../../../../../mcu/apollo4l/hal/am_hal_access.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_adc.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_dcu.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_global.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_gpio.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_i2s.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_otp.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_pdm.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_pin.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_pwrctrl.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_queue.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_security.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_stimer.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_timer.c
SRCS += ../../../../../mcu/apollo4l/hal/am_hal_wdt.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_bootrom_helper.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_cachectrl.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_card.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_card_host.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_clkgen.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_cmdq.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_fault.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_interrupt.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_iom.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_ios.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_itm.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_mcuctrl.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_mpu.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_mram.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_mspi.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_reset.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_rtc.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_sdhc.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_secure_ota.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_sysctrl.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_systick.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_tpiu.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_uart.c
SRCS += ../../../../../mcu/apollo4l/hal/mcu/am_hal_utils.c

$(CONFIG)/$(TARGET).a: $(LIBS) $(INCS) $(SRCS)
	IarBuild.exe libam_hal.ewp -make Debug -log info

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

# BSP's need this.

clean:
	@echo Cleaning... ;\
	IarBuild.exe libam_hal.ewp -clean Debug -log all

endif
.PHONY: all clean directories

