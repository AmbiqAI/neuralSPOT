#ifndef __AUDIO_PIPE_WRAPPER_H__
#define __AUDIO_PIPE_WRAPPER_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
int AudioPipe_wrapper_init(void);
int AudioPipe_wrapper_reset(void);
int AudioPipe_wrapper_frameProc(
        int16_t *pcm_input,
        int16_t *pcm_output);
#ifdef __cplusplus
}
#endif
#endif
