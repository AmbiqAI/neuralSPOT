/**
 * @file ns_timer.c
 * @author Carlos Morales
 * @brief Simple timer facility
 * @version 0.1
 * @date 2022-10-11
 *
 * @copyright Copyright (c) 2022
 *
 */

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
