local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
sources   += $(local_src)
examples  += $(local_bin)/s2i.axf
examples  += $(local_bin)/s2i.bin
#