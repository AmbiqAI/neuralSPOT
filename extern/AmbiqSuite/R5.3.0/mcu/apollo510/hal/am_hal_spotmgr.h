//*****************************************************************************
//
//! @file am_hal_spotmgr.h
//!
//! @brief SPOT manager functions that manage power states.
//!
//! @addtogroup spotmgr5b SPOTMGR - SPOT Manager
//! @ingroup apollo510_hal
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
// This is part of revision release_sdk5p0p0-5f68a8286b of the AmbiqSuite Development Package.
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
//! Disable handling for FT, PCM0.7
#define AM_HAL_SPOTMGR_PCM0_7_DISABLE   0
//! Disable handling for PCM1.0
#define AM_HAL_SPOTMGR_PCM1_0_DISABLE   0
//! Disable handling for PCM1.1, PCM2.0
#define AM_HAL_SPOTMGR_PCM2_0_DISABLE   0
//! Disable handling for PCM2.1
#define AM_HAL_SPOTMGR_PCM2_1_DISABLE   0
//! Disable handling for PCM2.2
#define AM_HAL_SPOTMGR_PCM2_2_DISABLE   0

//! Macros to force all PCM version to be disabled when AM_HAL_DISABLE_SPOTMGR
//! is defined
#ifdef AM_HAL_DISABLE_SPOTMGR
#undef AM_HAL_SPOTMGR_PCM0_7_DISABLE
#undef AM_HAL_SPOTMGR_PCM1_0_DISABLE
#undef AM_HAL_SPOTMGR_PCM2_0_DISABLE
#undef AM_HAL_SPOTMGR_PCM2_1_DISABLE
#undef AM_HAL_SPOTMGR_PCM2_2_DISABLE
#define AM_HAL_SPOTMGR_PCM0_7_DISABLE    1
#define AM_HAL_SPOTMGR_PCM1_0_DISABLE    1
#define AM_HAL_SPOTMGR_PCM2_0_DISABLE    1
#define AM_HAL_SPOTMGR_PCM2_1_DISABLE    1
#define AM_HAL_SPOTMGR_PCM2_2_DISABLE    1
#endif

//! Macros to indicate whether Internal Timer is required by SPOT manager
#define AM_HAL_SPOTMGR_INTERNAL_TIMER_NEEDED    ((!AM_HAL_SPOTMGR_PCM2_1_DISABLE) || (!AM_HAL_SPOTMGR_PCM2_2_DISABLE))

//*****************************************************************************
//
//! Defines
//
//*****************************************************************************

//! Mask of all available peripherals for DEVPWRST
#define DEVPWRST_ALL_PERIPH_MASK   0x3FFFFFFF
//! Mask of all available peripherals for AUDSSPWRST
#define AUDSSPWRST_ALL_PERIPH_MASK 0x4C4
//! Check if it is the profile for collapsing STM and STM+Periph
#define IS_PROFILE_COLLAPSE_STM_AND_STMP (g_sSpotMgrProfile.PROFILE_b.COLLAPSESTMANDSTMP == AM_HAL_SPOTMGR_COLLAPSE_STM_STMP_EN)
//! Mask of DEV peripherals monitored by SPOT manager
#define DEVPWRST_MONITOR_PERIPH_MASK   DEVPWRST_ALL_PERIPH_MASK
//! Mask of AUDSS peripherals monitored by SPOT manager
#define AUDSSPWRST_MONITOR_PERIPH_MASK AUDSSPWRST_ALL_PERIPH_MASK
//! Maximum value of VREFGEN2_b.TVRGCVREFTRIM
#define MAX_VDDC_TRIM (MCUCTRL_VREFGEN2_TVRGCVREFTRIM_Msk >> MCUCTRL_VREFGEN2_TVRGCVREFTRIM_Pos)
//! Maximum value of VREFGEN4_b.TVRGFVREFTRIM
#define MAX_VDDF_TRIM (MCUCTRL_VREFGEN4_TVRGFVREFTRIM_Msk >> MCUCTRL_VREFGEN4_TVRGFVREFTRIM_Pos)
//! Maximum value of VREFGEN3_b.TVRGCLVVREFTRIM
#define MAX_VDDC_LV_TRIM (MCUCTRL_VREFGEN3_TVRGCLVVREFTRIM_Msk >> MCUCTRL_VREFGEN3_TVRGCLVVREFTRIM_Pos)
//! Replace the 50us blocking delay with WFI in am_hal_spotmgr_pcm2_2
#define AM_HAL_SPOTMGR_REPLACE_DELAY_WITH_WFI false

//*****************************************************************************
//
//! Typedef
//
//*****************************************************************************

//
//! Struct for PWRSTATE INFO1 regs and global variables
//
typedef union
{
    uint32_t PWRSTATE;
    struct
    {
        uint32_t TVRGFACTTRIM      : 7;
        uint32_t CORELDOACTTRIM    : 10;
        uint32_t CORELDOTEMPCOTRIM : 4;
        uint32_t TVRGCACTTRIM      : 7;
        uint32_t TVRGCVREFSEL      : 1; // This field is no longer used
        uint32_t                   : 3;
    } PWRSTATE_b;
} am_hal_spotmgr_trim_settings_t;

//
//! Struct for GPUVDDCTON INFO1 regs and global variables
//
typedef union
{
    uint32_t GPUVDDCTON;
    struct
    {
        uint32_t GPULPCPULPVDDCTON   : 5;
        uint32_t GPULPCPUHPVDDCTON   : 5;
        uint32_t GPUHPCPULPVDDCTON   : 5;
        uint32_t GPUHPCPUHPVDDCTON   : 5;
        uint32_t PWRSTATE12VDDCTON   : 5;
        uint32_t                     : 7;
    } GPUVDDCTON_b;
} am_hal_spotmgr_gpu_vddc_ton_settings_t;

//
//! Struct for GPUVDDFTON INFO1 regs and global variables
//
typedef union
{
    uint32_t GPUVDDFTON;
    struct
    {
        uint32_t GPULPCPULPVDDFTON   : 5;
        uint32_t GPULPCPUHPVDDFTON   : 5;
        uint32_t GPUHPCPULPVDDFTON   : 5;
        uint32_t GPUHPCPUHPVDDFTON   : 5;
        uint32_t                     : 12;
    } GPUVDDFTON_b;
} am_hal_spotmgr_gpu_vddf_ton_settings_t;

//
//! Struct for STMTON INFO1 regs and global variables
//
typedef union
{
    uint32_t STMTON;
    struct
    {
        uint32_t STMCPULPVDDCTON      : 5;
        uint32_t STMCPUHPVDDCTON      : 5;
        uint32_t STMCPULPVDDFTON      : 5;
        uint32_t STMCPUHPVDDFTON      : 5;
        uint32_t PWRSTATE8VDDCTON     : 5;
        uint32_t                      : 7;
    } STMTON_b;
} am_hal_spotmgr_stm_ton_settings_t;

//
//! Struct for DEFAULTTON INFO1 regs and global variables
//
typedef union
{
    uint32_t DEFAULTTON;
    struct
    {
        uint32_t VDDCACTLOWTON        : 5;
        uint32_t VDDCACTHIGHTON       : 5;
        uint32_t VDDFACTLOWTON        : 5;
        uint32_t VDDFACTHIGHTON       : 5;
        uint32_t                      : 12;
    } DEFAULTTON_b;
} am_hal_spotmgr_default_ton_settings_t;

//
//! Struct for MEMLDOCONFIG INFO1 regs and global variables
//
typedef union
{
    uint32_t MEMLDOCONFIG;
    struct
    {
        uint32_t MEMLDOD2ASPARE      : 2;
        uint32_t MEMLDOACTTRIM       : 6;
        uint32_t TVGRFPWLOFFSET      : 6;
        uint32_t DFLTMEMLDOACTTRIM   : 6;
        uint32_t VDDFCOMPTRIMMINUS   : 6; // PCM2.1 does not have this bit field in INFO1, we initialised this field in global variable in spotmgr init.
        uint32_t                     : 6;
    } MEMLDOCONFIG_b;
} am_hal_spotmgr_memldo_settings_t;

//
//! Struct for VDDCLVACTTRIMADJ INFO1 regs and global variables
//
typedef union
{
    uint32_t VDDCLVACTTRIMADJ;
    struct
    {
        uint32_t TVRGCLVACTTRIM0      : 7;
        uint32_t TVRGCLVACTTRIM1      : 7;
        uint32_t TVRGCLVACTTRIM2      : 7;
        uint32_t TVRGCLVACTTRIM3      : 7;
        uint32_t                      : 4;
    } VDDCLVACTTRIMADJ_b;
} am_hal_spotmgr_vddclv_trim_settings_t;

//
//! SPOT manager INFO1 register structure
//
typedef struct
{
    uint32_t ui32SpotMgrINFO1GlobalValid;
    am_hal_spotmgr_trim_settings_t         sPowerStateArray[20]; // 0x1370 ~ 0x13BC
    am_hal_spotmgr_gpu_vddc_ton_settings_t          sGpuVddcTon; // 0x13C0
    am_hal_spotmgr_gpu_vddf_ton_settings_t          sGpuVddfTon; // 0x13C4
    am_hal_spotmgr_stm_ton_settings_t                   sStmTon; // 0x13C8
    am_hal_spotmgr_default_ton_settings_t           sDefaultTon; // 0x13CC
    am_hal_spotmgr_vddclv_trim_settings_t     sVddclvActTrimAdj; // 0x13D0
    am_hal_spotmgr_memldo_settings_t                 sMemldoCfg; // 0x13E0
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
    AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_HP,
    AM_HAL_SPOTMGR_CPUSTATE_SLEEP_DEEP,
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
    AM_HAL_SPOTMGR_GPUSTATE_ACTIVE_LP,
    AM_HAL_SPOTMGR_GPUSTATE_ACTIVE_HP,
} am_hal_spotmgr_gpu_state_e;

//
//! Struct for reporting GPU state change
//
typedef struct
{
    //! Set this to true when GPU state is changing
    bool bReqNewGpuState;

    //! The new/target GPU state
    am_hal_spotmgr_gpu_state_e eGpuState;
} am_hal_spotmgr_gpu_state_req_t;

//
//! Temperature range enum
//
typedef enum
{
    //! -40C ~ -20C
    AM_HAL_SPOTMGR_TEMPCO_RANGE_VERY_LOW,
    //! -20C ~ 0C
    AM_HAL_SPOTMGR_TEMPCO_RANGE_LOW,
    //! 0C ~ 50C
    AM_HAL_SPOTMGR_TEMPCO_RANGE_MID,
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
} am_hal_spotmgr_stimulus_e;

//
//! Enum for collapsing STM and STM+Periph
//
typedef enum
{
    AM_HAL_SPOTMGR_COLLAPSE_STM_STMP_DIS  = 0,
    AM_HAL_SPOTMGR_COLLAPSE_STM_STMP_EN   = 1
} am_hal_spotmgr_collapse_stm_stmp_e;

//
//! Struct for SPOT manager profile
//
typedef union
{
    uint32_t PROFILE;
    struct
    {
        uint32_t COLLAPSESTMANDSTMP   : 1;  // Bit for enable/disable collapsing STM and STM+Periph
        uint32_t                      : 31; // Reserved
    } PROFILE_b;
} am_hal_spotmgr_profile_t;

//
// Include addtional header file for each PCM
//
#include "am_hal_spotmgr_pcm2_2.h"
#include "am_hal_spotmgr_pcm2_1.h"
#include "am_hal_spotmgr_pcm2_0.h"
#include "am_hal_spotmgr_pcm0_7.h"

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
extern bool g_bIsPCM2p2OrNewer;
extern bool g_bIsPCM2p1;
extern bool g_bIsB0PCM1p1OrNewer;
extern bool g_bIsPCM2p0;
extern bool g_bIsPCM2p1WoPatch;

extern am_hal_spotmgr_profile_t g_sSpotMgrProfile;
extern am_hal_spotmgr_info1_regs_t g_sSpotMgrINFO1regs;
extern bool g_bHpToDeepSleep;
extern bool g_bVddfLpMinusForLp;
extern bool g_bTempLessThan50C;
extern bool g_bSwitchingToHp;
extern bool g_bVddcaorVddcpuOverride;

//*****************************************************************************
//
//! @brief Function for initializing the timer for SPOT manager.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_timer_init(void);

//*****************************************************************************
//
//! @brief Function for starting the timer for SPOT manager.
//!
//! @param ui32TimerDelayInUs - Timer compare value in uSec.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_timer_start(uint32_t ui32TimerDelayInUs);

//*****************************************************************************
//
//! @brief Function for restarting the timer for SPOT manager.
//!
//! @param ui32TimerDelayInUs - Timer compare value in uSec.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_timer_restart(uint32_t ui32TimerDelayInUs);

//*****************************************************************************
//
//! @brief Function for stopping the timer for SPOT manager.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_timer_stop(void);

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
//! @brief Set flag to postpone tempco handling till the function
//!        am_hal_spotmgr_pcm2_0_tempco_pending_handler() is invoked
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_tempco_postpone(void);

//*****************************************************************************
//
//! @brief Check for pending tempco operation, execute it if pending, and clear
//!        the blocking flag.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_tempco_pending_handle(void);

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
//! @brief SIMOBUCK initialziation handling at stage just before overriding
//!        LDO/SIMOBUCK
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_simobuck_init_bfr_ovr(void);

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

//*****************************************************************************
//
//! @brief Intitialize TON Config parameter during low_power_init.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_ton_config_init(void);

//*****************************************************************************
//
//! @brief Update Ton Config
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_ton_config_update(bool bGpuOn, am_hal_pwrctrl_gpu_mode_e eGpuSt);

//*****************************************************************************
//
//! @brief Power settings after CPU LP to HP transition
//!
//! @return None.
//
//*****************************************************************************
extern uint32_t am_hal_spotmgr_post_lptohp_handle(void);

//*****************************************************************************
//
//! @brief Timer interrupt service for spotmgr
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_boost_timer_interrupt_service(void);

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
//! @brief Set profile of SPOT manager
//!        This API is used to set SPOT manager profile to change the logic of
//!        power state determination. We implemented 2 profiles as below.
//!
//!        When psProfile->PROFILE_b.COLLAPSESTMANDSTMP ==
//!        AM_HAL_SPOTMGR_COLLAPSE_STM_STMP_DIS (the default profile), keep the
//!        original power state.
//!
//!        When psProfile->PROFILE_b.COLLAPSESTMANDSTMP ==
//!        AM_HAL_SPOTMGR_COLLAPSE_STM_STMP_EN, collapse the STM and STM+periph
//!        power states(0&4, 1&5, 2&6, 3&7, 8&12, 9&13, 10&14, 11&15). We suggest
//!        falling back to this profile for scenarios with frequent peripheral
//!        bursts and the overall time of all peripherals off state is short,
//!        which result in a lower overall average power compared to the default.
//!
//! Important:
//!        After setting the profile, the new profile
//!        takes effect in the next calling to am_hal_spotmgr_power_state_update,
//!        except the calling to am_hal_spotmgr_power_state_update when waking up MCU
//!        from normal sleep or deep sleep, entering normal sleep from CPU HP or LP
//!        mode, entering deepsleep from HP mode.
//!
//! @param psProfile - Pointer of am_hal_spotmgr_profile_t struct.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_profile_set(am_hal_spotmgr_profile_t * psProfile);

//*****************************************************************************
//
//! @brief Get current profile which is saved to g_ui32SpotMgrProfile
//!
//! @param psProfile - Pointer of am_hal_spotmgr_profile_t struct.
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_profile_get(am_hal_spotmgr_profile_t * psProfile);

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


