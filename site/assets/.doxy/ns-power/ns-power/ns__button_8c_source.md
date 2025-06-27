

# File ns\_button.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_button.c**](ns__button_8c.md)

[Go to the documentation of this file](ns__button_8c.md)


```C++
//*****************************************************************************
//
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
    // ns_lp_printf("ns_peripheral_button_init\n");
    ui32IntStatus = ns_button_platform_init(cfg);

    return ui32IntStatus;
}
```


