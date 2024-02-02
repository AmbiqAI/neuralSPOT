/**
 * @brief Power Control Utilities
 *
 *
 */

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

uint32_t ns_set_performance_mode(ns_power_mode_e eAIPowerMode) {
    am_hal_burst_mode_e eBurstMode;

    // Configure power mode
    if ((eAIPowerMode == NS_MAXIMUM_PERF) || (eAIPowerMode == NS_MEDIUM_PERF))
        return am_hal_burst_mode_enable(&eBurstMode);
    else
        return am_hal_burst_mode_disable(&eBurstMode);
}

//*****************************************************************************
//
// Internal method for turning off peripherals
//
//*****************************************************************************
void ns_power_down_peripherals(const ns_power_config_t *pCfg) {}

// Main function for power configuration
uint32_t ns_power_platform_config(const ns_power_config_t *pCfg) {
    uint32_t ui32ReturnStatus = AM_HAL_STATUS_SUCCESS;

    am_bsp_low_power_init();

    g_ns_state.cryptoWantsToBeEnabled = pCfg->bNeedCrypto;
    g_ns_state.cryptoCurrentlyEnabled = pCfg->bNeedCrypto;
    g_ns_state.itmPrintWantsToBeEnabled = pCfg->bNeedITM;

    return ui32ReturnStatus;
}

/**
 * @brief Wraps am_hal_sysctrl_sleep to enable and disable
 * systems as needed.
 *
 */
void ns_platform_deep_sleep(void) {

    //     if (g_ns_state.uartPrintCurrentlyEnabled) {
    //         am_bsp_uart_printf_disable();
    //         g_ns_state.uartPrintCurrentlyEnabled = false;
    //     } else if (g_ns_state.itmPrintCurrentlyEnabled) {
    //         ns_cryptoless_itm_printf_disable();
    //         g_ns_state.itmPrintCurrentlyEnabled = false;
    //     }

    //     if (g_ns_state.cryptoCurrentlyEnabled) {
    //         am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
    //         g_ns_state.cryptoCurrentlyEnabled = false;
    //     }

    // #if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L)
    //     // TEMPCO not supported in this version
    // #else
    //     if (g_ns_state.tempcoCurrentlyEnabled) {
    //         am_hal_adc_power_control(g_ns_tempco_ADCHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);
    //     }
    // #endif

    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    // if (g_ns_state.tempcoWantsToBeEnabled) { // isr turns ADC on
    // }
}
