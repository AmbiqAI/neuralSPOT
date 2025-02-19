//*****************************************************************************
//
//! @file am_hal_spotmgr.h
//!
//! @brief SPOT manager functions that manage power states.
//!
//! @addtogroup spotmgr5b SPOTMGR - SPOT Manager
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
#ifndef AM_HAL_SPOTMGR_H
#define AM_HAL_SPOTMGR_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Defines
//
//*****************************************************************************

//! Mask of all available peripherals for DEVPWRST
#define DEVPWRST_ALL_PERIPH_MASK   0x3FFFFFFF
//! Mask of all available peripherals for AUDSSPWRST
#define AUDSSPWRST_ALL_PERIPH_MASK 0x4C4
//! Mask of DEV peripherals monitored by SPOT manager
#define DEVPWRST_MONITOR_PERIPH_MASK   DEVPWRST_ALL_PERIPH_MASK
//! Mask of AUDSS peripherals monitored by SPOT manager
#define AUDSSPWRST_MONITOR_PERIPH_MASK AUDSSPWRST_ALL_PERIPH_MASK
//! Only bit[0:11] are used to determine power state.
#define PWR_STATE_DESC_MASK 0x00000FFF
//! Only bit[0:3] and bit[12:19] are used to determine power state.
#define TON_STATE_DESC_MASK 0x000FF00F
//! Timer instance for LDOs boosting, If timer number < 10, it is a must to insert
//! a 0 to the left to ensure 2-digit width.
#define AM_HAL_INTERNAL_TIMER_NUM_A 15
//! VDDCLVACTLOWTONTRIM trim code for power state 1 and 5
#define VDDCLVACTLOWTONTRIM_POWER_STATE_1_5 4
//! VDDCLVACTLOWTONTRIM trim code for all other power states except power state 1 and 5
#define VDDCLVACTLOWTONTRIM_DEFAULT 6

//*****************************************************************************
//
//! Power states and trim settings
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
        uint32_t                     : 12;
    } MEMLDOCONFIG_b;
} am_hal_spotmgr_memldo_settings_t;

//
//! SPOT manager INFO1 register structure
//
typedef struct
{
    uint32_t ui32SpotMgrINFO1GlobalValid;
    am_hal_spotmgr_trim_settings_t         sPowerStateArray[16]; // 0x1370 ~ 0x13AC
    am_hal_spotmgr_gpu_vddc_ton_settings_t          sGpuVddcTon; // 0x13C0
    am_hal_spotmgr_gpu_vddf_ton_settings_t          sGpuVddfTon; // 0x13C4
    am_hal_spotmgr_stm_ton_settings_t                   sStmTon; // 0x13C8
    am_hal_spotmgr_default_ton_settings_t           sDefaultTon; // 0x13CC
    am_hal_spotmgr_memldo_settings_t                 sMemldoCfg; // 0x13E0
} am_hal_spotmgr_info1_regs_t;

//
//! Enum for power states descriptions, only bit[0:11] are used to determine power state.
//
typedef enum
{
    AM_HAL_SPOTMGR_POWER_STATE_DESC_0  = 0x300, // CPULP, Temp 3
    AM_HAL_SPOTMGR_POWER_STATE_DESC_1  = 0x200, // CPULP, Temp 2
    AM_HAL_SPOTMGR_POWER_STATE_DESC_2  = 0x100, // CPULP, Temp 1
    AM_HAL_SPOTMGR_POWER_STATE_DESC_3  = 0x000, // CPULP, Temp 0
    AM_HAL_SPOTMGR_POWER_STATE_DESC_4  = 0x310, // CPULP + G/P, Temp 3
    AM_HAL_SPOTMGR_POWER_STATE_DESC_5  = 0x210, // CPULP + G/P, Temp 2
    AM_HAL_SPOTMGR_POWER_STATE_DESC_6  = 0x110, // CPULP + G/P, Temp 1
    AM_HAL_SPOTMGR_POWER_STATE_DESC_7  = 0x010, // CPULP + G/P, Temp 0
    AM_HAL_SPOTMGR_POWER_STATE_DESC_8  = 0x301, // CPUHP, Temp 3
    AM_HAL_SPOTMGR_POWER_STATE_DESC_9  = 0x201, // CPUHP, Temp 2
    AM_HAL_SPOTMGR_POWER_STATE_DESC_10 = 0x101, // CPUHP, Temp 1
    AM_HAL_SPOTMGR_POWER_STATE_DESC_11 = 0x001, // CPUHP, Temp 0
    AM_HAL_SPOTMGR_POWER_STATE_DESC_12 = 0x311, // CPUHP + G/P, Temp 3
    AM_HAL_SPOTMGR_POWER_STATE_DESC_13 = 0x211, // CPUHP + G/P, Temp 2
    AM_HAL_SPOTMGR_POWER_STATE_DESC_14 = 0x111, // CPUHP + G/P, Temp 1
    AM_HAL_SPOTMGR_POWER_STATE_DESC_15 = 0x011  // CPUHP + G/P, Temp 0
} am_hal_spotmgr_power_state_desc_e;

//
//! Enum for Ton states descriptions, only bit[0:3] and bit[12:19] are used to determine Ton state.
//
typedef enum
{
    AM_HAL_SPOTMGR_TON_STATE_DESC_0  = 0x00000, // CPULP
    AM_HAL_SPOTMGR_TON_STATE_DESC_1  = 0x00001, // CPUHP
    AM_HAL_SPOTMGR_TON_STATE_DESC_2  = 0x01000, // CPULP + GPU
    AM_HAL_SPOTMGR_TON_STATE_DESC_3  = 0x11000, // CPULP + GPU + Periph
    AM_HAL_SPOTMGR_TON_STATE_DESC_4  = 0x02000, // CPULP + GPUHP
    AM_HAL_SPOTMGR_TON_STATE_DESC_5  = 0x12000, // CPULP + GPUHP + Periph
    AM_HAL_SPOTMGR_TON_STATE_DESC_6  = 0x01001, // CPUHP + GPU
    AM_HAL_SPOTMGR_TON_STATE_DESC_7  = 0x11001, // CPUHP + GPU + Periph
    AM_HAL_SPOTMGR_TON_STATE_DESC_8  = 0x02001, // CPUHP + GPUHP
    AM_HAL_SPOTMGR_TON_STATE_DESC_9  = 0x12001, // CPUHP + GPUHP + Periph
    AM_HAL_SPOTMGR_TON_STATE_DESC_10 = 0x10000, // CPULP + Periph
    AM_HAL_SPOTMGR_TON_STATE_DESC_11 = 0x10001  // CPUHP + Periph
} am_hal_spotmgr_ton_state_desc_e;

//
//! Bitfield definitions for power states descriptions
//
typedef union
{
    am_hal_spotmgr_power_state_desc_e ePwrStateDesc;

    am_hal_spotmgr_ton_state_desc_e   eTonStateDesc;

    struct
    {
        //! CPU performance mode
        uint32_t CPUMODE        : 4;
        //! GPU and all peripheral power state
        uint32_t GPUPERIPHMODE  : 4;
        //! Temperature range
        uint32_t TEMPRANGE      : 4;
        //! GPU power state
        uint32_t GPUMODE        : 4;
        //! Peripheral power state
        uint32_t PERIPHMODE     : 4;
        //! Reserved
        uint32_t                : 12;
    } PWRSTATEDESC_b;
} am_hal_spotmgr_power_state_desc_t;

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

//*****************************************************************************
//
//! Globals
//
//*****************************************************************************
extern am_hal_spotmgr_info1_regs_t g_sSpotMgrINFO1regs;
extern bool g_bVddcaorVddcpuOverride;
extern bool g_bVddfLpMinusForLp;
extern bool g_bTempLessThan50C;
extern bool g_bSwitchingToHp;

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
//! bOn is ignored, and pArgs must point to a am_hal_spotmgr_tempco_range_e enum.
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
extern int32_t am_hal_spotmgr_power_state_update(am_hal_spotmgr_stimulus_e eStimulus, bool bOn, void *pArgs);

//*****************************************************************************
//
//! @brief SPOT manager init
//!        This API should be called from am_hal_pwrctrl_low_power_init, to
//!        initialise SPOT manager.
//!
//! @return SUCCESS or other Failures.
//
//*****************************************************************************
extern int32_t am_hal_spotmgr_init(void);

//*****************************************************************************
//
//! @brief Timer interrupt service for boosting LDOs and bucks
//!
//! @return None.
//
//*****************************************************************************
extern void am_hal_spotmgr_boost_timer_interrupt_service(void);

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


