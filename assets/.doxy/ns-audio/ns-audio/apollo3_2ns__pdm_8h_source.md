

# File ns\_pdm.h

[**File List**](files.md) **>** [**apollo3**](dir_1a531e93c11b219ab9869f377e190a5d.md) **>** [**ns\_pdm.h**](apollo3_2ns__pdm_8h.md)

[Go to the documentation of this file](apollo3_2ns__pdm_8h.md)


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

extern uint32_t pdm_init(ns_audio_config_t *config);
extern void pdm_deinit(ns_audio_config_t *config);
#ifdef __cplusplus
}
#endif

#endif // NS_PDM_AUDIO
```


