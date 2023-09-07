local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/$(PART)/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.s)
ifeq ($(USB_PRESENT),1)
local_src += $(wildcard $(subdirectory)/src/usb/*.c)
endif

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
lib_prebuilt += $(subdirectory)/lib/$(PART)/libam_hal.a
lib_prebuilt += $(subdirectory)/lib/$(PART)/$(EVB)/libam_bsp.a
endif
# lib_prebuilt += $(subdirectory)/lib/libarm_cortexM4lf_math.a

# Third-Party (FreeRTOS)
includes_api += $(subdirectory)/third_party/FreeRTOSv10.5.1/Source/include
includes_api += $(subdirectory)/third_party/FreeRTOSv10.5.1/Source/portable/GCC/AMapollo4

# Third-Party (TinyUSB)
includes_api += $(subdirectory)/third_party/tinyusb/src
includes_api += $(subdirectory)/third_party/tinyusb/source/src
includes_api += $(subdirectory)/third_party/tinyusb/source/src/common
includes_api += $(subdirectory)/third_party/tinyusb/source/src/osal
includes_api += $(subdirectory)/third_party/tinyusb/source/src/class/cdc
includes_api += $(subdirectory)/third_party/tinyusb/source/src/device


local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

LINKER_FILE := $(subdirectory)/src/linker_script.ld
STARTUP_FILE := ./startup_$(COMPILERNAME).c

$(eval $(call make-library, $(local_bin)/ambiqsuite.a, $(local_src)))