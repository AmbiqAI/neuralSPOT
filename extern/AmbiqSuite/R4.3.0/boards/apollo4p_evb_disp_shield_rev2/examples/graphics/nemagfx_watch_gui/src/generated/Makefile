PLATFORM?=sw_linux_sdl

NEMAGFX_SDK_PATH?=../../../NemaGFX_SDK

ifeq ($(PLATFORM),sw_linux_sdl)
        CFLAGS+=-DBACK_BUFFER_FORMAT=NEMA_RGB565
endif

C_SRCS +=NemaGUI/main.c

INCPATHS += NemaGUI

include $(NEMAGFX_SDK_PATH)/platforms/$(PLATFORM)/common/make.mk
