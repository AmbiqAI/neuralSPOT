//*****************************************************************************
//
//! @file nemagfx_vg_test.c
//!
//! @brief This example include all the build-in VG demos in the NemaSDK.
//! Modified the macro defines in nemagfx_vg_test.h(line 64-71) to select
//! among the following demos.
//! <masking_example>: this demo shows how to use the masking feature with NemaVG.
//! <paint_example>: this demo shows how to use different paint features.
//! <paint_lut_example>: this demo shows how to use the LUT format texture
//!                      in the VG paint object.
//! <render_vg_font>: this demo shows how to render the TTF font with NemaVG.
//! <shape>: this demo shows various pre-defined shapes with different paint settings.
//! <text_transformation>: this demo shows how to move the text object by setting
//!                        different transform matrixes.
//! <tsvg_benchmark>: run the SVG benchmark by rotating a tiger head image in SVG format.
//! <tsvg_render_example>: render an SVG image that includes both shapes and fonts.
//! Note: Only one of the above macro defines can be enable.
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

#include "nemagfx_vg_test.h"


//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
static uint32_t        ui32DMATCBBuffer[2560];
void            *g_pPsramHandle;
void            *g_pMSPIPsramHandle;

am_devices_mspi_psram_config_t g_sMspiPsramConfig =
{
#ifdef AM_PART_APOLLO4P
#ifdef APS25616N_OCTAL_MODE_EN
    .eDeviceConfig            = AM_BSP_MSPI_PSRAM_MODULE_OCTAL_DDR_CE,
#else
     .eDeviceConfig            = AM_HAL_MSPI_FLASH_HEX_DDR_CE1,
#endif
#else
    .eDeviceConfig            = AM_BSP_MSPI_PSRAM_MODULE_OCTAL_DDR_CE,
#endif
    .eClockFreq               = AM_HAL_MSPI_CLK_96MHZ,
    .ui32NBTxnBufLength       = sizeof(ui32DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = ui32DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

//! MSPI interrupts.
static const IRQn_Type MspiInterrupts[] =
{
    MSPI0_IRQn,
    MSPI1_IRQn,
    MSPI2_IRQn,
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define psram_mspi_isr                                                          \
    am_mspi_isr1(MSPI_PSRAM_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

//*****************************************************************************
//
// MSPI ISRs.
//
//*****************************************************************************
void psram_mspi_isr(void)
{
   uint32_t      ui32Status;

   am_hal_mspi_interrupt_status_get(g_pMSPIPsramHandle, &ui32Status, false);

   am_hal_mspi_interrupt_clear(g_pMSPIPsramHandle, ui32Status);

   am_hal_mspi_interrupt_service(g_pMSPIPsramHandle, ui32Status);

}

//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Status;

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#ifdef DEBUG_PIN
    //debug pin setting
    am_hal_gpio_pinconfig(DEBUG_PIN, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_clear(DEBUG_PIN);
#endif

#ifdef NEMA_WAIT_CL_PIN_1
    am_hal_gpio_pinconfig(NEMA_WAIT_CL_PIN_1, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_clear(NEMA_WAIT_CL_PIN_1);
#endif

#ifdef NEMA_WAIT_CL_PIN_2
    am_hal_gpio_pinconfig(NEMA_WAIT_CL_PIN_2, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_clear(NEMA_WAIT_CL_PIN_2);
#endif

    //Power on extended ram(DSP ram).
    am_hal_pwrctrl_dsp_memory_config_t sDSPMemCfg = g_DefaultDSPMemCfg;
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP0, &sDSPMemCfg);
    am_hal_pwrctrl_dsp_memory_config(AM_HAL_DSP1, &sDSPMemCfg);

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();

    //
    // Enable global IRQ.
    //
    am_hal_interrupt_master_enable();

    //
    // Configure the MSPI and PSRAM Device.
    //
#ifdef AM_PART_APOLLO4P
    ui32Status = am_devices_mspi_psram_aps25616n_ddr_init(MSPI_PSRAM_MODULE, &g_sMspiPsramConfig, &g_pPsramHandle, &g_pMSPIPsramHandle);
#else
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_init(MSPI_PSRAM_MODULE, &g_sMspiPsramConfig, &g_pPsramHandle, &g_pMSPIPsramHandle);
#endif
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
    }
    NVIC_SetPriority(MspiInterrupts[MSPI_PSRAM_MODULE], 5);
    NVIC_EnableIRQ(MspiInterrupts[MSPI_PSRAM_MODULE]);

    //
    // Enable XIP mode.
    //
#ifdef AM_PART_APOLLO4P
    ui32Status = am_devices_mspi_psram_aps25616n_ddr_enable_xip(g_pPsramHandle);
#else
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_xip(g_pPsramHandle);
#endif
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to enable XIP mode in the MSPI!\n");
    }

    //
    // Initialize plotting interface.
    //
    am_util_stdio_printf("nemagfx_vg_paint Example\n");

#ifdef RUN_IN_HP_MODE
    if ( am_hal_pwrctrl_mcu_mode_select(AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE) != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("HP_LP:Enter HP mode failed!\n");
    }
#endif

    //Power on GPU
    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_GFX);

#if defined(RUN_MASKING_EXAMPLE)
    extern int masking_example(void);
    masking_example();
#elif defined(RUN_PAINT_EXAMPLE)
    extern int paint_example(void);
    paint_example();
#elif defined(RUN_PAINT_LUT_EXAMPLE)
    extern int paint_lut_example(void);
     paint_lut_example();
#elif defined(RUN_RENDER_VG_FONT)
    extern int render_vg_font(void);
    render_vg_font();
#elif defined(RUN_SHAPE)
    extern int shapes(void);
    shapes();
#elif defined(RUN_TEXT_TRANSFORMATION)
    extern int text_transformation(void);
    text_transformation();
// #elif defined(RUN_TSVG_BENCHMARK)
//     extern int tsvg_benchmark(void);
//     tsvg_benchmark();
// #elif defined(RUN_TSVG_RENDER_EXAMPLE)
//     extern int tsvg_render_example(void);
//     tsvg_render_example();
#else
    #error "Not supported selection!"
#endif

}

