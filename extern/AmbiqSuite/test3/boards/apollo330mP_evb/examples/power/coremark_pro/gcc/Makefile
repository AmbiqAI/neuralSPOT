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
TARGET := coremark_pro
COMPILERNAME := gcc
PROJECT := coremark_pro_gcc
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
DEFINES+= -DAM_FREERTOS
DEFINES+= -DAM_PACKAGE_BGA
DEFINES+= -DAM_PART_APOLLO330P
DEFINES+= -DAM_UTIL_FAULTISR_PRINT
DEFINES+= -DBMDEBUG=0
DEFINES+= -DDEBUG_ACCURATE_BITS=0
DEFINES+= -DENABLE_CPU_HP_250
DEFINES+= -DENABLE_PMU
DEFINES+= -DEZXML_NOMMAP=1
DEFINES+= -DHEAP_IN_PSRAM
DEFINES+= -DMITH_MEMORY_ONLY_VERSION=1
DEFINES+= -DPSRAM_APS25616N
DEFINES+= -DREPORT_THMALLOC_STATS=0
DEFINES+= -DRUN_IN_SINGLE_TASK=1
DEFINES+= -DRUN_PSRAM_TIMING_SCAN
DEFINES+= -DSELECT_PRESET_ID=1
DEFINES+= -DTHDEBUG=0
DEFINES+= -DUSE_CRC_TABLE=1
DEFINES+= -DUSE_PRESET=1
DEFINES+= -Dapollo330mP_evb
DEFINES+= -Dgcc

INCLUDES = -I../../../../../..
INCLUDES+= -I../../../../../../CMSIS/ARM/Include
INCLUDES+= -I../../../../../../CMSIS/AmbiqMicro/Include
INCLUDES+= -I../../../../../../devices
INCLUDES+= -I../../../../../../mcu/apollo330P
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.5.1/Source/include
INCLUDES+= -I../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5
INCLUDES+= -I../../../../../../utils
INCLUDES+= -I../../../../bsp
INCLUDES+= -I../src
INCLUDES+= -I../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/include
INCLUDES+= -I../src/FreeRTOS_Plus_POSIX/include
INCLUDES+= -I../src/FreeRTOS_Plus_POSIX/include/FreeRTOS_POSIX
INCLUDES+= -I../src/FreeRTOS_Plus_POSIX/include/private
INCLUDES+= -I../src/benchmarks/consumer_v2/cjpeg
INCLUDES+= -I../src/benchmarks/consumer_v2/cjpeg/data
INCLUDES+= -I../src/benchmarks/core
INCLUDES+= -I../src/benchmarks/darkmark/parser
INCLUDES+= -I../src/benchmarks/darkmark/sha
INCLUDES+= -I../src/benchmarks/darkmark/zip/zlib-1.2.8
INCLUDES+= -I../src/benchmarks/fp/fft_radix2
INCLUDES+= -I../src/benchmarks/fp/linpack
INCLUDES+= -I../src/benchmarks/fp/loops
INCLUDES+= -I../src/benchmarks/fp/nnet
INCLUDES+= -I../src/mith/al/include
INCLUDES+= -I../src/mith/include

VPATH = ../../../../../../CMSIS/AmbiqMicro/Source
VPATH+=:../../../../../../devices
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo5
VPATH+=:../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang
VPATH+=:../../../../../../utils
VPATH+=:../src
VPATH+=:../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source
VPATH+=:../src/benchmarks/consumer_v2/cjpeg
VPATH+=:../src/benchmarks/consumer_v2/cjpeg/data
VPATH+=:../src/benchmarks/core
VPATH+=:../src/benchmarks/darkmark/parser
VPATH+=:../src/benchmarks/darkmark/sha
VPATH+=:../src/benchmarks/darkmark/zip
VPATH+=:../src/benchmarks/darkmark/zip/zlib-1.2.8
VPATH+=:../src/benchmarks/fp/fft_radix2
VPATH+=:../src/benchmarks/fp/fft_radix2/ref-sp
VPATH+=:../src/benchmarks/fp/linpack
VPATH+=:../src/benchmarks/fp/linpack/ref
VPATH+=:../src/benchmarks/fp/loops
VPATH+=:../src/benchmarks/fp/loops/ref-sp
VPATH+=:../src/benchmarks/fp/nnet
VPATH+=:../src/benchmarks/fp/nnet/ref-sp
VPATH+=:../src/mith/al/src
VPATH+=:../src/mith/src
VPATH+=:../src/workloads/cjpeg-rose7-preset
VPATH+=:../src/workloads/core
VPATH+=:../src/workloads/linear_alg-mid-100x100-sp
VPATH+=:../src/workloads/loops-all-mid-10k-sp
VPATH+=:../src/workloads/nnet_test
VPATH+=:../src/workloads/parser-125k
VPATH+=:../src/workloads/radix2-big-64k
VPATH+=:../src/workloads/sha-test
VPATH+=:../src/workloads/zip-test

SRC = 100.c
SRC += 100k.c
SRC += 100kdot.c
SRC += 10k.c
SRC += 10kdot.c
SRC += 1k.c
SRC += 1kdot.c
SRC += 1letter.c
SRC += 2K.c
SRC += 32.c
SRC += 32K.c
SRC += 4K.c
SRC += FreeRTOS_POSIX_clock.c
SRC += FreeRTOS_POSIX_mqueue.c
SRC += FreeRTOS_POSIX_pthread.c
SRC += FreeRTOS_POSIX_pthread_barrier.c
SRC += FreeRTOS_POSIX_pthread_cond.c
SRC += FreeRTOS_POSIX_pthread_mutex.c
SRC += FreeRTOS_POSIX_sched.c
SRC += FreeRTOS_POSIX_semaphore.c
SRC += FreeRTOS_POSIX_timer.c
SRC += FreeRTOS_POSIX_unistd.c
SRC += FreeRTOS_POSIX_utils.c
SRC += Rose256_bmp.c
SRC += adler32.c
SRC += al_file.c
SRC += al_single.c
SRC += al_smp.c
SRC += am_resources.c
SRC += am_time.c
SRC += am_util_delay.c
SRC += am_util_pmu.c
SRC += am_util_stdio.c
SRC += bm_lib.c
SRC += bmark_lite.c
SRC += cdjpeg.c
SRC += cjpeg-rose7-preset.c
SRC += cjpeg.c
SRC += compress.c
SRC += core.c
SRC += core_list_join.c
SRC += core_matrix.c
SRC += core_mith.c
SRC += core_portme.c
SRC += core_state.c
SRC += core_util.c
SRC += coremark_pro.c
SRC += crc32.c
SRC += data3_big.c
SRC += data4_mid.c
SRC += data5_small.c
SRC += deflate.c
SRC += event_groups.c
SRC += ezxml.c
SRC += fft_radix2.c
SRC += filedata.c
SRC += goose_bmp.c
SRC += gzclose.c
SRC += gzlib.c
SRC += heap_4.c
SRC += heap_4_external.c
SRC += heap_5_external.c
SRC += infback.c
SRC += inffast.c
SRC += inflate.c
SRC += inftrees.c
SRC += inputs_f32.c
SRC += jcapimin.c
SRC += jcapistd.c
SRC += jccoefct.c
SRC += jccolor.c
SRC += jcdctmgr.c
SRC += jchuff.c
SRC += jcinit.c
SRC += jcmainct.c
SRC += jcmarker.c
SRC += jcmaster.c
SRC += jcomapi.c
SRC += jcparam.c
SRC += jcprepct.c
SRC += jcsample.c
SRC += jdatadst.c
SRC += jerror.c
SRC += jfdctint.c
SRC += jmemansi.c
SRC += jmemmgr.c
SRC += jutils.c
SRC += letters.c
SRC += linear_alg-mid-100x100-sp.c
SRC += linpack.c
SRC += list.c
SRC += loops-all-mid-10k-sp.c
SRC += loops.c
SRC += md5.c
SRC += mith_lib.c
SRC += mith_workload.c
SRC += mpu_config.c
SRC += mspi_psram_init.c
SRC += nnet.c
SRC += nnet_test.c
SRC += parser-125k.c
SRC += parser.c
SRC += port.c
SRC += port_stimer.c
SRC += port_systick_stimer.c
SRC += portasm.c
SRC += queue.c
SRC += radix2-big-64k.c
SRC += rdbmp.c
SRC += rtos.c
SRC += sha-test.c
SRC += sha256.c
SRC += shabench.c
SRC += tasks.c
SRC += th_al.c
SRC += th_bignum.c
SRC += th_encode.c
SRC += th_getopt.c
SRC += th_lib.c
SRC += th_math.c
SRC += th_rand.c
SRC += timers.c
SRC += trees.c
SRC += uncompr.c
SRC += zip-test.c
SRC += zip_darkmark.c
SRC += zutil.c
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
LFLAGS+= --specs=nosys.specs

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

