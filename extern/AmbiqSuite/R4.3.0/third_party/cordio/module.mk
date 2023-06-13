local_src := $(wildcard $(subdirectory)/ble-host/sources/hci/ambiq/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/hci/ambiq/cooper*.c)
# local_src := $(wildcard $(subdirectory)/ble-host/sources/hci/common/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/sec/common/*.c)
# local_src := $(wildcard $(subdirectory)/ble-host/sources/sec/pal/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/sec/uecc/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/stack/att/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/stack/cfg/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/stack/dm/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/stack/hci/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/stack/i2c/*.c)
local_src := $(wildcard $(subdirectory)/ble-host/sources/stack/smp/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/apps/hidapp/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/apps/app/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/apps/app/common*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/profiles/hid/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/profiles/bas/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/profiles/gap/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/profiles/gatt/*.c)
local_src := $(wildcard $(subdirectory)/ble-profiles/sources/services/*.c)
local_src := $(wildcard $(subdirectory)/wsf/sources/util/*.c)
local_src := $(wildcard $(subdirectory)/wsf/sources/port/freertos/*.c)


# Third-Party (Cordio)
includes_api += $(subdirectory)/ble-profiles/include/app
includes_api += $(subdirectory)/ble-profiles/sources/profiles/include
includes_api += $(subdirectory)/ble-profiles/sources/profiles/gatt
includes_api += $(subdirectory)/ble-profiles/sources/services
includes_api += $(subdirectory)/wsf/include
includes_api += $(subdirectory)/wsf/sources/port/freertos
includes_api += $(subdirectory)/wsf/sources
includes_api += $(subdirectory)/wsf/sources/util
includes_api += $(subdirectory)/ble-host/include
includes_api += $(subdirectory)/ble-host/sources/stack/cfg
includes_api += $(subdirectory)/ble-host/sources/hci/ambiq
includes_api += $(subdirectory)/ble-host/sources/hci/ambiq/cooper

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/cordio.a, $(local_src)))