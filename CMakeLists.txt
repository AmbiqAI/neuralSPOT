cmake_minimum_required(VERSION 3.24)

include(cmake/default-settings.cmake)
# include(cmake/arm-gcc-toolchain.cmake)
# include(cmake/ambiq-ai-options.cmake)

project(neuralspot VERSION 0.0.1 LANGUAGES ASM C CXX)
set(TARGET_NAME "hello_world" CACHE STRING "Target Application") # default

add_subdirectory(extern)
add_subdirectory(libraries)
add_subdirectory(examples)