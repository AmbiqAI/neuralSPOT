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
COMPILERNAME := Keil6
PROJECT := uart_hci_bridge_Keil6
CONFIG := bin
AM_SoftwareRoot ?= ../../../../../..

SHELL:=/bin/bash
#### Required Executables ####
K := $(shell type -p UV5.exe)
RM := $(shell which rm 2>/dev/null)

ifeq ($(K),)
all clean:
	$(info Tools w/$(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ../../../../../../mcu/apollo330P/hal/mcu/keil6/bin/libam_hal.lib
LIBS+= ../../../../bsp/keil6/bin/libam_bsp.lib
INCS = ../../../../../../third_party/open-amp/apps/ipc_service/backends
INCS+= ../../../../../../third_party/open-amp/lib/include/internal
INCS+= ../../../../../../third_party/open-amp/lib/include
INCS+= ../../../../../../third_party/open-amp/lib/include/openamp
INCS+= ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/apollo330P
INCS+= ../../../../../..
INCS+= ../../../../../../third_party/open-amp/lib/rpmsg
INCS+= ../../../../../../CMSIS/ARM/Include
INCS+= ../../../../../../mcu/apollo330P/hal
INCS+= ../../../../../../devices
INCS+= ../../../../../../mcu/apollo330P
INCS+= ../../../../../../utils
INCS+= ../../../../../../third_party/open-amp/apps/ipc_service/include/ipc
INCS+= ../../../../../../third_party/open-amp
INCS+= ../../../../../../third_party/FreeRTOSv10.5.1/Source/include
INCS+= ../src
INCS+= ../../../../bsp
INCS+= ../../../../../../CMSIS/AmbiqMicro/Include
INCS+= ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../../../../../../third_party/open-amp/apps/ipc_service/backends/ipc_rpmsg_static_vrings.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/device.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/dma.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/init.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/io.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/irq.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/log.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/shmem.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/softirq.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/version.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_condition.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_device.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_init.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_io.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_irq.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_mqueue.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_shmem.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_thread.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/freertos_time.c
SRCS += ../../../../../../third_party/open-amp/lib/virtio/virtio.c
SRCS += ../../../../../../third_party/open-amp/lib/virtio/virtqueue.c
SRCS += ../../../../../../third_party/open-amp/lib/utils/utilities.c
SRCS += ../../../../../../third_party/open-amp/libmetal/lib/system/freertos/apollo330P/sys.c
SRCS += ../src/am_resources.c
SRCS += ../src/ipc_tx_task.c
SRCS += ../src/main.c
SRCS += ../src/rtos.c
SRCS += ../src/uart_hci_bridge.c
SRCS += ../../../../../../third_party/open-amp/apps/ipc_service/ipc_service.c
SRCS += ../../../../../../third_party/open-amp/apps/ipc_service/lib/ipc_rpmsg.c
SRCS += ../../../../../../third_party/open-amp/apps/ipc_service/lib/ipc_static_vrings.c
SRCS += ../../../../../../third_party/open-amp/lib/rpmsg/rpmsg.c
SRCS += ../../../../../../third_party/open-amp/lib/rpmsg/rpmsg_virtio.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/port.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/portasm.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/port_stimer.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/port_systick_stimer.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/event_groups.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/list.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/queue.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/tasks.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/timers.c
SRCS += ../../../../../../utils/am_util_debug.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_faultisr.c
SRCS += ../../../../../../utils/am_util_id.c
SRCS += ../../../../../../utils/am_util_stdio.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang/heap_4.c
SRCS += ../../../../../../devices/am_devices_510L_radio.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	UV5.exe -b -t "uart_hci_bridge" uart_hci_bridge.uvprojx -j0 || [ $$? -eq 1 ]

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning...
	$(RM) -rf $(CONFIG)

endif
.PHONY: all clean directories

