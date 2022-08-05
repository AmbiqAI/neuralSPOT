local_src := $(wildcard $(subdirectory)/src/*.c)
includes_api += $(subdirectory)/boards/apollo4b_evb/bsp
includes_api += $(subdirectory)/CMSIS/ARM/Include
includes_api += $(subdirectory)/CMSIS/AmbiqMicro/Include
includes_api += $(subdirectory)/devices
includes_api += $(subdirectory)/mcu/apollo4b
includes_api += $(subdirectory)/utils

lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal.a
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp.a
lib_prebuilt += $(subdirectory)/lib/libarm_cortexM4l_math.a
lib_prebuilt += $(subdirectory)/lib/libarm_cortexM4lf_math.a

local_bin := $(subdirectory)/bin
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/ambiqsuite.a, $(local_src)))