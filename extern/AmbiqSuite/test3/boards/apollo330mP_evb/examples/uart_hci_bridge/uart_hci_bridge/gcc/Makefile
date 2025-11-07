#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2025, Ambiq Micro, Inc.
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
# This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
#
#******************************************************************************
TARGET := uart_hci_bridge
COMPILERNAME := gcc
PROJECT := uart_hci_bridge_gcc
CONFIG := bin
SHELL:=/bin/bash

# Enable printing explicit commands with 'make VERBOSE=1'
ifneq ($(VERBOSE),1)
Q:=@
endif

#### Setup ####

TOOLCHAIN ?= arm-none-eabi
PART = apollo330P
CPU = cortex-m55
FPU = auto
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
DEFINES+= -DAM_DEBUG_PRINTF
DEFINES+= -DAM_FREERTOS
DEFINES+= -DAM_PACKAGE_BGA
DEFINES+= -DAM_PART_APOLLO330P
DEFINES+= -DAM_UTIL_FAULTISR_PRINT
DEFINES+= -Dapollo330mP_evb
DEFINES+= -Dgcc

INCLUDES = -I../../../../../..
INCLUDES+= -I../../../../../../CMSIS/ARM/Include
INCLUDES+= -I../../../../../../CMSIS/AmbiqMicro/Include
INCLUDES+= -I../../../../../../devices
INCLUDES+= -I../../../../../../mcu/apollo330P
INCLUDES+= -I../../../../../../mcu/apollo330P/hal
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.5.1/Source/include
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5
INCLUDES+= -I../../../../../../third_party/open-amp
INCLUDES+= -I../../../../../../third_party/open-amp/apps/ipc_service/backends
INCLUDES+= -I../../../../../../third_party/open-amp/apps/ipc_service/include/ipc
INCLUDES+= -I../../../../../../third_party/open-amp/lib/include
INCLUDES+= -I../../../../../../third_party/open-amp/lib/include/internal
INCLUDES+= -I../../../../../../third_party/open-amp/lib/include/openamp
INCLUDES+= -I../../../../../../third_party/open-amp/lib/rpmsg
INCLUDES+= -I../../../../../../third_party/open-amp/libmetal/lib/system/freertos/apollo330P
INCLUDES+= -I../../../../../../utils
INCLUDES+= -I../../../../bsp
INCLUDES+= -I../src

VPATH = ../../../../../../CMSIS/AmbiqMicro/Source
VPATH+=:../../../../../../devices
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang
VPATH+=:../../../../../../third_party/open-amp/apps/ipc_service
VPATH+=:../../../../../../third_party/open-amp/apps/ipc_service/backends
VPATH+=:../../../../../../third_party/open-amp/apps/ipc_service/lib
VPATH+=:../../../../../../third_party/open-amp/lib/rpmsg
VPATH+=:../../../../../../third_party/open-amp/lib/utils
VPATH+=:../../../../../../third_party/open-amp/lib/virtio
VPATH+=:../../../../../../third_party/open-amp/libmetal/lib
VPATH+=:../../../../../../third_party/open-amp/libmetal/lib/system/freertos
VPATH+=:../../../../../../third_party/open-amp/libmetal/lib/system/freertos/apollo330P
VPATH+=:../../../../../../utils
VPATH+=:../src

SRC = am_devices_510L_radio.c
SRC += am_resources.c
SRC += am_util_debug.c
SRC += am_util_delay.c
SRC += am_util_faultisr.c
SRC += am_util_id.c
SRC += am_util_stdio.c
SRC += device.c
SRC += dma.c
SRC += event_groups.c
SRC += freertos_condition.c
SRC += freertos_device.c
SRC += freertos_init.c
SRC += freertos_io.c
SRC += freertos_irq.c
SRC += freertos_mqueue.c
SRC += freertos_shmem.c
SRC += freertos_thread.c
SRC += freertos_time.c
SRC += heap_4.c
SRC += init.c
SRC += io.c
SRC += ipc_rpmsg.c
SRC += ipc_rpmsg_static_vrings.c
SRC += ipc_service.c
SRC += ipc_static_vrings.c
SRC += ipc_tx_task.c
SRC += irq.c
SRC += list.c
SRC += log.c
SRC += main.c
SRC += port.c
SRC += port_stimer.c
SRC += port_systick_stimer.c
SRC += portasm.c
SRC += queue.c
SRC += rpmsg.c
SRC += rpmsg_virtio.c
SRC += rtos.c
SRC += shmem.c
SRC += softirq.c
SRC += sys.c
SRC += tasks.c
SRC += timers.c
SRC += uart_hci_bridge.c
SRC += utilities.c
SRC += version.c
SRC += virtio.c
SRC += virtqueue.c
SRC += startup_gcc.c
SRC += system_apollo330P.c

CSRC = $(filter %.c,$(SRC))
ASRC = $(filter %.s,$(SRC))
CPPSRC = $(filter %.cpp,$(SRC))

OBJS = $(CSRC:%.c=$(CONFIG)/%.o)
OBJS+= $(ASRC:%.s=$(CONFIG)/%.o)
OBJS+= $(CPPSRC:%.cpp=$(CONFIG)/%.o)

DEPS = $(CSRC:%.c=$(CONFIG)/%.d)
DEPS+= $(ASRC:%.s=$(CONFIG)/%.d)
DEPS+= $(CPPSRC:%.cpp=$(CONFIG)/%.d)

LIBS = ../../../../../../CMSIS/ARM/Lib/ARM/DSP_LIB_CM55/libarm_cortexM55f_math.a
LIBS += ../../../../../../mcu/apollo330P/hal/mcu/gcc/bin/libam_hal.a
LIBS += ../../../../bsp/gcc/bin/libam_bsp.a

# Compiler cflags
CFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS+= -ffunction-sections -fdata-sections -fomit-frame-pointer
CFLAGS+= -MMD -MP -std=c99 -Wall -g
CFLAGS+= -O3
CFLAGS+= $(DEFINES)
CFLAGS+= $(INCLUDES)
CFLAGS+= -pedantic-errors
CFLAGS+= 

# Compiler cxxflags
CXXFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CXXFLAGS+= -ffunction-sections -fdata-sections -fomit-frame-pointer
CXXFLAGS+= -MMD -MP -std=c++11 -fno-use-cxa-atexit -Wall -g
CXXFLAGS+= -O3
CXXFLAGS+= $(DEFINES)
CXXFLAGS+= $(INCLUDES)
CXXFLAGS+= 

LFLAGS = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
LFLAGS+= -nostartfiles -static
LFLAGS+= -Wl,--gc-sections,--entry,Reset_Handler,-Map,$(CONFIG)/$(TARGET).map
LFLAGS+= -Wl,--start-group -lm -lc -lgcc         -lstdc++ $(LIBS) -Wl,--end-group
LFLAGS+= -Wl,--no-warn-rwx-segments
LFLAGS+= 

# Additional user specified CFLAGS
CFLAGS+=$(EXTRA_CFLAGS)

CPFLAGS = -Obinary

ODFLAGS = -S

#### Rules ####
all: directories $(CONFIG)/$(TARGET).bin

ifeq ($(OS),Windows_NT)
    JLINK := JLink.exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        JLINK := JLinkExe
    else ifeq ($(UNAME_S),Darwin)
        JLINK := JLinkExe
	else
        $(error OS not support)
    endif
endif

.PHONY: flash
flash: all
	$(JLINK) -commanderscript flash.jlink

.PHONY: ozone
ozone: all
	ozone oz.jdebug &

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

$(CONFIG)/%.o: %.c $(CONFIG)/%.d
	@echo " Compiling $(COMPILERNAME) $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(CONFIG)/%.o: %.cpp $(CONFIG)/%.d
	@echo " Compiling $(COMPILERNAME) $<"
	$(Q) $(CC) -c $(CXXFLAGS) $< -o $@

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
	@echo "Cleaning..."
	$(Q) $(RM) -rf $(CONFIG)
$(CONFIG)/%.d: ;

../../../../../../mcu/apollo330P/hal/mcu/gcc/bin/libam_hal.a:
	$(MAKE) -C ../../../../../../mcu/apollo330P/hal/mcu

../../../../bsp/gcc/bin/libam_bsp.a:
	$(MAKE) -C ../../../../bsp

# Automatically include any generated dependencies
-include $(DEPS)
endif
.PHONY: all clean directories

