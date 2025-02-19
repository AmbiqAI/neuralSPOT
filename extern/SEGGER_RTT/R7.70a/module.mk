local_src := $(wildcard $(subdirectory)/RTT/*.c)
local_src += $(wildcard $(subdirectory)/RTT/*.S)
includes_api += $(subdirectory)/RTT
includes_api += $(subdirectory)/Config

local_bin := $(BINDIR)/$(subdirectory)
bindirs   += $(local_bin)

$(eval $(call make-library, $(local_bin)/segger_rtt.a, $(local_src)))