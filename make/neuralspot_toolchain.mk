# Makefile include for common toolchain definitions

# Enable printing explicit commands with 'make VERBOSE=1'
ifneq ($(VERBOSE),1)
Q:=@
endif

.PRECIOUS: %.o

#### Required Executables ####
ifeq ($(TOOLCHAIN),arm-none-eabi)
CC = $(TOOLCHAIN)-gcc$(EXEEXT)
GCC = $(TOOLCHAIN)-gcc$(EXEEXT)
CPP = $(TOOLCHAIN)-cpp$(EXEEXT)
LD = $(TOOLCHAIN)-ld$(EXEEXT)
CP = $(TOOLCHAIN)-objcopy$(EXEEXT)
OD = $(TOOLCHAIN)-objdump$(EXEEXT)
RD = $(TOOLCHAIN)-readelf$(EXEEXT)
AR = $(TOOLCHAIN)-ar$(EXEEXT)
SIZE = $(TOOLCHAIN)-size$(EXEEXT)
else ifeq ($(TOOLCHAIN),arm)
CC = armclang$(EXEEXT)
GCC = armclang$(EXEEXT)
CPP = armclang$(EXEEXT)
LD = armlink$(EXEEXT)
CP = fromelf$(EXEEXT)
OD = fromelf$(EXEEXT)
AR = armar$(EXEEXT)
endif


LINT = clang-tidy$(EXEEXT)
RM = $(shell which rm 2>/dev/null)
MKD = "mkdir"
DOX = doxygen$(EXEEXT)


# EXECUTABLES = CC LD CP OD AR RD SIZE GCC JLINK JLINK_SWO
# K := $(foreach exec,$(EXECUTABLES),\
#         $(if $(shell which $($(exec)) 2>/dev/null),,\
#         $(info $(exec) not found on PATH ($($(exec))).)$(exec)))
# $(if $(strip $(value K)),$(info Required Program(s) $(strip $(value K)) not found))

# ifneq ($(strip $(value K)),)
# all clean:
# 	$(info Tools $(TOOLCHAIN)-$(COMPILERNAME) not installed.)
# 	$(RM) -rf bin
# else

ifeq ($(TOOLCHAIN),arm-none-eabi)
CFLAGS+= -mthumb -mcpu=$(CPU) $(FPU_FLAG) -mfloat-abi=$(FABI)
CFLAGS+= -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-exceptions
CCFLAGS+= -fno-use-cxa-atexit
CFLAGS+= -MMD -MP -Wall
CONLY_FLAGS+= -std=c99
ifeq ($(GCC13_EXPERIMENTAL),1)
CFLAGS+= -g -O2 -ffast-math
else
CFLAGS+= -g -O3 -ffast-math
# CFLAGS+= -g -O0 -ffast-math
endif
# CFLAGS+= -g -O2 -ffast-math

ifeq ($(ARCH),apollo5)
LINKER_FILE := ./neuralspot/ns-core/src/$(BOARD)/gcc/linker_script_$(BOOTLOADER).ld
else
LINKER_FILE := ./neuralspot/ns-core/src/$(BOARD)/gcc/linker_script.ld
endif

LFLAGS = -mthumb -mcpu=$(CPU) $(FPU_FLAG) -mfloat-abi=$(FABI)
LFLAGS+= -nostartfiles -static -fno-exceptions -fdiagnostics-show-option
ifeq ($(GCC14),1)
LFLAGS+= -Wl,--no-warn-rwx-segments
endif
LFLAGS+= -Wl,--wrap=_write_r -Wl,--wrap=_close_r -Wl,--wrap=_lseek_r -Wl,--wrap=_read_r -Wl,--wrap=_kill_r -Wl,--wrap=_getpid_r -Wl,--wrap=_fstat_r -Wl,--wrap=_isatty_r
LFLAGS+= -Wl,--gc-sections,--entry,Reset_Handler,-Map,$(BINDIR)/output.map
LFLAGS+= -Wl,--start-group -lm -lc -lgcc -lnosys -Wl,--whole-archive $(override_libraries) -Wl,--no-whole-archive $(libraries) $(lib_prebuilt) -lstdc++ -Wl,--end-group
LFLAGS+=

CPFLAGS = -Obinary
ODFLAGS = -S
ARFLAGS = rsc


else ifeq ($(TOOLCHAIN),arm)
# Armlink keeps removing stuff from static libs, so have to add some objs to the linker command line
ifeq ($(USB_PRESENT),1)
ARMLINKER_IS_NO_BUENO := $(BINDIR)/neuralspot/ns-usb/src/overrides/usb_descriptors.o
ARMLINKER_IS_NO_BUENO += $(BINDIR)/neuralspot/ns-usb/src/overrides/webusb_controller.o
ARMLINKER_IS_NO_BUENO += $(BINDIR)/neuralspot/ns-usb/src/overrides/ns_usb_overrides.o
endif
ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_resources.o
ifeq ($(ARCH),apollo5)
ARMLINKER_IS_NO_BUENO += $(BINDIR)/neuralspot/ns-core/src/$(BOARD)/armclang/startup_keil6.o
ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_hal_utils.o
ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_hal_pwrctrl.o
ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_hal_dcu.o
endif


CONLY_FLAGS+= -xc -std=c99
CFLAGS+= --target=arm-arm-none-eabi -mcpu=$(CPU) $(FPU_FLAG)  -mfloat-abi=$(FABI) -c
CFLAGS+= -fno-rtti -funsigned-char -fshort-enums -fshort-wchar
CFLAGS+= -gdwarf-4 -Ofast
# CFLAGS+= -gdwarf-4
CFLAGS+= -ffunction-sections -Wno-packed -Wno-missing-variable-declarations
CFLAGS+= -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-typedef-redefinition
CFLAGS+= -Wno-nonportable-include-path -Wno-reserved-id-macro -Wno-unused-macros
CFLAGS+= -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management
CFLAGS+= -Wno-parentheses-equality -Wno-reserved-identifier
CFLAGS+= -MMD -MP
CCFLAGS+= -fno-use-cxa-atexit

# LFLAGS+= --cpu=Cortex-M4.fp.sp --output_float_abi=hard --fpu=FPv4-SP --datacompressor=off
LFLAGS+= --cpu=$(ARMLINK_CPU) --output_float_abi=hard --fpu=$(ARMLINK_FPU)
ifeq ($(ARCH),apollo5)
LFLAGS+= --strict --scatter "neuralspot/ns-core/src/$(BOARD)/armclang/linker_script_$(BOOTLOADER).sct" --undefined=__scatterload_copy
else
LFLAGS+= --strict --scatter "neuralspot/ns-core/src/$(BOARD)/armclang/linker_script.sct" --undefined=__scatterload_copy
endif

ifeq ($(USB_PRESENT),1)
LFLAGS+= --keep=tud_cdc_rx_cb --keep=tud_cdc_tx_complete_cb --keep=tud_vendor_control_xfer_cb
LFLAGS+= --keep=tud_descriptor_bos_cb --keep=tud_descriptor_string_cb
endif
LFLAGS+= --keep=vTaskSwitchContext
# LFLAGS+= --lto --summary_stderr --info summarysizes --map --load_addr_map_info --xref --callgraph --symbols
LFLAGS+= --summary_stderr --info summarysizes --map --load_addr_map_info --xref --callgraph --symbols
LFLAGS+= --info sizes --info totals --info unused --info veneers --debug

CPFLAGS = --bin --output
ODFLAGS = -cedrst
ARFLAGS= -r -s -c
ASMFLAGS+= --target=arm-arm-none-eabi -mthumb -mcpu=$(CPU) -mfpu=fpv4-sp-d16 -mfloat-abi=hard -masm=auto

# --target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -masm=auto
ASMFLAGS+= -Wa,armasm,--diag_suppress=A1950W -c
ASMFLAGS+= -gdwarf-4
# -IC:/Users/xbox/AppData/Local/Arm/Packs/AmbiqMicro/Apollo_DFP/1.3.2/Device/Include
ASMFLAGS+= -Wa,armasm,--pd,"__UVISION_VERSION SETA 538" -Wa,armasm,--pd,"APOLLO4p_2048 SETA 1"
endif

$(info Running makefile for $(PLATFORM))
DEFINES+= NEURALSPOT

DEFINES+= $(PLATFORM)
DEFINES+= PART_$(PART)
ifeq ($(PART),apollo4b)
DEFINES+= AM_PART_APOLLO4B
endif
ifeq ($(PART),apollo4p)
DEFINES+= AM_PART_APOLLO4P
endif
ifeq ($(PART),apollo4l)
DEFINES+= AM_PART_APOLLO4L
DEFINES+= APOLLO4L_PRE_SDK
endif
ifeq ($(PART),apollo3p)
	DEFINES+= AM_PART_APOLLO3P
	DEFINES+= PART_APOLLO3P
endif
ifeq ($(PART),apollo3)
	DEFINES+= AM_PART_APOLLO3
	DEFINES+= PART_APOLLO3
endif
ifeq ($(PART),apollo5a)
DEFINES+= AM_PART_APOLLO5A
ifeq ($(EVB),eb)
DEFINES+= apollo5_eb
else
DEFINES+= apollo5_eb_revb
endif
endif
ifeq ($(PART),apollo5b)
DEFINES+= AM_PART_APOLLO5B
ifeq ($(EVB),eb)
DEFINES+= apollo5_eb
else 
ifeq ($(EVB),eb_revb)
DEFINES+= apollo5_eb_revb
else
DEFINES+= apollo510_evb
endif
endif
endif


DEFINES+= AM_PACKAGE_BGA
DEFINES+= __FPU_PRESENT

ifeq ($(TOOLCHAIN), arm)
DEFINES+= __UVISION_VERSION="538"
DEFINES+= _RTE_
DEFINES+= keil6
else
DEFINES+= gcc
endif

DEFINES+= TF_LITE_STATIC_MEMORY

# Enable ML Debug and Symbols with 'make MLDEBUG=1'
# Enable TFLM profiling with 'make MLPROFILE=1'
ifeq ($(MLPROFILE),1)
DEFINES+= NS_MLPROFILE
endif

ifeq ($(TFLM_VALIDATOR),1)
DEFINES+= NS_TFLM_VALIDATOR
endif

DEFINES+= NS_PROFILER_RPC_EVENTS_MAX=$(TFLM_VALIDATOR_MAX_EVENTS)

ifeq ($(MLDEBUG),1)
DEFINES+= NS_MLDEBUG
else
ifneq ($(MLPROFILE),1)
DEFINES+= TF_LITE_STRIP_ERROR_STRINGS
endif
endif

ifeq ($(AUDIO_DEBUG),1)
DEFINES+= AUDIODEBUG
DEFINES+= 'SEGGER_RTT_SECTION="SHARED_RW"'
endif
ifeq ($(ENERGY_MODE),1)
DEFINES+= ENERGYMODE
endif

# Opus defines
DEFINES += USE_ALLOCA
DEFINES += OPUS_BUILD


DEFINES+= STACK_SIZE=$(STACK_SIZE_IN_32B_WORDS)
DEFINES+= NS_MALLOC_HEAP_SIZE_IN_K=$(NS_MALLOC_HEAP_SIZE_IN_K)

DEFINES+= NS_AMBIQSUITE_VERSION_$(subst .,_,$(AS_VERSION))
DEFINES+= NS_TF_VERSION_$(subst .,_,$(TF_VERSION))
DEFINES+= NS_SR_VERSION_$(subst .,_,$(SR_VERSION))
DEFINES+= NS_ERPC_VERSION_$(subst .,_,$(ERPC_VERSION))
DEFINES+= NS_CMSIS_VERSION_$(subst .,_,$(subst -,_,$(CMSIS_VERSION)))
