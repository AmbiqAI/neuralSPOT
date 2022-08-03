set(APP_NAME "hello_world")

add_executable(${APP_NAME}.axf main.c)
target_link_libraries(${APP_NAME}.axf PRIVATE ambiqsuite ns-ipc ns-audio)
generate_other_files(${APP_NAME})
