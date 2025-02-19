/**
 * @file ns_pdm.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-05-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NS_PDM_AUDIO
#define NS_PDM_AUDIO

#ifdef __cplusplus
extern "C" {
#endif

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_audio.h"

#define NS_AUDIO_PDM_SAMPLE_16BIT 2

extern ns_pdm_cfg_t ns_pdm_default;

/// AUDADC subsystem init - should only be invoked by ns_audio, not directly
extern uint32_t
pdm_init(ns_audio_config_t *config);

#ifdef __cplusplus
}
#endif

#endif // NS_PDM_AUDIO
