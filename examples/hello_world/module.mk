local_src := $(wildcard $(subdirectory)/*.c)

local_bin := $(subdirectory)/$(BINDIR)
bindirs   += $(local_bin)
sources   += $(local_src)
examples  += $(local_bin)/hello_world.axf
examples  += $(local_bin)/hello_world.bin
#$(eval $(call make-image, $(local_bin)/ambiqsuite.a, $(local_src)))