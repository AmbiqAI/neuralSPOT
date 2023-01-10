# Tensorflow Lite for Microcontrollers
NeuralSPOT support 3 versions of TFLM - as new versions are added, the oldest are deprecated (not necessarily deleted, just not tested).

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
To compile the release version:
```bash
cd tflite-micro
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn BUILD_TYPE=release CORE_OPTIMIZATION_LEVEL=-O3 KERNEL_OPTIMIZATION_LEVEL=-O3 THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-O3 microlite
```
The will generate a static library at tflite_micro/gen/cortex_m_generic_cortex-m4+fp_release/lib/libtensorflow-microlite.a

To compile a debug version of TFLM:
```bash
cd tflite-micro
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn BUILD_TYPE=debug microlite
```
The will generate a static library at tflite_micro/gen/cortex_m_generic_cortex-m4+fp_debug/lib/libtensorflow-microlite.a

## Create TFLM minimal code tree
TFLM includes a script to generate a code tree with only the header and license files needed to link the library in. Run it thusly:

```bash
cd tflite-micro
python ./tensorflow/lite/micro/tools/project_generation/create_tflm_tree.py treedir
```
--makefile_options
## Copy needed files into neuralSPOT
neuralSPOT only needs the static libraries and minimal code tree.

```bash
cd neuralSPOT/extern/tensorflow/new_version_dir
mkdir lib
cp .../tflite_micro/gen/cortex_m_generic_cortex-m4+fp_debug/lib/libtensorflow-microlite-debug.a
cp .../tflite_micro/gen/cortex_m_generic_cortex-m4+fp_release/lib/libtensorflow-microlite.a
cp -R .../tflite_micro/treedir . # treedir is from the 'minimal code tree' step above
cp ../fecdd5d/module.mk . # use an existing module.mk as a starting point
```

## Update neuralSPOT makefiles to point to new version
There are two steps here:
1. Modify the module.mk as needed (typically only needed if TFLM added/changed directory structures)
2. Point TF_VERSION (defined in make/neuralspot_config.mk) to new version

## Resolve code changes
TFLM occasionally breaks API and/or code structure compatibility. neuralSPOT `#defines` a macro to help delineate these changes to maintain backwards compatiblity (`NS_TF_VERSION_<new version>`).

