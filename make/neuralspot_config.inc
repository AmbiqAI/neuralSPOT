
##### Toolchain Defaults #####
TOOLCHAIN ?= arm-none-eabi
COMPILERNAME := gcc
BINDIR := bin
SHELL:=/bin/bash

##### Target Hardware Defaults #####
BOARD   := apollo4p     # apollo4p or apollo4b
PART     = $(BOARD)
CPU      = cortex-m4
FPU      = fpv4-sp-d16
# Default to FPU hardware calling convention.  However, some customers and/or
# applications may need the software calling convention.
#FABI     = softfp
FABI     = hard

##### Extern Library Defaults #####
AMBIQSUITE_VERSION := R4.1.0

##### Application Defaults #####
TARGET      := examples     # set to 'examples' to compile app inside 
                            # examples, or 'neuralspot' to create static ns library
PROJECT     := main
APPLICATION := hello_world

##### Common AI Precompiler Directives #####
MLDEBUG     := 0    # 1 = load TF library with debug info, turn on TF debug statements
AUDIO_DEBUG := 0    # 1 = link in RTT, dump audio to RTT console
ENERGY_MODE := 0    # 1 = enable energy measurements via UART1