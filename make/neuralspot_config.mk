
##### Toolchain Defaults #####
TOOLCHAIN ?= arm-none-eabi
COMPILERNAME := gcc
BINDIR := build
NESTDIR := nest
SHELL  :=/bin/bash

##### Target Hardware Defaults #####
BOARD  :=apollo4p
EVB    :=evb
PART   = $(BOARD)
CPU    = cortex-m4
FPU    = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI     = softfp
FABI     = hard

##### Extern Library Defaults #####
AS_VERSION := R4.3.0
TF_VERSION := 0c46d6e
SR_VERSION := R7.70a
ERPC_VERSION := R1.9.1
CMSIS_VERSION := CMSIS_5-5.9.0

##### Application Defaults #####
# default target for binary-specific operations such as 'deploy'
TARGET      := basic_tf_stub
NESTCOMP    := extern/AmbiqSuite
NESTEGG := basic_tf_stub
NESTSOURCEDIR := examples/$(NESTEGG)/src

# application stack and heap size
STACK_SIZE := 4096
NS_MALLOC_HEAP_SIZE_IN_K := 16

##### TinyUSB Default Config #####
DEFINES+= CFG_TUSB_MCU=OPT_MCU_APOLLO4

##### Common AI Precompiler Directives #####
MLDEBUG     := 0    # 1 = load TF library with debug info, turn on TF debug statements
# AUDIO_DEBUG := 0    # 1 = link in RTT, dump audio to RTT console
# ENERGY_MODE := 0    # 1 = enable energy measurements via UART1

##### AmbiqSuite Config #####
DEFINES+= AM_HAL_TEMPCO_LP
