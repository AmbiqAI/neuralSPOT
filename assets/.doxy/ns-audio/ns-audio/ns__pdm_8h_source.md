

# File ns\_pdm.h

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-audio**](dir_45211a8475460839574f71aa108f4957.md) **>** [**src**](dir_e70eef2d5115541d1d6cb7ad27f30382.md) **>** [**ns\_pdm.h**](ns__pdm_8h.md)

[Go to the documentation of this file](ns__pdm_8h.md)

```C++


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

extern uint32_t
pdm_init(ns_audio_config_t *config);

#ifdef __cplusplus
}
#endif

#endif // NS_PDM_AUDIO

```

