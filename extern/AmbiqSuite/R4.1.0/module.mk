local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/boards/apollo4b_evb/bsp
includes_api += $(subdirectory)/CMSIS/ARM/Include
includes_api += $(subdirectory)/CMSIS/AmbiqMicro/Include
includes_api += $(subdirectory)/devices
includes_api += $(subdirectory)/mcu/apollo4b
includes_api += $(subdirectory)/utils

lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal.a
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp.a
lib_prebuilt += $(subdirectory)/lib/libarm_cortexM4lf_math.a

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)

LINKER_FILE := $(subdirectory)/src/linker_script.ld
STARTUP_FILE := ./startup_$(COMPILERNAME).c

$(eval $(call make-library, $(local_bin)/ambiqsuite.a, $(local_src)))