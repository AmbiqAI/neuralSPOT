//*****************************************************************************
//
//! @file nemagfx_blend.c
//!
//! @brief NemaGFX example.
//! brief Example that demonstrates blend feature
//! Blending requires a series of calculations between the source (foreground)
//! and destination (background)color fragments for producing the final color,
//! which will be written in memory.This example use a constent table inside
//! most of the supported blending mode.demonstrates each more every 1 second.
//! the dst color is nema_rgba(0xff, 0, 0, 0x80), which is red color with 50%
//! alpha blending, the src color is nema_rgba(0, 0, 0xff, 0x80), which is blue
//! color with 50% alpha blending.
//!
//! Printing takes place over the ITM at 1M Baud.

//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "nemagfx_blend.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "nema_hal.h"

//*****************************************************************************
//
// Enable printing to the console.
//
//*****************************************************************************
void
enable_print_interface(void)
{
    //
    // Initialize a debug printing interface.
    //
    am_bsp_debug_printf_enable();
}

//*****************************************************************************
//
// Enable printing to the console.
//
//*****************************************************************************
void
disable_print_interface(void)
{
    //
    // Initialize a debug printing interface.
    //
    am_bsp_debug_printf_disable();
    am_hal_itm_disable();
}

//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{
    //
    // External power on
    //
    am_bsp_external_pwr_on();
    am_util_delay_ms(100);
    //
    // RAM retention
    //
//    am_hal_pwrctrl_mcu_memory_config_t sMcuMemCfg =
//    {
//        .eCacheCfg    = AM_HAL_PWRCTRL_CACHE_ALL,
//        .bRetainCache = true,
//        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_384K,
//        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_384K,
//        .bEnableNVM0  = true,
//        .bRetainNVM0  = false
//    };
//    am_hal_pwrctrl_sram_memcfg_t sSRAMMemCfg =
//    {
//        .eSRAMCfg         = AM_HAL_PWRCTRL_SRAM_1M,
//        .eActiveWithMCU   = AM_HAL_PWRCTRL_SRAM_NONE,
//        .eActiveWithDSP   = AM_HAL_PWRCTRL_SRAM_NONE,
//        .eSRAMRetain      = AM_HAL_PWRCTRL_SRAM_1M
//    };
    am_hal_pwrctrl_dsp_memory_config_t sExtSRAMMemCfg =
    {
        .bEnableICache      = false,
        .bRetainCache       = false,
        .bEnableRAM         = true,
        .bActiveRAM         = false,
        .bRetainRAM         = true
    };
    //am_hal_pwrctrl_mcu_memory_config(&sMcuMemCfg);
    //am_hal_pwrctrl_sram_config(&sSRAMMemCfg); //!< 0.2mA
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP0, &sExtSRAMMemCfg);
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP1, &sExtSRAMMemCfg);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    am_bsp_low_power_init();
    //
    // Disable crypto
    //
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO); //!< do NOT do this on B0!!! // 0.5mA
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    // am_bsp_itm_printf_enable(); //!< 0.25mA
    //
    // Clear the terminal and print the banner.
    //
    // am_util_stdio_terminal_clear();
    //
    // If you want to change display configurations in example, please change them here as below.
    //
    // g_eDispType = RM67162_DSI;
    // g_sDsiCfg.ui8NumLanes = 1;
    // g_sDsiCfg.eDbiWidth = AM_HAL_DSI_DBI_WIDTH_8;
    // g_sDsiCfg.eDsiFreq = AM_HAL_DSI_FREQ_TRIM_X12;

    if ((g_sDispCfg[g_eDispType].eInterface == IF_DSI) || (g_sDispCfg[g_eDispType].bUseDPHYPLL == true))
    {
        //
        // VDD18 control callback function
        //
        am_hal_dsi_register_external_vdd18_callback(am_bsp_external_vdd18_switch);
        //
        // Enable DSI power and configure DSI clock.
        //
        am_hal_dsi_init();
    }
    else
    {
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC96, NULL);
        am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_DCCLK_ENABLE, NULL);
    }
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_GFX);
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_DISP);
    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

//    am_hal_timer_config_t sTimerConfig;
//    am_hal_timer_default_config_set(&sTimerConfig);
//    sTimerConfig.eInputClock = AM_HAL_TIMER_CLOCK_HFRC_DIV16;
//    sTimerConfig.eFunction = AM_HAL_TIMER_FN_CONTINUOUS;
//    am_hal_timer_config(0, &sTimerConfig);
//    am_hal_timer_start(0);//0.32mA

#ifdef BAREMETAL
    am_test_blend();
#else
    run_tasks();
#endif

    while (1)
    {
    }
}
