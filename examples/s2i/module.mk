local_src := $(wildcard $(subdirectory)/*.c)
local_src += $(wildcard $(subdirectory)/*.cc)

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
sources   += $(local_src)
examples  += $(local_bin)/s2i.axf
examples  += $(local_bin)/s2i.bin
#$(eval $(call make-image, $(local_bin)/ambiqsuite.a, $(local_src)))