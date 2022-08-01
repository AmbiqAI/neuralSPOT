
add_executable(${TARGET_NAME}.axf hello_world.c)
target_link_libraries(${TARGET_NAME}.axf PRIVATE ambiqsuite ns-ipc ns-audio)