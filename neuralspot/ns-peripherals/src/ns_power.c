//*****************************************************************************
//
//! @file power.c
//!
//! @brief Utilities for MLPerf
//!
//
//*****************************************************************************

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
#include "ns_peripherals_power.h"

const ns_power_config_t ns_development_default = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = true,
    .bNeedBluetooth = true,
    .bNeedUSB = true,
    .bNeedIOM = true,
    .bNeedAlternativeUART = true,
    .b128kTCM = false
};

const ns_power_config_t ns_good_default =        {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = true,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false
};

const ns_power_config_t ns_mlperf_mode1 = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = true,
    .b128kTCM = false
};

const ns_power_config_t ns_mlperf_mode2 = {
    .eAIPowerMode = NS_MINIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = true,
    .b128kTCM = false
};

const ns_power_config_t ns_mlperf_mode3 = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = true,
    .b128kTCM = true
};

const ns_power_config_t ns_audio_default = {
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = true,
    .bNeedSharedSRAM = false,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = false,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false
};

//*****************************************************************************
//
// Internal method for turning off peripherals
//
//*****************************************************************************
void
ns_power_down_peripherals(const ns_power_config_t *pCfg) {

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
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 0;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
#endif

#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    if (pCfg->bNeedBluetooth == false) {
        //
        // For SiP packages, put the BLE Controller in reset.
        //
        am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN,
                                AM_HAL_GPIO_OUTPUT_CLEAR);
        am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN,
                              am_hal_gpio_pincfg_output);
        am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN,
                                AM_HAL_GPIO_OUTPUT_SET);
        am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN,
                                AM_HAL_GPIO_OUTPUT_CLEAR);
    }
#endif // AM_DEVICES_BLECTRLR_RESET_PIN

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
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DISP);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DISPPHY);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_SDIO);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USB);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_USBPHY);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DEBUG);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDREC);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDPB);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM0);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM1);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM2);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM3);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_I2S0);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_I2S1);

    if (pCfg->bNeedAudAdc == false) {
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_AUDADC);
    }

    if (pCfg->bNeedCrypto == false) {
        //
        // Power down Crypto.
        //
        am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);

    }

    //
    // XTAL powerdown
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
}

uint32_t
ns_power_config(const ns_power_config_t *pCfg) {
    uint32_t ui32ReturnStatus = AM_HAL_STATUS_SUCCESS;
    am_bsp_low_power_init();

    // configure SRAM & other memories
    if (pCfg->bNeedSharedSRAM == false) {
        am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg = {
            .eSRAMCfg = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithMCU = AM_HAL_PWRCTRL_SRAM_NONE,
            .eActiveWithDSP = AM_HAL_PWRCTRL_SRAM_NONE,
            .eSRAMRetain = AM_HAL_PWRCTRL_SRAM_NONE};
        am_hal_pwrctrl_sram_config(&SRAMMemCfg);
    }

    // The following two lines cause audio capture to be distorted - TBI
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // configure peripherals
    ns_power_down_peripherals(pCfg);

    // Configure power mode
    if ((pCfg->eAIPowerMode == NS_MAXIMUM_PERF) ||
        (pCfg->eAIPowerMode == NS_MEDIUM_PERF))
        am_hal_pwrctrl_mcu_mode_select(
            AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE);
    else
        am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER);

    if (pCfg->b128kTCM == true) {

        am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
        {
            .eCacheCfg    = AM_HAL_PWRCTRL_CACHE_ALL,
            .bRetainCache = false,
            .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_128K,
            .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_128K,
            .bEnableNVM0  = true,
            .bRetainNVM0  = true
        };

        am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);
    }

    return ui32ReturnStatus;
}