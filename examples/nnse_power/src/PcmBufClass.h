#ifndef __PCM_BUF_CLASS_H__
#define __PCM_BUF_CLASS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

// a block of pcm buffer
typedef struct {
    int16_t *pcm_buffer;
    int16_t idx_set;
    int16_t idx_data_latest;
    int16_t num_frs;
    int16_t smpls_fr;
} PcmBufClass;

void PcmBufClass_init(PcmBufClass *pt_inst);

void PcmBufClass_reset(PcmBufClass *pt_inst);

void PcmBufClass_setData(PcmBufClass *pt_inst, int16_t *pcm_input);

void PcmBufClass_getData(
    PcmBufClass *pt_inst, int16_t lookbk_frs, int16_t fetching_frames, int16_t *pcm_out);

#ifdef __cplusplus
}
#endif
#endif
