includes_api += $(subdirectory)/tensorflow
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include

ifeq ($(MLDEBUG),1)
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-debug.a
else
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite.a
endif
