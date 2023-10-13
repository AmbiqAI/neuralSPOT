

# File ns\_core.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-core**](dir_7a01d249276e526cbac17daf32597066.md) **>** [**src**](dir_2b540daedd13b1486e7bf344eba1e313.md) **>** [**ns\_core.c**](ns__core_8c.md)

[Go to the documentation of this file](ns__core_8c.md)

```C++


#include "ns_core.h"

ns_desired_state_t g_ns_state;

//*** API Versions
#define NS_CORE_API_ID 0xCA0000

const ns_core_api_t ns_core_V0_0_1 = {.apiId = NS_CORE_API_ID, .version = NS_CORE_V0_0_1};

const ns_core_api_t ns_core_V1_0_0 = {.apiId = NS_CORE_API_ID, .version = NS_CORE_V1_0_0};

const ns_core_api_t ns_core_oldest_supported_version = {
    .apiId = NS_CORE_API_ID, .version = NS_CORE_V0_0_1};

const ns_core_api_t ns_core_current_version = {.apiId = NS_CORE_API_ID, .version = NS_CORE_V1_0_0};
//***

static int semver_compare(const ns_semver_t *c, const ns_semver_t *n) {
    uint64_t cMajor = (uint64_t)c->major;
    uint64_t cMinor = (uint64_t)c->minor;
    uint64_t nMajor = (uint64_t)n->major;
    uint64_t nMinor = (uint64_t)n->minor;
    uint64_t c64 = (cMajor << 32) + (cMinor << 16) + c->revision;
    uint64_t n64 = (nMajor << 32) + (nMinor << 16) + n->revision;
    if (c64 == n64) {
        return 0;
    } else if (c64 < n64) {
        return -1;
    } else {
        return 1;
    }
}

extern uint32_t ns_core_check_api(
    const ns_core_api_t *submitted, const ns_core_api_t *oldest, const ns_core_api_t *newest) {

    if (submitted->apiId != newest->apiId) {
        return NS_STATUS_INVALID_VERSION;
    }

    // check version > oldest and < newest
    int resolution = semver_compare(&(submitted->version), &(oldest->version));
    if (resolution == -1) {
        // submitted version is lower than oldest supported version
        return NS_STATUS_INVALID_VERSION;
    }
    resolution = semver_compare(&(submitted->version), &(newest->version));
    if (resolution == 1) {
        // submitted version is higher than newest supported version
        return NS_STATUS_INVALID_VERSION;
    }

    return NS_STATUS_SUCCESS;
}

uint32_t ns_core_init(ns_core_config_t *cfg) {
#ifndef NS_DISABLE_API_VALIDATION
    //
    // Check the handle.
    //
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    // check API version
    if (ns_core_check_api(cfg->api, &ns_core_oldest_supported_version, &ns_core_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
    g_ns_state.api = cfg->api;
    g_ns_state.initialized = true;
    g_ns_state.itmPrintWantsToBeEnabled = false;
    g_ns_state.itmPrintCurrentlyEnabled = false;
    g_ns_state.uartPrintWantsToBeEnabled = false;
    g_ns_state.uartPrintCurrentlyEnabled = false;
    g_ns_state.tempcoWantsToBeEnabled = false;
    g_ns_state.tempcoCurrentlyEnabled = false;
    g_ns_state.cryptoWantsToBeEnabled = true; // reset state of Crypto is 'on'
    g_ns_state.cryptoCurrentlyEnabled = true;
    return NS_STATUS_SUCCESS;
}

void ns_core_fail_loop() {
    while (1)
        ;
}

```

