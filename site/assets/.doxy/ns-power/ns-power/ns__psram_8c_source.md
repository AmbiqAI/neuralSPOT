

# File ns\_psram.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_psram.c**](ns__psram_8c.md)

[Go to the documentation of this file](ns__psram_8c.md)


```C++

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
```


