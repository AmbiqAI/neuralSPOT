cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "neuralspot")

include(cmake/default-settings.cmake)
include(cmake/arm-gcc-toolchain.cmake)
# include(cmake/ambiq-ai-options.cmake)

project(${TARGET_NAME} VERSION 0.0.1 LANGUAGES ASM C CXX)

add_subdirectory(extern)
add_subdirectory(libraries)
add_subdirectory(examples)
