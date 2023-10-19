This component contains libs compiled for several toolchains, platforms, and targets



Compiling for amclang + apollo4
The makefile inc has to be modified to produce libs compatible with the rest of our stack:
  FLAGS_ARMC = \
    --target=arm-arm-none-eabi \
    -fshort-enums \
    -gdwarf-4 \
    -fshort-wchar \
    -mcpu=$(TARGET_ARCH)

 make -f ./tensorflow/lite/micro/tools/make/Makefile TOOLCHAIN=armclang TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn CORE_OPTIMIZATION_LEVEL=-Ofast THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-Ofast BUILD_TYPE=release microlite

 make -f ./tensorflow/lite/micro/tools/make/Makefile TOOLCHAIN=armclang TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn CORE_OPTIMIZATION_LEVEL=-Ofast THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-Ofast BUILD_TYPE=release_with_logs microlite

Compiling for gcc + apollo4
 make -f ./tensorflow/lite/micro/tools/make/Makefile TOOLCHAIN=gcc TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn CORE_OPTIMIZATION_LEVEL=-O3 THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-O3 BUILD_TYPE=release microlite




  make -f ./tensorflow/lite/micro/tools/make/Makefile TOOLCHAIN=armclang TARGET=cortex_m_generic TARGET_ARCH=cortex-m55 OPTIMIZED_KERNEL_DIR=cmsis_nn CORE_OPTIMIZATION_LEVEL=-Omax THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=-Omax BUILD_TYPE=release microlite