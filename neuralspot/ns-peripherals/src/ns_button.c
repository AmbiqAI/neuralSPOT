//*****************************************************************************
//
//! @file button.c
//!
//! @brief Utility for reading EVB Buttons.
//!
//! Purpose: Reading EVB buttons
//!
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"
#include "ns_peripherals_button.h"

const ns_core_api_t ns_button_V0_0_1 = {.apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V0_0_1};

const ns_core_api_t ns_button_V1_0_0 = {.apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V1_0_0};

const ns_core_api_t ns_button_oldest_supported_version = {
    .apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V0_0_1};

const ns_core_api_t ns_button_current_version = {
    .apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V1_0_0};

int volatile *g_ns_peripheral_button0;
int volatile *g_ns_peripheral_button1;
int volatile *g_ns_peripheral_joulescope_trigger;

void ns_button_0_handler(void *pArg) { *g_ns_peripheral_button0 = 1; }
void ns_button_1_handler(void *pArg) { *g_ns_peripheral_button1 = 1; }
void ns_joulescope_trigger_handler(void *pArg) { *g_ns_peripheral_joulescope_trigger = 1; }

extern uint32_t ns_button_platform_init(ns_button_config_t *cfg);

uint32_t ns_peripheral_button_init(ns_button_config_t *cfg) {
    uint32_t ui32IntStatus = NS_STATUS_SUCCESS;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            cfg->api, &ns_button_oldest_supported_version, &ns_button_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
    ui32IntStatus = ns_button_platform_init(cfg);

    return ui32IntStatus;
}
