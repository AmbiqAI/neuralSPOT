

# File ns\_timer.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_timer.c**](ns__timer_8c.md)

[Go to the documentation of this file](ns__timer_8c.md)

```C++


#include "ns_timer.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"

const ns_core_api_t ns_timer_V0_0_1 = {.apiId = NS_TIMER_API_ID, .version = NS_TIMER_V0_0_1};

const ns_core_api_t ns_timer_V1_0_0 = {.apiId = NS_TIMER_API_ID, .version = NS_TIMER_V1_0_0};

const ns_core_api_t ns_timer_oldest_supported_version = {
    .apiId = NS_TIMER_API_ID, .version = NS_TIMER_V0_0_1};

const ns_core_api_t ns_timer_current_version = {
    .apiId = NS_TIMER_API_ID, .version = NS_TIMER_V1_0_0};

ns_timer_config_t *ns_timer_config[NS_TIMER_TEMPCO + 1];

extern bool ns_core_initialized(void) { return g_ns_state.initialized; }
extern uint32_t ns_timer_platform_init(ns_timer_config_t *cfg);

uint32_t ns_timer_init(ns_timer_config_t *cfg) {
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            cfg->api, &ns_timer_oldest_supported_version, &ns_timer_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if (cfg->timer > NS_TIMER_TEMPCO) {
        return NS_STATUS_INVALID_CONFIG;
    }
    if ((cfg->enableInterrupt) && (cfg->callback == NULL)) {
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    ns_timer_config[cfg->timer] = cfg;

    ui32Status = ns_timer_platform_init(cfg);

    return ui32Status;
}

```

