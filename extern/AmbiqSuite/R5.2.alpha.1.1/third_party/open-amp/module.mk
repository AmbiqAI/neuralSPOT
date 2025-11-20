# local_src := $(wildcard $(subdirectory)/libmetal/lib/*.c)
# local_src += $(wildcard $(subdirectory)/lib/rpmsg/*.c)
# local_src += $(wildcard $(subdirectory)/lib/virtio/*.c)
# local_src += $(wildcard $(subdirectory)/lib/utils/*.c)
# local_src += $(wildcard $(subdirectory)/libmetal/lib/system/freertos/*.c)
# local_src += $(wildcard $(subdirectory)/libmetal/lib/system/freertos/apollo330P/*.c)
# local_src += $(wildcard $(subdirectory)/apps/ipc_service/*.c)
# local_src += $(wildcard $(subdirectory)/apps/ipc_service/lib/*.c)
# local_src += $(wildcard $(subdirectory)/apps/ipc_service/backends/*.c)

includes_api += $(subdirectory)
includes_api += $(subdirectory)/apps/ipc_service/backends
includes_api += $(subdirectory)/apps/ipc_service/include/ipc
includes_api += $(subdirectory)/lib/include
includes_api += $(subdirectory)/lib/include/internal
includes_api += $(subdirectory)/lib/include/openamp
includes_api += $(subdirectory)/lib/rpmsg
includes_api += $(subdirectory)/libmetal/lib/system/freertos
includes_api += $(subdirectory)/libmetal/lib/system/freertos/apollo510L


# To compile cordio.a, uncomment the next 3 lines, and comment out local_src above and the lib_prebuilt line below.
# --- THIS ---
# local_bin := $(BINDIR)/$(subdirectory)
# bindirs   += $(local_bin)
# $(eval $(call make-library, $(local_bin)/open-amp.a, $(local_src)))
# --- OR ---
lib_prebuilt += $(subdirectory)/lib_prebuilt/$(BOARD)/$(EVB)/$(COMPILERNAME)/open-amp.a

