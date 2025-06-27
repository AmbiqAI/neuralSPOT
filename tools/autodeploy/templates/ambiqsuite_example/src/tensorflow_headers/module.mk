includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include
includes_api += $(subdirectory)/third_party/gemmlowp

ifeq ($(TOOLCHAIN),arm)
	ifeq ($(MLDEBUG),1)
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-debug.a
	else
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-release.a
	endif
else
	ifeq ($(MLDEBUG),1)
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-gcc-debug.a
	else
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-gcc-release.a
	endif
endif
