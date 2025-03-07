local_app_name := rpc_profiler 

#set in make invoke: MLPROFILE := 1 

local_src := $(wildcard $(subdirectory)/src/*.c)
local_src += $(wildcard $(subdirectory)/src/*.cc)
local_src += $(wildcard $(subdirectory)/src/*.cpp)
local_src += $(wildcard $(subdirectory)/src/*.s)
libs += libs/ns-harness.a 

local_bin := $(BINDIR)/$(subdirectory)

bindirs   += $(local_bin)
examples  += $(local_bin)/rpc_profiler.axf
examples  += $(local_bin)/rpc_profiler.bin
$(eval $(call make-axf, $(local_bin)/rpc_profiler, $(local_src)))
