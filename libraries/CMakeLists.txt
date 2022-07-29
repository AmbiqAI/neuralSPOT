
# The ns-audio library facilitates interaction with audio peripherals and contains audio-centric
# AI feature extraction helpers
add_library(ns-ipc STATIC 
    src/ns-ipc/am_app_utils_ring_buffer.c 
)
target_include_directories(ns-ipc PUBLIC "${CMAKE_SOURCE_DIR}/include/ns-ipc")
target_link_libraries(ns-ipc PRIVATE ambiqsuite)

add_library(ns-audio STATIC 
    src/ns-audio/am_ai_audio.c 
    src/ns-audio/am_ai_audadc.c
)
target_include_directories(ns-audio PUBLIC "${CMAKE_SOURCE_DIR}/include/ns-audio")
target_link_libraries(ns-audio PRIVATE ambiqsuite ns-ipc)