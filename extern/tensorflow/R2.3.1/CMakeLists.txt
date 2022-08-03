# This cmake file causes ambiqsuite to be compiled and eventually linked into targets

# This is the code that is outside the precompiled libraries
#add_library(tflm STATIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/libtensorflow-microlite.a)
#target_link_libraries(tflm STATIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/libtensorflow-microlite.a)
# Pre-compiled AmbiqSuite static libraries
#file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_library(tlfm STATIC IMPORTED GLOBAL)
#add_library(tlfm INTERFACE)
#target_link_libraries(tlfm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libtensorflow-microlite.a})
# target_include_directories(tflm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
#set(THIS_IS_STUPID  )
set_target_properties(tlfm PROPERTIES NO_SYSTEM_FROM_IMPORTED true)
set_target_properties(tlfm PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libtensorflow-microlite.a)
set_property(TARGET tlfm PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ruy/ruy/profiler
    ${CMAKE_CURRENT_SOURCE_DIR}/tensorflow-includes
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gemmlowp/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gemmlowp/fixedpoint
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/kissfft
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/flatbuffers/include
)

message(STATUS "We hit TF with ${CMAKE_CURRENT_SOURCE_DIR}")
# Make the header files accessible to the target binary#
# target_include_directories(tflm INTERFACE 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ruy/profiler
#     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gemmlowp/internal
#     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gemmlowp/fixedpoint
#     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/kissff
#     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tools
#     ${CMAKE_CURRENT_SOURCE_DIR}/third_party/flatbuffers/include
# )

# Force linked to resolve circular library dependencies -
#target_link_libraries(ambiqsuite PRIVATE
#    "$<LINK_GROUP:RESCAN,-lm,-lc,-lgcc,-lnosys,libam_hal,libam_bsp,libarm_cortexM4l_math,libarm_cortexM4lf_math>"
#)

#target_link_options(ambiqsuite PUBLIC -T ${CMAKE_CURRENT_SOURCE_DIR}/src/linker_script.ld)