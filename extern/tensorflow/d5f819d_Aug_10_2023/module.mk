includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include
includes_api += $(subdirectory)/third_party/gemmlowp

# ifeq ($(MLDEBUG),1)
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-debug.a
# else
# ifeq ($(MLPROFILE),1)
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-withlogs.a
# else
ifeq ($(TOOLCHAIN),arm)
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-release.a
else
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-gcc-release.a
endif
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-gcc.a
# endif
# endif