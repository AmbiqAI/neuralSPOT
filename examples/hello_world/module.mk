local_app_name := hello_world
local_src := $(wildcard $(subdirectory)/src/*.c)

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
sources   += $(local_src)
examples  += $(local_bin)/hello_world.axf
examples  += $(local_bin)/hello_world.bin
mains     += $(local_bin)/$(local_app_name).o
# $(eval $(call make-axf, $(local_src)), hello_world)