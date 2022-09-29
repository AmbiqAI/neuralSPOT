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
TARGET := nemagfx_vg_test
COMPILERNAME := iar
PROJECT := nemagfx_vg_test_iar
CONFIG := bin
AM_SoftwareRoot ?= ../../../../..

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

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../src/text_transformation/text_transformation.c
SRCS += ../src/masking_example/masking_example.c
SRCS += ../src/paint_lut_example/paint_lut_example.c
SRCS += ../src/am_resources.c
SRCS += ../src/display.c
SRCS += ../src/nemagfx_vg_test.c
SRCS += ../src/psram.c
SRCS += ../src/tsvg_render_example/Arial_ttf.c
SRCS += ../src/tsvg_render_example/tsvg_render_example.c
SRCS += ../src/render_vg_font/render_vg_font.c
SRCS += ../src/shapes/shapes.c
SRCS += ../src/paint_example/paint_example.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_dc_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_event.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_hal.c
SRCS += ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/nema_utils.c
SRCS += ../src/tsvg_benchmark/tsvg_benchmark.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_faultisr.c
SRCS += ../../../../../../utils/am_util_stdio.c
SRCS += ../../../../../../devices/am_devices_dsi_rm67162.c
SRCS += ../../../../../../devices/am_devices_nemadc_rm67162.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	IarBuild.exe nemagfx_vg_test.ewp -make Debug -log info

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning... ;\
	IarBuild.exe nemagfx_vg_test.ewp -clean Debug -log all

endif
.PHONY: all clean directories

