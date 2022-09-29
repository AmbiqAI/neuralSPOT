PLATFORM?=zc70x_linux

NEMAGFX_SDK_PATH?=../../../NemaGFX_SDK
NEMA_GUI_PATH?=../../../NemaGUI

ifeq ($(PLATFORM),sw_linux_sdl)
        CFLAGS+=-DBACK_BUFFER_FORMAT=NEMA_RGBA8888
endif

#Project specific files
C_SRCS +=main.c
C_SRCS +=event_list.c
C_SRCS +=gui_tree.c
C_SRCS +=fonts.c
C_SRCS +=images.c
C_SRCS +=framebuffers.c
C_SRCS +=screen_groups.c
C_SRCS +=custom_callbacks.c

#Include paths
INCPATHS += $(NEMA_GUI_PATH)/include
INCPATHS += $(NEMA_GUI_PATH)/include/gitems

#Link the generated code against libNemaGUI
LIBPATHS+=$(NEMA_GUI_PATH)/lib
LDFLAGS+=-lNemaGUI

include $(NEMAGFX_SDK_PATH)/platforms/$(PLATFORM)/common/make.mk
