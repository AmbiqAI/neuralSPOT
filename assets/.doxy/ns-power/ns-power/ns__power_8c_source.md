

# File ns\_power.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-peripherals**](dir_62cbd78784261bb0d09981988628a167.md) **>** [**src**](dir_cf9eff0d6bf97258df730d615fa0f132.md) **>** [**ns\_power.c**](ns__power_8c.md)

[Go to the documentation of this file](ns__power_8c.md)

```C++


//*****************************************************************************
//
// Copyright (c) Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite
// Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"
#include "ns_peripherals_power.h"

const ns_core_api_t ns_power_V0_0_1 = {.apiId = NS_POWER_API_ID, .version = NS_POWER_V0_0_1};

const ns_core_api_t ns_power_V1_0_0 = {.apiId = NS_POWER_API_ID, .version = NS_POWER_V1_0_0};

const ns_core_api_t ns_power_oldest_supported_version = {
    .apiId = NS_POWER_API_ID, .version = NS_POWER_V0_0_1};

const ns_core_api_t ns_power_current_version = {
    .apiId = NS_POWER_API_ID, .version = NS_POWER_V1_0_0};

const ns_power_config_t ns_development_default = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = true,
    .bNeedIOM = true,
    .bNeedAlternativeUART = true,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = true,
    .bNeedXtal = true};

const ns_power_config_t ns_debug_default = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = false,
    .bNeedBluetooth = true,
    .bNeedUSB = true,
    .bNeedIOM = true,
    .bNeedAlternativeUART = true,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = true,
    .bNeedXtal = true};

const ns_power_config_t ns_good_default = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = true,
    .bNeedXtal = false};

const ns_power_config_t ns_mlperf_mode1 = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = true,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false,
    .bNeedXtal = false};

const ns_power_config_t ns_mlperf_mode2 = {
    .eAIPowerMode = NS_MINIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = true,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false,
    .bNeedXtal = false};

const ns_power_config_t ns_mlperf_mode3 = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = true,
    .b128kTCM = true,
    .bEnableTempCo = false,
    .bNeedITM = false,
    .bNeedXtal = false};

const ns_power_config_t ns_audio_default = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .api = &ns_power_V1_0_0,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = false,
    .bNeedXtal = false};

extern uint32_t ns_set_performance_mode(ns_power_mode_e eAIPowerMode);
extern uint32_t ns_power_platform_config(const ns_power_config_t *pCfg);

// Main function for power configuration
uint32_t ns_power_config(const ns_power_config_t *pCfg) {
    uint32_t ui32ReturnStatus = AM_HAL_STATUS_SUCCESS;

#ifndef NS_DISABLE_API_VALIDATION
    if (pCfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            pCfg->api, &ns_power_oldest_supported_version, &ns_power_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }

    if (!ns_core_initialized()) {
        // Power needs ns_core to be initialized first
        return NS_STATUS_INIT_FAILED;
    }
#endif

    ui32ReturnStatus = ns_power_platform_config(pCfg);

    return ui32ReturnStatus;
}

extern void ns_platform_deep_sleep(void);

void ns_deep_sleep(void) { ns_platform_deep_sleep(); }

```

