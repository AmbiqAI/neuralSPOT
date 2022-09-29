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
TARGET := nemagfx_vg_svg
COMPILERNAME := gcc
PROJECT := nemagfx_vg_svg_gcc
CONFIG := bin

SHELL:=/bin/bash

# Enable printing explicit commands with 'make VERBOSE=1'
ifneq ($(VERBOSE),1)
Q:=@
endif

#### Setup ####

TOOLCHAIN ?= arm-none-eabi
PART = apollo4p
CPU = cortex-m4
FPU = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI = softfp
FABI = hard

LINKER_FILE := ./linker_script.ld
STARTUP_FILE := ./startup_$(COMPILERNAME).c

#### Required Executables ####
CC = $(TOOLCHAIN)-gcc
GCC = $(TOOLCHAIN)-gcc
CPP = $(TOOLCHAIN)-cpp
LD = $(TOOLCHAIN)-ld
CP = $(TOOLCHAIN)-objcopy
OD = $(TOOLCHAIN)-objdump
RD = $(TOOLCHAIN)-readelf
AR = $(TOOLCHAIN)-ar
SIZE = $(TOOLCHAIN)-size
RM = $(shell which rm 2>/dev/null)

EXECUTABLES = CC LD CP OD AR RD SIZE GCC
K := $(foreach exec,$(EXECUTABLES),\
        $(if $(shell which $($(exec)) 2>/dev/null),,\
        $(info $(exec) not found on PATH ($($(exec))).)$(exec)))
$(if $(strip $(value K)),$(info Required Program(s) $(strip $(value K)) not found))

ifneq ($(strip $(value K)),)
all clean:
	$(info Tools $(TOOLCHAIN)-$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

DEFINES = -DPART_$(PART)
DEFINES+= -DUSE_TSI_MALLOC
DEFINES+= -DRUN_IN_HP_MODE
DEFINES+= -DAM_UTIL_FAULTISR_PRINT
DEFINES+= -DENABLE_TE
DEFINES+= -DLD_LIBRARY_PATH=../../../../../third_party/ThinkSi/NemaGFX_SDK/lib
DEFINES+= -DNEMAGFX_SDK_PATH=../../../../../third_party/ThinkSi/NemaGFX_SDK
DEFINES+= -DAM_PART_APOLLO4P
DEFINES+= -DAM_PACKAGE_BGA
DEFINES+= -DAM_FREERTOS
DEFINES+= -DPLATFORM=apollo4p_nemagfx
DEFINES+= -DVMEM_SIZE=0x180000
DEFINES+= -Dapollo4p_evb_disp_shield_rev2
DEFINES+= -DVMEM_BASEADDR=0x10060400
DEFINES+= -DAPS25616N_OCTAL_MODE_EN
DEFINES+= -Dgcc

INCLUDES = -I../../../../../../utils
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaDC
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/common/mem
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4
INCLUDES+= -I../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.1.1/Source/include
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/common
INCLUDES+= -I../../../../../../mcu/apollo4p
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/NemaGFX
INCLUDES+= -I../../../../../../CMSIS/AmbiqMicro/Include
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaVG
INCLUDES+= -I../../../../../../devices
INCLUDES+= -I../../../../../..
INCLUDES+= -I../../../../../../CMSIS/ARM/Include
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaGFX
INCLUDES+= -I../../../../bsp
INCLUDES+= -I../src
INCLUDES+= -I../../../../../../mcu/apollo4p/hal
INCLUDES+= -I../../../../../../third_party/ThinkSi/NemaGFX_SDK/include/tsi/NemaDC

VPATH = ../../../../../../third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo4
VPATH+=:../../../../../../devices
VPATH+=:../../../../../../utils
VPATH+=:../../../../../../third_party/FreeRTOSv10.1.1/Source
VPATH+=:../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx
VPATH+=:../../../../../../third_party/FreeRTOSv10.1.1/Source/portable/MemMang
VPATH+=:../src

SRC = port.c
SRC += nema_dc_hal.c
SRC += nema_event.c
SRC += nema_hal.c
SRC += nema_utils.c
SRC += am_resources.c
SRC += display_task.c
SRC += graphics_power_control.c
SRC += gui_task.c
SRC += nemagfx_vg_svg.c
SRC += psram.c
SRC += render_task.c
SRC += rtos.c
SRC += am_devices_dsi_rm67162.c
SRC += am_devices_nemadc_rm67162.c
SRC += am_util_delay.c
SRC += am_util_faultisr.c
SRC += am_util_stdio.c
SRC += event_groups.c
SRC += list.c
SRC += queue.c
SRC += tasks.c
SRC += timers.c
SRC += heap_4.c
SRC += startup_gcc.c

CSRC = $(filter %.c,$(SRC))
ASRC = $(filter %.s,$(SRC))

OBJS = $(CSRC:%.c=$(CONFIG)/%.o)
OBJS+= $(ASRC:%.s=$(CONFIG)/%.o)

DEPS = $(CSRC:%.c=$(CONFIG)/%.d)
DEPS+= $(ASRC:%.s=$(CONFIG)/%.d)

LIBS = ../../../../../../third_party/ThinkSi/config/apollo4p_nemagfx/gcc/bin/lib_nema_apollo4p_nemagfx.a
LIBS += ../../../../bsp/gcc/bin/libam_bsp.a
LIBS += ../../../../../../mcu/apollo4p/hal/mcu/gcc/bin/libam_hal.a

CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS+= -ffunction-sections -fdata-sections -fomit-frame-pointer
CFLAGS+= -MMD -MP -std=c99 -Wall -g
CFLAGS+= -O0
CFLAGS+= $(DEFINES)
CFLAGS+= $(INCLUDES)
CFLAGS+= 

LFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS+= -nostartfiles -static
LFLAGS+= -Wl,--gc-sections,--entry,Reset_Handler,-Map,$(CONFIG)/$(TARGET).map
LFLAGS+= -Wl,--start-group -lm -lc -lgcc -lnosys $(LIBS) -Wl,--end-group
LFLAGS+= 

# Additional user specified CFLAGS
CFLAGS+=$(EXTRA_CFLAGS)

CPFLAGS = -Obinary

ODFLAGS = -S

#### Rules ####
all: directories $(CONFIG)/$(TARGET).bin

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

$(CONFIG)/%.o: %.c $(CONFIG)/%.d
	@echo " Compiling $(COMPILERNAME) $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/%.o: %.s $(CONFIG)/%.d
	@echo " Assembling $(COMPILERNAME) $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/$(TARGET).axf: $(OBJS) $(LIBS)
	@echo " Linking $(COMPILERNAME) $@"
	$(Q) $(CC) -Wl,-T,$(LINKER_FILE) -o $@ $(OBJS) $(LFLAGS)

$(CONFIG)/$(TARGET).bin: $(CONFIG)/$(TARGET).axf
	@echo " Copying $(COMPILERNAME) $@..."
	$(Q) $(CP) $(CPFLAGS) $< $@
	$(Q) $(OD) $(ODFLAGS) $< > $(CONFIG)/$(TARGET).lst
	$(Q) $(SIZE) $(OBJS) $(LIBS) $(CONFIG)/$(TARGET).axf >$(CONFIG)/$(TARGET).size

clean:
	@echo "Cleaning..." ;
	$(Q) $(RM) -rf $(CONFIG)
$(CONFIG)/%.d: ;

../../../../bsp/gcc/bin/libam_bsp.a:
	$(MAKE) -C ../../../../bsp

../../../../../../mcu/apollo4p/hal/mcu/gcc/bin/libam_hal.a:
	$(MAKE) -C ../../../../../../mcu/apollo4p/hal/mcu

# Automatically include any generated dependencies
-include $(DEPS)
endif
.PHONY: all clean directories

