
##### Toolchain Defaults #####
TOOLCHAIN ?= arm-none-eabi
COMPILERNAME := gcc
BINDIR := build
NESTDIR := nest
SHELL  :=/bin/bash

##### Target Hardware Defaults #####
BOARD  :=apollo4p
BOARDROOT := $(firstword $(subst _, ,$(BOARD)))
EVB    :=evb
PART   = $(BOARDROOT)
CPU    = cortex-m4
FPU    = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI     = softfp
FABI     = hard

##### Extern Library Defaults #####
AS_VERSION := R4.3.0
TF_VERSION := fecdd5d
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
STACK_SIZE_IN_32B_WORDS := 4096
NS_MALLOC_HEAP_SIZE_IN_K := 16

##### TinyUSB Default Config #####
DEFINES+= CFG_TUSB_MCU=OPT_MCU_APOLLO4

##### BLE Defines
DEFINES+= SEC_ECC_CFG=SEC_ECC_CFG_HCI
DEFINES+= WSF_TRACE_ENABLED
DEFINES+= AM_DEBUG_PRINTF

##### Common AI Precompiler Directives #####
# 1 = load TF library with debug info, turn on TF debug statements
MLDEBUG := 0

# 1 = load optimized TF library with prints enabled, turn on TF profiler
MLPROFILE := 0
TFLM_VALIDATOR := 0
TFLM_VALIDATOR_MAX_EVENTS := 40
# AUDIO_DEBUG := 0    # 1 = link in RTT, dump audio to RTT console
# ENERGY_MODE := 0    # 1 = enable energy measurements via UART1

##### AmbiqSuite Config #####
DEFINES+= AM_HAL_TEMPCO_LP
