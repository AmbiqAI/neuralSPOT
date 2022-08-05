local_src := $(wildcard $(subdirectory)/src/*.c)
includes_api += $(subdirectory)/includes-api

local_bin := $(subdirectory)/bin
bindirs   += $(local_bin)
$(info www$(bindirs)www)
$(eval $(call make-library, $(local_bin)/ns-ipc.a, $(local_src)))