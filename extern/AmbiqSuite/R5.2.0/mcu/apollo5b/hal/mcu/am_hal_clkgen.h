//*****************************************************************************
//
//! @file am_hal_clkgen.h
//!
//! @brief Functions for interfacing with the CLKGEN.
//!
//! @addtogroup clkgen4 CLKGEN - Clock Generator
//! @ingroup apollo5b_hal
//! @{
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_CLKGEN_H
#define AM_HAL_CLKGEN_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @name System Clock max frequency
//! @{
//! Defines the maximum clock frequency for this device.
//! These macros provide a definition of the maximum clock frequency.
//
//*****************************************************************************
#ifdef APOLLO5_FPGA
#define AM_HAL_CLKGEN_FREQ_MAX_HZ           (APOLLO5_FPGA * 1000000)
#define AM_HAL_CLKGEN_FREQ_HP250_HZ         (20 * 1000000)
#else // APOLLO5_FPGA
#define AM_HAL_CLKGEN_FREQ_MAX_HZ           96000000
#define AM_HAL_CLKGEN_FREQ_HP250_HZ         250000000
#endif // APOLLO5_FPGA

#define AM_HAL_CLKGEN_FREQ_MAX_KHZ      (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000)
#define AM_HAL_CLKGEN_FREQ_HP250_KHZ    (AM_HAL_CLKGEN_FREQ_HP250_HZ / 1000)
#define AM_HAL_CLKGEN_FREQ_MAX_MHZ      (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000000)
#define AM_HAL_CLKGEN_FREQ_HP250_MHZ    (AM_HAL_CLKGEN_FREQ_HP250_HZ / 1000000)
#define AM_HAL_CLKGEN_CORESEL_MAXDIV    1
//! @}

// Delay time for HFRC2 to ramp up
#define AM_HAL_CLKGEN_HFRC2_DELAY       15

//
//! Control operations.
//
typedef enum
{
    AM_HAL_CLKGEN_CONTROL_RTC_SEL_XTAL,
    AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC,
    AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE,
    AM_HAL_CLKGEN_CONTROL_HFADJ_DISABLE,
    AM_HAL_CLKGEN_CONTROL_HF2ADJ_ENABLE,
    AM_HAL_CLKGEN_CONTROL_HF2ADJ_DISABLE,
    AM_HAL_CLKGEN_CONTROL_HFRC2_ON_REQ,
    AM_HAL_CLKGEN_CONTROL_HFRC2_OFF_REQ,
    AM_HAL_CLKGEN_CONTROL_XTAL24M_ENABLE,
    AM_HAL_CLKGEN_CONTROL_XTAL24M_DISABLE,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_0,      // Drive strength
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_1,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_2,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_3,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_4,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_5,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_6,
    AM_HAL_CLKGEN_CONTROL_XTAL24MDS_7,
    AM_HAL_CLKGEN_CONTROL_DCCLK_ENABLE,
    AM_HAL_CLKGEN_CONTROL_DCCLK_DISABLE,
    AM_HAL_CLKGEN_CONTROL_DBICLKSEL_DBIB_CLK,
    AM_HAL_CLKGEN_CONTROL_DBICLKSEL_FORMAT_CLK,
    AM_HAL_CLKGEN_CONTROL_DBICLKSEL_PLL_CLK,
    AM_HAL_CLKGEN_CONTROL_DBICLKDIV2EN_ENABLE,
    AM_HAL_CLKGEN_CONTROL_DBICLKDIV2EN_DISABLE,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_OFF,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC12,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC24,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC48,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC96,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_HFRC192,
    AM_HAL_CLKGEN_CONTROL_DISPCLKSEL_DPHYPLL,
    AM_HAL_CLKGEN_CONTROL_PLLCLK_ENABLE,
    AM_HAL_CLKGEN_CONTROL_PLLCLK_DISABLE,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_OFF,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC24,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFRC12,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFXT,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_HFXT_DIV2,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_EXTREFCLK,
    AM_HAL_CLKGEN_CONTROL_PLLCLKSEL_EXTREFCLK_DIV2,
} am_hal_clkgen_control_e;

//
//! Current RTC oscillator.
//
typedef enum
{
    AM_HAL_CLKGEN_STATUS_RTCOSC_XTAL,
    AM_HAL_CLKGEN_STATUS_RTCOSC_LFRC,
} am_hal_clkgen_status_rtcosc_e;

//
//! Clock Generation CLKOUT
//
typedef enum
{
    // XTAL
    AM_HAL_CLKGEN_CLKOUT_XTAL_32768     = CLKGEN_CLKOUT_CKSEL_XT,           // XTAL
    AM_HAL_CLKGEN_CLKOUT_XTAL_16384     = CLKGEN_CLKOUT_CKSEL_XT_DIV2,      // XTAL / 2
    AM_HAL_CLKGEN_CLKOUT_XTAL_8192      = CLKGEN_CLKOUT_CKSEL_XT_DIV4,      // XTAL / 4
    AM_HAL_CLKGEN_CLKOUT_XTAL_4096      = CLKGEN_CLKOUT_CKSEL_XT_DIV8,      // XTAL / 8
    AM_HAL_CLKGEN_CLKOUT_XTAL_2048      = CLKGEN_CLKOUT_CKSEL_XT_DIV16,     // XTAL / 16
    AM_HAL_CLKGEN_CLKOUT_XTAL_1024      = CLKGEN_CLKOUT_CKSEL_XT_DIV32,     // XTAL / 32
    AM_HAL_CLKGEN_CLKOUT_XTAL_128       = CLKGEN_CLKOUT_CKSEL_XT_DIV256,    // XTAL / 256   = 128 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_4         = CLKGEN_CLKOUT_CKSEL_XT_DIV8K,     // XTAL / 8192  =  4 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_0_5       = CLKGEN_CLKOUT_CKSEL_XT_DIV64K,    // XTAL / 65536 =  0.5 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_0_015     = CLKGEN_CLKOUT_CKSEL_XT_DIV2M,     // XTAL / 2097152 = 0.015625 Hz

    // LFRC
    AM_HAL_CLKGEN_CLKOUT_LFRC           = CLKGEN_CLKOUT_CKSEL_LFRC,         // LFRC
    AM_HAL_CLKGEN_CLKOUT_LFRC_512       = CLKGEN_CLKOUT_CKSEL_LFRC_DIV2,    // LFRC / 2     = 512 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_32        = CLKGEN_CLKOUT_CKSEL_LFRC_DIV32,   // LFRC / 32    =  32 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_2         = CLKGEN_CLKOUT_CKSEL_LFRC_DIV512,  // LFRC / 512   =   2 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_0_03      = CLKGEN_CLKOUT_CKSEL_LFRC_DIV32K,  // LFRC / 32768 = 0.03125 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_0_0010    = CLKGEN_CLKOUT_CKSEL_LFRC_DIV1M,   // LFRC / 1M    = 0.0009765625 Hz

    // HFRC2
    AM_HAL_CLKGEN_CLKOUT_HFRC2_250M     = CLKGEN_CLKOUT_CKSEL_HFRC2,        // HFRC2        = 250      = 250MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_125M     = CLKGEN_CLKOUT_CKSEL_HFRC2_DIV2,   // HFRC2        = 250 / 2  = 125MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_31M      = CLKGEN_CLKOUT_CKSEL_HFRC2_DIV8,   // HFRC2        = 250 / 8  = 31.25MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_16M      = CLKGEN_CLKOUT_CKSEL_HFRC2_DIV16,  // HFRC2        = 250 / 16 = 15.625MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_8M       = CLKGEN_CLKOUT_CKSEL_HFRC2_DIV32,  // HFRC2        = 250 / 32 = 7.8MHz
    AM_HAL_CLKGEN_CLKOUT_HFRC2_4M       = CLKGEN_CLKOUT_CKSEL_HFRC2_DIV64,  // HFRC2        = 250 / 64 = 3.9MHz

    // Uncalibrated LFRC
    AM_HAL_CLKGEN_CLKOUT_ULFRC_64       = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV16,  // ULFRC / 16   = 64 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_8        = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV128, // ULFRC / 128  =  8 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_1        = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV1K,  // ULFRC / 1024 =  1 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_0_25     = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV4K,  // ULFRC / 4096 = 0.25 Hz (uncal LFRC)
    AM_HAL_CLKGEN_CLKOUT_ULFRC_0_0009   = CLKGEN_CLKOUT_CKSEL_ULFRC_DIV1M,  // ULFRC / 1M   = 0.000976 Hz (uncal LFRC)

    // Not Autoenabled ("NE")
    AM_HAL_CLKGEN_CLKOUT_XTALNE_32768   = CLKGEN_CLKOUT_CKSEL_XTNE,         // XTALNE / 1   = 32768 Hz
    AM_HAL_CLKGEN_CLKOUT_XTALNE_2048    = CLKGEN_CLKOUT_CKSEL_XTNE_DIV16,   // XTALNE / 16  =  2048 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRCNE         = CLKGEN_CLKOUT_CKSEL_LFRCNE,       // LFRCNE / 32  =    32 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRCNE_32      = CLKGEN_CLKOUT_CKSEL_LFRCNE_DIV32, // LFRCNE / 32  =    32 Hz

    // Misc clocks
    AM_HAL_CLKGEN_CLKOUT_RTC_1HZ        = CLKGEN_CLKOUT_CKSEL_RTC_1Hz,      // RTC
    AM_HAL_CLKGEN_CLKOUT_CG_100         = CLKGEN_CLKOUT_CKSEL_CG_100Hz,     // ClkGen 100Hz
} am_hal_clkgen_clkout_e;

#define AM_HAL_CLKGEN_CLKOUT_MAX        AM_HAL_CLKGEN_CLKOUT_HFRC2_250M     // Highest valid CKSEL enum value

// //! HFRC ADJ enum for adjust enable
// typedef enum
// {
//     AM_HAL_CLKGEN_HFRCADJ_EN = CLKGEN_HFADJ_HFADJEN_DIS,
//     AM_HAL_CLKGEN_HFRCADJ_EN = CLKGEN_HFADJ_HFADJEN_EN,
// } am_hal_clkgen_hfrcadj_enable_e;

//! HFRC ADJ enum for repeat period
typedef enum
{
    AM_HAL_CLKGEN_HFRCADJ_RPT_4_SEC    = CLKGEN_HFADJ_HFADJCK_4SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_16_SEC   = CLKGEN_HFADJ_HFADJCK_16SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_32_SEC   = CLKGEN_HFADJ_HFADJCK_32SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_64_SEC   = CLKGEN_HFADJ_HFADJCK_64SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_128_SEC  = CLKGEN_HFADJ_HFADJCK_128SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_256_SEC  = CLKGEN_HFADJ_HFADJCK_256SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_512_SEC  = CLKGEN_HFADJ_HFADJCK_512SEC,
    AM_HAL_CLKGEN_HFRCADJ_RPT_1024_SEC = CLKGEN_HFADJ_HFADJCK_1024SEC,
} am_hal_clkgen_hfrcadj_repeat_period_e;

//! HFRC ADJ enum for XT warmup time
typedef enum
{
    AM_HAL_CLKGEN_HFRCAJD_XTWARMUP_1SEC = CLKGEN_HFADJ_HFWARMUP_1SEC,
    AM_HAL_CLKGEN_HFRCAJD_XTWARMUP_2SEC = CLKGEN_HFADJ_HFWARMUP_2SEC,
} am_hal_clkgen_hfrcadj_warmup_e;

//! HFRC ADJ enum for attack speed
typedef enum
{
    AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1       = CLKGEN_HFADJ_HFADJGAIN_Gain_of_1,       // Fastest Attack Speed
    AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1_IN_2  = CLKGEN_HFADJ_HFADJGAIN_Gain_of_1_in_2,
    AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1_IN_4  = CLKGEN_HFADJ_HFADJGAIN_Gain_of_1_in_4,
    AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1_IN_8  = CLKGEN_HFADJ_HFADJGAIN_Gain_of_1_in_8,
    AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1_IN_16 = CLKGEN_HFADJ_HFADJGAIN_Gain_of_1_in_16,
    AM_HAL_CLKGEN_HFRCADJ_ATTACK_SPEED_1_IN_32 = CLKGEN_HFADJ_HFADJGAIN_Gain_of_1_in_32, // Slowest Attack Speed
} am_hal_clkgen_hfrcadj_attack_speed_e;

//! HFRC ADJ enum for maxdelta enable
typedef enum
{
    AM_HAL_CLKGEN_HFRCADJ_MAXDELTA_DIS = CLKGEN_HFADJ_HFADJMAXDELTA_DISABLED,
    AM_HAL_CLKGEN_HFRCADJ_MAXDELTA_EN  = CLKGEN_HFADJ_HFADJMAXDELTA_ENABLED,
} am_hal_clkgen_hfrcadj_maxdelta_en_e;

//! HFRC2 ADJ enum for XTALDIVRATIO
typedef enum
{
    AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_DIV_1 = CLKGEN_HF2ADJ2_HF2ADJXTALDIVRATIO_XTAL32M,
    AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_DIV_2 = CLKGEN_HF2ADJ2_HF2ADJXTALDIVRATIO_XTAL32M_DIV2,
    AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_DIV_4 = CLKGEN_HF2ADJ2_HF2ADJXTALDIVRATIO_XTAL32M_DIV4,
    AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_DIV_8 = CLKGEN_HF2ADJ2_HF2ADJXTALDIVRATIO_XTAL32M_DIV8,
} am_hal_clkgen_hfrc2adj_refclk_div_e;

typedef enum
{
    AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_MUX_XTAL_HS     = 0,
    AM_HAL_CLKGEN_HFRC2ADJ_REFCLK_MUX_EXTREF_CLK  = 1,
} am_hal_clkgen_hfrc2adj_refclk_mux_e;

//
//! Status structure.
//
typedef struct
{
    //
    // ui32SysclkFreq
    //!  Returns the current system clock frequency, in hertz.
    //
    uint32_t    ui32SysclkFreq;

    //
    // eRTCOSC
    //
    //!  Returns the current RTC oscillator as one of:
    //!  AM_HAL_CLKGEN_STATUS_RTCOSC_LFRC
    //!  AM_HAL_CLKGEN_STATUS_RTCOSC_XTAL
    //
    uint32_t    eRTCOSC;

    //
    // bXtalFailure
    //!  true = XTAL has failed (is enabled but not oscillating).  Also if the
    //!         LFRC is selected as the oscillator in OCTRL.OSEL.
    //
    bool        bXtalFailure;

    //
    // enable status for all the peripheral clocks.
    //  1: enable
    //  0: disable
    //
    //uint32_t    ui32Clockenstat;
    //uint32_t    ui32Clocken2stat;
    //uint32_t    ui32Clocken3stat;
} am_hal_clkgen_status_t;

//
//! HFRC ADJ Structure
//
typedef union
{
    uint32_t HFRCAdj;

    struct
    {
        uint32_t                                            : 1;
        am_hal_clkgen_hfrcadj_repeat_period_e eRepeat       : 3;
        uint32_t                                            : 4;
        uint32_t                              ui32TargetVal : 12;
        am_hal_clkgen_hfrcadj_warmup_e        eWarmup       : 1;
        am_hal_clkgen_hfrcadj_attack_speed_e  eSpeed        : 3;
        am_hal_clkgen_hfrcadj_maxdelta_en_e   eMaxDelta     : 5;
        uint32_t                                            : 3;
    } HFRCAdj_b;
} am_hal_clkgen_hfrcadj_t;


//
//! HFRC2 ADJ Structure
//
typedef struct
{
    am_hal_clkgen_hfrc2adj_refclk_mux_e eRefClkSel;
    am_hal_clkgen_hfrc2adj_refclk_div_e eRefClkDiv;
    uint32_t                            ui32AdjRatio;
} am_hal_clkgen_hfrc2adj_t;

// ****************************************************************************
//
//! @brief Apply various specific commands/controls on the CLKGEN module.
//!
//! This function is used to apply various controls on CLKGEN.
//!
//! @note IMPORTANT! This function MUST be called very early in execution of
//!       an application with the parameter AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX
//!       in order to set Apollo5 to its required operating frequency.
//!
//! @param eControl - One of the following:
//!     AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX
//!     AM_HAL_CLKGEN_CONTROL_RTC_SEL_XTAL
//!     AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC
//!     AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE
//!     AM_HAL_CLKGEN_CONTROL_HFADJ_DISABLE
//! @param pArgs - Pointer to arguments for Control Switch Case
//!
//! @return status      - generic or interface specific status.
//!
//! @note After starting the XTAL, a 2 second warm-up delay is required.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_control(am_hal_clkgen_control_e eControl,
                                      void *pArgs);

// ****************************************************************************
//
//! @brief Get CLKGEN status.
//!
//! This function returns the current value of various CLKGEN statuses.
//!
//! @param psStatus - ptr to a status structure to receive the current statuses.
//!
//! @return status      - generic or interface specific status.
//!
//! @note After selection of the RTC Oscillator, a 2 second delay is required
//! before the new oscillator takes effect.  Therefore the CLKGEN.STATUS.OMODE
//! bit will not reflect the new status until after the 2s wait period.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_status_get(am_hal_clkgen_status_t *psStatus);

// ****************************************************************************
//
//! @brief Enable CLKOUT.
//!
//! This function is used to enable and select a CLKOUT frequency.
//!
//! @param bEnable: true to enable, false to disable.
//! @param eClkSelect - One of the following:
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_32768
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_16384
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_8192
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_4096
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_2048
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_1024
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_128
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_4
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_0_5
//!     AM_HAL_CLKGEN_CLKOUT_XTAL_0_015
//!
//!     AM_HAL_CLKGEN_CLKOUT_LFRC
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_512
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_32
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_2
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_0_03
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_0_0010
//!
//!     AM_HAL_CLKGEN_CLKOUT_HFRC2_24M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC2_12M
//!     AM_HAL_CLKGEN_CLKOUT_HFRC2_6M
//!
//!     // Uncalibrated LFRC
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_64
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_8
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_1
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_0_25
//!     AM_HAL_CLKGEN_CLKOUT_ULFRC_0_0009
//!
//!     // Not Autoenabled ("NE")
//!     AM_HAL_CLKGEN_CLKOUT_XTALNE_32768
//!     AM_HAL_CLKGEN_CLKOUT_XTALNE_2048
//!     AM_HAL_CLKGEN_CLKOUT_LFRCNE
//!     AM_HAL_CLKGEN_CLKOUT_LFRCNE_32
//!
//!     // Misc clocks
//!     AM_HAL_CLKGEN_CLKOUT_RTC_1HZ
//!     AM_HAL_CLKGEN_CLKOUT_CG_100
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_clkgen_clkout_enable(bool bEnable,
                                            am_hal_clkgen_clkout_e eClkSelect);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CLKGEN_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

