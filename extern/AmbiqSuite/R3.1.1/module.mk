# # This is so stupid - make sure this happens first
# # Armclang linker removes stuff agressively, make sure some of it stays.
# local_src := $(wildcard $(subdirectory)/src/overrides/*.c)
# local_src += $(wildcard $(subdirectory)/src/overrides/*.cc)
# local_src += $(wildcard $(subdirectory)/src/overrides/*.s)

# local_bin := $(BINDIR)/$(subdirectory)
# # bindirs   += $(local_bin)
# $(eval $(call make-override-library, $(local_bin)/ambiqsuite_overrides.a, $(local_src)))

# The real stuff
local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)

# Base AmbiqSuite
includes_api += $(subdirectory)/boards/$(BOARD)_$(EVB)/bsp
includes_api += $(subdirectory)/CMSIS/ARM/Include
includes_api += $(subdirectory)/CMSIS/AmbiqMicro/Include
includes_api += $(subdirectory)/devices
includes_api += $(subdirectory)/mcu/$(BOARDROOT)
includes_api += $(subdirectory)/mcu/$(BOARDROOT)/hal/mcu
includes_api += $(subdirectory)/utils

ifeq ($(TOOLCHAIN),arm)
lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal.lib
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp.lib
else
ifeq ($(GCC13_EXPERIMENTAL),1)
lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal-gcc13.a
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp-gcc13.a
else
lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal.a
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp.a
endif
endif
# lib_prebuilt += $(subdirectory)/lib/libarm_cortexM4lf_math.a

# Third-Party (FreeRTOS)
includes_api += $(subdirectory)/third_party/FreeRTOSv10.1.1/Source/include
includes_api += $(subdirectory)/third_party/FreeRTOSv10.1.1/Source/portable/GCC/AMapollo2

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

STARTUP_FILE := ./startup_$(COMPILERNAME).c

$(eval $(call make-library, $(local_bin)/ambiqsuite.a, $(local_src)))