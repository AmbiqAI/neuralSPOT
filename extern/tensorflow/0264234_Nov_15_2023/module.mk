includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include
includes_api += $(subdirectory)/third_party/gemmlowp

DEFINES+= NS_TFSTRUCTURE_RECENT

ifeq ($(TOOLCHAIN),arm)
	ifeq ($(MLDEBUG),1)
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-debug.a
	else
		ifeq ($(MLPROFILE),1)
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-release-with-logs.a
		else
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-release.a
		endif
	endif
else
	ifeq ($(MLDEBUG),1)
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-gcc-debug.a
	else
		ifeq ($(MLPROFILE),1)
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-gcc13-release-with-logs.a
		else
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-gcc13-release.a
		endif
	endif
endif