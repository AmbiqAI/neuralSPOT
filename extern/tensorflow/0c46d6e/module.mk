includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
# includes_api += $(subdirectory)/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include
includes_api += $(subdirectory)/third_party/flatbuffers/include

ifeq ($(MLDEBUG),1)
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-debug.a
else
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-o2.a
endif

