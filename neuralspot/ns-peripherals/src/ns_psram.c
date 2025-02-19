/**
 * @file ns_psram.c
 * @author Ambiq
 * @brief Initialize PSRAM per the configuration struct
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"
#include "ns_peripherals_psram.h"

const ns_core_api_t ns_psram_V0_0_1 = {.apiId = NS_PSRAM_API_ID, .version = NS_PSRAM_V0_0_1};

const ns_core_api_t ns_psram_oldest_supported_version = {
    .apiId = NS_PSRAM_API_ID, .version = NS_PSRAM_V0_0_1};

const ns_core_api_t ns_psram_current_version = {
    .apiId = NS_PSRAM_API_ID, .version = NS_PSRAM_V0_0_1};

extern uint32_t ns_psram_platform_init(ns_psram_config_t *cfg);

uint32_t ns_psram_init(ns_psram_config_t *cfg) {
    uint32_t ui32IntStatus = NS_STATUS_SUCCESS;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            cfg->api, &ns_psram_oldest_supported_version, &ns_psram_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
    ui32IntStatus = ns_psram_platform_init(cfg);
    return ui32IntStatus;
}
