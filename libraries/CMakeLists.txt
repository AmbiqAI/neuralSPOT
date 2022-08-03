
# The ns-audio library facilitates interaction with audio peripherals and contains audio-centric
# AI feature extraction helpers
add_library(ns-ipc STATIC 
    ns-ipc/am_app_utils_ring_buffer.c 
)
target_include_directories(ns-ipc PUBLIC "ns-ipc")
target_link_libraries(ns-ipc PRIVATE ambiqsuite)

add_library(ns-peripherals STATIC 
    ns-peripherals/button.c 
    ns-peripherals/power.c
    ns-peripherals/debug_log.cc
)
target_include_directories(ns-peripherals PUBLIC "ns-peripherals")
target_link_libraries(ns-peripherals PRIVATE ambiqsuite tlfm ns-ipc)
target_link_libraries(ns-peripherals INTERFACE tlfm)

add_library(ns-audio STATIC 
    ns-audio/am_ai_audio.c 
    ns-audio/am_ai_audadc.c
    ns-audio/mfcc.c
)
target_include_directories(ns-audio PUBLIC "ns-audio")
target_link_libraries(ns-audio PRIVATE ambiqsuite ns-ipc)