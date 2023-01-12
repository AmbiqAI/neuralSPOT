includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include

ifeq ($(MLDEBUG),1)
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-debug.a
else
ifeq ($(MLPROFILE),1)
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-withlogs.a
else
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite.a
endif
endif