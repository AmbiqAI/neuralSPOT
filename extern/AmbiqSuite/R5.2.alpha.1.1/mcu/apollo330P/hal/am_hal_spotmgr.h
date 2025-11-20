//*****************************************************************************
//
//! @file am_hal_spotmgr.h
//!
//! @brief SPOT manager functions that manage power states.
//!
//! @addtogroup spotmgr5b_ap510L SPOTMGR - SPOT Manager
//! @ingroup apollo330P_hal
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
#ifndef AM_HAL_SPOTMGR_H
#define AM_HAL_SPOTMGR_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Configurations
//
//*****************************************************************************
//! Disable handling for trimver1
#define AM_HAL_SPOTMGR_TRIMVER_1_DISABLE   0
//! Disable handling for trimver2
#define AM_HAL_SPOTMGR_TRIMVER_2_DISABLE   0

//! Macros to force all PCM version to be disabled when AM_HAL_DISABLE_SPOTMGR
//! is defined
#ifdef AM_HAL_DISABLE_SPOTMGR
#undef AM_HAL_SPOTMGR_TRIMVER_1_DISABLE
#undef AM_HAL_SPOTMGR_TRIMVER_2_DISABLE
#define AM_HAL_SPOTMGR_TRIMVER_1_DISABLE    1
#define AM_HAL_SPOTMGR_TRIMVER_2_DISABLE    1
#endif

//! Macros to indicate whether Internal Timer is required by SPOT manager
#define AM_HAL_SPOTMGR_INTERNAL_TIMER_NEEDED    (0)

//*****************************************************************************
//
//! Defines
//
//*****************************************************************************

//! Mask of all available peripherals for DEVPWRST, excluding SYSPLL
#define DEVPWRST_ALL_PERIPH_MASK   0xFFFDE6FE
//! Mask of peripherals which need to boost both VDDC and VDDF for DEVPWRST,
//! including NETAOL, OTP, USBPHY/USB, SDIO, CRYPTO, DISP/DISPPHY, GFX, MSPI,
//! I2S, PDM, I3C/I3CPHY, DBG, IOM4/5.
#define DEVPWRST_BOOST_VDDCF_PERIPH_MASK   0xCFFDC0E0
//! Mask of peripherals which need to boost VDDC only for DEVPWRST, including
//! IOS, UART, IOM0/1/2/3.
#define DEVPWRST_BOOST_VDDC_PERIPH_MASK   0x3000061E
//! Mask of all available peripherals for AUDSSPWRST
#define AUDSSPWRST_ALL_PERIPH_MASK 0x44
//! Mask of DEV peripherals monitored for power state 1
#define DEVPWRST_MONITOR_PERIPH_MASK_PS1   DEVPWRST_BOOST_VDDCF_PERIPH_MASK
//! Mask of DEV peripherals monitored for power state 2
#define DEVPWRST_MONITOR_PERIPH_MASK_PS2   DEVPWRST_BOOST_VDDC_PERIPH_MASK
//! Mask of DEV peripherals monitored by SPOT manager
#define DEVPWRST_MONITOR_PERIPH_MASK   DEVPWRST_ALL_PERIPH_MASK
//! Mask of AUDSS peripherals monitored for power state 1
#define AUDSSPWRST_MONITOR_PERIPH_MASK_PS1 AUDSSPWRST_ALL_PERIPH_MASK
//! Mask of AUDSS peripherals monitored by SPOT manager
#define AUDSSPWRST_MONITOR_PERIPH_MASK AUDSSPWRST_ALL_PERIPH_MASK

//*****************************************************************************
//
//! Typedef
//
//*****************************************************************************

//
//! Struct for PWRSTATE 0 trims
//
typedef union
{
    uint32_t PWRSTATE0TRIM;
    struct
    {
        uint32_t VDDCRXCOMPTRIMMINUS : 5;
        uint32_t VDDFRXCOMPTRIMMINUS : 5;
        uint32_t                     : 22;
    } PWRSTATE0TRIM_b;
} am_hal_spotmgr_trim_settings_t;

//
//! Struct for SCM CNTRCTRL2
//
typedef union
{
    uint32_t CNTRCTRL2;
    struct
    {
        uint32_t FCNT1 : 16;
        uint32_t FCNT2 : 16;
    } CNTRCTRL2_b;
} am_hal_spotmgr_scm_cntrctrl2_settings_t;

//
//! Struct for SCM LPTHRESHVDDS/F/C/CLV/RF
//
typedef union
{
    uint32_t LPTHRESH;
    struct
    {
        uint32_t LPTHRESHVDD : 20;
        uint32_t             : 12;
    } LPTHRESH_b;
} am_hal_spotmgr_scm_lpthresh_settings_t;

//
//! Struct for SCM LPHYSTCNT
//
typedef union
{
    uint32_t LPHYST;
    struct
    {
        uint32_t LPHYSTCNT   : 20;
        uint32_t             : 12;
    } LPHYST_b;
} am_hal_spotmgr_scm_lphyst_settings_t;

//
//! Struct for SCM ACTTHRESH1
//
typedef union
{
    uint32_t ACTTHRESH1;
    struct
    {
        uint32_t ACTTHRESHVDDF   : 16;
        uint32_t ACTTHRESHVDDS   : 16;
    } ACTTHRESH1_b;
} am_hal_spotmgr_scm_actthresh1_t;

//
//! Struct for SCM ACTTHRESH2
//
typedef union
{
    uint32_t ACTTHRESH2;
    struct
    {
        uint32_t ACTTHRESHVDDCLV : 16;
        uint32_t ACTTHRESHVDDC   : 16;
    } ACTTHRESH2_b;
} am_hal_spotmgr_scm_actthresh2_t;

//
//! Struct for SCM ACTTHRESH3
//
typedef union
{
    uint32_t ACTTHRESH3;
    struct
    {
        uint32_t ACTTHRESHVDDRF  : 16;
        uint32_t                 : 16;
    } ACTTHRESH3_b;
} am_hal_spotmgr_scm_actthresh3_t;

//
//! SPOT manager INFO1 register structure
//
typedef struct
{
    uint32_t ui32SpotMgrINFO1GlobalValid;
    am_hal_spotmgr_trim_settings_t                 sPowerState0Trims; // 0x1440
    am_hal_spotmgr_scm_cntrctrl2_settings_t            sScmCntrCtrl2; // 0x1444
    am_hal_spotmgr_scm_lpthresh_settings_t          sScmLpThreshVdds; // 0x1448
    am_hal_spotmgr_scm_lpthresh_settings_t          sScmLpThreshVddf; // 0x144C
    am_hal_spotmgr_scm_lpthresh_settings_t          sScmLpThreshVddc; // 0x1450
    am_hal_spotmgr_scm_lpthresh_settings_t        sScmLpThreshVddclv; // 0x1454
    am_hal_spotmgr_scm_lpthresh_settings_t         sScmLpThreshVddrf; // 0x1458
    am_hal_spotmgr_scm_lphyst_settings_t               sScmLpHystCnt; // 0x145C
    am_hal_spotmgr_scm_actthresh1_t            sScmActThresh1; // 0x1460
    am_hal_spotmgr_scm_actthresh2_t            sScmActThresh2; // 0x1464
    am_hal_spotmgr_scm_actthresh3_t            sScmActThresh3; // 0x1468
} am_hal_spotmgr_info1_regs_t;

//*****************************************************************************
//
//! SPOTmanager stimulus and profiles
//
//*****************************************************************************

//
//! CPU/CM55 status enum
//
typedef enum
{
    AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_LP,
    AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_HP1,
    AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_HP2,
    AM_HAL_SPOTMGR_CPUSTATE_SLEEP_DEEP,
    AM_HAL_SPOTMGR_CPUSTATE_SLEEP_DEEPER,
    AM_HAL_SPOTMGR_CPUSTATE_SLEEP_ARM, // Place holder - may be used in the future
    AM_HAL_SPOTMGR_CPUSTATE_SLEEP_AMB, // Place holder - may be used in the future
} am_hal_spotmgr_cpu_state_e;

//
//! GPU status enum
//! We never transition from LP to HP or vice versa - we always go to off first.
//
typedef enum
{
    AM_HAL_SPOTMGR_GPUSTATE_OFF,
    AM_HAL_SPOTMGR_GPUSTATE_ACTIVE,
} am_hal_spotmgr_gpu_state_e;

//
//! Temperature range enum
//
typedef enum
{
    //! -40C ~ 50C
    AM_HAL_SPOTMGR_TEMPCO_RANGE_LOW,
    //! > 50C
    AM_HAL_SPOTMGR_TEMPCO_RANGE_HIGH,
    AM_HAL_SPOTMGR_TEMPCO_OUT_OF_RANGE
} am_hal_spotmgr_tempco_range_e;

//
//! CPU, GPU, memories and peripherals power status, temperature range.
//
typedef struct
{
    uint32_t ui32DevPwrSt; // Place holder - may be used in the future
    uint32_t ui32AudSSPwrSt; // Place holder - may be used in the future
    uint32_t ui32MemPwrSt; // Place holder - may be used in the future
    uint32_t ui32SsramPwrSt; // Place holder - may be used in the future
    am_hal_spotmgr_tempco_range_e eTempRange;
    am_hal_spotmgr_cpu_state_e eCpuState;
    am_hal_spotmgr_gpu_state_e eGpuState;
} am_hal_spotmgr_power_status_t;

//
//! Temperature Stimulus param struct
//
typedef struct
{
    //! Current temperature
    float fTemperature;
    //! returned value for lower temperature range
    float fRangeLower;
    //! returned value for higher temperature range
    float fRangeHigher;
} am_hal_spotmgr_tempco_param_t;

//
//! Stimulus for power state transition
//
typedef enum
{
    //! CPU power and performance state
    AM_HAL_SPOTMGR_STIM_CPU_STATE,
    //! GPU power and performance state
    AM_HAL_SPOTMGR_STIM_GPU_STATE,
    //! Temperature range
    AM_HAL_SPOTMGR_STIM_TEMP,
    //! Peripherals power state included in DEVPWRSTATUS regs, except GPU.
    AM_HAL_SPOTMGR_STIM_DEVPWR,
    //! Peripherals power state included in AUDSSPWRSTATUS regs.
    AM_HAL_SPOTMGR_STIM_AUDSSPWR,
    //! Memory power state included in MEMPWRSTATUS regs.
    AM_HAL_SPOTMGR_STIM_MEMPWR, // Place holder - may be used in the future
    //! SSRAM power state included in SSRAMPWRST regs.
    AM_HAL_SPOTMGR_STIM_SSRAMPWR, // Place holder - may be used in the future
    //! Initialise MCU power state after SPOTMGR and SIMOBUCK are both initialised
    AM_HAL_SPOTMGR_STIM_INIT_STATE,
} am_hal_spotmgr_stimulus_e;


//
// Include addtional header file for each PCM
//
#include "am_hal_spotmgr_trimver_1.h"
#include "am_hal_spotmgr_trimver_2.h"

//
// Define macro AM_HAL_SPOTMGR_PROFILING to enable SPOTMGR profiling functionality.
// Define macro AM_HAL_SPOTMGR_PROFILING_VERBOSE to enable verbose entries.
//
// #define AM_HAL_SPOTMGR_PROFILING
// #define AM_HAL_SPOTMGR_PROFILING_VERBOSE

//
// SPOT manager Profiling struct and markers
//
#ifdef AM_HAL_SPOTMGR_PROFILING
typedef struct
{
  union
  {
    struct
    {
      uint32_t pwrState  : 6;
      uint32_t tonState  : 6;
      uint32_t eStimulus : 4;
      uint32_t bOn       : 2;
      uint32_t resv      : 14;
    } s;
    uint32_t u32;
  } u;
  uint32_t args;
} am_hal_spotmgr_changelog_t;

#define AM_HAL_SPOTMGR_PROFILING_PWRST_INVALID 0x3F
#define AM_HAL_SPOTMGR_PROFILING_TONST_INVALID 0x3F
#define AM_HAL_SPOTMGR_PROFILING_TONST_VERBOSE 0x3E
#define AM_HAL_SPOTMGR_PROFILING_ESTIM_INVALID 0xF
#define AM_HAL_SPOTMGR_PROFILING_BON_INVALID   0x3
#endif

//*****************************************************************************
//
//! Global variables
//
//*****************************************************************************
//! Trim Version booleans for optimization of trim version eval
extern bool g_bIsTrimver1;
extern bool g_bIsTrimver1OrNewer;
extern bool g_bIsTrimver2OrNewer;

//! This table will be populated with SPOT manager related INFO1 values and
//! will be used for easy lookup after OTP is powered down.
extern am_hal_spotmgr_info1_regs_t g_sSpotMgrINFO1regs;

//*****************************************************************************
//
//! @brief Power states update
//!        This API should be called before turning things on and after turning
//!        things off, or before switching to high performance mode and after
//!        switching to low power mode, or when temperature range is changed.
//!
//! @param eStimulus - Stimilus for power states transition. For GPU state/power
//!                    changes, please use AM_HAL_SPOTMGR_STIM_GPU_STATE but not
//!                    AM_HAL_SPOTMGR_STIM_DEVPWR.
//! @param bOn       - Only needs to be set to true/false when turning on/off
//!                    peripherals or memories included in DEVPWRSTATUS,
//!                    AUDSSPWRSTATUS, MEMPWRSTATUS and SSRAMPWRST. It is
//!                    ignored when updating temperature, CPU state and GPU state.
//! @param pArgs     - Pointer to arguments for power states update, assign it
//!                    to NULL if not needed.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_CPU_STATE,
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_cpu_state_e enum.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_GPU_STATE,
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_gpu_state_e enum.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_TEMP,
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_tempco_param_t struct.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_DEVPWR,
//! bOn must be set to true when turning on a peripheral,
//! bOn must be set to false when turning off a peripheral,
//! and pArgs must point to the DEVPWRSTATUS_MASK of the peripheral to be opened
//! when turning on a peripheral, pArgs is ignored when turning off a peripheral.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_AUDSSPWR,
//! bOn must be set to true when turning on a peripheral,
//! bOn must be set to false when turning off a peripheral,
//! and pArgs must point to the AUDSSPWRSTATUS_MASK of the peripheral to be opened
//! when turning on a peripheral, pArgs is ignored when turning off a peripheral.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_MEMPWR,
//! bOn is ignored, and pArgs must point to the entire MEMPWRSTATUS.
//!
//! When eStimulus is AM_HAL_SPOTMGR_STIM_SSRAMPWR,
//! bOn must be set to true when turning on partial or entire SSRAM,
//! bOn must be set to false when turning off partial or entire SSRAM,
//! and pArgs must point to the expected SSRAMPWRST when turning on,
//! pArgs is ignored when turning off.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_power_state_update(am_hal_spotmgr_stimulus_e eStimulus, bool bOn, void *pArgs);

//*****************************************************************************
//
//! @brief Restore power state to POR default
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_default_reset(void);

//*****************************************************************************
//
//! @brief SIMOBUCK initialziation handling at stage just before enabling
//!        SIMOBUCK
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_simobuck_init_bfr_enable(void);

//*****************************************************************************
//
//! @brief SIMOBUCK initialziation handling at stage just after enabling
//!        SIMOBUCK
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_simobuck_init_aft_enable(void);

#if NO_TEMPSENSE_IN_DEEPSLEEP
//*****************************************************************************
//
//! @brief Prepare SPOT manager for suspended tempco during deep sleep
//!
//! @return SUCCESS or other Failures.
//!
//! This API is to be called before entering deepsleep if there is no handling
//! to execute temperature sensing periodically after the deepsleep is entered.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_tempco_suspend(void);
#endif

#if AM_HAL_PWRCTRL_SIMOLP_AUTOSWITCH
//*****************************************************************************
//
//! @brief Initialize registers for SIMOBUCK LP auto switch
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_simobuck_lp_autosw_init(void);

//*****************************************************************************
//
//! @brief Enable SIMOBUCK LP auto switch
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_simobuck_lp_autosw_enable(void);

//*****************************************************************************
//
//! @brief Disable SIMOBUCK LP auto switch
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_simobuck_lp_autosw_disable(void);
#endif

//*****************************************************************************
//
//! @brief SPOT manager init
//!        This API should be called from am_hal_pwrctrl_low_power_init, to
//!        initialise SPOT manager.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_init(void);

//*****************************************************************************
//
//! @brief Dummy Weak function for spotmgr log change event handler
//!
//! @param pChangeLog - Pointer of am_hal_spotmgr_changelog_t struct.
//!
//! @return None.
//
//*****************************************************************************
#ifdef AM_HAL_SPOTMGR_PROFILING
extern void am_hal_spotmgr_log_change(am_hal_spotmgr_changelog_t *pChangeLog);
#endif

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_SPOTMGR_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************


