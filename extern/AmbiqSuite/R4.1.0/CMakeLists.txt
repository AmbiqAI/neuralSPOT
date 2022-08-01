# This cmake file causes ambiqsuite to be compiled and eventually linked into targets

# This is the code that is outside the precompiled libraries
add_library(ambiqsuite STATIC
    src/startup_gcc.c
    src/am_resources.c
    src/am_util_delay.c
    src/am_util_id.c
    src/am_util_stdio.c
)

# Pre-compiled AmbiqSuite static libraries
add_library(libam_hal STATIC IMPORTED)
set_target_properties(libam_hal PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/apollo4b/libam_hal.a)
add_library(libam_bsp STATIC IMPORTED)
set_target_properties(libam_bsp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/apollo4b/libam_bsp.a)
add_library(libarm_cortexM4l_math STATIC IMPORTED)
set_target_properties(libarm_cortexM4l_math PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libarm_cortexM4l_math.a)
add_library(libarm_cortexM4lf_math STATIC IMPORTED)
set_target_properties(libarm_cortexM4lf_math PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libarm_cortexM4lf_math.a)

# Make the header files accessible to the target binary
target_include_directories(ambiqsuite PUBLIC
    boards/apollo4b_evb/bsp
    CMSIS/ARM/Include
    CMSIS/AmbiqMicro/Include
    devices
    mcu/apollo4b
    utils
)

# Force linked to resolve circular library dependencies -
target_link_libraries(ambiqsuite PRIVATE
    "$<LINK_GROUP:RESCAN,-lm,-lc,-lgcc,-lnosys,libam_hal,libam_bsp,libarm_cortexM4l_math,libarm_cortexM4lf_math>"
)

target_link_options(ambiqsuite PUBLIC -T ${CMAKE_CURRENT_SOURCE_DIR}/src/linker_script.ld)