local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src := $(filter-out $(subdirectory)/src/erpc_usb_cdc_transport.cpp, $(wildcard $(subdirectory)/src/*.cpp))
local_src := $(filter-out $(subdirectory)/src/erpc_setup_usb_cdc.cpp, $(local_src))
local_src += $(wildcard $(subdirectory)/src/*.s)
includes_api += $(subdirectory)/includes-api
includes_api := $(filter-out $(subdirectory)/includes-api/erpc_usb_cdc_transport.hpp, $(includes_api))

# Conditionally include USB source files
ifeq ($(USB_PRESENT), 1)
local_src += $(subdirectory)/src/erpc_usb_cdc_transport.cpp
local_src += $(subdirectory)/src/erpc_setup_usb_cdc.cpp
includes_api := $(subdirectory)/includes-api/erpc_usb_cdc_transport.hpp, $(includes_api)
endif
local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/erpc.a, $(local_src)))