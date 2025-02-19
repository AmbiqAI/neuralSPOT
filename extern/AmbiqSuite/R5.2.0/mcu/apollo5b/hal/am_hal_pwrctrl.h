//*****************************************************************************
//
//! @file am_hal_pwrctrl.h
//!
//! @brief Functions for enabling and disabling power domains.
//!
//! @addtogroup pwrctrl4 PWRCTRL - Power Control
//! @ingroup apollo5b_hal
//! @{

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

#ifndef AM_HAL_PWRCTRL_H
#define AM_HAL_PWRCTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_hal_spotmgr.h"

//*****************************************************************************
//
//! @name Delays (in uS) required for VDDF/VDDC trim enhancements.
//! @{
//
//*****************************************************************************
#define AM_HAL_PWRCTRL_VDDF_BOOST_DELAY     20
#define AM_HAL_PWRCTRL_MEMLDO_BOOST_DELAY   20
#define AM_HAL_PWRCTRL_VDDC_BOOST_DELAY     20
#define AM_HAL_PWRCTRL_GOTOLDO_DELAY        20
//! @}

//*****************************************************************************
//
//! Option for coreldo and memldo to operate in parallel with simobuck
//!  AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL
//!      0 = Do not turn on LDOs in parallel with simobuck.
//!      1 = Turn on LDOs in parallel with simobuck and set their voltage levels
//!          ~35mV lower than minimum buck voltages.
//  Default: 1
//  NOTE: FOR Apollo5b - this must be 1
//
//*****************************************************************************
#define AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL    1  // Enabled for Apollo5b silicon

#if (AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL == 0)
#error Apollo5b requires AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL.
#endif

//*****************************************************************************
//
//! Option to assist VDDC by activating LDOs when disabling SIMOBUCK.
//!  AM_HAL_PWRCTRL_LDOS_FOR_VDDC
//!      0 = Do not assist VDDC.
//!      1 = Activate LDOs in parallel when disabling SIMOBUCK.
//!  Default: 1
//
//*****************************************************************************
#define AM_HAL_PWRCTRL_LDOS_FOR_VDDC                    0
#if ( (AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL == 0) && (AM_HAL_PWRCTRL_LDOS_FOR_VDDC != 0) )
#warning AM_HAL_PWRCTRL_LDOS_FOR_VDDC requires AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL.
#endif

//*****************************************************************************
//
//! Option to connect MCU core to VDDC_LV for increased power efficiency. Ambiq
//! recommends this option be enabled for all new applications
//
//
//! Default: 1 for RevC
//*****************************************************************************
#define AM_HAL_PWRCTRL_CORE_PWR_OPTIMAL_EFFICIENCY      0

//*****************************************************************************
//
//! Option to enable Simobuck to switch to Active from LP mode in DeepSleep
//! if there is increase in load
//! PCM 0.7 trim allows keeping this option disabled as long as temp < 70C
//! More code updates needed before this can be enabled
//
//! Default: 1
//*****************************************************************************
#define AM_HAL_PWRCTRL_SIMOLP_AUTOSWITCH                1

//*****************************************************************************
//
//! If the application wants to suspend tempsensing during deeplseep, it is a
//! must to set NO_TEMPSENSE_IN_DEEPSLEEP to true to restore VDDC and VDDF
//! settings before deepsleep. This MACRO is set to false by default.
//
//*****************************************************************************
#define NO_TEMPSENSE_IN_DEEPSLEEP                       false

//*****************************************************************************
//
//! Option for the TempCo power minimum power.
//! The tempco algorithm is still being tuned for optimal operation, and hence
//! it is possible that for certain temperature ranges, it does not yet get the
//! most optimal settings (might increase power, when compared to without tempco).
//
//*****************************************************************************
//*****************************************************************************
//
//! Enable VDDC TempCo
//! VDDC Tempco was found to created elevated current under room temperature,
//! and hence is disabled by default currently.
//
//*****************************************************************************
#define AM_HAL_ENABLE_TEMPCO_VDDC                       false

//*****************************************************************************
//
//! Enable VDDD TempCo
//
//*****************************************************************************
#define AM_HAL_ENABLE_TEMPCO_VDDF                       true


//*****************************************************************************
//
//! Definition of temperature threshold of buck state control before seepsleep
//! and VDDC & VDDF temperrature compensation.
//
//*****************************************************************************
#define BUCK_LP_TEMP_THRESHOLD      50.0f
#define VDDC_VDDF_TEMPCO_THRESHOLD  35.0f
#define TEMP_HYSTERESIS              2.0f
#define LOW_LIMIT                 -273.0f
#define HIGH_LIMIT                1000.0f
#define VDDC_VDDF_TEMPCO_THRESHOLD_LOW  -20.0f
#define VDDC_VDDF_TEMPCO_THRESHOLD_MID    0.0f
#define VDDC_VDDF_TEMPCO_THRESHOLD_HIGH  50.0f

//
//! Maximum time to wait for performance switch acknowledge before declaring
//! error.
//
#define AM_HAL_PWRCTRL_PERF_SWITCH_WAIT_US  20

//
//! When waking up from deepsleep to HP mode, CPU will wake up even if not operating on HP mode
//! yet. During the transition period, it will run as LP mode.
//! Default setting of 0 is better for reducing wakeup latency
//! Set AM_HAL_STALL_CPU_HPWAKE to 1 if we need to block CPU after wakeup till running in HP mode
//
#define AM_HAL_STALL_CPU_HPWAKE      0

//
//! VDDC to VDDC_LV short and VDDS to VDDF short
//! bShort - true:  Short VDDC to VDDC_LV and VDDS to VDDF.
//!          false: Remove VDDC and VDDC_LV short and VDDS to VDDF short.
//!
//
#define vddc_to_vddclv_and_vdds_to_vddf_short(bShort) \
do                                                    \
{                                                     \
    if (bShort)                                       \
    {                                                 \
        MCUCTRL->PWRSW1_b.SHORTVDDCVDDCLVORVAL = 1;   \
        MCUCTRL->PWRSW1_b.SHORTVDDCVDDCLVOREN = 1;    \
        MCUCTRL->PWRSW1_b.SHORTVDDFVDDSORVAL = 1;     \
        MCUCTRL->PWRSW1_b.SHORTVDDFVDDSOREN = 1;      \
    }                                                 \
    else                                              \
    {                                                 \
        MCUCTRL->PWRSW1_b.SHORTVDDCVDDCLVORVAL = 0;   \
        MCUCTRL->PWRSW1_b.SHORTVDDCVDDCLVOREN = 0;    \
        MCUCTRL->PWRSW1_b.SHORTVDDFVDDSORVAL = 0;     \
        MCUCTRL->PWRSW1_b.SHORTVDDFVDDSOREN = 0;      \
    }                                                 \
} while (0);

//*****************************************************************************
//
//! @name Performace mode enums.
//! @{
//
//*****************************************************************************
//
//! Peripheral MCU Mode power enum.
//
typedef enum
{
    AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER        = PWRCTRL_MCUPERFREQ_MCUPERFSTATUS_LP, // 96 MHz
    AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE = PWRCTRL_MCUPERFREQ_MCUPERFSTATUS_HP, // 192 MHz or 250 MHz
} am_hal_pwrctrl_mcu_mode_e;

//
//! Peripheral GPU Mode power enum.
//
typedef enum
{
    AM_HAL_PWRCTRL_GPU_MODE_HFRC_LP          = PWRCTRL_GFXPERFREQ_GFXPERFREQ_HFRC_96MHz,    // HFRC   96 MHz LP mode
    AM_HAL_PWRCTRL_GPU_MODE_HFRC_HP1         = PWRCTRL_GFXPERFREQ_GFXPERFREQ_HFRC_192MHz,   // HFRC  192 MHz HP1 mode
    AM_HAL_PWRCTRL_GPU_MODE_HFRC2_HP2        = PWRCTRL_GFXPERFREQ_GFXPERFREQ_HFRC2_125MHz,  // HFRC2 125 MHz HP2 mode
    AM_HAL_PWRCTRL_GPU_MODE_HFRC2_HP3        = PWRCTRL_GFXPERFREQ_GFXPERFREQ_HFRC2_250MHz,  // HFRC2 250 MHz HP3 mode
} am_hal_pwrctrl_gpu_mode_e;
#define AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER           AM_HAL_PWRCTRL_GPU_MODE_HFRC_LP         // Deprecated enum
#define AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE    AM_HAL_PWRCTRL_GPU_MODE_HFRC2_HP3       // Deprecated enum

//
//! vddf boost mode enum.
//
typedef enum
{
    AM_HAL_PWRCTRL_VDDF_BOOST_DEFAULT,
    AM_HAL_PWRCTRL_VDDF_BOOST_LEVEL1,
    AM_HAL_PWRCTRL_VDDF_BOOST_LEVEL2,
} am_hal_pwrctrl_vddf_boost_level_e;

//! @}

//*****************************************************************************
//
//! Peripheral power enum.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_PERIPH_IOS0,
    AM_HAL_PWRCTRL_PERIPH_IOSFD0,
    AM_HAL_PWRCTRL_PERIPH_IOSFD1,
    AM_HAL_PWRCTRL_PERIPH_IOM0,
    AM_HAL_PWRCTRL_PERIPH_IOM1,
    AM_HAL_PWRCTRL_PERIPH_IOM2,
    AM_HAL_PWRCTRL_PERIPH_IOM3,
    AM_HAL_PWRCTRL_PERIPH_IOM4,
    AM_HAL_PWRCTRL_PERIPH_IOM5,
    AM_HAL_PWRCTRL_PERIPH_IOM6,
    AM_HAL_PWRCTRL_PERIPH_IOM7,
    AM_HAL_PWRCTRL_PERIPH_UART0,
    AM_HAL_PWRCTRL_PERIPH_UART1,
    AM_HAL_PWRCTRL_PERIPH_UART2,
    AM_HAL_PWRCTRL_PERIPH_UART3,
    AM_HAL_PWRCTRL_PERIPH_ADC,
    AM_HAL_PWRCTRL_PERIPH_MSPI0,
    AM_HAL_PWRCTRL_PERIPH_MSPI1,
    AM_HAL_PWRCTRL_PERIPH_MSPI2,
    AM_HAL_PWRCTRL_PERIPH_MSPI3,
    AM_HAL_PWRCTRL_PERIPH_GFX,
    AM_HAL_PWRCTRL_PERIPH_DISP,
    AM_HAL_PWRCTRL_PERIPH_DISPPHY,
    AM_HAL_PWRCTRL_PERIPH_CRYPTO,
    AM_HAL_PWRCTRL_PERIPH_SDIO0,
    AM_HAL_PWRCTRL_PERIPH_SDIO1,
    AM_HAL_PWRCTRL_PERIPH_USB,
    AM_HAL_PWRCTRL_PERIPH_USBPHY,
    AM_HAL_PWRCTRL_PERIPH_DEBUG,
    AM_HAL_PWRCTRL_PERIPH_OTP,
    AM_HAL_PWRCTRL_PERIPH_PDM0,
    AM_HAL_PWRCTRL_PERIPH_I2S0,
    AM_HAL_PWRCTRL_PERIPH_I2S1,
    AM_HAL_PWRCTRL_PERIPH_AUDADC,
    AM_HAL_PWRCTRL_PERIPH_MAX
} am_hal_pwrctrl_periph_e;

//*****************************************************************************
//
// MCU memory control settings.
//
//*****************************************************************************

//
//! DTCM enable settings.
//
typedef enum
{
    AM_HAL_PWRCTRL_ITCM_DTCM_NONE       = PWRCTRL_MEMPWREN_PWRENTCM_NONE,
    AM_HAL_PWRCTRL_ITCM32K_DTCM128K     = PWRCTRL_MEMPWREN_PWRENTCM_ITCM32K_DTCM128K,
    AM_HAL_PWRCTRL_ITCM128K_DTCM256K    = PWRCTRL_MEMPWREN_PWRENTCM_ITCM128K_DTCM256K,
    AM_HAL_PWRCTRL_ITCM256K_DTCM512K    = PWRCTRL_MEMPWREN_PWRENTCM_ITCM256K_DTCM512K,
} am_hal_pwrctrl_dtcm_select_e;

//
//! DTCM retain settings.
//
typedef enum
{
  AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN = PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
  AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN = PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN,
} am_hal_pwrctrl_dtcm_retain_e;

//
//! NVM enable settings.
//
typedef enum
{
    AM_HAL_PWRCTRL_NVM_NONE       = 0,
    AM_HAL_PWRCTRL_NVM0_ONLY      = 1,
    AM_HAL_PWRCTRL_NVM0_AND_NVM1  = 3,
} am_hal_pwrctrl_nvm_select_e;

//
//!ROM settings.
//
typedef enum
{
    AM_HAL_PWRCTRL_ROM_ALWAYS_ON,               // Turn on ROM and always leave it on.
    AM_HAL_PWRCTRL_ROM_AUTO                     // Allow HAL to manage ROM power state.
} am_hal_pwrctrl_rom_select_e;

//
//! MCU memory configuration structure.
//
typedef struct
{
    //
    //! ROM mode selection.
    //
    am_hal_pwrctrl_rom_select_e         eROMMode;

    //
    //! DTCM configuration
    //
    am_hal_pwrctrl_dtcm_select_e        eDTCMCfg;
    am_hal_pwrctrl_dtcm_retain_e        eRetainDTCM;

    //
    //! NVM configuration.
    //
    am_hal_pwrctrl_nvm_select_e         eNVMCfg;
    bool                                bKeepNVMOnInDeepSleep;
} am_hal_pwrctrl_mcu_memory_config_t;

//
//! Miscellaneous power controls.
//
typedef enum
{
    AM_HAL_PWRCTRL_CONTROL_SIMOBUCK_INIT,       // Enable the SIMOBUCK
    AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN,    // Power down Crypto
    AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, // Allow the crystal to power down during deepsleep
    AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL,     // Power down all peripherals
} am_hal_pwrctrl_control_e;

//*****************************************************************************
//
//! Shared memory control settings.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_SRAM_NONE      = PWRCTRL_SSRAMPWREN_PWRENSSRAM_NONE,
    AM_HAL_PWRCTRL_SRAM_1M        = PWRCTRL_SSRAMPWREN_PWRENSSRAM_GROUP0,
    AM_HAL_PWRCTRL_SRAM_2M        = PWRCTRL_SSRAMPWREN_PWRENSSRAM_GROUP0 | PWRCTRL_SSRAMPWREN_PWRENSSRAM_GROUP1,
    AM_HAL_PWRCTRL_SRAM_3M        = PWRCTRL_SSRAMPWREN_PWRENSSRAM_ALL,
} am_hal_pwrctrl_sram_select_e;

typedef struct
{
    //
    //! SRAM banks to enable.
    //
    am_hal_pwrctrl_sram_select_e        eSRAMCfg;

    //
    //! For each of the eActiveWithXxx settings:
    //!  AM_HAL_PWRCTRL_SRAM_NONE    = This component has no association with SSRAM.
    //!  AM_HAL_PWRCTRL_SRAM_1M_GRP0 = Group1 ignored by this component. Group0 is active when this component is powered on.
    //!  AM_HAL_PWRCTRL_SRAM_1M_GRP1 = Group0 ignored by this component. Group1 is active when this component is powered on.
    //!  AM_HAL_PWRCTRL_SRAM_ALL     = All SRAM active when this component is powered on.
    //

    //
    //! Activate SRAM when the MCU is active.
    //
    am_hal_pwrctrl_sram_select_e        eActiveWithMCU;

    //
    //! Activate SRAM when the Graphics is active.
    //
    am_hal_pwrctrl_sram_select_e        eActiveWithGFX;

    //
    //! Activate SRAM when the DISPLAY is active.
    //
    am_hal_pwrctrl_sram_select_e        eActiveWithDISP;

    //
    //! Retain SRAM in deep sleep.
    //! For SRAM retention:
    //!  AM_HAL_PWRCTRL_SRAM_NONE    = Retain all SRAM in deepsleep.
    //!  AM_HAL_PWRCTRL_SRAM_1M_GRP0 = Retain Group0, power down Group1 in deepsleep.
    //!  AM_HAL_PWRCTRL_SRAM_1M_GRP1 = Retain Group1, power down Group0 in deepsleep.
    //!  AM_HAL_PWRCTRL_SRAM_ALL     = Retain all SRAM in deepsleep.
    //
    am_hal_pwrctrl_sram_select_e        eSRAMRetain;

} am_hal_pwrctrl_sram_memcfg_t;

//*****************************************************************************
//
//! System power status structure
//
//*****************************************************************************
typedef struct
{
    //
    //! DEVPWRSTATUS - Device Power ON Status
    //
    uint32_t ui32Device;

    //
    //! AUDSSPWRSTATUS - Audio Subsystem ON Status
    //
    uint32_t ui32AudioSS;

    //
    //! MEMPWRSTATUS - MCU Memory Power ON Status
    //
    uint32_t ui32Memory;

    //
    //! SYSPWRSTATUS - Power ON Status for MCU Core
    //
    uint32_t ui32System;

    //
    //! SSRAMPWRST - Shared SRAM Power ON Status
    //
    uint32_t ui32SSRAM;

    //
    //! AUDSSPWRSTATUS - Audio Subsystem Power ON Status
    //
    uint32_t ui32Audio;

    //
    //! VRSTATUS - Voltage Regulators status
    //
    uint32_t ui32VRStatus;

    //
    //! ADCSTATUS - Power Status Register for ADC Block
    //
    uint32_t ui32ADC;

    //
    //! AUDADCSTATUS - Power Status Register for audio ADC Block
    //
    uint32_t ui32AudioADC;
} am_hal_pwrctrl_status_t;

//*****************************************************************************
//
//! INFO1 register structure
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32INFO1GlobalValid;

    uint32_t ui32SBLVer0;                   // 00 Apollo5b MRAM
    uint32_t ui32SBLVer1;                   // 04 Apollo5b MRAM

    uint32_t ui32MAINPTR;                   // 10
    uint32_t ui32SBLOTA;                    // 18 Apollo5b

    uint32_t ui32SOCID0;                    // 20 OTP
    uint32_t ui32SOCID1;                    // 24 OTP
    uint32_t ui32SOCID2;                    // 28 OTP
    uint32_t ui32SOCID3;                    // 2C OTP
    uint32_t ui32SOCID4;                    // 30 OTP
    uint32_t ui32SOCID5;                    // 34 OTP
    uint32_t ui32SOCID6;                    // 38 OTP
    uint32_t ui32SOCID7;                    // 3C OTP
    uint32_t ui32PATCH_TRACKER0;            // 40

    uint32_t ui32TEMP_CAL_ATE;              // 00
    uint32_t ui32TEMP_CAL_MEASURED;         // 04
    uint32_t ui32TEMP_CAL_ADC_OFFSET;       // 08
    uint32_t ui32ADC_GAIN_ERR;              // 28
    uint32_t ui32ADC_OFFSET_ERR;            // 2C

    uint32_t ui32AUDADC_A0_LG_OFFSET;       // 40
    uint32_t ui32AUDADC_A0_HG_SLOPE;        // 44
    uint32_t ui32AUDADC_A0_HG_INTERCEPT;    // 48
    uint32_t ui32AUDADC_A1_LG_OFFSET;       // 4C
    uint32_t ui32AUDADC_A1_HG_SLOPE;        // 50
    uint32_t ui32AUDADC_A1_HG_INTERCEPT;    // 54
    uint32_t ui32AUDADC_B0_LG_OFFSET;       // 58
    uint32_t ui32AUDADC_B0_HG_SLOPE;        // 5C
    uint32_t ui32AUDADC_B0_HG_INTERCEPT;    // 60
    uint32_t ui32AUDADC_B1_LG_OFFSET;       // 64
    uint32_t ui32AUDADC_B1_HG_SLOPE;        // 68
    uint32_t ui32AUDADC_B1_HG_INTERCEPT;    // 6C
} am_hal_pwrctrl_info1_regs_t;

#define INFO1DATAENTRIES    (sizeof(am_hal_pwrctrl_info1_regs_t) / 4)

//
// Define a value used in ui32INFO1GlobalValid and ui32SpotMgrINFO1GlobalValid to indicate
// that INFO1 has been saved globally.
//
#define INFO1GLOBALVALID          0x1F01600D

extern am_hal_pwrctrl_info1_regs_t g_sINFO1regs;

//*****************************************************************************
//
//! RLPSTATE, powerup state for PDRAMS power domain.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_RLP_ON,
    AM_HAL_PWRCTRL_RLP_RESERVED,
    AM_HAL_PWRCTRL_RLP_RESERVED1,
    AM_HAL_PWRCTRL_RLP_OFF
} am_hal_pwrctrl_pwrmodctl_rlp_e;

//*****************************************************************************
//
//! ELPSTATE, type of low-power state for PDEPU.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_ELP_ON,
    AM_HAL_PWRCTRL_ELP_ON_CLK_OFF,
    AM_HAL_PWRCTRL_ELP_RET,
    AM_HAL_PWRCTRL_ELP_OFF
} am_hal_pwrctrl_pwrmodctl_elp_e;

//*****************************************************************************
//
//! CLPSTATE, type of low-power state for PDCORE.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_CLP_ON,
    AM_HAL_PWRCTRL_CLP_ON_CLK_OFF,
    AM_HAL_PWRCTRL_CLP_RET,
    AM_HAL_PWRCTRL_CLP_OFF
} am_hal_pwrctrl_pwrmodctl_clp_e;

//*****************************************************************************
//
//! CPDLPSTATE structure
//
//*****************************************************************************
typedef struct
{
    //
    //!RLPSTATE
    //
    am_hal_pwrctrl_pwrmodctl_rlp_e eRlpConfig;

    //
    //!ELPSTATE
    //
    am_hal_pwrctrl_pwrmodctl_elp_e eElpConfig;

    //
    //!CLPSTATE
    //
    am_hal_pwrctrl_pwrmodctl_clp_e eClpConfig;
} am_hal_pwrctrl_pwrmodctl_cpdlp_t;

//*****************************************************************************
//
//! Temperature threshold structure
//
//*****************************************************************************
typedef struct
{
    float fLowThresh;
    float fHighThresh;
} am_hal_pwrctrl_temp_thresh_t;

//*****************************************************************************
//
//! Temperature range enum for tempco
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_TEMPCO_RANGE_LOW,
    AM_HAL_PWRCTRL_TEMPCO_RANGE_MID,
    AM_HAL_PWRCTRL_TEMPCO_RANGE_HIGH,
    AM_HAL_PWRCTRL_TEMPCO_OUT_OF_RANGE
} am_hal_pwrctrl_tempco_range_e;

//*****************************************************************************
//
//! Mask of PWRCTRL->SYSPWRSTATUS sleep power status bits.
//! This mask is useful for clearing all the bits after reading them, e.g.
//! PWRCTRL->SYSPWRSTATUS = PWRCTRL_SYSPWRSTATUS_SLEEP_Msk;
//
//*****************************************************************************
#define PWRCTRL_SYSPWRSTATUS_SLEEP_Msk                  \
            (PWRCTRL_SYSPWRSTATUS_SYSDEEPSLEEP_Msk  |   \
             PWRCTRL_SYSPWRSTATUS_COREDEEPSLEEP_Msk |   \
             PWRCTRL_SYSPWRSTATUS_CORESLEEP_Msk)

//*****************************************************************************
//
//! @name Default configurations
//! @{
//
//*****************************************************************************
extern am_hal_pwrctrl_mcu_memory_config_t            g_DefaultMcuMemCfg;
extern const am_hal_pwrctrl_sram_memcfg_t            g_DefaultSRAMCfg;
//! @}

//*****************************************************************************
//
//! @name Global Variables
//! @{
//
// ****************************************************************************
extern bool     g_bOrigTrimsStored;
extern uint32_t g_ui32TrimVer;
extern am_hal_pwrctrl_mcu_mode_e g_eCurMcuPwrMode;
extern am_hal_pwrctrl_gpu_mode_e g_eCurGpuPwrMode;
//! @}


// ****************************************************************************
//
//! @brief Return the current MCU performance mode.
//!
//! @param peCurrentPowerMode is a ptr to a variable to save the current status.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors.
//
// ****************************************************************************
extern uint32_t am_hal_pwrctrl_mcu_mode_status(am_hal_pwrctrl_mcu_mode_e *peCurrentPowerMode);

//*****************************************************************************
//
//! @brief Change the MCU performance mode.
//!
//! @param ePowerMode is the performance mode for the MCU
//!
//! Select the power mode for the MCU.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_mcu_mode_select(am_hal_pwrctrl_mcu_mode_e ePowerMode);

// ****************************************************************************
//
//! @brief Return the current GPU performance mode.
//!
//! @param peCurrentPowerMode is a ptr to a variable to save the current status.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors.
//
// ****************************************************************************
extern uint32_t am_hal_pwrctrl_gpu_mode_status(am_hal_pwrctrl_gpu_mode_e *peCurrentPowerMode);

//*****************************************************************************
//
//! @brief Change the GPU performance mode.
//!
//! @param ePowerMode is the performance mode for the GPU
//!
//! Select the power mode for the GPU.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_gpu_mode_select(am_hal_pwrctrl_gpu_mode_e ePowerMode);

//*****************************************************************************
//
//! @brief Configure the power settings for the MCU memory.
//!
//! @param psConfig is a structure describing the desired memory configuration.
//!
//! Use this function to enable, disable, or change the sleep configuration of
//! MCU memory.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_mcu_memory_config(am_hal_pwrctrl_mcu_memory_config_t *psConfig);

//*****************************************************************************
//
//! @brief Read the power settings for the MCU memory.
//!
//! @param psConfig is a structure describing the desired memory configuration.
//!
//! Use this function to check the current settings for the MCU memories.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_mcu_memory_config_get(am_hal_pwrctrl_mcu_memory_config_t *psConfig);

//*****************************************************************************
//
//! @brief Enable the ROM power domain.
//!
//! @param none.
//!
//! Use this function to enable the ROM power domain.  If the global ROM power
//! state is AM_HAL_PWRCTRL_ROM_ALWAYS_ON, then this function will have no effect.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_rom_enable(void);

//*****************************************************************************
//
//! @brief Disable the ROM power domain.
//!
//! @param none.
//!
//! Use this function to disable the ROM power domain.  If the global ROM power
//! state is AM_HAL_PWRCTRL_ROM_ALWAYS_ON, then this function will have no effect.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_rom_disable(void);

//*****************************************************************************
//
//! @brief Configure the power settings for the Shared RAM.
//!
//! @param psConfig is a structure describing the desired memory configuration.
//!
//! Use this function to enable, disable, or change the sleep configuration of
//! MCU memory.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_sram_config(am_hal_pwrctrl_sram_memcfg_t *psConfig);

//*****************************************************************************
//
//! @brief Get the power settings for the Shared RAM.
//!
//! @param psConfig is a structure describing the desired memory configuration.
//!
//! Use this function to check the current settings for the MCU memories.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_sram_config_get(am_hal_pwrctrl_sram_memcfg_t *psConfig);

//*****************************************************************************
//
//! @brief Enable power to a peripheral.
//!
//! @param ePeripheral - The peripheral to enable.
//!
//! This function enables power to the peripheral and waits for a
//! confirmation from the hardware.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_periph_enable(am_hal_pwrctrl_periph_e ePeripheral);

//*****************************************************************************
//
//! @brief Disable power to a peripheral.
//!
//! @param ePeripheral - The peripheral to disable.
//!
//! This function disables power to the peripheral and waits for a
//! confirmation from the hardware.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_periph_disable(am_hal_pwrctrl_periph_e ePeripheral);

//*****************************************************************************
//
//! @brief Determine whether a peripheral is currently enabled.
//!
//! @param ePeripheral - The peripheral to enable.
//! @param bEnabled - Pointer to a ui32 that will return as 1 or 0.
//!
//! This function determines to the caller whether a given peripheral is
//! currently enabled or disabled.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_periph_enabled(am_hal_pwrctrl_periph_e ePeripheral,
                                              bool *bEnabled);

//*****************************************************************************
//
//! @brief Get the current powercontrol status registers.
//!
//! @param psStatus returns a structure containing power status information.
//!
//! This function can be used to determine the current status of a wide variety
//! of system components.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_status_get(am_hal_pwrctrl_status_t *psStatus);

//*****************************************************************************
//
//! @brief Initialize system for low power configuration.
//!
//! This function implements various low power initialization and optimizations.
//! - See also am_hal_pwrctrl_control() for other power saving techniques.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_low_power_init(void);

//*****************************************************************************
//
//! @brief Miscellaneous power saving controls
//!
//! @param eControl - Power saving type, one of the following:
//!     AM_HAL_PWRCTRL_CONTROL_SIMOBUCK_INIT        - Enable the SIMOBUCK
//!     AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP  - Allow the crystal to power
//!                                                   down during deepsleep
//!
//! @param pArgs - Pointer to arguments for Control Switch Case
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_control(am_hal_pwrctrl_control_e eControl, void *pArgs);

//*****************************************************************************
//
//! @brief Restore original Power settings
//!
//! This function restores default power trims reverting relative
//! changes done as part of low_power_init and SIMOBUCK init.
//! User needs to make sure device is running in Low Power mode before calling
//! this function.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_settings_restore(void);


//*****************************************************************************
//
//! @brief Configure CPDLPSTATE.
//!
//! @param sCpdlpConfig - CPDLPSTATE configurations.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_pwrmodctl_cpdlp_config(am_hal_pwrctrl_pwrmodctl_cpdlp_t sCpdlpConfig);

//*****************************************************************************
//
//! @brief Get CPDLPSTATE configurations.
//!
//! @param psCpdlpConfig - Pointer of CPDLPSTATE configurations structure.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_pwrctrl_pwrmodctl_cpdlp_get(am_hal_pwrctrl_pwrmodctl_cpdlp_t * psCpdlpConfig);

// ****************************************************************************
//
//! @brief A function for temperature compensation for power.
//!
//! @param fCurTemp     - Current temperature in Celsius.
//! @param psTempThresh - This API returns this structure for notifying of thresholds
//!                       when application should make this call again (When temp goes
//!                       lower than fLowThresh or rises above fHighThresh).
//!
//! @return AM_HAL_STATUS_SUCCESS or errors
//!
//! If current temperature is higher than BUCK_LP_TEMP_THRESHOLD(e.g. 50c),
//! bFrcBuckAct must be set to true. Otherwise, set bFrcBuckAct to false.
//
// ****************************************************************************
extern uint32_t am_hal_pwrctrl_temp_update(float fCurTemp,
                                           am_hal_pwrctrl_temp_thresh_t * psTempThresh);

//*****************************************************************************
//
//! @brief Restore VDDC and VDDF settings before deepsleep to allow application to
//! suspend tempsensing during deeplseep.
//
//*****************************************************************************
#if NO_TEMPSENSE_IN_DEEPSLEEP
extern void vddc_vddf_setting_restore(void);
#endif

//*****************************************************************************
//
//! @brief Enable power to system PLL.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_syspll_enable(void);

//*****************************************************************************
//
//! @brief Disable power to system PLL.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_syspll_disable(void);

// ****************************************************************************
//
//! @brief  Determine whether system PLL is currently enabled.
//! @param  bEnabled - Pointer to a bool that will return as true if system PLL
//!                    is powered up, false otherwise.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
// ****************************************************************************
extern uint32_t am_hal_pwrctrl_syspll_enabled(bool *bEnabled);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_PWRCTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
