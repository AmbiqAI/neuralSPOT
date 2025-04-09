//*****************************************************************************
//
//! @file am_hal_sysctrl_clk_mux_reset.h
//!
//! @brief Internal api definition for Clock Mux Reset Operation
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5p0p0-5f68a8286b of the AmbiqSuite Development Package.
//
//*****************************************************************************
//! @cond SYSCTRL_CLK_MUX_RESET
#ifndef AM_HAL_SYSCTRL_CLK_MUX_RESET_H
#define AM_HAL_SYSCTRL_CLK_MUX_RESET_H

#ifdef __cplusplus
extern "C"
{
#endif

#define HALSTATE        STIMER->HALSTATES
#define HALSTATE_b      STIMER->HALSTATES_b
#define SYSCTRL_CLKMUXRST_SIGNATURE 0x5AF0

//*****************************************************************************
//
//! CLKMUXRST Clock Source Bitmap
//
//*****************************************************************************
#define SYSCTRL_CLKMUXRST_CLK_CLKGEN        0x00
#define SYSCTRL_CLKMUXRST_CLK_HFRC_DED      0x01
#define SYSCTRL_CLKMUXRST_CLK_HFRC2         0x02
#define SYSCTRL_CLKMUXRST_CLK_XTAL          0x04
#define SYSCTRL_CLKMUXRST_CLK_EXTCLK        0x08
#define SYSCTRL_CLKMUXRST_CLK_PLL           0x10

//*****************************************************************************
//
//! CLKMUXRST clock mux enum
//
//*****************************************************************************
typedef enum
{
    SYSCTRL_CLKMUXRST_MUX_CG_AUDADC,
    SYSCTRL_CLKMUXRST_MUX_LL_AUDADC,
    SYSCTRL_CLKMUXRST_MUX_LL_PDM,
    SYSCTRL_CLKMUXRST_MUX_LL_I2S0,
    SYSCTRL_CLKMUXRST_MUX_LL_I2S1,
    SYSCTRL_CLKMUXRST_MUX_LL_USB,

    // SYSCTRL_CLKMUXRST_CLK_MAX Should always be the last entry
    SYSCTRL_CLKMUXRST_CLK_MAX,
} am_hal_sysctrl_clkmuxrst_clk_e;

//*****************************************************************************
//
//! @brief Update clocks needed for clock muxes reset operation
//!
//! @param bClkgenOff - AUDADC ClockGen clock selection is OFF
//
//*****************************************************************************
extern void am_hal_sysctrl_clkmuxrst_audadc_clkgen_off_update(bool bClkgenOff);

//*****************************************************************************
//
//! @brief Update clocks needed for clock muxes reset operation
//!
//! @param eFref - Reference Clock select for PLL
//
//*****************************************************************************
extern void am_hal_sysctrl_clkmuxrst_pll_fref_update(MCUCTRL_PLLCTL0_FREFSEL_Enum eFref);

//*****************************************************************************
//
//! @brief Update PLL FREF selection
//!
//! @param eClk - am_hal_sysctrl_clkmuxrst_clk_e value for mux to update.
//! @param ui8ClkSrcBm - bitmap representing clocks needed by the mux
//
//*****************************************************************************
extern void am_hal_sysctrl_clkmuxrst_clkneeded_update(am_hal_sysctrl_clkmuxrst_clk_e eClk, uint8_t ui8ClkSrcBm);

//*****************************************************************************
//
//! @brief Handle clock muxes reset during low_power_init
//
//*****************************************************************************
extern void am_hal_sysctrl_clkmuxrst_low_power_init();

#ifdef __cplusplus
}
#endif

#endif //AM_HAL_SYSCTRL_CLK_MUX_RESET_H

//! @endcond SYSCTRL_CLK_MUX_RESET
