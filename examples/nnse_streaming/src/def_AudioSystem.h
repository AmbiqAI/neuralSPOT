#ifndef __AUDIO_SYSTEM_WRAPPER_H__
#define __AUDIO_SYSTEM_WRAPPER_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "ns_audio.h"
#define NUM_CHANNELS 1
#define NUM_FRAMES 100
#define SAMPLES_IN_FRAME 160
#define SAMPLE_RATE 16000
#define AUDIO_SOURCE NS_AUDIO_SOURCE_PDM
#define MODEL_FORMAT_TFLM 0
#ifdef __cplusplus
}
#endif
#endif
