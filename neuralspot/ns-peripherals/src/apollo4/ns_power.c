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
#include "ns_tempco.h"

uint32_t ns_set_performance_mode(ns_power_mode_e eAIPowerMode) {
    // Configure power mode
    if ((eAIPowerMode == NS_MAXIMUM_PERF) || (eAIPowerMode == NS_MEDIUM_PERF))
        am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
    else if (eAIPowerMode == NS_MINIMUM_PERF)
        am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);
    else 
        return NS_STATUS_INVALID_CONFIG;

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

#if defined(AM_PART_APOLLO4)
    //
    // Configure the MRAM for low power mode
    //
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
#endif
// return; ok
#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    if (pCfg->bNeedBluetooth == false) {
        //
        // For SiP packages, put the BLE Controller in reset.
        //
        am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
        am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN, am_hal_gpio_pincfg_output);
        am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
        am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
    }
#endif // AM_DEVICES_BLECTRLR_RESET_PIN
       // return; ok

    //
    // Disable all peripherals
    //
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOS);

    if (pCfg->bNeedIOM == false) {
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM0);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM1);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM2);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM3);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM4);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM5);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM6);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOM7);
    };

    if (pCfg->bNeedAlternativeUART == false) {
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART0);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART3);
    }

    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART1);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_UART2);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_MSPI0);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_MSPI1);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_MSPI2);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_GFX);

#ifndef AM_PART_APOLLO4L
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DISP);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DISPPHY);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USB);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USBPHY);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM1);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM2);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM3);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_I2S1);
#endif

    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_SDIO);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DEBUG);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDREC);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDPB);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM0);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_I2S0);
// return;
#ifndef AM_PART_APOLLO4L
    if (pCfg->bNeedAudAdc == false) {
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDADC);
    }
#endif

    if (pCfg->bNeedCrypto == false) {
        // Power down Crypto.
        am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
    }
    // return;
    if (pCfg->bNeedXtal == false) {
        // XTAL powerdown
        am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
    }
}

// Main function for power configuration
uint32_t ns_power_platform_config(const ns_power_config_t *pCfg) {
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

    // configure SRAM & other memories
    if (pCfg->bNeedSharedSRAM == false) {
        am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg = {
            .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithMCU = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithDSP = AM_HAL_PWRCTRL_SRAM_NONE,
            .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_NONE};
        am_hal_pwrctrl_sram_config(&SRAMMemCfg);

#ifndef AM_PART_APOLLO4L
        am_hal_pwrctrl_dsp_memory_config_t sExtSRAMMemCfg = {
            .bEnableICache = false,
            .bRetainCache = false,
            .bEnableRAM = false,
            .bActiveRAM = false,
            .bRetainRAM = false};
        am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP0, &sExtSRAMMemCfg);
#endif
    } else {
        am_hal_daxi_config_t DaxiConfigLongAging = {
            .bDaxiPassThrough = false,
            .bAgingSEnabled = false, // false means only age-out write/modified lines    
            .eAgingCounter = AM_HAL_DAXI_CONFIG_AGING_1024, // 1024 will age out a line in ~ 96
                                                            // cycles.  Optimal for most use cases
                                                            // should be 256, 512, 1024, or 2048
            .eNumBuf = AM_HAL_DAXI_CONFIG_NUMBUF_32,
            .eNumFreeBuf = AM_HAL_DAXI_CONFIG_NUMFREEBUF_3,
        };
        am_hal_daxi_config(&DaxiConfigLongAging);
    }

    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // configure peripherals
    ns_power_down_peripherals(pCfg);

    // Configure power mode
    if(ns_set_performance_mode(pCfg->eAIPowerMode) != NS_STATUS_SUCCESS) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (pCfg->b128kTCM == true) {

        am_hal_pwrctrl_mcu_memory_config_t McuMemCfg = {
            .eCacheCfg = AM_HAL_PWRCTRL_CACHE_ALL,
            .bRetainCache = true,
            .eDTCMCfg = AM_HAL_PWRCTRL_DTCM_128K,
            .eRetainDTCM = AM_HAL_PWRCTRL_DTCM_128K,
            .bEnableNVM0 = true,
            .bRetainNVM0 = false};

        am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
    }

    if (pCfg->bEnableTempCo) {
#if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L)
        ns_lp_printf("WARNING TempCo not supported by AmbiqSuite R4.1.0 or Apollo4 Lite\n");
#else
        // Make sure the trim version is high enough before attempting to init
        uint32_t ui32Ret, ui32TrimVer;
        ui32Ret = am_hal_mram_info_read(1, AM_REG_INFO1_TRIM_REV_O / 4, 1, &ui32TrimVer);
        if ((ui32Ret == 0) && (ui32TrimVer != 0xFFFFFFFF) && (ui32TrimVer >= 6)) {
            if (ns_tempco_init() != 0) {
                ns_printf("ERROR TempCo init failed\n");
            }
        }
#endif
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

    if (g_ns_state.uartPrintCurrentlyEnabled) {
        am_bsp_uart_printf_disable();
        g_ns_state.uartPrintCurrentlyEnabled = false;
    } else if (g_ns_state.itmPrintCurrentlyEnabled) {
        ns_cryptoless_itm_printf_disable();
        g_ns_state.itmPrintCurrentlyEnabled = false;
    }

    if (g_ns_state.cryptoCurrentlyEnabled) {
        am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
        g_ns_state.cryptoCurrentlyEnabled = false;
    }

#if defined(NS_AMBIQSUITE_VERSION_R4_1_0) || defined(AM_PART_APOLLO4L)
    // TEMPCO not supported in this version
#else
    if (g_ns_state.tempcoCurrentlyEnabled) {
        am_hal_adc_power_control(g_ns_tempco_ADCHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);
    }
#endif

    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

    if (g_ns_state.tempcoWantsToBeEnabled) { // isr turns ADC on
    }
}
