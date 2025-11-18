//*****************************************************************************
//
//! @file am_hal_pwrctrl.h
//!
//! @brief Functions for enabling and disabling power domains.
//!
//! @addtogroup pwrctrl4_ap510L PWRCTRL - Power Control
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

#ifndef AM_HAL_PWRCTRL_H
#define AM_HAL_PWRCTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

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
//  Default: 0
//  NOTE: FOR Apollo510L - this must be 0
//
//*****************************************************************************
#define AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL    0  // Disabled for Apollo510L

#if (AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL == 1)
#error AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL must be set to 0 for Apollo510L.
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
//! Disable this option for Apollo510L
//! Default: 0
//*****************************************************************************
#define AM_HAL_PWRCTRL_SIMOLP_AUTOSWITCH                0

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
//! Compile switch for boosting VDDF for SDIO, it is false by default
//
//*****************************************************************************
#define BOOST_VDDF_FOR_SDIO                             false


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
// #define AM_HAL_STALL_CPU_HPWAKE      0

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
    AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1 = PWRCTRL_MCUPERFREQ_MCUPERFSTATUS_HP1, // 192 MHz
    AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2 = PWRCTRL_MCUPERFREQ_MCUPERFSTATUS_HP2, // 250 MHz
} am_hal_pwrctrl_mcu_mode_e;

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
    AM_HAL_PWRCTRL_PERIPH_IOM0,
    AM_HAL_PWRCTRL_PERIPH_IOM1,
    AM_HAL_PWRCTRL_PERIPH_IOM2,
    AM_HAL_PWRCTRL_PERIPH_IOM3,
    AM_HAL_PWRCTRL_PERIPH_IOM4,
    AM_HAL_PWRCTRL_PERIPH_IOM5,
    AM_HAL_PWRCTRL_PERIPH_I3CPHY,
    AM_HAL_PWRCTRL_PERIPH_UART0,
    AM_HAL_PWRCTRL_PERIPH_UART1,
    AM_HAL_PWRCTRL_PERIPH_ADC,
    AM_HAL_PWRCTRL_PERIPH_MSPI0,
    AM_HAL_PWRCTRL_PERIPH_MSPI1,
    AM_HAL_PWRCTRL_PERIPH_MSPI2,
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
    AM_HAL_PWRCTRL_PERIPH_IOSFD0,
    AM_HAL_PWRCTRL_PERIPH_IOSFD1,
    AM_HAL_PWRCTRL_PERIPH_I3C,
    AM_HAL_PWRCTRL_PERIPH_NETAOL,
    AM_HAL_PWRCTRL_PERIPH_PDM0,
    AM_HAL_PWRCTRL_PERIPH_I2S0,
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
    AM_HAL_PWRCTRL_DTCM_NONE       = PWRCTRL_MEMPWREN_PWRENTCM_NONE,
    AM_HAL_PWRCTRL_DTCM128K        = PWRCTRL_MEMPWREN_PWRENTCM_DTCM128K,
    AM_HAL_PWRCTRL_DTCM256K        = PWRCTRL_MEMPWREN_PWRENTCM_DTCM256K,
} am_hal_pwrctrl_dtcm_select_e;

//
//! DTCM retain settings.
//
typedef enum
{
  AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN = PWRCTRL_MEMRETCFG_TCMPWDSLP_DIS,
  AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN = PWRCTRL_MEMRETCFG_TCMPWDSLP_EN,
} am_hal_pwrctrl_dtcm_retain_e;

//
//! NVM enable settings.
//
typedef enum
{
    AM_HAL_PWRCTRL_NVM_NONE       = 0,
    AM_HAL_PWRCTRL_NVM            = 1,
} am_hal_pwrctrl_nvm_select_e;

//
//! NVM power down modes settings.
//
typedef enum
{
    AM_HAL_PWRCTRL_NVM_RETENTION_POWER_DOWN       = 0,
    AM_HAL_PWRCTRL_NVM_DEEP_POWER_DOWN            = 1,
    AM_HAL_PWRCTRL_NVM_FULL_POWER_DOWN            = 2,
} am_hal_pwrctrl_nvm_power_down_select_e;

//
//!ROM settings.
//
typedef enum
{
    AM_HAL_PWRCTRL_ROM_ALWAYS_ON,               // Turn on ROM and always leave it on.
    AM_HAL_PWRCTRL_ROM_AUTO                     // Allow HAL to manage ROM power state.
} am_hal_pwrctrl_rom_select_e;

//*****************************************************************************
//
//! CLPSTATE, type of low-power state for PDCORE.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_PWRCTRL_CM4_POWEROFF = PWRCTRL_CM4PWRSTATE_CM4PWRSTATUS_OFF,
    AM_HAL_PWRCTRL_CM4_POWERON = PWRCTRL_CM4PWRSTATE_CM4PWRSTATUS_ON,
    AM_HAL_PWRCTRL_CM4_POWERORET = PWRCTRL_CM4PWRSTATE_CM4PWRSTATUS_RET
} am_hal_pwrctrl_cm4_pwr_state_e;

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

    //
    //! NVM power down modes configuration.
    //
    am_hal_pwrctrl_nvm_power_down_select_e eNVMPowerDown;

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
    AM_HAL_PWRCTRL_SRAM_NONE         = PWRCTRL_SSRAMPWREN_PWRENSSRAM_NONE,
    AM_HAL_PWRCTRL_SRAM_1M           = PWRCTRL_SSRAMPWREN_PWRENSSRAM_GROUP0,
    AM_HAL_PWRCTRL_SRAM_0P75M        = PWRCTRL_SSRAMPWREN_PWRENSSRAM_GROUP1,
    AM_HAL_PWRCTRL_SRAM_1P75M        = PWRCTRL_SSRAMPWREN_PWRENSSRAM_ALL,
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

    uint32_t ui32SBLVer0;                   // 00 Apollo510L MRAM
    uint32_t ui32SBLVer1;                   // 04 Apollo510L MRAM

    uint32_t ui32MAINPTR;                   // 10
    uint32_t ui32SBLOTA;                    // 18 Apollo510L

    uint32_t ui32SOCID0;                    // 20 OTP
    uint32_t ui32SOCID1;                    // 24 OTP
    uint32_t ui32SOCID2;                    // 28 OTP
    uint32_t ui32SOCID3;                    // 2C OTP
    uint32_t ui32SOCID4;                    // 30 OTP
    uint32_t ui32SOCID5;                    // 34 OTP
    uint32_t ui32SOCID6;                    // 38 OTP
    uint32_t ui32SOCID7;                    // 3C OTP

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
extern uint32_t g_orig_CORELDOACTIVETRIM;

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
#define PWRCTRL_SYSPWRSTATUS_SLEEP_Msk                      \
            (PWRCTRL_SYSPWRSTATUS_SYSDEEPSLEEP_Msk      |   \
             PWRCTRL_SYSPWRSTATUS_COREDEEPSLEEP_Msk     |   \
             PWRCTRL_SYSPWRSTATUS_CORESLEEP_Msk         |   \
             PWRCTRL_SYSPWRSTATUS_SYSDEEPERSLEEP_Msk)

//*****************************************************************************
//
//! @name Default configurations
//! @{
//
//*****************************************************************************
extern const am_hal_pwrctrl_mcu_memory_config_t      g_DefaultMcuMemCfg;
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
//! This function does not support SYSPLL power control, please use
//! am_hal_pwrctrl_syspll_enable and am_hal_pwrctrl_syspll_disable for
//! SYSPLL power control.
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
//! This function does not support SYSPLL power control, please use
//! am_hal_pwrctrl_syspll_enable and am_hal_pwrctrl_syspll_disable for
//! SYSPLL power control.
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
//! This function does not support SYSPLL power status check, please use
//! am_hal_pwrctrl_syspll_enabled to check SYSPLL power status.
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
//!
//! @note IMPORTANT - This function requires access to INFO0 and/or INFO1 data.
//! Therefore, if either INFO0 or INFO1 is in OTP, OTP must be powered up prior
//! to calling this function. For more information, see the description for
//! am_hal_info0_read().
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
//! @brief This function no longer restores MCU to its boot state; instead, it prepares
//!        MCU for the next image to start in the correct state.
//!
//! Important:
//!
//! - This function must be called before
//!   transition to a new application, such as the case of a secondary bootloader
//!   transitioning to an application. Before calling this function, users
//!   should switch CPU to LP if it is in HP mode, and turned off GPU and all other
//!   peripherals if anyone was turned on, except OTP.
//!   Before calling this function, in order to release all clocks when exiting
//!   second boot loader, we suggest users to disable peripherals which are used
//!   in second bootloader and already requested clocks through clkmgr. Users
//!   should disable peripherals by calling the separated API for peripheral
//!   disable/deinit/powerdown, but not by writing to the power enable registers
//!   directly.
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
//! @brief Decides CPU will enter HP1 and wait until PLL_LOCK and switch to HP2.
//!
//! @param bWaitPllockForHp2 - true: CPU will NOT enter HP1. It will Wait for PLL_LOCK and then directly enter HP2 mode.
//!                            false:CPU will enter HP1 until PLL lock and then switch to HP2.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_pwrctrl_wait_pll_lock_for_hp2(bool bWaitPllockForHp2);

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

// ****************************************************************************
//
//! @brief Get the CM4 power state.
//!
//! @param pCm4pwrstate is a ptr to a variable to save the current status.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors.
//
// ****************************************************************************
extern uint32_t am_hal_pwrctrl_get_cm4_pwrstate(am_hal_pwrctrl_cm4_pwr_state_e * pCm4pwrstate);

//*****************************************************************************
//
//! @brief Wake up the CM4.
//!
//! @param none.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_cm4_wakeup_req(void);

//*****************************************************************************
//
//! @brief Power Up RSS .
//!
//! @param none.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************

extern uint32_t am_hal_pwrctrl_rss_bootup(void);

//*****************************************************************************
//
//! @brief Power off the RSS.
//!
//! @param none.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable PWRCTRL errors
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_rss_pwroff(void);

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
