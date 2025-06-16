# File: make/neuralspot_config.mk

# -----------------------------------------------------------------------------
# 1) TOOLCHAIN SELECTION & VERSION DETECTION
# -----------------------------------------------------------------------------

# Default toolchain if not overridden by environment
TOOLCHAIN ?= arm-none-eabi

ifeq ($(TOOLCHAIN),arm-none-eabi)
  COMPILERNAME := gcc

  # Detect GCC version for conditional flags later
  GCC_VER := $(shell $(TOOLCHAIN)-gcc --version | grep -E -o '[0-9]+\.[0-9]+\.[0-9]+')
  ifeq ($(shell expr $(GCC_VER) \>= 14),1)
    GCC14 := 1
  else
    GCC14 := 0
  endif
else ifeq ($(TOOLCHAIN),arm)
  COMPILERNAME := clang
else ifeq ($(TOOLCHAIN),llvm)
  COMPILERNAME := clang
else
  $(error "Unsupported TOOLCHAIN: $(TOOLCHAIN). Valid values are arm-none-eabi, arm, llvm.")
endif


# -----------------------------------------------------------------------------
# 2) DEFAULTS & NAMING CONVENTIONS
# -----------------------------------------------------------------------------

# Where “nest” mode will copy source files
NESTDIR := nest

# If no PLATFORM is specified from outside, pick a sane default
ifndef PLATFORM
  PLATFORM := apollo510_evb
endif

# For Apollo5 variants that use “apollo510” in names older than R5.3.0
ifndef AS_VERSION
  AS_VERSION := R5.3.0
endif

# PLATFORM is in the form “<BOARD>_<EVB>” (e.g. apollo4p_blue_evb)
# Split it into BOARD and EVB
BOARD      := $(firstword $(subst _, ,$(PLATFORM)))

# Pre-R5.3.0 SDKs put code under “apollo5b”; newer ones use “apollo510”
ifneq ($(AS_VERSION),R5.3.0)
  ifneq ($(filter apollo510,$(BOARD)),)
    BOARD := apollo5b
  endif
endif

EVB        := $(wordlist 2,$(words $(subst _, ,$(PLATFORM))),$(subst _, ,$(PLATFORM)))
BOARDROOT  := $(BOARD)
PART       := $(BOARDROOT)

# Replace any spaces in EVB with underscores (unlikely, but for safety)
space      := $(null) #
EVB        := $(subst $(space),_,$(EVB))

# Where to emit build artifacts
BINDIRROOT := build
BINDIR      := $(BINDIRROOT)/$(BOARDROOT)_$(EVB)/$(TOOLCHAIN)

# -----------------------------------------------------------------------------
# 3) ARCHITECTURE & CPU/FPU BOOTSTRAP
# -----------------------------------------------------------------------------

ifeq ($(findstring apollo3,$(BOARD)),apollo3)
  ARCH := apollo3
else ifeq ($(findstring apollo4,$(BOARD)),apollo4)
  ARCH := apollo4
else ifeq ($(findstring apollo5,$(BOARD)),apollo5)
  ARCH := apollo5
else
  $(error "Unable to detect ARCH from BOARD='$(BOARD)'. Must contain apollo3, apollo4, or apollo5.")
endif

# Core CPU & FPU settings by ARCH
ifeq ($(ARCH),apollo5)
  CPU         := cortex-m55
  ARMLINK_CPU := Cortex-M55
  ARMLINK_FPU := FPv5_D16
else
  CPU         := cortex-m4
  FPU_FLAG    := -mfpu=fpv4-sp-d16
  ARMLINK_CPU := Cortex-M4.fp.sp
  ARMLINK_FPU := FPv4-SP
endif

# Calling convention: default to hardware FPU
FABI := hard

# -----------------------------------------------------------------------------
# 4) APPLICATION DEFAULTS
# -----------------------------------------------------------------------------

# If you only want to build one example, set EXAMPLE=xyz from the command line.
ifndef EXAMPLE
  EXAMPLE := all
endif

# Default binary target when invoking “make deploy” etc.
ifndef TARGET
  TARGET := basic_tf_stub
endif

# Example directory & nesting info
NESTCOMP      := extern/AmbiqSuite
NESTEGG       := basic_tf_stub
NESTSOURCEDIR := examples/$(NESTEGG)/src

# When building all examples, these are the choices
TARGETS := \
  apollo3p_evb \
  apollo4p_evb \
  apollo4p_blue_kbr_evb \
  apollo4p_blue_kxr_evb \
  apollo4l_evb \
  apollo4l_blue_evb \
  apollo5a_evb

# -----------------------------------------------------------------------------
# 5) EXTERNAL LIBRARY VERSIONS
# -----------------------------------------------------------------------------

ifndef TF_VERSION
  TF_VERSION := ns_tflm_v1_0_0
endif

SR_VERSION     := R7.70a
ERPC_VERSION   := R1.9.1
CMSIS_VERSION  := CMSIS_5-5.9.0

ifndef CMSIS_DSP_VERSION
  CMSIS_DSP_VERSION := CMSIS-DSP-1.16.2
endif


# -----------------------------------------------------------------------------
# 6) HARDWARE FEATURE FLAGS
# -----------------------------------------------------------------------------

# BLE_PRESENT: any “blue” EVB (or apollo3 always has BLE)
ifeq ($(EVB),blue_evb)
  BLE_PRESENT := 1
else ifeq ($(EVB),blue_kbr_evb)
  BLE_PRESENT := 1
else ifeq ($(EVB),blue_kxr_evb)
  BLE_PRESENT := 1
else ifeq ($(ARCH),apollo3)
  BLE_PRESENT := 1
else
  BLE_PRESENT := 0
endif

# USB_PRESENT: only certain PARTs support USB
ifeq ($(findstring $(PART),apollo4p apollo5a apollo5b apollo510),$(PART))
  USB_PRESENT := 1
else
  USB_PRESENT := 0
endif

# NS_AUDADC_PRESENT & AM_HAL_TEMPCO_LP for apollo4p
ifeq ($(PART),apollo4p)
  DEFINES += NS_AUDADC_PRESENT
  DEFINES += AM_HAL_TEMPCO_LP
endif

# NS_PDM1TO3_PRESENT: only on apollo4p EVB (non-BLE version)
ifeq ($(PART),apollo4p)
  ifeq ($(EVB),evb)
    DEFINES += NS_PDM1TO3_PRESENT
  endif
endif

# Add USB define if USB is supported
ifeq ($(USB_PRESENT),1)
  DEFINES += NS_USB_PRESENT
endif

# Add NS_BLE_SUPPORTED if BLE is possible and SDK version is >= R4.3.0
ifeq ($(AS_VERSION),R4.3.0)
  ifeq ($(BLE_PRESENT),1)
    DEFINES += NS_BLE_SUPPORTED
    BLE_SUPPORTED := 1
  endif
else ifeq ($(AS_VERSION),R4.4.1)
  ifeq ($(BLE_PRESENT),1)
    DEFINES += NS_BLE_SUPPORTED
    BLE_SUPPORTED := 1
  endif
else ifeq ($(AS_VERSION),R4.5.0)
  ifeq ($(BLE_PRESENT),1)
    DEFINES += NS_BLE_SUPPORTED
    BLE_SUPPORTED := 1
  endif
else ifeq ($(AS_VERSION),R3.1.1)
  ifeq ($(BLE_PRESENT),1)
    DEFINES += NS_BLE_SUPPORTED
    BLE_SUPPORTED := 1
  endif
else
  BLE_SUPPORTED := 0
endif

ifndef BOOTLOADER
  BOOTLOADER := sbl
endif

# -----------------------------------------------------------------------------
# 7) MEMORY & MISCELLANEOUS DEFINES
# -----------------------------------------------------------------------------

# Default stack & heap (in 32-bit words or KB)
ifndef STACK_SIZE_IN_32B_WORDS
  STACK_SIZE_IN_32B_WORDS := 4096
endif

ifndef NS_MALLOC_HEAP_SIZE_IN_K
  NS_MALLOC_HEAP_SIZE_IN_K := 32
endif

# LEGACY_MALLOC toggles whether heap is auto-allocated
ifeq ($(LEGACY_MALLOC),1)
  DEFINES += configAPPLICATION_ALLOCATED_HEAP=0
else
  DEFINES += configAPPLICATION_ALLOCATED_HEAP=1
endif
DEFINES += STACK_SIZE=$(STACK_SIZE_IN_32B_WORDS)
DEFINES += NS_MALLOC_HEAP_SIZE_IN_K=$(NS_MALLOC_HEAP_SIZE_IN_K)

DEFINES += SEC_ECC_CFG=SEC_ECC_CFG_HCI
DEFINES += AM_DEBUG_PRINTF

# AI precompiler settings (TFLM)
ifndef MLDEBUG
  MLDEBUG := 0
endif
DEFINES += NS_TFSTRUCTURE_RECENT

MLPROFILE := 0
TFLM_VALIDATOR := 0
TFLM_VALIDATOR_MAX_EVENTS := 40

DEFINES += OPUS_ARM_INLINE_ASM
DEFINES += OPUS_ARM_ASM

DEFINES += NS_AMBIQSUITE_VERSION_$(subst .,_,$(AS_VERSION))
DEFINES += NS_TF_VERSION_$(subst .,_,$(TF_VERSION))
DEFINES += NS_SR_VERSION_$(subst .,_,$(SR_VERSION))
DEFINES += NS_ERPC_VERSION_$(subst .,_,$(ERPC_VERSION))
DEFINES += NS_CMSIS_VERSION_$(subst .,_,$(subst -,_,$(CMSIS_VERSION)))

ifeq ($(MLPROFILE),1)
  DEFINES += NS_MLPROFILE
endif

ifeq ($(TFLM_VALIDATOR),1)
  DEFINES += NS_TFLM_VALIDATOR
endif

DEFINES += NS_PROFILER_RPC_EVENTS_MAX=$(TFLM_VALIDATOR_MAX_EVENTS)

# TinyUSB settings
ifeq ($(ARCH),apollo5)
  DEFINES+= CFG_TUSB_MCU=OPT_MCU_APOLLO5
ifeq ($(PART),apollo5a)
  DEFINES+= BOARD_DEVICE_RHPORT_SPEED=OPT_MODE_FULL_SPEED
else ifeq ($(PART),apollo5b)
  DEFINES+= BOARD_DEVICE_RHPORT_SPEED=OPT_MODE_FULL_SPEED
endif
else
  DEFINES+= CFG_TUSB_MCU=OPT_MCU_APOLLO4
endif

# -----------------------------------------------------------------------------
# 8) INCLUDE PATHS, SOURCES, AND BINDIR INITIALIZATION
# -----------------------------------------------------------------------------

# The “includes” and “sources” variables will be populated by each module’s module.mk
# includes_api := 
# sources      :=

# BINDIR must exist before any compilation
# .PHONY: init_bindir
# init_bindir:
# 	@mkdir -p $(BINDIR)

# -----------------------------------------------------------------------------
# 9) MODULE LISTS (populated from top-level Makefile)
# -----------------------------------------------------------------------------

# examples      := 
# mains         := 
# libraries     := 
# lib_prebuilt  := 
# override_libraries := 
# obs           := $(call source-to-object,$(sources))
# dependencies  := $(subst .o,.d,$(obs))
# objects       := $(filter-out $(mains),$(call source-to-object,$(sources)))


# -----------------------------------------------------------------------------
# 10) SUMMARY PRINT
# -----------------------------------------------------------------------------

# $(info ==== neuralspot_config.mk ===)
# $(info TOOLCHAIN:      $(TOOLCHAIN))
# $(info COMPILERNAME:   $(COMPILERNAME))
# $(info PLATFORM:       $(PLATFORM).)
# $(info BOARD:          $(BOARD).)
# $(info BOARDROOT:      $(BOARDROOT).)
# $(info PART:           $(PART).)
# $(info EVB:            $(EVB).)
# $(info ARCH:           $(ARCH).)
# $(info CPU:            $(CPU).)
# $(info TARGET:         $(TARGET).)
# $(info EXAMPLE:        $(EXAMPLE).)
# $(info FPU_FLAG:       $(FPU_FLAG).)
# $(info BINDIR:         $(BINDIR).)
# $(info NESTDIR:        $(NESTDIR).)
# $(info BINDIRROOT:     $(BINDIRROOT).)
# $(info BLE_SUPPORTED:  $(BLE_SUPPORTED))
# $(info USB_PRESENT:    $(USB_PRESENT))
# $(info TF_VERSION:     $(TF_VERSION))
# $(info AS_VERSION:     $(AS_VERSION))
# $(info --------------------------------)


# End of neuralspot_config.mk
