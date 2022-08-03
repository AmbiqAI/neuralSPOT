set(DEFAULT_TOOLCHAIN       arm-none-eabi-gcc)
set(CMAKE_TOOLCHAIN_FILE    ${CMAKE_CURRENT_LIST_DIR}/arm-gcc-toolchain.cmake)

set(EVB_ARM_TOOLCHAIN_PATH    "~/AmbiqDev/gcc-arm-none-eabi-10.3-2021.07/bin")
set(CPU                       "cortex-m4")
set(FPU                       "fpv4-sp-d16")
set(FABI                      "hard")

set(part                      apollo4b)
string(TOUPPER ${part}        PART) # AmbiqSuite uses both upper and lower case defs
set(EVB                       evb)

add_compile_definitions(PART_${part})
add_compile_definitions(AM_PART_${PART})
add_compile_definitions(AM_PACKAGE_BGA)
add_compile_definitions(gcc)
add_compile_definitions(TF_LITE_STATIC_MEMORY)
add_compile_definitions(TF_LITE_STRIP_ERROR_STRINGS)