# File: make/neuralspot_toolchain.mk

# -----------------------------------------------------------------------------
# 1) REQUIRED EXECUTABLES & DEFAULT COMMANDS
# -----------------------------------------------------------------------------

ifeq ($(TOOLCHAIN),arm-none-eabi)
  CC   := $(TOOLCHAIN)-gcc$(EXEEXT)
  GCC  := $(TOOLCHAIN)-gcc$(EXEEXT)
  CPP  := $(TOOLCHAIN)-cpp$(EXEEXT)
  LD   := $(TOOLCHAIN)-ld$(EXEEXT)
  CP   := $(TOOLCHAIN)-objcopy$(EXEEXT)
  OD   := $(TOOLCHAIN)-objdump$(EXEEXT)
  RD   := $(TOOLCHAIN)-readelf$(EXEEXT)
  AR   := $(TOOLCHAIN)-ar$(EXEEXT)
  SIZE := $(TOOLCHAIN)-size$(EXEEXT)
else ifeq ($(TOOLCHAIN),arm)
  CC   := armclang$(EXEEXT)
  GCC  := armclang$(EXEEXT)
  CPP  := armclang$(EXEEXT)
  LD   := armlink$(EXEEXT)
  CP   := fromelf$(EXEEXT)
  OD   := fromelf$(EXEEXT)
  AR   := armar$(EXEEXT)
endif

LINT := clang-tidy$(EXEEXT)
RM   := $(shell which rm 2>/dev/null)
MKD  := mkdir -p
DOX  := doxygen$(EXEEXT)

# -----------------------------------------------------------------------------
# 2) FLAGS INITIALIZATION
# -----------------------------------------------------------------------------
  # Linker file selection
  ifeq ($(ARCH),apollo330)
    BL := _$(BOOTLOADER)
    $(info BL: $(BL))
  else ifeq ($(ARCH),apollo5)
    BL := _$(BOOTLOADER)
  else
    BL :=
  endif
  ifeq ($(TFLM_IN_ITCM),1)
    BL := _itcm$(BL)
  endif
$(info BL: $(BL))

# Always enable sectioning, debugging info, and optimizations
ifeq ($(TOOLCHAIN),arm-none-eabi)
  CONLY_FLAGS += -std=c99
  ASMFLAGS    += -mthumb -mcpu=$(CPU) $(FPU_FLAG) -mfloat-abi=$(FABI) -c

  CFLAGS  += \
    -mthumb \
    -mcpu=$(CPU) \
    $(FPU_FLAG) \
    -mfloat-abi=$(FABI) \
    -ffunction-sections \
    -fdata-sections \
    -fomit-frame-pointer \
    -fno-exceptions \
    -MMD -MP -Wall \
    -g -O3 -ffast-math 

  CCFLAGS := -fno-use-cxa-atexit

  ifndef LINKER_FILE
  LINKER_FILE := ./neuralspot/ns-core/src/$(BOARD)/gcc/linker_script$(BL).ld
  endif
  
  # Linker flags
  LFLAGS  += \
    -mthumb \
    -mcpu=$(CPU) \
    $(FPU_FLAG) \
    -mfloat-abi=$(FABI) \
    -nostartfiles \
    -static \
    -fno-exceptions \
    -fdiagnostics-show-option

  ifeq ($(GCC14),1)
    LFLAGS += -Wl,--no-warn-rwx-segments
  endif

  # Avoid linker warnings
  LFLAGS += \
    -Wl,--wrap=_write_r \
    -Wl,--wrap=_close_r \
    -Wl,--wrap=_lseek_r \
    -Wl,--wrap=_read_r \
    -Wl,--wrap=_kill_r \
    -Wl,--wrap=_getpid_r \
    -Wl,--wrap=_fstat_r \
    -Wl,--wrap=_isatty_r

  LFLAGS += \
    -Wl,--gc-sections,--entry,Reset_Handler \
    -Wl,--start-group -lm -lc -lgcc -lnosys \
    -Wl,--sort-section=name \
    -Wl,--whole-archive $(override_libraries) -Wl,--no-whole-archive $(libraries) $(lib_prebuilt) -lstdc++ \
    -Wl,--end-group

  CPFLAGS := -Obinary
  ODFLAGS := -S
  ARFLAGS := rscD

else ifeq ($(TOOLCHAIN),arm)

  # Armlink keeps removing stuff from static libs, so have to add some objs to the linker command line
  ifeq ($(USB_PRESENT),1)
    ARMLINKER_IS_NO_BUENO := $(BINDIR)/neuralspot/ns-usb/src/overrides/usb_descriptors.o
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/neuralspot/ns-usb/src/overrides/webusb_controller.o
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/neuralspot/ns-usb/src/overrides/ns_usb_overrides.o
  endif
  ifneq ($(ARCH),apollo3)
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_resources.o
  endif
  ifeq ($(ARCH),apollo5)
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/neuralspot/ns-core/src/$(BOARD)/armclang/startup_keil6.o
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_hal_utils.o
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_hal_pwrctrl.o
    ARMLINKER_IS_NO_BUENO += $(BINDIR)/extern/AmbiqSuite/$(AS_VERSION)/src/am_hal_dcu.o
  endif

  CONLY_FLAGS += -xc -std=c99
  ASMFLAGS    += \
    --target=arm-arm-none-eabi \
    -mthumb -mcpu=$(CPU) $(FPU_FLAG) -mfloat-abi=$(FABI) \
    -masm=auto \
    -gdwarf-4 \
    -Wa,armasm,--diag_suppress=A1950W \
    -Wa,armasm,--pd,"__UVISION_VERSION SETA 538" \
    -Wa,armasm,--pd,"APOLLO4p_2048 SETA 1"

  CFLAGS += \
    --target=arm-arm-none-eabi \
    -mcpu=$(CPU) \
    $(FPU_FLAG) \
    -mfloat-abi=$(FABI) \
    -c \
    -Ofast \
    -fno-rtti \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -gdwarf-4 \
    -ffunction-sections \
    -Wno-packed \
    -Wno-missing-variable-declarations \
    -Wno-missing-prototypes \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-typedef-redefinition \
    -Wno-nonportable-include-path \
    -Wno-reserved-id-macro \
    -Wno-unused-macros \
    -Wno-documentation-unknown-command \
    -Wno-documentation \
    -Wno-license-management \
    -Wno-parentheses-equality \
    -Wno-reserved-identifier \
    -MMD -MP
    # -fno-omit-frame-pointer -funwind-tables \

  CCFLAGS += -fno-use-cxa-atexit

  ifndef LINKER_FILE
    LINKER_FILE := ./neuralspot/ns-core/src/$(BOARD)/armclang/linker_script$(BL).sct
  endif

  LFLAGS += \
    --cpu=$(ARMLINK_CPU) \
    --output_float_abi=hard \
    --fpu=$(ARMLINK_FPU) \
    --strict \
    --scatter $(LINKER_FILE) \
    --undefined=__scatterload_copy \
    --summary_stderr \
    --info summarysizes \
    --map \
    --load_addr_map_info \
    --xref \
    --callgraph \
    --symbols \
    --info sizes \
    --info totals \
    --info unused \
    --info veneers \
    --debug

  ifeq ($(USB_PRESENT),1)
    LFLAGS += \
      --keep=tud_cdc_rx_cb \
      --keep=tud_cdc_tx_complete_cb \
      --keep=tud_vendor_control_xfer_cb \
      --keep=tud_descriptor_bos_cb \
      --keep=tud_descriptor_string_cb
  endif

  CPFLAGS := --bin --output
  ODFLAGS := -cedrst
  ARFLAGS := -r -s -c
endif


# -----------------------------------------------------------------------------
# 3) RUNTIME MESSAGE
# -----------------------------------------------------------------------------

# $(info ==== neuralspot_toolchain.mk ====)
# $(info TOOLCHAIN:       $(TOOLCHAIN))
# $(info CC:              $(CC))
# $(info CFLAGS:          $(CFLAGS))
# $(info LFLAGS:          $(LFLAGS))
# $(info LINKER_FILE:     $(LINKER_FILE))
# $(info ------------------------------------)


# -----------------------------------------------------------------------------
# 4) FINAL “DEFINES” MERGED INTO CFLAGS
# -----------------------------------------------------------------------------

# Always compile with these built-in defines
DEFINES += NEURALSPOT
DEFINES += $(PLATFORM)
DEFINES += PART_$(PART)

ifeq ($(PART),apollo4b)
  DEFINES += AM_PART_APOLLO4B
endif

ifeq ($(PART),apollo4p)
  DEFINES += AM_PART_APOLLO4P
endif

ifeq ($(PART),apollo4l)
  DEFINES += AM_PART_APOLLO4L
  DEFINES += APOLLO4L_PRE_SDK
endif

ifeq ($(PART),apollo3p)
  DEFINES += AM_PART_APOLLO3P
  DEFINES += PART_APOLLO3P
endif

ifeq ($(PART),apollo3)
  DEFINES += AM_PART_APOLLO3
  DEFINES += PART_APOLLO3
endif

ifeq ($(PART),apollo510L)
  DEFINES += AM_PART_APOLLO510L
  # DEFINES += AM_PART_APOLLO330P_510L
  DEFINES += PART_APOLLO510L
  DEFINES += ARMCM55
else ifeq ($(PART),apollo510b)
  DEFINES += AM_PART_APOLLO510B
  DEFINES += PART_APOLLO510B
  DEFINES += ARMCM55
endif

ifeq ($(PART),apollo330P)
  DEFINES += AM_PART_APOLLO330P
  # DEFINES += AM_PART_APOLLO330P_510L
  DEFINES += PART_APOLLO330P
  DEFINES += ARMCM55
endif

ifneq ($(filter apollo5b apollo510 apollo510b,$(PART)),)
  DEFINES += AM_PART_APOLLO5B
  DEFINES += AM_PART_APOLLO510
  DEFINES += ARMCM55

  ifeq ($(EVB),eb)
    DEFINES += apollo5_eb
  else ifeq ($(EVB),eb_revb)
    DEFINES += apollo5_eb_revb
  else
    DEFINES += apollo510_evb
  endif
endif
DEFINES += AM_PACKAGE_BGA
DEFINES += __FPU_PRESENT

ifeq ($(TOOLCHAIN),arm)
  DEFINES += __UVISION_VERSION="538"
  DEFINES += _RTE_
  DEFINES += keil6
else
  DEFINES += gcc
endif

DEFINES += TF_LITE_STATIC_MEMORY

# Re-append MLDEBUG/MLPROFILE flags if set
ifeq ($(MLPROFILE),1)
  DEFINES += NS_MLPROFILE
endif

ifeq ($(MLDEBUG),1)
  DEFINES += NS_MLDEBUG
else
  ifneq ($(MLPROFILE),1)
    DEFINES += TF_LITE_STRIP_ERROR_STRINGS
  endif
endif

ifeq ($(AUDIO_DEBUG),1)
  DEFINES += AUDIODEBUG
  DEFINES += 'SEGGER_RTT_SECTION="SHARED_RW"'
endif

ifeq ($(ENERGY_MODE),1)
  DEFINES += ENERGYMODE
endif


# -----------------------------------------------------------------------------
# 5) QUIET‐BUILD SUPPORT
# -----------------------------------------------------------------------------

# When MAKE VERBOSE=1, print every command. Otherwise, prefix with “@”
ifeq ($(VERBOSE),1)
  Q :=
else
  Q := @
endif


# -----------------------------------------------------------------------------
# 6) MARK DEFAULT RULES
# -----------------------------------------------------------------------------

# .PRECIOUS: %.o

# End of neuralspot_toolchain.mk
