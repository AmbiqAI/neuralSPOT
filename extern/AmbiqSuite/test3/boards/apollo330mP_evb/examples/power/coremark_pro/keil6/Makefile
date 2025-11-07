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
COMPILERNAME := Keil6
PROJECT := coremark_pro_Keil6
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

LIBS = ../../../../bsp/keil6/bin/libam_bsp.lib
LIBS+= ../../../../../../CMSIS/ARM/Lib/ARM/DSP_LIB_CM55/arm_cortexM55f_math.lib
LIBS+= ../../../../../../mcu/apollo330P/hal/mcu/keil6/bin/libam_hal.lib
INCS = ../src/FreeRTOS_Plus_POSIX/include/private
INCS+= ../../../../../..
INCS+= ../src/benchmarks/consumer_v2/cjpeg
INCS+= ../src/benchmarks/darkmark/parser
INCS+= ../src
INCS+= ../src/benchmarks/darkmark/zip/zlib-1.2.8
INCS+= ../src/FreeRTOS_Plus_POSIX/include
INCS+= ../src/benchmarks/consumer_v2/cjpeg/data
INCS+= ../../../../../../devices
INCS+= ../src/mith/al/include
INCS+= ../../../../../../third_party/FreeRTOSv10.5.1/Source/include
INCS+= ../../../../../../CMSIS/AmbiqMicro/Include
INCS+= ../../../../bsp
INCS+= ../src/benchmarks/darkmark/sha
INCS+= ../../../../../../CMSIS/ARM/Include
INCS+= ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/include
INCS+= ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5
INCS+= ../src/benchmarks/fp/linpack
INCS+= ../../../../../../mcu/apollo330P
INCS+= ../src/benchmarks/fp/nnet
INCS+= ../../../../../../utils
INCS+= ../src/benchmarks/fp/fft_radix2
INCS+= ../src/benchmarks/core
INCS+= ../src/mith/include
INCS+= ../src/benchmarks/fp/loops
INCS+= ../src/FreeRTOS_Plus_POSIX/include/FreeRTOS_POSIX

all: directories $(CONFIG)/$(TARGET).bin

# Source Dependencies must be defined before they are used.
SRCS = ../src/workloads/nnet_test/nnet_test.c
SRCS += ../src/mith/src/md5.c
SRCS += ../src/mith/src/mith_lib.c
SRCS += ../src/mith/src/mith_workload.c
SRCS += ../src/mith/src/th_bignum.c
SRCS += ../src/mith/src/th_encode.c
SRCS += ../src/mith/src/th_getopt.c
SRCS += ../src/mith/src/th_lib.c
SRCS += ../src/mith/src/th_math.c
SRCS += ../src/mith/src/th_rand.c
SRCS += ../src/workloads/radix2-big-64k/radix2-big-64k.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/port.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/portasm.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/port_stimer.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/Keil6/AMapollo5/port_systick_stimer.c
SRCS += ../src/workloads/core/core.c
SRCS += ../src/mith/al/src/al_file.c
SRCS += ../src/mith/al/src/al_single.c
SRCS += ../src/mith/al/src/al_smp.c
SRCS += ../src/mith/al/src/th_al.c
SRCS += ../src/am_resources.c
SRCS += ../src/am_time.c
SRCS += ../src/coremark_pro.c
SRCS += ../src/heap_4_external.c
SRCS += ../src/heap_5_external.c
SRCS += ../src/mpu_config.c
SRCS += ../src/mspi_psram_init.c
SRCS += ../src/rtos.c
SRCS += ../src/workloads/parser-125k/parser-125k.c
SRCS += ../src/workloads/linear_alg-mid-100x100-sp/linear_alg-mid-100x100-sp.c
SRCS += ../src/workloads/zip-test/zip-test.c
SRCS += ../src/workloads/sha-test/sha-test.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c
SRCS += ../src/FreeRTOS_Plus_POSIX/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c
SRCS += ../src/workloads/loops-all-mid-10k-sp/loops-all-mid-10k-sp.c
SRCS += ../src/workloads/cjpeg-rose7-preset/cjpeg-rose7-preset.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/bmark_lite.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/bm_lib.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/cdjpeg.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/cjpeg.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/filedata.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcapimin.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcapistd.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jccoefct.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jccolor.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcdctmgr.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jchuff.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcinit.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcmainct.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcmarker.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcmaster.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcomapi.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcparam.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcprepct.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jcsample.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jdatadst.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jerror.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jfdctint.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jmemansi.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jmemmgr.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/jutils.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/rdbmp.c
SRCS += ../src/benchmarks/darkmark/parser/ezxml.c
SRCS += ../src/benchmarks/darkmark/parser/parser.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/adler32.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/compress.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/crc32.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/deflate.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/gzclose.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/gzlib.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/infback.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/inffast.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/inflate.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/inftrees.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/trees.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/uncompr.c
SRCS += ../src/benchmarks/darkmark/zip/zlib-1.2.8/zutil.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/data/goose_bmp.c
SRCS += ../src/benchmarks/consumer_v2/cjpeg/data/Rose256_bmp.c
SRCS += ../src/benchmarks/fp/nnet/ref-sp/1letter.c
SRCS += ../src/benchmarks/fp/nnet/ref-sp/letters.c
SRCS += ../src/benchmarks/darkmark/sha/sha256.c
SRCS += ../src/benchmarks/darkmark/sha/shabench.c
SRCS += ../src/benchmarks/fp/linpack/ref/inputs_f32.c
SRCS += ../src/benchmarks/fp/linpack/linpack.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/event_groups.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/list.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/queue.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/tasks.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/timers.c
SRCS += ../src/benchmarks/fp/nnet/nnet.c
SRCS += ../../../../../../utils/am_util_delay.c
SRCS += ../../../../../../utils/am_util_pmu.c
SRCS += ../../../../../../utils/am_util_stdio.c
SRCS += ../src/benchmarks/fp/fft_radix2/fft_radix2.c
SRCS += ../src/benchmarks/core/core_list_join.c
SRCS += ../src/benchmarks/core/core_matrix.c
SRCS += ../src/benchmarks/core/core_mith.c
SRCS += ../src/benchmarks/core/core_portme.c
SRCS += ../src/benchmarks/core/core_state.c
SRCS += ../src/benchmarks/core/core_util.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/100.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/100k.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/100kdot.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/10k.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/10kdot.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/1k.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/1kdot.c
SRCS += ../src/benchmarks/fp/loops/ref-sp/32.c
SRCS += ../../../../../../third_party/FreeRTOSv10.5.1/Source/portable/MemMang/heap_4.c
SRCS += ../src/benchmarks/fp/loops/loops.c
SRCS += ../src/benchmarks/darkmark/zip/zip_darkmark.c
SRCS += ../src/benchmarks/fp/fft_radix2/ref-sp/2K.c
SRCS += ../src/benchmarks/fp/fft_radix2/ref-sp/32K.c
SRCS += ../src/benchmarks/fp/fft_radix2/ref-sp/4K.c
SRCS += ../src/benchmarks/fp/fft_radix2/ref-sp/data3_big.c
SRCS += ../src/benchmarks/fp/fft_radix2/ref-sp/data4_mid.c
SRCS += ../src/benchmarks/fp/fft_radix2/ref-sp/data5_small.c

$(CONFIG)/$(TARGET).bin: $(LIBS) $(INCS) $(SRCS)
	UV5.exe -b -t "coremark_pro" coremark_pro.uvprojx -j0 || [ $$? -eq 1 ]

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@


clean:
	@echo Cleaning...
	$(RM) -rf $(CONFIG)

endif
.PHONY: all clean directories

