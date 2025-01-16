local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src := $(filter-out $(subdirectory)/src/erpc_usb_cdc_transport.cpp, $(wildcard $(subdirectory)/src/*.cpp))
local_src := $(filter-out $(subdirectory)/src/erpc_setup_usb_cdc.cpp, $(local_src))
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api

# Conditionally include USB source files
ifdef USB_PRESENT
local_src += $(subdirectory)/src/erpc_usb_cdc_transport.cpp
local_src += $(subdirectory)/src/erpc_setup_usb_cdc.cpp
endif
local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/erpc.a, $(local_src)))