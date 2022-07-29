add_executable(ns-hello-world hello_world.c)
#target_link_libraries(ns-hello-world PRIVATE -Wl,--start-group)
target_link_libraries(ns-hello-world PRIVATE ambiqsuite ns-ipc ns-audio)
#target_link_libraries(ns-hello-world PRIVATE -Wl,--end-group)
