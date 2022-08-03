local_src := $(wildcard $(subdirectory)/*.c)

$(eval $(call make-library, bin/ns-ipc.a, $(local_src)))