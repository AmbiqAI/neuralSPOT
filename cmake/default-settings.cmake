set(DEFAULT_TOOLCHAIN       arm-none-eabi-gcc)
set(CMAKE_TOOLCHAIN_FILE    ${CMAKE_CURRENT_LIST_DIR}/arm-gcc-toolchain.cmake)

set(EVB_ARM_TOOLCHAIN_PATH    "~/AmbiqDev/gcc-arm-none-eabi-10.3-2021.07/bin")
set(CPU                       "cortex-m4")
set(FPU                       "fpv4-sp-d16")
set(FABI                      "hard")

add_compile_definitions(PART_apollo4b)
add_compile_definitions(AM_PART_APOLLO4B)
add_compile_definitions(DAM_PACKAGE_BGA)
add_compile_definitions(gcc)
add_compile_definitions(DTF_LITE_STATIC_MEMORY)
