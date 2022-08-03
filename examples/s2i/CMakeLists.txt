set(APP_NAME "s2i")
add_executable(${APP_NAME}.axf main.cc slu_model_act8.h)
set_target_properties(${APP_NAME}.axf PROPERTIES NO_SYSTEM_FROM_IMPORTED ON) # cmake is not the best
target_link_libraries(${APP_NAME}.axf PRIVATE ambiqsuite ns-ipc ns-peripherals ns-audio)
target_link_libraries(${APP_NAME}.axf INTERFACE tflm)
generate_other_files(${APP_NAME})
