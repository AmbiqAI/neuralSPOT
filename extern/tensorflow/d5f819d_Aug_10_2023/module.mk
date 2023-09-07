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
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-clang-nofp-mve.a
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-clang-shortenum.a
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-clang-debug-lm-dwarf4-O0-fp.a
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-release-Ofast-fp.a
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-windowsclang-dwarf-shortenum.a
lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm4-armclang-release.a
# lib_prebuilt += $(subdirectory)/lib/libtensorflow-microlite-cm55-gcc.a
# endif
# endif