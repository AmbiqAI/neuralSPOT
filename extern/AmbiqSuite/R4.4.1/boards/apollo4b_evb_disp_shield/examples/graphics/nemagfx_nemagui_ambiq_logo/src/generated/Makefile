PLATFORM?=sw_linux_sdl

NEMAGFX_SDK_PATH?=../../../NemaGFX_SDK
NEMA_GUI_PATH?=NemaGUI

ifeq ($(PLATFORM),sw_linux_sdl)
        CFLAGS+=-DBACK_BUFFER_FORMAT=NEMA_RGB565
endif

CFLAGS+=-DSTANDALONE

#Nema GUI core
C_SRCS +=$(NEMA_GUI_PATH)/ng_event.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_callbacks.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_gitem.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_draw.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_display.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_animation.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_main.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_tree.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_draw_prim.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_gestures.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_screen_trans.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_globals.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_timer.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_utils.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_event_oneshot.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_event_transition.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_event_periodic.c
C_SRCS +=$(NEMA_GUI_PATH)/ng_event_periodic_transition.c

#GItems
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_button.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_rect.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_image.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_circular_progress.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_checkbox.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_circle.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_container.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_digimeter.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_label.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_gauge.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_needle.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_progress_bar.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_radio_button.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_rounded_rect.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_screen.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_slider.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_slider_hor.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_slider_ver.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_watchface.c
C_SRCS +=$(NEMA_GUI_PATH)/gitems/ng_window.c

#Project specific files
C_SRCS +=event_list.c
C_SRCS +=gui_tree.c
C_SRCS +=fonts.c
C_SRCS +=images.c
C_SRCS +=framebuffers.c
C_SRCS +=screen_groups.c
C_SRCS +=custom_callbacks.c

#Include paths
INCPATHS += $(NEMA_GUI_PATH)
INCPATHS += $(NEMA_GUI_PATH)/gitems

include $(NEMAGFX_SDK_PATH)/platforms/$(PLATFORM)/common/make.mk
