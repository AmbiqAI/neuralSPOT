local_app_name := hello_world
local_src := $(wildcard $(subdirectory)/src/*.c)
local_bin := $(subdirectory)/$(BINDIR)

bindirs   += $(local_bin)
sources   += $(local_src)
examples  += $(local_bin)/$(local_app_name).axf
examples  += $(local_bin)/$(local_app_name).bin
mains     += $(local_bin)/$(local_app_name).o