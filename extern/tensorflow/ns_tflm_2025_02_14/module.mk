includes_api += $(subdirectory)/.
includes_api += $(subdirectory)/third_party
includes_api += $(subdirectory)/third_party/flatbuffers/include
includes_api += $(subdirectory)/third_party/gemmlowp

DEFINES+= NS_TFSTRUCTURE_RECENT

ifeq ($(ARCH),apollo5)
	TFP := cm55
	GCC_VERSION +=-ambiq
else
	TFP := cm4
endif

ifeq ($(TOOLCHAIN),arm)
	ifeq ($(MLDEBUG),1)
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-armclang-debug.a
	else
		ifeq ($(MLPROFILE),1)
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-armclang-release-with-logs.a
		else
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-armclang-release.a
		endif
	endif
else
	ifeq ($(GCC13_EXPERIMENTAL),1)
		GCC_VERSION :=13
	endif
	ifeq ($(MLDEBUG),1)
		lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-gcc$(GCC_VERSION)-debug.a
	else
		ifeq ($(MLPROFILE),1)
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-gcc$(GCC_VERSION)-release-with-logs.a
		else
			lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-$(TFP)-gcc$(GCC_VERSION)-release.a
		endif
	endif
endif