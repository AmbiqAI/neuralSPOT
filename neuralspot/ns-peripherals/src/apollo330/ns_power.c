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
#ifdef apollo510b_evb
#include "am_devices_em9305.h"
#endif

#include "am_hal_clkmgr.h"
#include "am_hal_spotmgr.h"

static ns_power_mode_e current_power_mode = NS_POWER_MODE_NOT_SET;
static ns_power_mode_e desired_power_mode = NS_POWER_MODE_NOT_SET;
uint32_t ns_set_performance_mode(ns_power_mode_e eAIPowerMode) {
    // Configure power mode
    uint32_t retval = NS_STATUS_SUCCESS;
    current_power_mode = eAIPowerMode;
    desired_power_mode = eAIPowerMode;
    if (eAIPowerMode == NS_MAXIMUM_PERF) {
        retval = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2);
    }
    else if (eAIPowerMode == NS_MEDIUM_PERF) {
        retval = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1);
    } else {
        retval = am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
    }

    return retval;
}

uint32_t ns_power_pdm_workaround_pre(void) {
    if (current_power_mode == NS_MAXIMUM_PERF) {
        am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1);
        current_power_mode = NS_MEDIUM_PERF;
    }
    return NS_STATUS_SUCCESS;
}

uint32_t ns_power_pdm_workaround_post(void) {
    if (current_power_mode != desired_power_mode) {
        ns_set_performance_mode(desired_power_mode);
    }
    return NS_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Internal method for turning off peripherals
//
//*****************************************************************************
void ns_power_down_peripherals(const ns_power_config_t *pCfg) {

    //
    // Disable the XTAL.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC, 0);

    //
    // Turn off the voltage comparator.
    //
    VCOMP->PWDKEY = _VAL2FLD(VCOMP_PWDKEY_PWDKEY, VCOMP_PWDKEY_PWDKEY_Key);


}

void ns_power_memory_config(const ns_power_config_t *pCfg) {
    // configure SRAM & other memories
    #define ALL_RETAIN 1
    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eROMMode       = AM_HAL_PWRCTRL_ROM_AUTO,

        .eROMMode       = AM_HAL_PWRCTRL_ROM_AUTO,
#if ALL_RETAIN
#if defined(AM_PART_APOLLO330P_510L)
        .eDTCMCfg       = AM_HAL_PWRCTRL_DTCM256K,
#else
        .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
#endif
        .eRetainDTCM    = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
#else
#if defined(AM_PART_APOLLO330P_510L)
        .eDTCMCfg       = AM_HAL_PWRCTRL_DTCM128K,
#else
        .eDTCMCfg       = AM_HAL_PWRCTRL_ITCM32K_DTCM128K,
#endif
        .eRetainDTCM    = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
#endif
#if defined(AM_PART_APOLLO330P_510L)
        .eNVMCfg        = AM_HAL_PWRCTRL_NVM,
#else
        .eNVMCfg        = AM_HAL_PWRCTRL_NVM0_ONLY,
#endif
        .bKeepNVMOnInDeepSleep     = false

    };

    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);

// #if defined(AM_PART_APOLLO5A)
//     MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
//     MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
//     MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
// #else
//     MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0LPREN = 1;
//     MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0SLPEN = 0;
//     MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM0PWRCTRL = 1;
//     #ifndef apollo510L_eb
//         MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1LPREN = 1;
//         MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1SLPEN = 0;
//         MCUCTRL->MRAMCRYPTOPWRCTRL_b.MRAM1PWRCTRL = 1;
//     #endif
// #endif

    if (pCfg->bNeedSharedSRAM == false) {
        am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg = {
            .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithMCU  = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithGFX  = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithDISP = AM_HAL_PWRCTRL_SRAM_NONE,          
            .eSRAMRetain     = AM_HAL_PWRCTRL_SRAM_NONE
        };
        am_hal_pwrctrl_sram_config(&SRAMMemCfg);

    } else {
         am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg = {
            #ifndef AM_PART_APOLLO330P
            .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_3M,
            #else
            .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_1P75M,
            #endif
            // .eActiveWithMCU   = PWRCTRL_SSRAMPWREN_PWRENSSRAM_ALL,
            .eActiveWithMCU   = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithGFX   = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithDISP  = AM_HAL_PWRCTRL_SRAM_NONE,   
            #ifndef AM_PART_APOLLO330P
            .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_3M
            #else
            .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_1P75M
            #endif
        };       
        am_hal_pwrctrl_sram_config(&SRAMMemCfg);
    };
}


int32_t ns_power_platform_config(const ns_power_config_t *pCfg) {
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

    am_bsp_low_power_init();
    return NS_STATUS_SUCCESS;
    // am_hal_spotmgr_profile_t spotmgr_profile; 
    // spotmgr_profile.PROFILE_b.COLLAPSESTMANDSTMP = 1;
    // am_hal_spotmgr_profile_set(&spotmgr_profile);
    #define ELP_ON                              1
    am_hal_pwrctrl_pwrmodctl_cpdlp_t sDefaultCpdlpConfig =
    {
         .eRlpConfig = AM_HAL_PWRCTRL_RLP_ON,
         #if ELP_ON
            .eElpConfig = AM_HAL_PWRCTRL_ELP_ON,
         #else
            .eElpConfig = AM_HAL_PWRCTRL_ELP_RET,
         #endif
         .eClpConfig = AM_HAL_PWRCTRL_CLP_ON
    };

    // Configure the cache, Enable the I-Cache and D-Cache.
    am_hal_cachectrl_icache_enable();
    am_hal_cachectrl_dcache_enable(true);
    am_hal_pwrctrl_pwrmodctl_cpdlp_config(sDefaultCpdlpConfig);


    // am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL, 0);
    // MCUCTRL->XTALCTRL = 0;
    // am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_LFRC); // Use LFRC instead of XT
    am_hal_rtc_osc_disable();
    VCOMP->PWDKEY = VCOMP_PWDKEY_PWDKEY_Key;
    MCUCTRL->DBGCTRL = 0;
    // Powering down various peripheral power domains
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DEBUG);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);

    ns_power_memory_config(pCfg);

    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);

    // configure peripherals
    // ns_power_down_peripherals(pCfg);

    #if defined(AM_PART_APOLLO510B)
    if (pCfg->bNeedBluetooth == false) {
        am_devices_em9305_shutdown();
    }
    #endif

    #if defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P)
    //
    // Power off the RSS
    //
    if (pCfg->bNeedBluetooth == false) {
        am_hal_pwrctrl_rss_pwroff();
    }
    // am_hal_pwrctrl_rss_pwroff();
    #endif

    // Configure power mode
    ns_delay_us(10000);
    // return NS_STATUS_SUCCESS;

    NS_TRY(ns_set_performance_mode(pCfg->eAIPowerMode), "Set CPU Perf mode failed.");

    MCUCTRL->MRAMCRYPTOPWRCTRL_b.CRYPTOCLKGATEN = 1;

    if (pCfg->bEnableTempCo) {
        ns_lp_printf("WARNING TempCo not supported.\n");
    }
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

    // if (g_ns_state.uartPrintCurrentlyEnabled) {




        
    //     am_bsp_uart_printf_disable();
    //     g_ns_state.uartPrintCurrentlyEnabled = false;
    // } else if (g_ns_state.itmPrintCurrentlyEnabled) {
    //     ns_cryptoless_itm_printf_disable();
    //     g_ns_state.itmPrintCurrentlyEnabled = false;
    // }

    // if (g_ns_state.cryptoCurrentlyEnabled) {
    //     am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
    //     g_ns_state.cryptoCurrentlyEnabled = false;
    // }

    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    // if (g_ns_state.tempcoWantsToBeEnabled) { // isr turns ADC on
    // }
}
