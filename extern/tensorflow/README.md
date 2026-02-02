# Tensorflow Lite for Microcontrollers
neuralSPOT support 3 versions of TFLM - as new versions are added, the oldest are deprecated (not necessarily deleted, just not tested).

> *NOTE* TFLM doesn't label releases - instead, we're using the first 7 digits of the git checkin hash to identify TFLM versions.

Adding a new version is a fairly straightforward process (more details below):
1. Clone the desired TFLM version from (here)[https://github.com/tensorflow/tflite-micro]
2. Compile release and debug versions of the static library
3. Create the minimal tree
4. Create a directory for new version at extern/tensorflow/<new dir>
5. Copy the TFLM tree and compiled libraries to neuralSPOT
6. Point neuralSPOT makefiles at the new version
5. Resolve code changes if the new version of TFLM changed code structure or APIs

## Compiling TLFM
To compile the 3 needed version (release, release_with_logs, and debug):
```bash
cd tflite-micro
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn BUILD_TYPE=release CORE_OPTIMIZATION_LEVEL=-O3 KERNEL_OPTIMIZATION_LEVEL=-O3 THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-O3 microlite
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn BUILD_TYPE=release_with_logs CORE_OPTIMIZATION_LEVEL=-O3 KERNEL_OPTIMIZATION_LEVEL=-O3 THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-O3 microlite
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn BUILD_TYPE=debug microlite
```

The will generate a static libraries at
- tflite_micro/gen/cortex_m_generic_cortex-m4+fp_release/lib/libtensorflow-microlite.a
- tflite_micro/gen/cortex_m_generic_cortex-m4+fp_release_with_logs/lib/libtensorflow-microlite.a
- tflite_micro/gen/cortex_m_generic_cortex-m4+fp_debug/lib/libtensorflow-microlite.a

## Create TFLM minimal code tree
TFLM includes a script to generate a code tree with only the header and license files needed to link the library in. Run it thusly:

```bash
cd tflite-micro
python ./tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py --makefile_options "TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn" treedir
```

## Copy needed files into neuralSPOT
neuralSPOT only needs the static libraries and minimal code tree.

```bash
cd neuralSPOT/extern/tensorflow/new_version_dir
mkdir lib
cp .../tflite_micro/gen/cortex_m_generic_cortex-m4+fp_debug/lib/libtensorflow-microlite-debug.a lib/libtensorflow-microlite-cm4-gcc-debug.a
cp .../tflite_micro/gen/cortex_m_generic_cortex-m4+fp_release/lib/libtensorflow-microlite.a lib/libtensorflow-microlite-cm4-gcc-release.a
cp .../tflite_micro/gen/cortex_m_generic_cortex-m4+fp_release_with_logs/lib/libtensorflow-microlite-withlogs.a lib/libtensorflow-microlite-cm4-gcc-release-with-logs.a
cp -R .../tflite_micro/treedir . # treedir is from the 'minimal code tree' step above
cp ../0264234_Nov_15_2023/module.mk . # use an existing module.mk as a starting point
```

## Update neuralSPOT makefiles to point to new version
There are two steps here:
1. Modify the module.mk as needed (typically only needed if TFLM added/changed directory structures)
2. Point TF_VERSION (defined in make/neuralspot_config.mk) to new version

## Resolve code changes
TFLM occasionally breaks API and/or code structure compatibility. neuralSPOT `#defines` a macro to help delineate these changes to maintain backwards compatiblity (`NS_TF_VERSION_<new version>`).
neuralSPOT's makefiles also define `NS_TFSTRUCTURE_RECENT` to identify releases after the micro_reporter refactor (which changed directory structures and got rid of the 'all ops resolver'). Add the new version to where neuralspot_config.mk sets this define.

# Compiling TFLM with Armclang on Windows WSL
TFLM doesn't support compiling on Windows. Firstly, its scripts use Linux commands, so the 'make' has to be run in WSL. Make sure you clone the directory in WSL too, otherwise you'll get a bunch of ^M characters all over the place and nothing will run. WSL has a bunch of quirks that need to be worked around as well (binary names and CLI limits)

1. Make within WSL, not other shells
2. Clone in WSL, not other shells
3. Binary names are munged. You'll have to change them in the makefile (alias doesn't work for some reason)
4. We need a few flags to make the resulting libs compatible with neuralSPOT (specifically AmbiqSuite)
5. Windows (even WSL) doesn't support CLI commands over 5000 characters (because we're stuck in 1994, apparently), so you'll have to modify another makefile to break apart the archive step.

## Makefile Changes
First, cortex_m_generic_makefile.inc:

```
 diff  tensorflow/lite/micro/tools/make/targets/cortex_m_generic_makefile.inc ../../tflm-nov-10/tflite-micro/tensorflow/lite/micro/tools/make/targets
/cortex_m_generic_makefile.inc
121,124c121,124
<   CXX_TOOL  := armclang.exe
<   CC_TOOL   := armclang.exe
<   AR_TOOL   := armar.exe
<   LD        := armlink.exe
---
>   CXX_TOOL  := armclang
>   CC_TOOL   := armclang
>   AR_TOOL   := armar
>   LD        := armlink
128,130d127
<     -fshort-enums \
<     -gdwarf-4 \
<     -fshort-wchar \
```

Also (see below, this no longer works on latest)
```
diff --git a/tensorflow/lite/micro/tools/make/Makefile b/tensorflow/lite/micro/tools/make/Makefile
index 08eb057e..816de78d 100644
--- a/tensorflow/lite/micro/tools/make/Makefile
+++ b/tensorflow/lite/micro/tools/make/Makefile
@@ -835,8 +835,13 @@ microlite: $(MICROLITE_LIB_PATH)
 # Gathers together all the objects we've compiled into a single '.a' archive.
 $(MICROLITE_LIB_PATH): $(MICROLITE_LIB_OBJS) $(MICROLITE_KERNEL_OBJS) $(MICROLITE_THIRD_PARTY_OBJS) $(MICROLITE_THIRD_PARTY_KERNEL_OBJS) $(MICROLITE_CUSTOM_OP_OBJS)
        @mkdir -p $(dir $@)
-       $(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_LIB_OBJS) \
-       $(MICROLITE_KERNEL_OBJS) $(MICROLITE_THIRD_PARTY_OBJS) $(MICROLITE_THIRD_PARTY_KERNEL_OBJS) $(MICROLITE_CUSTOM_OP_OBJS)
+# Windows Sucks, specifically the 5000 character limit on command line arguments
+       $(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_LIB_OBJS)
+       $(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_KERNEL_OBJS)
+       $(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_THIRD_PARTY_OBJS)
+       $(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_THIRD_PARTY_KERNEL_OBJS)
+       $(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_CUSTOM_OP_OBJS)
+#      $(MICROLITE_KERNEL_OBJS) $(MICROLITE_THIRD_PARTY_OBJS) $(MICROLITE_THIRD_PARTY_KERNEL_OBJS) $(MICROLITE_CUSTOM_OP_OBJS)

 $(BINDIR)%_test : $(CORE_OBJDIR)%_test.o $(MICROLITE_LIB_PATH)
        @mkdir -p $(dir $@)
```

Commands
Finally, the command to make is a bit different:

```
 make -f ./tensorflow/lite/micro/tools/make/Makefile TOOLCHAIN=armclang TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn  BUILD_TYPE=release microlite
```

> *NOTE*: setting -Omax causes all kinds of trouble. Don't do it.


# A few updates
Optimizations: modify the optimization levels in Makefile:
```
ifeq ($(TOOLCHAIN), armclang)
  CORE_OPTIMIZATION_LEVEL := -Ofast
  KERNEL_OPTIMIZATION_LEVEL := -Ofast
  THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL := -Ofast
else
  CORE_OPTIMIZATION_LEVEL := -Os
  KERNEL_OPTIMIZATION_LEVEL := -O2
  THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL := -O2
endif
```

A better AR patch (note that the first AR includes two lists instead of just one. This is because they both contain kernel_utils.o (different files in different paths, but same name), and ARing each separately was causing AR to replace rather than append)
```
# Windows Sucks, specifically the 5000 character limit on command line arguments
# Gathers together all the objects we've compiled into a single '.a' archive.
$(MICROLITE_LIB_PATH): $(MICROLITE_LIB_OBJS) $(MICROLITE_KERNEL_OBJS) $(MICROLITE_THIRD_PARTY_OBJS) $(MICROLITE_THIRD_PARTY_KERNEL_OBJS) $(MICROLITE_CUSTOM_OP_OBJS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_KERNEL_OBJS) $(MICROLITE_LIB_OBJS)
	$(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_THIRD_PARTY_OBJS)
	$(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_THIRD_PARTY_KERNEL_OBJS)
	$(AR) $(ARFLAGS) $(MICROLITE_LIB_PATH) $(MICROLITE_CUSTOM_OP_OBJS)
```
