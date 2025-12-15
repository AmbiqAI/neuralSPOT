//*****************************************************************************
//
//! @file am_hal_clkgen.h
//!
//! @brief Functions for interfacing with the CLKGEN.
//!
//! @addtogroup clkgen4_ap510L CLKGEN - Clock Generator
//! @ingroup apollo510L_hal
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
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
#define AM_HAL_CLKGEN_FREQ_HP192_HZ         (16 * 1000000)
#define AM_HAL_CLKGEN_FREQ_HP250_HZ         (20 * 1000000)
#else // APOLLO5_FPGA
#define AM_HAL_CLKGEN_FREQ_MAX_HZ           96000000
#define AM_HAL_CLKGEN_FREQ_HP192_HZ         192000000
#define AM_HAL_CLKGEN_FREQ_HP250_HZ         250000000
#endif // APOLLO5_FPGA

#define AM_HAL_CLKGEN_FREQ_MAX_KHZ      (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000)
#define AM_HAL_CLKGEN_FREQ_HP192_KHZ    (AM_HAL_CLKGEN_FREQ_HP192_HZ / 1000)
#define AM_HAL_CLKGEN_FREQ_HP250_KHZ    (AM_HAL_CLKGEN_FREQ_HP250_HZ / 1000)
#define AM_HAL_CLKGEN_FREQ_MAX_MHZ      (AM_HAL_CLKGEN_FREQ_MAX_HZ / 1000000)
#define AM_HAL_CLKGEN_FREQ_HP192_MHZ    (AM_HAL_CLKGEN_FREQ_HP192_HZ / 1000000)
#define AM_HAL_CLKGEN_FREQ_HP250_MHZ    (AM_HAL_CLKGEN_FREQ_HP250_HZ / 1000000)
#define AM_HAL_CLKGEN_CORESEL_MAXDIV    1
//! @}

//
//! Control operations.
//
typedef enum
{
    AM_HAL_CLKGEN_CONTROL_RTC_SEL_XTAL,
    AM_HAL_CLKGEN_CONTROL_RTC_SEL_LFRC,
    AM_HAL_CLKGEN_CONTROL_HFADJ_ENABLE,
    AM_HAL_CLKGEN_CONTROL_HFADJ_DISABLE,
    AM_HAL_CLKGEN_CONTROL_I3CCLKEN_ENABLE,
    AM_HAL_CLKGEN_CONTROL_I3CCLKEN_DISABLE,
    AM_HAL_CLKGEN_CONTROL_PLLVCOEN_ENABLE,
    AM_HAL_CLKGEN_CONTROL_PLLVCOEN_NORMALOP,
    AM_HAL_CLKGEN_CONTROL_PLLDIVEN_ENABLE,
    AM_HAL_CLKGEN_CONTROL_PLLDIVEN_NORMALOP,
    AM_HAL_CLKGEN_CONTROL_SELECT_EXTREF,
    AM_HAL_CLKGEN_CONTROL_SELECT_XT,
    AM_HAL_CLKGEN_CONTROL_DISPCTRLCLK_ENABLE,
    AM_HAL_CLKGEN_CONTROL_DISPCTRLCLK_DISABLE,
    AM_HAL_CLKGEN_CONTROL_GFXCORECLK_ENABLE,
    AM_HAL_CLKGEN_CONTROL_GFXCORECLK_DISABLE
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
    AM_HAL_CLKGEN_CLKOUT_XTAL_32768     = CLKGEN_CLKOUT_CKSEL_XT_DIV2,      // XTAL         = 32KHz
    AM_HAL_CLKGEN_CLKOUT_XTAL_16384     = CLKGEN_CLKOUT_CKSEL_XT_DIV4,      // XTAL / 4     = 16KHz
    AM_HAL_CLKGEN_CLKOUT_XTAL_8192      = CLKGEN_CLKOUT_CKSEL_XT_DIV8,      // XTAL / 8     = 8KHz
    AM_HAL_CLKGEN_CLKOUT_XTAL_4096      = CLKGEN_CLKOUT_CKSEL_XT_DIV16,     // XTAL / 16    = 4KHz
    AM_HAL_CLKGEN_CLKOUT_XTAL_2048      = CLKGEN_CLKOUT_CKSEL_XT_DIV32,     // XTAL / 32    = 2KHz
    AM_HAL_CLKGEN_CLKOUT_XTAL_1024      = CLKGEN_CLKOUT_CKSEL_XT_DIV256,    // XTAL / 256   = 1KHz
    AM_HAL_CLKGEN_CLKOUT_XTAL_128       = CLKGEN_CLKOUT_CKSEL_XT_DIV8K,     // XTAL / 256   = 128 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_4         = CLKGEN_CLKOUT_CKSEL_XT_DIV64K,    // XTAL / 8192  =  4 Hz
    AM_HAL_CLKGEN_CLKOUT_XTAL_0_5       = CLKGEN_CLKOUT_CKSEL_XT_DIV2M,     // XTAL / 65536 =  0.5 Hz

    // RTC
    AM_HAL_CLKGEN_CLKOUT_RTC_1Hz        = CLKGEN_CLKOUT_CKSEL_RTC_1Hz,      // RTC          = 1Hz
    AM_HAL_CLKGEN_CLKOUT_RTC_100Hz      = CLKGEN_CLKOUT_CKSEL_RTC_CG_100Hz, // RTC          = 100Hz

    // LFRC
    AM_HAL_CLKGEN_CLKOUT_LFRC_NOMINAL   = CLKGEN_CLKOUT_CKSEL_LFRC_NOMINAL, // LFRC         ~ 900 Hz
    AM_HAL_CLKGEN_CLKOUT_LFRC_DIV2      = CLKGEN_CLKOUT_CKSEL_LFRC_DIV2,    // LFRC / 2     ~ 450 Hz

    // PLL VCO250M
    AM_HAL_CLKGEN_CLKOUT_PLL_VCO250M    = CLKGEN_CLKOUT_CKSEL_PLLVCO_250,   // PLLVCO 250MHz
    AM_HAL_CLKGEN_CLKOUT_PLL_VCO125M    = CLKGEN_CLKOUT_CKSEL_PLLVCO_125,   // PLLVCO 125MHz

    // PLL
    AM_HAL_CLKGEN_CLKOUT_PLL_POSTDIV    = CLKGEN_CLKOUT_CKSEL_PLLPOSTDIV,   // PLL            PLL POSTDIV
    AM_HAL_CLKGEN_CLKOUT_PLL_OUT2       = CLKGEN_CLKOUT_CKSEL_PLLFOUT2,     // PLL            PLLFOUT2
    AM_HAL_CLKGEN_CLKOUT_PLL_OUT3       = CLKGEN_CLKOUT_CKSEL_PLLFOUT3,     // PLL            PLLFOUT3
    AM_HAL_CLKGEN_CLKOUT_PLL_OUT4       = CLKGEN_CLKOUT_CKSEL_PLLFOUT4,     // PLL            PLLFOUT4

    // RF XTAL
    AM_HAL_CLKGEN_CLKOUT_RF_XTAL_48M    = CLKGEN_CLKOUT_CKSEL_RFXTAL48,     // RF XTAL       = 48MHz
    AM_HAL_CLKGEN_CLKOUT_RF_XTAL_24M    = CLKGEN_CLKOUT_CKSEL_RFXTAL24,     // RF XTAL       = 24MHz
    AM_HAL_CLKGEN_CLKOUT_RF_XTAL_12M    = CLKGEN_CLKOUT_CKSEL_RFXTAL12,     // RF XTAL       = 12MHz

    // Secure RTC clocks
    AM_HAL_CLKGEN_CLKOUT_SECURE_RTC_1HZ   = CLKGEN_CLKOUT_CKSEL_SECURE_RTC_1Hz,      // Secure RTC 1Hz
    AM_HAL_CLKGEN_CLKOUT_SECURE_RTC_100HZ = CLKGEN_CLKOUT_CKSEL_SECURE_RTC_CG_100Hz, // Secure RTC 100Hz

    //AO CLK
    AM_HAL_CLKGEN_CLKOUT_AO_CLK_DIV16     = CLKGEN_CLKOUT_CKSEL_AO_DIV_CLK,  //AO_CLK div16
} am_hal_clkgen_clkout_e;

#define AM_HAL_CLKGEN_CLKOUT_MAX        AM_HAL_CLKGEN_CLKOUT_AO_CLK_DIV16         // Highest valid CKSEL enum value


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
    //!         LFRC is selected as the oscillator in OCTRL.RTCOSEL.
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
//!
//!     AM_HAL_CLKGEN_CLKOUT_RTC_1Hz
//!     AM_HAL_CLKGEN_CLKOUT_RTC_100Hz
//!
//!     AM_HAL_CLKGEN_CLKOUT_LFRC
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_512
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_32
//!     AM_HAL_CLKGEN_CLKOUT_LFRC_2
//!
//!     AM_HAL_CLKGEN_CLKOUT_PLL_POSTDIV
//!     AM_HAL_CLKGEN_CLKOUT_PLL_OUT2
//!     AM_HAL_CLKGEN_CLKOUT_PLL_OUT3
//!     AM_HAL_CLKGEN_CLKOUT_PLL_OUT4
//!
//!     AM_HAL_CLKGEN_CLKOUT_RF_XTAL_48M
//!     AM_HAL_CLKGEN_CLKOUT_RF_XTAL_24M
//!     AM_HAL_CLKGEN_CLKOUT_RF_XTAL_12M
//!
//!     AM_HAL_CLKGEN_CLKOUT_SECURE_RTC_1HZ
//!     AM_HAL_CLKGEN_CLKOUT_SECURE_RTC_100HZ
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
