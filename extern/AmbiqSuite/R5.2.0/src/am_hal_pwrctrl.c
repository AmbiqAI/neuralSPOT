//*****************************************************************************
//
//! @file am_hal_pwrctrl.c
//!
//! @brief Functions for enabling and disabling power domains.
//!
//! @addtogroup pwrctrl4 PWRCTRL - Power Control
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
#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "mcu/am_hal_sysctrl_ton_config.h"
#include "mcu/am_hal_sysctrl_clk_mux_reset.h"

static inline void vddc_vddf_tempco(am_hal_pwrctrl_tempco_range_e eTempCoRange);



//*****************************************************************************
//
// Local defines
//
//*****************************************************************************
//
// The CORELDOACTIVETRIM trim code reduced in simobuck init
//
#define CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT          6

//
// The CORELDOACTIVETRIM trim code reduced in simobuck init for both
// PCM1.1 (B0 trim version 3) and PCM2.0 (B1 trim version 1)
//
#define CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT_B0TRIM3_B1TRIM1  7

//
//! Maximum number of checks to memory power status before declaring error
// (5 x 1usec = 5usec).
//
#define AM_HAL_PWRCTRL_MAX_WAIT_US          5

#define AM_HAL_PWRCTRL_MAX_WAIT_OTP_US      100000
#define AM_HAL_PWRCTRL_MAX_BOOTROM_COUNT    10000

#define AM_HAL_PWRCTRL_MEMPWRSTATUS_MASK    ( PWRCTRL_MEMPWRSTATUS_PWRSTTCM_Msk        |    \
                                              PWRCTRL_MEMPWRSTATUS_PWRSTNVM0_Msk       |    \
                                              PWRCTRL_MEMPWRSTATUS_PWRSTNVM1_Msk       |    \
                                              PWRCTRL_MEMPWRSTATUS_PWRSTROM_Msk )

//
// Power trims
//
#define ADJUST_POWER_SETTING           true
#define RESTORE_POWER_SETTING          false
#define CORELDOTEMPCOTRIM_VAL          2

#define MEMLDOACTIVETRIM_DELTA_0        5
#define TVRGFVREFTRIM_DELTA_0           24

#define MEMLDOACTIVETRIM_DELTA_1        23

#define TVRGFVREFTRIM_VAL_0             127
#define TVRGFTEMPCOTRIM_VAL_0           31

//
//! Trim revision 6 is required for the following pwrctrl adjustments:
//!  - Sourcing MCUH from the VDDC_LV rail
//!  - Reliable application of TempCo
//!  - Applying Crypto boosts
//
#define TRIMREV_PWRCTRL         6

#define AM_HAL_PWRCTRL_GPU_VOLTADJ_WAIT  1
#define AM_HAL_PWRCTRL_GPU_PWRADJ_WAIT   6
#define AM_HAL_PWRCTRL_GPU_PWRON_WAIT    1

#define CORE_SLEEP_DURATION              2  //in us

//
//! Maximum delay for waiting for timer AM_HAL_INTERNAL_TIMER_NUM_A is disabled
//
#define TIMER_A_DISABLE_WAIT_IN_US   2500

void buck_ldo_update_override(bool bEnable);

//*****************************************************************************
//
//! @name Define max values of some useful fields
//! @{
//
// ****************************************************************************
#define MAX_ACTTRIMVDDF         _FLD2VAL(MCUCTRL_SIMOBUCK12_ACTTRIMVDDF, 0xFFFFFFFF)    // Buck VDDF
#define MAX_MEMLDOACTIVETRIM    _FLD2VAL(MCUCTRL_LDOREG2_MEMLDOACTIVETRIM, 0xFFFFFFFF)  // LDO VDDF
#define MAX_LPTRIMVDDF          _FLD2VAL(MCUCTRL_SIMOBUCK12_LPTRIMVDDF, 0xFFFFFFFF)     // VDDF LP
#define MAX_MEMLPLDOTRIM        _FLD2VAL(MCUCTRL_LDOREG2_MEMLPLDOTRIM, 0xFFFFFFFF)      // MEM LP LDO
#define MAX_BUCKVDDCTRIM        _FLD2VAL(MCUCTRL_VREFGEN2_TVRGVREFTRIM, 0xFFFFFFFF)     // Buck VDDC
//! @}

// ****************************************************************************
//
// Global variables.
//
// ****************************************************************************

//*****************************************************************************
//
//! @name Global State Variables for the VDDF and VDDC boosting
//! @{
//
// ****************************************************************************
am_hal_pwrctrl_mcu_mode_e       g_eCurMcuPwrMode    = AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER;
am_hal_pwrctrl_gpu_mode_e       g_eCurGpuPwrMode    = AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER;
am_hal_pwrctrl_gpu_mode_e       g_ePreGpuPwrMode    = AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER;
am_hal_pwrctrl_rom_select_e     g_eCurROMPwrMode    = AM_HAL_PWRCTRL_ROM_ALWAYS_ON;

uint32_t g_ui32TrimVer                      = 0xFFFFFFFF;
uint32_t g_ui32origSimobuckVDDStrim         = 0xFFFFFFFF;
uint32_t g_ui32VDDCBoostReqCnt              = 0;
uint32_t g_ui32VDDFBoostReqLevel1Cnt        = 0;
uint32_t g_ui32VDDFBoostReqLevel2Cnt        = 0;
//! @}

//*****************************************************************************
//
//! @name Save factory trim values.
//! @{
//
// ****************************************************************************
bool     g_bOrigTrimsStored          = false;
static uint32_t g_orig_ACTTRIMVDDF          = 0;
static uint32_t g_orig_MEMLDOACTIVETRIM     = 0;
static uint32_t g_orig_LPTRIMVDDF           = 0;
static uint32_t g_orig_MEMLPLDOTRIM         = 0;
static uint32_t g_orig_TVRGCVREFTRIM        = 0;
static uint32_t g_orig_TVRGFVREFTRIM        = 0;
static uint32_t g_orig_VDDCLKGTRIM          = 0;
static uint32_t g_orig_CORELDOTEMPCOTRIM    = 0;
static uint32_t g_orig_CORELDOACTIVETRIM    = 0;
static uint32_t g_orig_D2ASPARE             = 0;
static uint32_t g_orig_VDDCACTLOWTONTRIM    = 0;
static uint32_t g_orig_VDDCACTHIGHTONTRIM   = 0;
static uint32_t g_orig_VDDCLVACTLOWTONTRIM  = 0;
static uint32_t g_orig_VDDCLVACTHIGHTONTRIM = 0;
static uint32_t g_orig_VDDFACTLOWTONTRIM    = 0;
static uint32_t g_orig_VDDFACTHIGHTONTRIM   = 0;

//! @}

static uint32_t g_ui32CurCORELDOTEMPCOTRIMLDO  = 0;
static uint32_t g_ui32CurCORELDOTEMPCOTRIMBuck = 0;
static uint32_t g_ui32CurMEMLDOACTIVETRIMLDO   = 0;
static bool     g_bPostponeTempco              = false;
static bool     g_bTempcoPending               = false;
static am_hal_pwrctrl_tempco_range_e g_eTempCoRange = AM_HAL_PWRCTRL_TEMPCO_RANGE_LOW;
extern uint8_t g_ui8TRCENAcount;    // am_hal_debug.c


//*****************************************************************************
//
// This table will be populated with all available INFO1 values and
// will be used for easy lookup after OTP is powered down.
//
//*****************************************************************************
am_hal_pwrctrl_info1_regs_t g_sINFO1regs;

//*****************************************************************************
//
//! @name DEVPWREN and DEVPWRSTATUS Mask Macros
//! @{
//! The below DEVPWREN and DEVPWRSTATUS masks are used to check if a peripheral
//!    has been disabled properly
//!
//! The original check of ((PWRCTRL->DEVPWRSTATUS & ui32PeriphStatus) == 0)
//!     will fail when more than one enable in the same domain is set and the
//!     user tries disable only one.
// ****************************************************************************

#define PWRCTRL_HCPB_DEVPWREN_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM0, PWRCTRL_DEVPWREN_PWRENIOM0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM1, PWRCTRL_DEVPWREN_PWRENIOM1_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM2, PWRCTRL_DEVPWREN_PWRENIOM2_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM3, PWRCTRL_DEVPWREN_PWRENIOM3_EN))

#define PWRCTRL_HCPC_DEVPWREN_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM4, PWRCTRL_DEVPWREN_PWRENIOM4_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM5, PWRCTRL_DEVPWREN_PWRENIOM5_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM6, PWRCTRL_DEVPWREN_PWRENIOM6_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM7, PWRCTRL_DEVPWREN_PWRENIOM7_EN))

#define PWRCTRL_HCPA_DEVPWREN_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART0, PWRCTRL_DEVPWREN_PWRENUART0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART1, PWRCTRL_DEVPWREN_PWRENUART1_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART2, PWRCTRL_DEVPWREN_PWRENUART2_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART3, PWRCTRL_DEVPWREN_PWRENUART3_EN))

#define PWRCTRL_I2S_DEVPWREN_MASK       ( \
    _VAL2FLD(PWRCTRL_AUDSSPWREN_PWRENI2S0, PWRCTRL_AUDSSPWREN_PWRENI2S0_EN) | \
    _VAL2FLD(PWRCTRL_AUDSSPWREN_PWRENI2S1, PWRCTRL_AUDSSPWREN_PWRENI2S1_EN))

#define PWRCTRL_IOS_DEVPWREN_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOS0, PWRCTRL_DEVPWREN_PWRENIOS0_EN)     | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOSFD0, PWRCTRL_DEVPWREN_PWRENIOSFD0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOSFD1, PWRCTRL_DEVPWREN_PWRENIOSFD1_EN))

#define PWRCTRL_HCPB_DEVPWRSTATUS_MASK      ( \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM0_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM1_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM2_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM3_Msk)

#define PWRCTRL_HCPC_DEVPWRSTATUS_MASK      ( \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM4_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM5_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM6_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOM7_Msk)

#define PWRCTRL_HCPA_DEVPWRSTATUS_MASK          ( \
    PWRCTRL_DEVPWRSTATUS_PWRSTUART0_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTUART1_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTUART2_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTUART3_Msk)

#define PWRCTRL_I2S_DEVPWRSTATUS_MASK      ( \
    PWRCTRL_AUDSSPWRSTATUS_PWRSTI2S0_Msk   | \
    PWRCTRL_AUDSSPWRSTATUS_PWRSTI2S1_Msk)

#define PWRCTRL_IOS_DEVPWRSTATUS_MASK      ( \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOS0_Msk     | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOSFD0_Msk   | \
    PWRCTRL_DEVPWRSTATUS_PWRSTIOSFD1_Msk)
//! @}

// **********************************************
//! Define the peripheral control structure.
// **********************************************
struct am_pwr_s
{
    uint32_t    ui32PwrEnRegAddr;
    uint32_t    ui32PeriphEnable;
    uint32_t    ui32PwrStatReqAddr;
    uint32_t    ui32PeriphStatus;
};

//
//! Peripheral control data structure
//
#ifndef AM_HAL_PWRCTRL_RAM_TABLE
const struct am_pwr_s am_hal_pwrctrl_peripheral_control[AM_HAL_PWRCTRL_PERIPH_MAX] =
{
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOS0, PWRCTRL_DEVPWREN_PWRENIOS0_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_IOS_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOSFD0, PWRCTRL_DEVPWREN_PWRENIOSFD0_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_IOS_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOSFD1, PWRCTRL_DEVPWREN_PWRENIOSFD1_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_IOS_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM0, PWRCTRL_DEVPWREN_PWRENIOM0_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPB_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM1, PWRCTRL_DEVPWREN_PWRENIOM1_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPB_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM2, PWRCTRL_DEVPWREN_PWRENIOM2_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPB_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM3, PWRCTRL_DEVPWREN_PWRENIOM3_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPB_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM4, PWRCTRL_DEVPWREN_PWRENIOM4_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPC_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM5, PWRCTRL_DEVPWREN_PWRENIOM5_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPC_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM6, PWRCTRL_DEVPWREN_PWRENIOM6_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPC_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENIOM7, PWRCTRL_DEVPWREN_PWRENIOM7_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPC_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART0, PWRCTRL_DEVPWREN_PWRENUART0_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPA_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART1, PWRCTRL_DEVPWREN_PWRENUART1_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPA_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART2, PWRCTRL_DEVPWREN_PWRENUART2_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPA_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART3, PWRCTRL_DEVPWREN_PWRENUART3_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPA_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENADC, PWRCTRL_DEVPWREN_PWRENADC_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTADC_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENMSPI0, PWRCTRL_DEVPWREN_PWRENMSPI0_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTMSPI0_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENMSPI1, PWRCTRL_DEVPWREN_PWRENMSPI1_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTMSPI1_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENMSPI2, PWRCTRL_DEVPWREN_PWRENMSPI2_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTMSPI2_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENMSPI3, PWRCTRL_DEVPWREN_PWRENMSPI3_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTMSPI3_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENGFX, PWRCTRL_DEVPWREN_PWRENGFX_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTGFX_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENDISP, PWRCTRL_DEVPWREN_PWRENDISP_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTDISP_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENDISPPHY, PWRCTRL_DEVPWREN_PWRENDISPPHY_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTDISPPHY_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENCRYPTO, PWRCTRL_DEVPWREN_PWRENCRYPTO_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTCRYPTO_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENSDIO0, PWRCTRL_DEVPWREN_PWRENSDIO0_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTSDIO0_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENSDIO1, PWRCTRL_DEVPWREN_PWRENSDIO1_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTSDIO1_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUSB, PWRCTRL_DEVPWREN_PWRENUSB_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTUSB_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUSBPHY, PWRCTRL_DEVPWREN_PWRENUSBPHY_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTUSBPHY_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENDBG, PWRCTRL_DEVPWREN_PWRENDBG_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTDBG_Msk
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENOTP, PWRCTRL_DEVPWREN_PWRENOTP_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTOTP_Msk
    },
    {
        AM_REGADDR(PWRCTRL, AUDSSPWREN),
        _VAL2FLD(PWRCTRL_AUDSSPWREN_PWRENPDM0, PWRCTRL_AUDSSPWREN_PWRENPDM0_EN),
        AM_REGADDR(PWRCTRL, AUDSSPWRSTATUS),
        PWRCTRL_AUDSSPWRSTATUS_PWRSTPDM0_Msk
    },
    {
        AM_REGADDR(PWRCTRL, AUDSSPWREN),
        _VAL2FLD(PWRCTRL_AUDSSPWREN_PWRENI2S0, PWRCTRL_AUDSSPWREN_PWRENI2S0_EN),
        AM_REGADDR(PWRCTRL, AUDSSPWRSTATUS),
        PWRCTRL_I2S_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, AUDSSPWREN),
        _VAL2FLD(PWRCTRL_AUDSSPWREN_PWRENI2S1, PWRCTRL_AUDSSPWREN_PWRENI2S1_EN),
        AM_REGADDR(PWRCTRL, AUDSSPWRSTATUS),
        PWRCTRL_I2S_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, AUDSSPWREN),
        _VAL2FLD(PWRCTRL_AUDSSPWREN_PWRENAUDADC, PWRCTRL_AUDSSPWREN_PWRENAUDADC_EN),
        AM_REGADDR(PWRCTRL, AUDSSPWRSTATUS),
        PWRCTRL_AUDSSPWRSTATUS_PWRSTAUDADC_Msk
    },
};


//*****************************************************************************
//
//! @brief  Return the pwr_ctrl entry for a given ePeripheral
//!
//! @param  pwr_ctrl address where the power entry is copied
//! @param  ePeripheral the peripheral to copy
//!
//! @return Returns AM_HAL_STATUS_SUCCESS on success
//
//*****************************************************************************
static inline uint32_t
am_get_pwrctrl(struct am_pwr_s *pwr_ctrl, uint32_t ePeripheral)
{
    if ( pwr_ctrl == NULL || ePeripheral >= AM_HAL_PWRCTRL_PERIPH_MAX )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    *pwr_ctrl = am_hal_pwrctrl_peripheral_control[ePeripheral];

    return AM_HAL_STATUS_SUCCESS;
} // am_get_pwrctrl()
#else
//*****************************************************************************
//
//! @brief  Return the pwr_ctrl entry for a given ePeripheral
//!
//! @param  pwr_ctrl address where the power entry is generated
//! @param  ePeripheral the peripheral for which to generate:
//!
//! @return Returns AM_HAL_STATUS_SUCCESS on success
//
//*****************************************************************************
static uint32_t
am_get_pwrctrl(struct am_pwr_s *pwr_ctrl, uint32_t ePeripheral)
{
    int shift_pos;

    if (pwr_ctrl == NULL || ePeripheral >= AM_HAL_PWRCTRL_PERIPH_MAX)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if (ePeripheral < AM_HAL_PWRCTRL_PERIPH_PDM0)
    {
        pwr_ctrl->ui32PwrEnRegAddr = AM_REGADDR(PWRCTRL, DEVPWREN);
        pwr_ctrl->ui32PwrStatReqAddr = AM_REGADDR(PWRCTRL, DEVPWRSTATUS);
        pwr_ctrl->ui32PeriphEnable = 1 << ePeripheral;
        pwr_ctrl->ui32PeriphStatus = 1 << ePeripheral;
    }
    else
    {
        shift_pos = (ePeripheral - AM_HAL_PWRCTRL_PERIPH_PDM0) + 2;
        if (ePeripheral > AM_HAL_PWRCTRL_PERIPH_PDM0)
        {
            shift_pos += 3;
        }
        if (ePeripheral > AM_HAL_PWRCTRL_PERIPH_I2S1)
        {
            shift_pos += 2;
        }
        pwr_ctrl->ui32PwrEnRegAddr =  AM_REGADDR(PWRCTRL, AUDSSPWREN);
        pwr_ctrl->ui32PwrStatReqAddr = AM_REGADDR(PWRCTRL, AUDSSPWRSTATUS);
        pwr_ctrl->ui32PeriphEnable = 1 << shift_pos;
        pwr_ctrl->ui32PeriphStatus = 1 << shift_pos;
    }

    return AM_HAL_STATUS_SUCCESS;
} // am_get_pwrctrl()
#endif // AM_HAL_PWRCTRL_RAM_TABLE

//*****************************************************************************
//
// Default configurations definitions
//
//*****************************************************************************
am_hal_pwrctrl_mcu_memory_config_t    g_DefaultMcuMemCfg =
{
    // For RevB0 - ROM needs to be kept ON
    // This should be reverted to AUTO for RevB1
    .eROMMode           = AM_HAL_PWRCTRL_ROM_ALWAYS_ON,
    .eDTCMCfg           = AM_HAL_PWRCTRL_ITCM256K_DTCM512K,
    .eRetainDTCM        = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
    .eNVMCfg            = AM_HAL_PWRCTRL_NVM0_AND_NVM1,
    .bKeepNVMOnInDeepSleep         = false,
};

const am_hal_pwrctrl_sram_memcfg_t          g_DefaultSRAMCfg =
{
    .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_3M,
    .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_NONE,
    .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_3M
};

//*****************************************************************************
//
// Function to determine the chip's TRIM version.
//
// return Status code.
//
// pui32TrimVer: The uint32_t that will receive the trim version number.
//               If no valid trim version found, *pui32TrimVer returns as 0.
//
//
//*****************************************************************************
static uint32_t
TrimVersionGet(uint32_t *pui32TrimVer)
{
    uint32_t ui32Ret;

    //
    // Get the TRIM version and set the global variable.
    // This only needs to be done and verified once.
    //
    if ( g_ui32TrimVer == 0xFFFFFFFF )
    {
        ui32Ret = am_hal_info1_read(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1,
                                    AM_REG_OTP_INFO1_TRIM_REV_O / 4,
                                    1, &g_ui32TrimVer);

        if ( (g_ui32TrimVer == 0x00000000)  ||
             (ui32Ret != AM_HAL_STATUS_SUCCESS) )
        {
            //
            // Invalid trim value. Set the global to indicate version 0.
            //
            g_ui32TrimVer = 0;
        }
    }

    if ( pui32TrimVer )
    {
        *pui32TrimVer = g_ui32TrimVer;
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

} // TrimVersionGet()

// ****************************************************************************
//
//  mcu_hp_lp_switch_sequence()
//  MCU HP/LP switching sequence.
//
// ****************************************************************************
static uint32_t
mcu_hp_lp_switch_sequence(am_hal_pwrctrl_mcu_mode_e ePowerMode)
{
    uint32_t ui32Status;

    AM_CRITICAL_BEGIN

    //
    //  Switch cpu mode
    //
    if (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE)
    {
        //
        // Report CPU state change
        //
        if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
        {
            am_hal_spotmgr_cpu_state_e eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_HP;
            am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_CPU_STATE, false, (void *) &eCpuSt);
        }
        // If HFRC2 is On - CPU clock is gated during the VDDC_LV to VDDF transition, as we want
        bool bHFRC2ForceOn = false;
        if (!CLKGEN->MISC_b.FRCHFRC2)
        {
            // Force On HFRC2
            CLKGEN->MISC_b.FRCHFRC2 = CLKGEN_MISC_FRCHFRC2_FRC;
            // Have explicit wait to address potential race condition when the READY is high by virtue
            // of other peripherals using HFRC2 (e.g. MSPI), and as the peripheral deassert the use
            am_hal_delay_us(1);
            am_hal_delay_us_status_change(AM_HAL_CLKGEN_HFRC2_DELAY,
                    (uint32_t)&CLKGEN->CLOCKENSTAT,
                    CLKGEN_CLOCKENSTAT_HFRC2READY_Msk,
                    CLKGEN_CLOCKENSTAT_HFRC2READY_Msk);
            bHFRC2ForceOn = true;
        }
        if (CLKGEN->CLOCKENSTAT_b.HFRC2READY)
        {
            //
            // Now it is safe to transition the supply
            // Set the MCU power mode.
            //
            PWRCTRL->MCUPERFREQ_b.MCUPERFREQ = ePowerMode;
            //
            // Wait for the ACK
            //
            ui32Status = AM_HAL_STATUS_TIMEOUT;
            for ( uint32_t i = 0; i < AM_HAL_PWRCTRL_PERF_SWITCH_WAIT_US; i++ )
            {
                if ( PWRCTRL->MCUPERFREQ_b.MCUPERFACK > 0 )
                {
                    ui32Status = AM_HAL_STATUS_SUCCESS;
                    break;
                }
                am_hal_delay_us(1);
            }
        }
        else
        {
            ui32Status = AM_HAL_STATUS_FAIL;
        }
        if (bHFRC2ForceOn)
        {
            // Revert override
            CLKGEN->MISC_b.FRCHFRC2 = CLKGEN_MISC_FRCHFRC2_NOFRC;
        }
    }
    else
    {
        //
        // Set the MCU power mode.
        //
        PWRCTRL->MCUPERFREQ_b.MCUPERFREQ = ePowerMode;
        //
        // Wait for the ACK
        //
        ui32Status = AM_HAL_STATUS_TIMEOUT;
        for ( uint32_t i = 0; i < AM_HAL_PWRCTRL_PERF_SWITCH_WAIT_US; i++ )
        {
            if ( PWRCTRL->MCUPERFREQ_b.MCUPERFACK > 0 )
            {
                ui32Status = AM_HAL_STATUS_SUCCESS;
                break;
            }
            am_hal_delay_us(1);
        }
    }

    if (ui32Status == AM_HAL_STATUS_SUCCESS)
    {
        g_eCurMcuPwrMode = ePowerMode;
        if (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE)
        {
            //
            // Report CPU state change
            //
            if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
            {
                am_hal_spotmgr_cpu_state_e eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_LP;
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_CPU_STATE, false, (void *) &eCpuSt);
            }
        }
    }
    else
    {
        if (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE)
        {
            //
            // Need to revert the power state
            //
            if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
            {
                am_hal_spotmgr_cpu_state_e eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_LP;
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_CPU_STATE, false, (void *) &eCpuSt);
            }
        }
    }

    AM_CRITICAL_END

    return ui32Status;
} // mcu_hp_lp_switch_sequence()

// ****************************************************************************
//
//  am_hal_pwrctrl_mcu_mode_status()
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_mcu_mode_status(am_hal_pwrctrl_mcu_mode_e *peCurrentPowerMode)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( peCurrentPowerMode == 0 )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    *peCurrentPowerMode = g_eCurMcuPwrMode;
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_mcu_mode_status()

// ****************************************************************************
//
//  am_hal_pwrctrl_mcu_mode_select()
//  Select the MCU power mode.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_mcu_mode_select(am_hal_pwrctrl_mcu_mode_e ePowerMode)
{
    uint32_t ui32Status;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_LOW_POWER)      &&
         (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#if !defined(APOLLO5_FPGA)
    //
    // We must be using SIMOBUCK in order to go to HP mode.
    //
    if ( (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE)   &&
         (PWRCTRL->VRSTATUS_b.SIMOBUCKST != PWRCTRL_VRSTATUS_SIMOBUCKST_ACT) )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Check if the request mode is already selected.  If so, return SUCCESS.
    //
    if ( ePowerMode == g_eCurMcuPwrMode )
    {
        return AM_HAL_STATUS_SUCCESS;
    }

    //
    // HP/LP switching sequence
    //
    ui32Status = mcu_hp_lp_switch_sequence(ePowerMode);

    //
    // Check for timeout.
    //
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        //
        // Caution: Reaching this point means the device is in an unpredictable
        //          state and may not be able to recover.
        //
        return ui32Status;
    }

    //
    // Check the MCU power mode status and return SUCCESS/FAIL.
    //
    if ( PWRCTRL->MCUPERFREQ_b.MCUPERFSTATUS == ePowerMode )
    {
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        //
        // Caution: Reaching this point means the device is in an unpredictable
        //          state and may not be able to recover.
        //
        return AM_HAL_STATUS_FAIL;
    }

} // am_hal_pwrctrl_mcu_mode_select()

// ****************************************************************************
//
//  am_hal_pwrctrl_gpu_mode_status()
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_gpu_mode_status(am_hal_pwrctrl_gpu_mode_e *peCurrentPowerMode)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( peCurrentPowerMode == 0 )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    *peCurrentPowerMode = g_eCurGpuPwrMode;

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_mcu_mode_status()

// ****************************************************************************
//
//  am_hal_pwrctrl_gpu_mode_select()
//  Select the GPU power mode.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_gpu_mode_select(am_hal_pwrctrl_gpu_mode_e ePowerMode)
{
    bool     bGFXEnabled;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( (ePowerMode != AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER)      &&
         (ePowerMode != AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

#if !defined(APOLLO5_FPGA)
    //
    // We must be using SIMOBUCK in order to go to HP mode.
    //
    if ( (ePowerMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)   &&
         (PWRCTRL->VRSTATUS_b.SIMOBUCKST != PWRCTRL_VRSTATUS_SIMOBUCKST_ACT) )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif

    //
    // Check if GPU is already enabled.  If so, return an error code.
    //
    if ( AM_HAL_STATUS_SUCCESS != am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_GFX, &bGFXEnabled) )
    {
        return AM_HAL_STATUS_FAIL;
    }
    else
    {
        if (bGFXEnabled)
        {
            return AM_HAL_STATUS_IN_USE;
        }
    }

    //
    // Check if the request mode is already selected.  If so, return SUCCESS.
    //
    if ( ePowerMode == g_eCurGpuPwrMode )
    {
        //
        // If GPU switched from HP to LP in API am_hal_pwrctrl_periph_disable,
        // it is allowed to change previous mode to LP.
        //
        if (g_ePreGpuPwrMode != g_eCurGpuPwrMode)
        {
            g_ePreGpuPwrMode = ePowerMode;
        }
        return AM_HAL_STATUS_SUCCESS;
    }

    AM_CRITICAL_BEGIN
    if (APOLLO5_B0_LE_PCM0P7)
    {
        //
        //  If switching to HP Mode, change TON first before GPU Mode switching
        //
        if ( ePowerMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE )
        {
            am_hal_sysctrl_ton_config_update(true, ePowerMode);
        }
    }

    //
    // Adjust the voltage supplied to the GPU.
    //
    if (ePowerMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)
    {
        PWRCTRL->GFXPWRSWSEL_b.GFXVDDSEL = PWRCTRL_GFXPWRSWSEL_GFXVDDSEL_VDDF;
    }
    else
    {
        PWRCTRL->GFXPWRSWSEL_b.GFXVDDSEL = PWRCTRL_GFXPWRSWSEL_GFXVDDSEL_VDDC;
    }

    //
    // Wait for voltage to settle.
    //
    am_hal_delay_us(AM_HAL_PWRCTRL_GPU_VOLTADJ_WAIT);

    PWRCTRL->GFXPERFREQ_b.GFXPERFREQ = ePowerMode;
    g_eCurGpuPwrMode = ePowerMode;
    g_ePreGpuPwrMode = ePowerMode;

    //
    // Wait for clock to settle.
    //
    am_hal_delay_us(AM_HAL_PWRCTRL_GPU_PWRADJ_WAIT);
    if (APOLLO5_B0_LE_PCM0P7)
    {
        //
        //  If switching to LP Mode, change TON after GPU Mode switching is done
        //
        if ( ePowerMode == AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER )
        {
            am_hal_sysctrl_ton_config_update(true, ePowerMode);
        }
    }

    AM_CRITICAL_END

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_gpu_mode_select()

// ****************************************************************************
//
//  am_hal_pwrctrl_mcu_memory_config()
//  Configure the MCU memory.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_mcu_memory_config(am_hal_pwrctrl_mcu_memory_config_t *psConfig)
{
    uint32_t      ui32Status;
    uint32_t      ui32PwrStatus = 0;
    uint32_t      ui32PwrEn = 0;
    bool          bForceAxiClkEn = false;
    uint32_t      ui32PwrStatusAfterDisables;

    //
    // Configure the ROM power mode.
    //
    g_eCurROMPwrMode = psConfig->eROMMode;

    switch (psConfig->eROMMode)
    {
        case AM_HAL_PWRCTRL_ROM_AUTO:
            //
            // Initialize the ROM to off.  Let the HAL access function control the power.
            //
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENROM, 0);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTROM, 0);
            break;
        case AM_HAL_PWRCTRL_ROM_ALWAYS_ON:
            //
            // Initialize the ROM to be always on.
            //
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENROM, 1);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTROM, 1);
            break;
    }

    //
    // Configure the MCU Tightly Coupled Memory.
    //
    ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENTCM, psConfig->eDTCMCfg);
    ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTTCM, psConfig->eDTCMCfg);

    //
    // Configure the Non-Volatile Memory.
    //
    switch (psConfig->eNVMCfg)
    {
        case AM_HAL_PWRCTRL_NVM_NONE:
            //
            // Power down all NVMs.
            //
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM, 0) | _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM1, 0);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM0, 0) | _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM1, 0);
            break;
        case AM_HAL_PWRCTRL_NVM0_ONLY:
            //
            // Power up NVM0, power down NVM1.
            //
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM, 1) | _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM1, 0);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM0, 1) | _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM1, 0);
            break;
        case AM_HAL_PWRCTRL_NVM0_AND_NVM1:
            //
            // Power up NVM0 and NVM1.
            //
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM, 1) | _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM1, 1);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM0, 1) | _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM1, 1);
            break;
    }

    DIAG_SUPPRESS_VOLATILE_ORDER()

    //
    // Special logic needed, when powering on NVM1 by itself
    // Need to force AXI Clock during the transition
    // If powering on both banks "at same time", no special processing needed
    //
    if ((psConfig->eNVMCfg == AM_HAL_PWRCTRL_NVM0_AND_NVM1) &&
        ((PWRCTRL->MEMPWRSTATUS & (PWRCTRL_MEMPWRSTATUS_PWRSTNVM1_Msk | PWRCTRL_MEMPWRSTATUS_PWRSTNVM0_Msk)) == PWRCTRL_MEMPWRSTATUS_PWRSTNVM0_Msk))
    {
        MCUCTRL->FORCEAXICLKEN_b.FRCAXICLKEN = 1;
        bForceAxiClkEn = true;
    }

    if (ui32PwrStatus != PWRCTRL->MEMPWRSTATUS)
    {
        //
        // Report MEM power status
        //
        if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
        {
            //
            // Calculate expected power status after writing memory disables.
            //
            ui32PwrStatusAfterDisables = PWRCTRL->MEMPWRSTATUS & ui32PwrStatus;

            //
            // Write memory disables firstly.
            //
            PWRCTRL->MEMPWREN &= ui32PwrEn;

            //
            // Wait for Status
            //
            ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                                      (uint32_t)&PWRCTRL->MEMPWRSTATUS,
                                                      AM_HAL_PWRCTRL_MEMPWRSTATUS_MASK,
                                                      ui32PwrStatusAfterDisables,
                                                      true);
            //
            // Check for timeout.
            //
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
            }
            else
            {
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_MEMPWR, true, (void *) &ui32PwrStatus);
            }
        }

        PWRCTRL->MEMPWREN = ui32PwrEn;

        //
        // Wait for Status
        //
        ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                                  (uint32_t)&PWRCTRL->MEMPWRSTATUS,
                                                  AM_HAL_PWRCTRL_MEMPWRSTATUS_MASK,
                                                  ui32PwrStatus,
                                                  true);
        if (bForceAxiClkEn)
        {
            MCUCTRL->FORCEAXICLKEN_b.FRCAXICLKEN = 0;
        }

        //
        // Check for timeout.
        //
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
        }

        //
        // Check the MCU power mode status and return SUCCESS/FAIL.
        //
        if ((PWRCTRL->MEMPWRSTATUS_b.PWRSTTCM != PWRCTRL->MEMPWREN_b.PWRENTCM)          ||
            (PWRCTRL->MEMPWRSTATUS_b.PWRSTNVM0 != PWRCTRL->MEMPWREN_b.PWRENNVM)         ||
            ((PWRCTRL->MEMPWRSTATUS_b.PWRSTNVM0 & PWRCTRL->MEMPWRSTATUS_b.PWRSTNVM1) != PWRCTRL->MEMPWREN_b.PWRENNVM1)        ||
            (PWRCTRL->MEMPWRSTATUS_b.PWRSTROM != PWRCTRL->MEMPWREN_b.PWRENROM)          ||
            (PWRCTRL->DEVPWRSTATUS_b.PWRSTOTP != PWRCTRL->DEVPWREN_b.PWRENOTP))
        {
            return AM_HAL_STATUS_FAIL;
        }
    }

    DIAG_DEFAULT_VOLATILE_ORDER()

    //
    // Configure the Non-Volatile Memory retention.
    //
    if (psConfig->bKeepNVMOnInDeepSleep)
    {
        PWRCTRL->MEMRETCFG_b.NVMPWDSLP = PWRCTRL_MEMRETCFG_NVMPWDSLP_DIS;
    }
    else
    {
        PWRCTRL->MEMRETCFG_b.NVMPWDSLP = PWRCTRL_MEMRETCFG_NVMPWDSLP_EN;
    }

    //
    // Configure the MCU Tightly Coupled Memory retention.
    //
    if ( psConfig->eRetainDTCM == AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN )
    {
        PWRCTRL->MEMRETCFG_b.TCMPWDSLP = PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN;
    }
    else if ( psConfig->eRetainDTCM == AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN )
    {
        PWRCTRL->MEMRETCFG_b.TCMPWDSLP = PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN;
    }
    else
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_mcu_memory_config()

// ****************************************************************************
//
//  am_hal_pwrctrl_mcu_memory_config_get()
//  Get the MCU Memory configuration.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_mcu_memory_config_get(am_hal_pwrctrl_mcu_memory_config_t *psConfig)
{
    //
    // Get the MCU Tightly Coupled Memory configuration.
    //
    psConfig->eDTCMCfg =
        (am_hal_pwrctrl_dtcm_select_e)PWRCTRL->MEMPWREN_b.PWRENTCM;

    //
    // Get the Non-Volatile Memory configuration.
    //
    psConfig->eNVMCfg =
        (am_hal_pwrctrl_nvm_select_e)((PWRCTRL->MEMPWREN_b.PWRENNVM1 << 1) | PWRCTRL->MEMPWREN_b.PWRENNVM);

    //
    // Configure the Non-Volatile Memory retention.
    //
    psConfig->bKeepNVMOnInDeepSleep =
        (PWRCTRL->MEMRETCFG_b.NVMPWDSLP == PWRCTRL_MEMRETCFG_NVMPWDSLP_DIS);

    //
    // Configure the MCU Tightly Coupled Memory retention.
    //
    if (PWRCTRL->MEMRETCFG_b.TCMPWDSLP == PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN)
    {
        psConfig->eRetainDTCM = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN;
    }
    else if (PWRCTRL->MEMRETCFG_b.TCMPWDSLP == PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN)
    {
        psConfig->eRetainDTCM = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN;
    }
    else
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_mcu_memory_config_get()

// ****************************************************************************
//
//  am_hal_pwrctrl_rom_enable()
//  Enable the ROM power domain if AUTO mode is selected.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_rom_enable(void)
{
    uint32_t    count;

    if ( AM_HAL_PWRCTRL_ROM_AUTO == g_eCurROMPwrMode )
    {
        //
        // Report rom power change
        //
        if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
        {
            uint32_t ui32MemSt = PWRCTRL_MEMPWRSTATUS_PWRSTROM_Msk | PWRCTRL->MEMPWRSTATUS;
            am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_MEMPWR, true, (void *) &ui32MemSt);
        }

        PWRCTRL->MEMPWREN_b.PWRENROM = PWRCTRL_MEMPWREN_PWRENROM_EN;

        //
        // Wait for Status
        //
        for (count = 0; count < AM_HAL_PWRCTRL_MAX_BOOTROM_COUNT; count++)
        {
            if (PWRCTRL->MEMPWRSTATUS_b.PWRSTROM == 1)
            {
                break;
            }
        }

        //
        // Check for timeout.
        //
        if (count == AM_HAL_PWRCTRL_MAX_BOOTROM_COUNT)
        {
            return AM_HAL_STATUS_TIMEOUT;
        }
    }

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_rom_enable()

// ****************************************************************************
//
//  am_hal_pwrctrl_rom_disable()
//  Enable the ROM power domain if AUTO mode is selected.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_rom_disable(void)
{
    uint32_t    ui32Count;

    if ( AM_HAL_PWRCTRL_ROM_AUTO == g_eCurROMPwrMode )
    {
        PWRCTRL->MEMPWREN_b.PWRENROM = PWRCTRL_MEMPWREN_PWRENROM_DIS;

        //
        // Wait for Status
        //
        for (ui32Count = 0; ui32Count < AM_HAL_PWRCTRL_MAX_BOOTROM_COUNT; ui32Count++)
        {
            if (PWRCTRL->MEMPWRSTATUS_b.PWRSTROM == 0)
            {
                break;
            }
        }

        //
        // Check for timeout.
        //
        if (ui32Count == AM_HAL_PWRCTRL_MAX_BOOTROM_COUNT)
        {
            return AM_HAL_STATUS_TIMEOUT;
        }
        //
        // Report rom power change
        //
        if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
        {
            uint32_t ui32MemSt = PWRCTRL->MEMPWRSTATUS;
            am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_MEMPWR, false, (void *) &ui32MemSt);
        }
    }

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_rom_disable()

// ****************************************************************************
//
//  am_hal_pwrctrl_sram_config()
//  Configure the Shared RAM.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_sram_config(am_hal_pwrctrl_sram_memcfg_t *psConfig)
{
    uint32_t      ui32Status;

    //
    // When bUpdateLater is true, means we expect to power down partial or whole SSRAM,
    // we should call am_hal_spotmgr_power_state_update after completing powering down.
    //
    bool          bUpdateLater = false;

    //
    // If psConfig->eSRAMCfg == PWRCTRL->SSRAMPWRST_b.SSRAMPWRST, skipped PWRCTRL->SSRAMPWREN_b.PWRENSSRAM setting.
    //
    if (psConfig->eSRAMCfg != PWRCTRL->SSRAMPWRST_b.SSRAMPWRST)
    {
        //
        // According to the enum type am_hal_pwrctrl_sram_select_e,
        // if users want to only enable 1M SSRAM, we must select SSRAM0 to enable.
        // If users want to only enable 2M SSRAM, we must select SSRAM0 and SSRAM1 to enable.
        // So it works to determine if any new SSRAM is being enabled by checking
        // if psConfig->eSRAMCfg > PWRCTRL->SSRAMPWRST_b.SSRAMPWRST.
        //
        if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
        {
            if (psConfig->eSRAMCfg > PWRCTRL->SSRAMPWRST_b.SSRAMPWRST)
            {
                //
                // Report SSRAM power change
                //
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_SSRAMPWR, true, (void *) &(psConfig->eSRAMCfg));
                bUpdateLater = false;
            }
            else // sConfig->eSRAMCfg < PWRCTRL->SSRAMPWRST_b.SSRAMPWRST
            {
                bUpdateLater = true;
            }
        }
        //
        // Configure the Shared RAM.
        //
        PWRCTRL->SSRAMPWREN_b.PWRENSSRAM = psConfig->eSRAMCfg;

        DIAG_SUPPRESS_VOLATILE_ORDER()

        //
        // Wait for Status
        //
        ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                                  (uint32_t)&PWRCTRL->SSRAMPWRST,
                                                  PWRCTRL_SSRAMPWRST_SSRAMPWRST_Msk,
                                                  PWRCTRL->SSRAMPWREN,
                                                  true);

        //
        // Check for error.
        //
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
        }

        //
        // Check the Shared RAM power mode status.
        //
        if (PWRCTRL->SSRAMPWRST_b.SSRAMPWRST != PWRCTRL->SSRAMPWREN_b.PWRENSSRAM)
        {
            return AM_HAL_STATUS_FAIL;
        }
        if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
        {
            if (bUpdateLater)
            {
                //
                // Report SSRAM power change
                //
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_SSRAMPWR, false, NULL);
            }
        }
        DIAG_DEFAULT_VOLATILE_ORDER()
    }
    //
    // Configure the Shared RAM domain active based on the states of the MCU,
    // graphics, and display.
    //
    PWRCTRL->SSRAMRETCFG_b.SSRAMACTMCU  = psConfig->eActiveWithMCU;
    PWRCTRL->SSRAMRETCFG_b.SSRAMACTGFX  = psConfig->eActiveWithGFX;
    PWRCTRL->SSRAMRETCFG_b.SSRAMACTDISP = psConfig->eActiveWithDISP;

    //
    // Configure the Shared RAM retention.
    //
    switch ( psConfig->eSRAMRetain )
    {
        case AM_HAL_PWRCTRL_SRAM_NONE:
            PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP = PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_ALL;
            break;
        case AM_HAL_PWRCTRL_SRAM_1M:    // Retain lower 1M, pwr dwn upper 2M
            PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP = PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP1 |
                                                 PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP2;
            break;
        case AM_HAL_PWRCTRL_SRAM_2M:    // Retain lower and middle 1M, pwr dwn upper 1M
            PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP = PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP2;
            break;
        case AM_HAL_PWRCTRL_SRAM_3M:    // Retain all SSRAM, pwr dwn none
            PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP = PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_NONE;
            break;
    }

    //
    // Disable overrides to allow the SSRAMRETCFG take effect
    //
    PWRCTRL->MMSOVERRIDE_b.MMSOVRSSRAMRETGFX = PWRCTRL_MMSOVERRIDE_MMSOVRSSRAMRETGFX_PGSTATE;
    PWRCTRL->MMSOVERRIDE_b.MMSOVRSSRAMRETDISP = PWRCTRL_MMSOVERRIDE_MMSOVRSSRAMRETDISP_PGSTATE;

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_sram_config()

// ****************************************************************************
//
//  am_hal_pwrctrl_sram_config_get()
//  Get the current Shared RAM configuration.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_sram_config_get(am_hal_pwrctrl_sram_memcfg_t *psConfig)
{
    //
    // Get the Shared RAM configuration.
    //
    psConfig->eSRAMCfg = (am_hal_pwrctrl_sram_select_e)PWRCTRL->SSRAMPWREN_b.PWRENSSRAM;

    //
    // Get the SRAM active configurations based for each of MCU, graphics, and display.
    //
    psConfig->eActiveWithMCU  = (am_hal_pwrctrl_sram_select_e)PWRCTRL->SSRAMRETCFG_b.SSRAMACTMCU;
    psConfig->eActiveWithGFX  = (am_hal_pwrctrl_sram_select_e)PWRCTRL->SSRAMRETCFG_b.SSRAMACTGFX;
    psConfig->eActiveWithDISP = (am_hal_pwrctrl_sram_select_e)PWRCTRL->SSRAMRETCFG_b.SSRAMACTDISP;

    //uint32_t val = (PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP1 || PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP2);
    //
    // Get the SRAM retention configuration.
    //
    if (PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP == PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_ALL)
    {
        psConfig->eSRAMRetain = AM_HAL_PWRCTRL_SRAM_NONE;
    }
    else if (PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP == PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_NONE)
    {
        psConfig->eSRAMRetain = AM_HAL_PWRCTRL_SRAM_3M;
    }
    else if (PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP == PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP2)
    {
        psConfig->eSRAMRetain = AM_HAL_PWRCTRL_SRAM_2M;
    }
    else if (PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP == (PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP1 | PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP2))
    {
        psConfig->eSRAMRetain = AM_HAL_PWRCTRL_SRAM_1M;
    }
    else
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_sram_config_get()

//******************************************************************************
//
// Function that waits for crypto peripheral to stabilize before or after
// powerup/powerdown.
//
//******************************************************************************
#define CRYPTO_WAIT_USEC        100
static uint32_t
crypto_quiesce(void)
{
    uint32_t ui32Status;

    //
    // We do not know if the application has already set CRYPTO->HOSTPOWERDOWN
    // Wait for CRYPTOPWRDOWREADY, assuming it has been set
    //
    ui32Status = am_hal_delay_us_status_change(CRYPTO_WAIT_USEC,
                                                (uint32_t)&MCUCTRL->MRAMCRYPTOPWRCTRL,
                                                MCUCTRL_MRAMCRYPTOPWRCTRL_CRYPTOPWRDOWREADY_Msk,
                                                MCUCTRL_MRAMCRYPTOPWRCTRL_CRYPTOPWRDOWREADY_Msk);
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        //
        // if this fails - it could be because CRYPTO->HOSTPOWERDOWN was never written to
        //
        //
        // Wait for crypto block to go idle.
        //
        ui32Status = am_hal_delay_us_status_change(CRYPTO_WAIT_USEC,
                                                   (uint32_t)&CRYPTO->HOSTCCISIDLE,
                                                   CRYPTO_HOSTCCISIDLE_HOSTCCISIDLE_Msk,
                                                   CRYPTO_HOSTCCISIDLE_HOSTCCISIDLE_Msk);
        if (AM_HAL_STATUS_SUCCESS == ui32Status)
        {

            //
            // Inform Crypto of imminent Power down
            //
            CRYPTO->HOSTPOWERDOWN_b.HOSTPOWERDOWN = 1;
            //
            // Wait for CRYPTOPWRDOWREADY
            //
            ui32Status = am_hal_delay_us_status_change(CRYPTO_WAIT_USEC,
                                                        (uint32_t)&MCUCTRL->MRAMCRYPTOPWRCTRL,
                                                        MCUCTRL_MRAMCRYPTOPWRCTRL_CRYPTOPWRDOWREADY_Msk,
                                                        MCUCTRL_MRAMCRYPTOPWRCTRL_CRYPTOPWRDOWREADY_Msk);
        }
    }

    return ui32Status;

} // crypto_quiesce()

// ****************************************************************************
//
//  Internal function for SIMOBUCK on/off
//
// ****************************************************************************
static inline void
simobuck_enable(bool bEnable)
{
    if (bEnable)
    {
        //
        // Enable the simobuck
        //
        PWRCTRL->VRCTRL_b.SIMOBUCKEN = 1;
        am_hal_delay_us(5);
        buck_ldo_update_override(true);
    }
    else
    {
        //
        // Disable the simobuck
        //
        buck_ldo_update_override(false);
        am_hal_delay_us(5);
        PWRCTRL->VRCTRL_b.SIMOBUCKEN = 0;
    }
}

// ****************************************************************************
//
//  gpu_on_sequence()
//
// ****************************************************************************
static inline void
gpu_on_sequence(void)
{
    am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_HFRC, AM_HAL_CLKMGR_USER_ID_GFX);
    if (g_eCurGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)
    {
        am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_HFRC2, AM_HAL_CLKMGR_USER_ID_GFX);
    }

    if (APOLLO5_B0_GE_PCM0P7 || APOLLO5_B1_LE_PCM2P0 || APOLLO5_B2_PCM2P0)
    {
        if (PWRCTRL->VRSTATUS_b.SIMOBUCKST == PWRCTRL_VRSTATUS_SIMOBUCKST_ACT) // Buck mode
        {
            if (APOLLO5_B0_PCM0P7)
            {
                if (g_bOrigTrimsStored)
                {
                    //
                    // Disable the simobuck
                    //
                    simobuck_enable(false);
                    //
                    // coreldo boost
                    //
                    MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM = 1;
                    //
                    // Simobuck VDDC boost
                    //
                    FINAL_VAL_OVF_CAP_BASE(g_orig_TVRGCVREFTRIM, 9, MCUCTRL, VREFGEN2, TVRGCVREFTRIM);
                    //
                    // Simobuck VDDC boost
                    //
                    MCUCTRL->D2ASPARE_b.D2ASPARE |= (0x1UL << 8);
                    //
                    // Simobuck VDDF and memldo boost
                    //
                    FINAL_VAL_OVF_CAP_BASE(g_orig_TVRGFVREFTRIM, 15, MCUCTRL, VREFGEN4, TVRGFVREFTRIM);
                    //
                    // Adjust the simobuck Ton gen clock to compensate for the VDDC boost
                    //
                    MCUCTRL->SIMOBUCK14_b.VDDCLKGTRIM = 3;
                    //
                    // Enable the simobuck
                    //
                    simobuck_enable(true);
                    //
                    // Delay 20us for VDDC and VDDF to boost
                    //
                    am_hal_delay_us(15);
                }
            }
            else // for B0 PCM1.0 and later trim, B1 PCM2.0 and previous trim
            {
                int32_t i32CORELDODiff;
                //
                // coreldo boost
                //
                g_ui32CurCORELDOTEMPCOTRIMBuck = MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM;
                MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM = 1;
                DIFF_OVF_CAP(i32CORELDODiff, 12, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
                MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM += i32CORELDODiff;
                //
                // memldo boost
                //
                MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 5;
                //
                // Delay 5us
                //
                am_hal_delay_us(5);
                //
                // VDDF boost
                //
                FINAL_VAL_OVF_CAP(15, MCUCTRL, VREFGEN4, TVRGFVREFTRIM);
                //
                // Short VDDC to VDDC_LV and VDDS to VDDF
                //
                vddc_to_vddclv_and_vdds_to_vddf_short(true);
                //
                // Delay 10us
                //
                am_hal_delay_us(10);
                //
                // VDDC boost
                //
                FINAL_VAL_OVF_CAP(9, MCUCTRL, VREFGEN2, TVRGCVREFTRIM );
                MCUCTRL->D2ASPARE_b.D2ASPARE |= (0x1UL << 8);
                //
                // VDDC and VDDCLV Ton updates
                //
                MCUCTRL->SIMOBUCK2_b.VDDCACTLOWTONTRIM = 10;
                MCUCTRL->SIMOBUCK2_b.VDDCACTHIGHTONTRIM = 10;
                MCUCTRL->SIMOBUCK4_b.VDDCLVACTLOWTONTRIM = 8;
                MCUCTRL->SIMOBUCK4_b.VDDCLVACTHIGHTONTRIM = 8;
                //
                // VDDF Ton updates
                //
                if (g_eCurGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER)
                {
                    MCUCTRL->SIMOBUCK7_b.VDDFACTLOWTONTRIM = 16;
                    MCUCTRL->SIMOBUCK6_b.VDDFACTHIGHTONTRIM = 20;
                }
                else
                {
                    MCUCTRL->SIMOBUCK7_b.VDDFACTLOWTONTRIM = 20;
                    MCUCTRL->SIMOBUCK6_b.VDDFACTHIGHTONTRIM = 22;
                }
                //
                // Remove VDDC and VDDC_LV short and VDDS to VDDF
                //
                vddc_to_vddclv_and_vdds_to_vddf_short(false);
                //
                // Reduce coreldo
                //
                MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM -= i32CORELDODiff;
                //
                // Reduce memldo
                //
                if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0 )
                {
                    MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 1;
                }
                else
                {
                    MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 0;
                }
            }
        }
        else // LDO mode
        {
            //
            // coreldo boost
            //
            g_ui32CurCORELDOTEMPCOTRIMLDO = MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM;
            MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM = 2;
            //
            // memldo boost
            //
            g_ui32CurMEMLDOACTIVETRIMLDO = MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM;
            FINAL_VAL_OVF_CAP(5, MCUCTRL, LDOREG2, MEMLDOACTIVETRIM);
            //
            // Delay 15us for coreldo and memldo to boost
            //
            am_hal_delay_us(15);
        }
    }
    else // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
    {
        //
        // Report GPU state change
        //
        am_hal_spotmgr_gpu_state_e eGpuSt;
        if (g_eCurGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER)
        {
            eGpuSt = AM_HAL_SPOTMGR_GPUSTATE_ACTIVE_LP;
        }
        else
        {
            eGpuSt = AM_HAL_SPOTMGR_GPUSTATE_ACTIVE_HP;
        }
        am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_GPU_STATE, true, (void *) &eGpuSt);
    }
} // gpu_on_sequence()

// ****************************************************************************
//
//  gpu_off_sequence()
//
// ****************************************************************************
static inline void
gpu_off_sequence(void)
{
    if (APOLLO5_B0_GE_PCM0P7 || APOLLO5_B1_LE_PCM2P0 || APOLLO5_B2_PCM2P0)
    {
        if (PWRCTRL->VRSTATUS_b.SIMOBUCKST == PWRCTRL_VRSTATUS_SIMOBUCKST_ACT) // Buck mode
        {
            if (APOLLO5_B0_PCM0P7)
            {
                if (g_bOrigTrimsStored)
                {
                    //
                    // Disable the simobuck
                    //
                    simobuck_enable(false);
                    //
                    // Set back to original value
                    //
                    MCUCTRL->SIMOBUCK14_b.VDDCLKGTRIM = g_orig_VDDCLKGTRIM;
                    //
                    // Set back to original value
                    //
                    MCUCTRL->VREFGEN4_b.TVRGFVREFTRIM = g_orig_TVRGFVREFTRIM;
                    //
                    // Set back to original value
                    //
                    MCUCTRL->D2ASPARE_b.D2ASPARE &= ~(0x1UL << 8);
                    //
                    // Set back to original value
                    //
                    MCUCTRL->VREFGEN2_b.TVRGCVREFTRIM = g_orig_TVRGCVREFTRIM;
                    //
                    // Set back to original value
                    //
                    MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM = g_orig_CORELDOTEMPCOTRIM;
                    //
                    // Enable the simobuck
                    //
                    simobuck_enable(true);
                }
            }
            else // for B0 PCM1.0 and later trim, B1 PCM2.0 and previous trim
            {
                int32_t i32CORELDODiff;
                //
                // coreldo boost
                //
                DIFF_OVF_CAP(i32CORELDODiff, 12, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
                MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM += i32CORELDODiff;
                //
                // memldo boost
                //
                MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 5;
                //
                // Delay 5us
                //
                am_hal_delay_us(5);
                //
                // Short VDDC to VDDC_LV and VDDS to VDDF
                //
                vddc_to_vddclv_and_vdds_to_vddf_short(true);
                //
                // Delay 10us
                //
                am_hal_delay_us(10);
                //
                // Ton updates
                //
                MCUCTRL->SIMOBUCK2_b.VDDCACTLOWTONTRIM = 6;
                MCUCTRL->SIMOBUCK2_b.VDDCACTHIGHTONTRIM = 6;
                MCUCTRL->SIMOBUCK4_b.VDDCLVACTLOWTONTRIM = 5;
                MCUCTRL->SIMOBUCK4_b.VDDCLVACTHIGHTONTRIM = 5;
                MCUCTRL->SIMOBUCK7_b.VDDFACTLOWTONTRIM = 7;
                MCUCTRL->SIMOBUCK6_b.VDDFACTHIGHTONTRIM = 10;
                //
                // Restore power trims
                //
                FINAL_VAL_UDF_CAP(15, MCUCTRL, VREFGEN4, TVRGFVREFTRIM);
                FINAL_VAL_UDF_CAP(9, MCUCTRL, VREFGEN2, TVRGCVREFTRIM);
                MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM = g_ui32CurCORELDOTEMPCOTRIMBuck;
                MCUCTRL->D2ASPARE_b.D2ASPARE &= ~(0x1UL << 8);
                MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM -= i32CORELDODiff;
                if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0 )
                {
                    MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 1;
                }
                else
                {
                    MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 0;
                }
                //
                // Remove VDDC and VDDC_LV short and VDDS to VDDF
                //
                vddc_to_vddclv_and_vdds_to_vddf_short(false);
            }
        }
        else // LDO mode
        {
            //
            // Set back to original value
            //
            MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = g_ui32CurMEMLDOACTIVETRIMLDO;
            //
            // Set back to original value
            //
            MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM = g_ui32CurCORELDOTEMPCOTRIMLDO;
        }
    }
    else // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
    {
        //
        // Report GPU state change
        //
        am_hal_spotmgr_gpu_state_e eGpuSt = AM_HAL_SPOTMGR_GPUSTATE_OFF;
        am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_GPU_STATE, true, (void *) &eGpuSt);
    }

    am_hal_clkmgr_clock_release_all(AM_HAL_CLKMGR_USER_ID_GFX);
} // gpu_off_sequence()

// ****************************************************************************
//
//  am_hal_pwrctrl_periph_enable()
//  Enable power for a peripheral.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_periph_enable(am_hal_pwrctrl_periph_e ePeripheral)
{
    uint32_t      ui32Status;
    struct am_pwr_s pwr_ctrl;

    ui32Status = am_get_pwrctrl(&pwr_ctrl, ePeripheral);

    if ( AM_HAL_STATUS_SUCCESS != ui32Status )
    {
        return ui32Status;
    }

    if ( AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable )
    {
        //
        // We're already enabled, nothing to do.
        //
        return AM_HAL_STATUS_SUCCESS;
    }

    //
    // If INFOC/OTP was not powered on, it is not allowed to power on CRYPTO.
    //
    if ( (ePeripheral == AM_HAL_PWRCTRL_PERIPH_CRYPTO)      &&
         (PWRCTRL->DEVPWRSTATUS_b.PWRSTOTP == PWRCTRL_DEVPWRSTATUS_PWRSTOTP_OFF))
    {
        return AM_HAL_STATUS_FAIL;
    }

    if (APOLLO5_B0_LE_PCM0P7)
    {
        //
        // Switch GPU to the previous mode or desired mode
        //
        AM_CRITICAL_BEGIN
        if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
        {
            if (g_ePreGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)
            {
                am_hal_pwrctrl_gpu_mode_select(AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE);
            }
            else
            {
                am_hal_sysctrl_ton_config_update(true, AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER);
            }

            //
            // GPU power on sequences
            //
            gpu_on_sequence();
        }

        //
        // Enable power control for the given device.
        //
        AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) |=
            pwr_ctrl.ui32PeriphEnable;
        AM_CRITICAL_END
    }
    else if ( APOLLO5_B0_GE_PCM1P0 || APOLLO5_B1_LE_PCM2P0 || APOLLO5_B2_PCM2P0 )
    {
        if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
        {
            g_bPostponeTempco = true;
            if (g_ePreGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)
            {
                am_hal_pwrctrl_gpu_mode_select(AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE);
            }

            //
            // GPU power on sequences
            //
            gpu_on_sequence();
            AM_CRITICAL_BEGIN
            AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) |=
                pwr_ctrl.ui32PeriphEnable;
            if (g_bTempcoPending)
            {
                vddc_vddf_tempco(g_eTempCoRange);
                g_bTempcoPending  = false;
            }
            g_bPostponeTempco = false;
            AM_CRITICAL_END
        }
        else
        {
            //
            // Enable power control for the given device.
            //
            AM_CRITICAL_BEGIN
            AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) |=
                pwr_ctrl.ui32PeriphEnable;
            AM_CRITICAL_END
        }
    }
    else // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2
    {
        //
        // Switch GPU to the previous mode or desired mode
        //
        if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
        {
            if (g_ePreGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)
            {
                am_hal_pwrctrl_gpu_mode_select(AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE);
            }

            //
            // GPU power on sequences
            //
            gpu_on_sequence();
        }
        else
        {
            //
            // Update power state
            //
            if ( (pwr_ctrl.ui32PeriphEnable & DEVPWRST_MONITOR_PERIPH_MASK) && (ePeripheral < AM_HAL_PWRCTRL_PERIPH_PDM0) )
            {
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_DEVPWR, true, (void *) &(pwr_ctrl.ui32PeriphStatus));
            }
            if ( (pwr_ctrl.ui32PeriphEnable & AUDSSPWRST_MONITOR_PERIPH_MASK) && (ePeripheral >= AM_HAL_PWRCTRL_PERIPH_PDM0) )
            {
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_AUDSSPWR, true, (void *) &(pwr_ctrl.ui32PeriphStatus));
            }
        }
        AM_CRITICAL_BEGIN
        //
        // Enable power control for the given device.
        //
        AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) |=
            pwr_ctrl.ui32PeriphEnable;
        AM_CRITICAL_END
    }

    ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                              pwr_ctrl.ui32PwrStatReqAddr,
                                              pwr_ctrl.ui32PeriphStatus,
                                              pwr_ctrl.ui32PeriphStatus,
                                              true);

    //
    // Check for timeout.
    //
    if (AM_HAL_STATUS_SUCCESS != ui32Status)
    {
        return ui32Status;
    }

    //
    // Crypto peripheral needs more time to power up after the normal status bit
    // is set. We'll wait for the IDLE signal from the NVM as our signal that
    // crypto is ready.
    //
    if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_CRYPTO)
    {
        ui32Status = am_hal_delay_us_status_change(CRYPTO_WAIT_USEC,
                                                   (uint32_t)&CRYPTO->NVMISIDLE,
                                                   CRYPTO_NVMISIDLE_NVMISIDLEREG_Msk,
                                                   CRYPTO_NVMISIDLE_NVMISIDLEREG_Msk);

        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
        }

        am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_HFRC, AM_HAL_CLKMGR_USER_ID_CRYPTO);
    }

    if ( ePeripheral == AM_HAL_PWRCTRL_PERIPH_OTP)
    {
        //
        // Perform a short delay after OTP power up.
        //
        am_hal_delay_us(100);
    }

    //
    // Check the device status.
    //
    if ( (AM_REGVAL(pwr_ctrl.ui32PwrStatReqAddr) &
          pwr_ctrl.ui32PeriphStatus) != 0)
    {
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        return AM_HAL_STATUS_FAIL;
    }

} // am_hal_pwrctrl_periph_enable()

// ****************************************************************************
//
//  am_hal_pwrctrl_periph_disable_msk_check()
//  Function checks the PWRCTRL->DEVPWREN
//
//  The original check of ((PWRCTRL->DEVPWRSTATUS & ui32PeriphStatus) == 0)
//  will fail when more than one enable in the same domain is set and the
//  user tries disable only one.
//  This function return bIsLastOneInDomain to indicate if this is the last
//  peripheral to be disabled in a power domain.
//
// ****************************************************************************
static bool
pwrctrl_periph_disable_msk_check(am_hal_pwrctrl_periph_e ePeripheral)
{
    uint32_t      ui32Status;
    struct am_pwr_s pwr_ctrl;
    bool bIsLastOneInDomain = true;

    ui32Status = am_get_pwrctrl(&pwr_ctrl, ePeripheral);

    if ( AM_HAL_STATUS_SUCCESS != ui32Status )
    {
        //
        // If am_get_pwrctrl does not return success, pad bIsLastOneInDomain to true.
        //
        bIsLastOneInDomain = true;
        return bIsLastOneInDomain;
    }

    switch (pwr_ctrl.ui32PeriphStatus)
    {
        case (PWRCTRL_HCPA_DEVPWRSTATUS_MASK):
            if (((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & PWRCTRL_HCPA_DEVPWREN_MASK) != 0) &&
                ((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable) == 0))
            {
                bIsLastOneInDomain = false;
            }
            break;

        case (PWRCTRL_HCPB_DEVPWRSTATUS_MASK):
            if (((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & PWRCTRL_HCPB_DEVPWREN_MASK) != 0) &&
                ((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable) == 0))
            {
                bIsLastOneInDomain = false;
            }
            break;

        case (PWRCTRL_HCPC_DEVPWRSTATUS_MASK):
            if (((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & PWRCTRL_HCPC_DEVPWREN_MASK) != 0) &&
                ((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable) == 0))
            {
                bIsLastOneInDomain = false;
            }
            break;

        case (PWRCTRL_I2S_DEVPWRSTATUS_MASK):
            if (((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & PWRCTRL_I2S_DEVPWREN_MASK) != 0) &&
                ((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable) == 0))
            {
                bIsLastOneInDomain = false;
            }
            break;

        case (PWRCTRL_IOS_DEVPWRSTATUS_MASK):
            if (((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & PWRCTRL_IOS_DEVPWREN_MASK) != 0) &&
                ((AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable) == 0))
            {
                bIsLastOneInDomain = false;
            }
            break;

        default:
            bIsLastOneInDomain = true;
            break;
    }

    return bIsLastOneInDomain;
} // pwrctrl_periph_disable_msk_check()

// ****************************************************************************
//
//  am_hal_pwrctrl_periph_disable()
//  Disable power for a peripheral.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_periph_disable(am_hal_pwrctrl_periph_e ePeripheral)
{
    uint32_t      ui32Status;
    struct am_pwr_s pwr_ctrl;

    ui32Status = am_get_pwrctrl(&pwr_ctrl, ePeripheral);

    if ( AM_HAL_STATUS_SUCCESS != ui32Status )
    {
        return ui32Status;
    }

    if ( !(AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) & pwr_ctrl.ui32PeriphEnable) )
    {
        //
        // We're already disabled, nothing to do.
        //
        return AM_HAL_STATUS_SUCCESS;
    }

    if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_OTP)
    {
        if (!APOLLO5_GE_B1 && (PWRCTRL->DEVPWRSTATUS_b.PWRSTCRYPTO))
        {
            // For RevB0 - OTP needs to be kept ON if Crypto is ON
            // This would be re-evaluated for RevB1
            return AM_HAL_STATUS_IN_USE;
        }
        // Need to wait for PTM status idle to ensure any outstanding OTP writes are done

        ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_OTP_US,
                                                  (uint32_t)&OTP->PTMSTAT,
                                                  OTP_PTMSTAT_BUSY_Msk,
                                                  0,
                                                  true);

        //
        // Check for timeout.
        //
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
        }
    }

    if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_DEBUG)
    {
        //
        // TRCENA must be off in order to disable the power domain.
        //
        DCB->DEMCR &= ~DCB_DEMCR_TRCENA_Msk;

        //
        // Disable the CM55 TPIU clock source in MCU control.
        //
        MCUCTRL->DBGCTRL_b.DBGTPIUTRACEENABLE = MCUCTRL_DBGCTRL_DBGTPIUTRACEENABLE_DIS;
        MCUCTRL->DBGCTRL_b.DBGTPIUCLKSEL      = MCUCTRL_DBGCTRL_DBGTPIUCLKSEL_OFF;
    }

    if (APOLLO5_B0_LE_PCM0P7)
    {
        AM_CRITICAL_BEGIN

        //
        // Disable power domain for the given device.
        //
        AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) &= ~pwr_ctrl.ui32PeriphEnable;

        //
        // Switch GPU to LP when powering down GPU.
        //
        if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
        {
            //
            // GPU power off sequences
            //
            gpu_off_sequence();

            //
            // Update TON configuration after GPU is done turned off
            //
            am_hal_sysctrl_ton_config_update(false, AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER);
        }
        AM_CRITICAL_END
    }
    else if ( APOLLO5_B0_GE_PCM1P0 || APOLLO5_B1_LE_PCM2P0 || APOLLO5_B2_PCM2P0 )
    {
        //
        // Switch GPU to LP when powering down GPU.
        //
        if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
        {
            g_bPostponeTempco = true;
            //
            // Disable power domain for the given device.
            //
            AM_CRITICAL_BEGIN
            AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) &= ~pwr_ctrl.ui32PeriphEnable;
            AM_CRITICAL_END
            //
            // GPU power off sequences
            //
            gpu_off_sequence();
            AM_CRITICAL_BEGIN
            if (g_bTempcoPending)
            {
                vddc_vddf_tempco(g_eTempCoRange);
                g_bTempcoPending  = false;
            }
            g_bPostponeTempco = false;
            AM_CRITICAL_END
        }
        else
        {
            //
            // Disable power domain for the given device.
            //
            AM_CRITICAL_BEGIN
            AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) &= ~pwr_ctrl.ui32PeriphEnable;
            AM_CRITICAL_END
        }
    }
    else  // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
    {
        AM_CRITICAL_BEGIN

        //
        // Disable power domain for the given device.
        //
        AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) &= ~pwr_ctrl.ui32PeriphEnable;

        AM_CRITICAL_END
    }

    //
    // When disabling the last peripheral in a power domain, do power status check.
    // When there are more than one enable in the same domain is set and the user
    // tries to disable only one, return AM_HAL_STATUS_SUCCESS.
    //
    if ( pwrctrl_periph_disable_msk_check(ePeripheral) )
    {
        ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                                  pwr_ctrl.ui32PwrStatReqAddr,
                                                  pwr_ctrl.ui32PeriphStatus,
                                                  pwr_ctrl.ui32PeriphStatus,
                                                  false);
        //
        // Check for success.
        //
        if (AM_HAL_STATUS_SUCCESS == ui32Status)
        {
            //
            // Release clock after disabled CRYPTO.
            // CRYPTO has its own power domain.
            //
            if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_CRYPTO)
            {
                am_hal_clkmgr_clock_release(AM_HAL_CLKMGR_CLK_ID_HFRC, AM_HAL_CLKMGR_USER_ID_CRYPTO);
            }
            //
            // Update GPU power mode after disabled GFX.
            // GFX has its own power domain.
            //
            if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
            {
                if (g_eCurGpuPwrMode == AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE)
                {
                    am_hal_pwrctrl_gpu_mode_select(AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER);
                    g_ePreGpuPwrMode = AM_HAL_PWRCTRL_GPU_MODE_HIGH_PERFORMANCE;
                }
                else
                {
                    g_ePreGpuPwrMode = AM_HAL_PWRCTRL_GPU_MODE_LOW_POWER;
                }

                if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                {
                    //
                    // GPU power off sequences
                    //
                    gpu_off_sequence();
                }
            }
            else
            {
                if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                {
                    //
                    // Update power state
                    //
                    if ( (pwr_ctrl.ui32PeriphEnable & DEVPWRST_MONITOR_PERIPH_MASK) && (ePeripheral < AM_HAL_PWRCTRL_PERIPH_PDM0) )
                    {
                        am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_DEVPWR, false, NULL);
                    }
                    if ( (pwr_ctrl.ui32PeriphEnable & AUDSSPWRST_MONITOR_PERIPH_MASK) && (ePeripheral >= AM_HAL_PWRCTRL_PERIPH_PDM0) )
                    {
                        am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_AUDSSPWR, false, NULL);
                    }
                }
            }
        }
        return ui32Status;
    }
    else
    {
        return AM_HAL_STATUS_SUCCESS;
    }

} // am_hal_pwrctrl_periph_disable()

// ****************************************************************************
//
//  am_hal_pwrctrl_periph_enabled()
//  Determine whether a peripheral is currently enabled.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_periph_enabled(am_hal_pwrctrl_periph_e ePeripheral,
                              bool *bEnabled)
{
    uint32_t      ui32Status;
    struct am_pwr_s pwr_ctrl;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( bEnabled == NULL )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Initialize bEnabled to false in case an error is encountered.
    //
    *bEnabled = false;

    ui32Status = am_get_pwrctrl(&pwr_ctrl, ePeripheral);

    if ( AM_HAL_STATUS_SUCCESS != ui32Status )
    {
        return ui32Status;
    }

    *bEnabled = ((AM_REGVAL(pwr_ctrl.ui32PwrStatReqAddr) &
                  pwr_ctrl.ui32PeriphStatus) != 0);

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_periph_enabled()

// ****************************************************************************
//
//  am_hal_pwrctrl_status_get()
//  Get the current powercontrol status registers.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_status_get(am_hal_pwrctrl_status_t *psStatus)
{
    //
    // Device Power ON Status
    //
    psStatus->ui32Device = PWRCTRL->DEVPWRSTATUS;

    //
    // Audio Subsystem ON Status
    //
    psStatus->ui32AudioSS = PWRCTRL->AUDSSPWRSTATUS;

    //
    // MCU Memory Power ON Status
    //
    psStatus->ui32Memory = PWRCTRL->MEMPWRSTATUS;

    //
    // Power ON Status for MCU Core
    //
    psStatus->ui32System = PWRCTRL->SYSPWRSTATUS;

    //
    // Shared SRAM Power ON Status
    //
    psStatus->ui32SSRAM = PWRCTRL->SSRAMPWRST;

    //
    // Voltage Regulators status
    //
    psStatus->ui32VRStatus = PWRCTRL->VRSTATUS;

    //
    // Power Status Register for ADC Block
    //
    psStatus->ui32ADC = PWRCTRL->ADCSTATUS;

    //
    // Power Status Register for audio ADC Block
    //
    psStatus->ui32AudioADC = PWRCTRL->AUDADCSTATUS;

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_status_get()

// ****************************************************************************
//
//  Collect and store all INFO1 data, after which OTP can be turned off.
//
// ****************************************************************************
static uint32_t
pwrctrl_INFO1_populate(void)
{
    //
    // To optimizize power, we'll capture the INFO1 registers of interest here
    //  and power down OTP. The registers will then always be available.
    //

    uint32_t ui32Status;
    uint32_t info1buf[12];

//
// Helper macros for pwrctrl_INFO1_populate().
//  CHK_OFFSET_DELTA: Helper macro to assure continguousness of registers.
//  RD_INFO1: Macro to call am_hal_info1_read() and check return status.
//
#define CHK_OFFSET_DELTA(offh, offl, n)     STATIC_ASSERT((((offh - offl) / 4) + 1) != n)

#define RD_INFO1(infospace, wdoffset, numwds, pData)                    \
    ui32Status = am_hal_info1_read(infospace, wdoffset, numwds, pData); \
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )                          \
    {                                                                   \
        return ui32Status;                                              \
    }

    if ( (MCUCTRL->SHADOWVALID_b.INFO1SELOTP != MCUCTRL_SHADOWVALID_INFO1SELOTP_VALID)  ||
         (PWRCTRL->DEVPWRSTATUS_b.PWRSTOTP   != PWRCTRL_DEVPWRSTATUS_PWRSTOTP_ON) )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    CHK_OFFSET_DELTA(AM_REG_INFO1_SBL_VERSION_1_O, AM_REG_INFO1_SBL_VERSION_0_O, 2 );
    RD_INFO1(AM_HAL_INFO_INFOSPACE_MRAM_INFO1,    (AM_REG_INFO1_SBL_VERSION_0_O / 4), 2, &info1buf[0]);
    g_sINFO1regs.ui32SBLVer0                = info1buf[0];
    g_sINFO1regs.ui32SBLVer1                = info1buf[1];

    RD_INFO1(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1, (AM_REG_OTP_INFO1_MAINPTR_O / 4), 1, &info1buf[0]);
    g_sINFO1regs.ui32MAINPTR                = info1buf[0];
    RD_INFO1(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1, (AM_REG_OTP_INFO1_SBLOTA_O / 4), 1, &info1buf[0]);
    g_sINFO1regs.ui32SBLOTA                 = info1buf[0];

    CHK_OFFSET_DELTA( AM_REG_OTP_INFO1_SOCID7_O, AM_REG_OTP_INFO1_SOCID0_O, 8 );
    RD_INFO1(AM_HAL_INFO_INFOSPACE_OTP_INFO1,     (AM_REG_OTP_INFO1_SOCID0_O / 4), 8, &info1buf[0]);
    g_sINFO1regs.ui32SOCID0                 = info1buf[0];
    g_sINFO1regs.ui32SOCID1                 = info1buf[1];
    g_sINFO1regs.ui32SOCID2                 = info1buf[2];
    g_sINFO1regs.ui32SOCID3                 = info1buf[3];
    g_sINFO1regs.ui32SOCID4                 = info1buf[4];
    g_sINFO1regs.ui32SOCID5                 = info1buf[5];
    g_sINFO1regs.ui32SOCID6                 = info1buf[6];
    g_sINFO1regs.ui32SOCID7                 = info1buf[7];

    RD_INFO1(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1, (AM_REG_OTP_INFO1_PATCH_TRACKER0_O / 4), 1, &info1buf[0]);
    g_sINFO1regs.ui32PATCH_TRACKER0                = info1buf[0];

    CHK_OFFSET_DELTA( AM_REG_OTP_INFO1_TEMP_CAL_ADC_OFFSET_O, AM_REG_OTP_INFO1_TEMP_CAL_ATE_O, 3);
    RD_INFO1(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1, (AM_REG_OTP_INFO1_TEMP_CAL_ATE_O  / 4), 3, &info1buf[0]);
    g_sINFO1regs.ui32TEMP_CAL_ATE           = info1buf[0];
    g_sINFO1regs.ui32TEMP_CAL_MEASURED      = info1buf[1];
    g_sINFO1regs.ui32TEMP_CAL_ADC_OFFSET    = info1buf[2];

    CHK_OFFSET_DELTA(AM_REG_OTP_INFO1_ADC_OFFSET_ERR_O, AM_REG_OTP_INFO1_ADC_GAIN_ERR_O, 2 );
    RD_INFO1(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1, (AM_REG_OTP_INFO1_ADC_GAIN_ERR_O  / 4), 2, &info1buf[0]);
    g_sINFO1regs.ui32ADC_GAIN_ERR           = info1buf[0];
    g_sINFO1regs.ui32ADC_OFFSET_ERR         = info1buf[1];

    CHK_OFFSET_DELTA(AM_REG_OTP_INFO1_AUDADC_B1_HG_INTERCEPT_O, AM_REG_OTP_INFO1_AUDADC_A0_LG_OFFSET_O, 12 );
    RD_INFO1(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1, (AM_REG_OTP_INFO1_AUDADC_A0_LG_OFFSET_O  / 4), 12, &info1buf[0]);
    g_sINFO1regs.ui32AUDADC_A0_LG_OFFSET    = info1buf[0];
    g_sINFO1regs.ui32AUDADC_A0_HG_SLOPE     = info1buf[1];
    g_sINFO1regs.ui32AUDADC_A0_HG_INTERCEPT = info1buf[2];
    g_sINFO1regs.ui32AUDADC_A1_LG_OFFSET    = info1buf[3];
    g_sINFO1regs.ui32AUDADC_A1_HG_SLOPE     = info1buf[4];
    g_sINFO1regs.ui32AUDADC_A1_HG_INTERCEPT = info1buf[5];
    g_sINFO1regs.ui32AUDADC_B0_LG_OFFSET    = info1buf[6];
    g_sINFO1regs.ui32AUDADC_B0_HG_SLOPE     = info1buf[7];
    g_sINFO1regs.ui32AUDADC_B0_HG_INTERCEPT = info1buf[8];
    g_sINFO1regs.ui32AUDADC_B1_LG_OFFSET    = info1buf[9];
    g_sINFO1regs.ui32AUDADC_B1_HG_SLOPE     = info1buf[10];
    g_sINFO1regs.ui32AUDADC_B1_HG_INTERCEPT = info1buf[11];

    //
    // All done, mark the data as valid
    //
    g_sINFO1regs.ui32INFO1GlobalValid = INFO1GLOBALVALID;

    return ui32Status;

} // pwrctrl_INFO1_populate()

// ****************************************************************************
//
//  am_hal_pwrctrl_low_power_init()
//  Initialize the device for low power operation.
//  It is not allowed to call this function more than once in application.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_low_power_init(void)
{
    uint32_t ui32TrimVer = 0;
    uint32_t ui32RegVal = 0;
    bool bEnabled = false;

    if ( RSTGEN->STAT_b.POASTAT )
    {
        //
        // Errata ERR027 BORSTAT can be inadvertantly set after a power cycle.
        //
        RSTGEN->STAT_b.BORSTAT = 0;
    }

    //
    // For lowest power, make sure TRCENA and TPIU clock are off.
    //
    if ( g_ui8TRCENAcount == 0 )
    {
        am_hal_debug_trace_disable();
        am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_DEBUG);
        MCUCTRL->DBGCTRL_b.DBGTPIUTRACEENABLE = MCUCTRL_DBGCTRL_DBGTPIUTRACEENABLE_DIS;
        MCUCTRL->DBGCTRL_b.DBGTPIUCLKSEL      = MCUCTRL_DBGCTRL_DBGTPIUCLKSEL_OFF;
    }

    //
    // Set the default CPDLPSTATE configuration in active mode.
    // Please refer to M55 TRM 5.17.1 for CPDLPSTATE definitions.
    // RLP and ELP are turned on here, users can re-configure them after this
    // by calling am_hal_pwrctrl_pwrmodctl_cpdlp_config().
    //
    am_hal_pwrctrl_pwrmodctl_cpdlp_t sDefaultCpdlpConfig =
    {
        .eRlpConfig = AM_HAL_PWRCTRL_RLP_ON,
        .eElpConfig = AM_HAL_PWRCTRL_ELP_ON,
        .eClpConfig = AM_HAL_PWRCTRL_CLP_ON
    };
    am_hal_pwrctrl_pwrmodctl_cpdlp_config(sDefaultCpdlpConfig);

    //
    // Set the default WIC configuration.
    //
    MCUCTRL->WICCONTROL |= _VAL2FLD(MCUCTRL_WICCONTROL_DEEPISWIC, 1);

    //
    // If info1 locates in OTP, we need to make sure OTP is powered up.
    //
    if (MCUCTRL->SHADOWVALID_b.INFO1SELOTP)
    {
        am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_OTP, &bEnabled);
        if ( !bEnabled )
        {
            am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP);
        }
    }

    //
    // Get trim version
    //
    TrimVersionGet(&ui32TrimVer);

    //
    // For power optimization, power down OTP.
    // Since OTP is being powered down, first power down Crypto.
    //
    // If INFO1 is currently selected as OTP, collect and save INFO1 data
    // for use by HAL functions. This will eliminate the need to have OTP
    // powered. If INFO1 is MRAM, any INFO1 data needed by HAL functions
    // will simply be read directly via am_hal_info1_read().
    //
    pwrctrl_INFO1_populate();
    //
    // Read PATCH_TRACKER0 from MRAM if INFO1 does not locate in OTP.
    //
    if ( g_sINFO1regs.ui32INFO1GlobalValid != INFO1GLOBALVALID )
    {
        am_hal_info1_read(AM_HAL_INFO_INFOSPACE_CURRENT_INFO1,
                          AM_REG_OTP_INFO1_PATCH_TRACKER0_O / 4,
                          1,
                          &g_sINFO1regs.ui32PATCH_TRACKER0);
    }

    //
    // Initialise SPOT manager
    //
    if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
    {
        am_hal_spotmgr_init();
    }

    //
    // Power down Crypto.
    //
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);

    //
    // Power down OTP.
    //
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);

    //
    // Set the default memory configuration.
    //
    // For RevB0 - ROM needs to be kept ON
    // This should be reverted to AUTO for RevB1
    if (APOLLO5_GE_B1)
    {
        g_DefaultMcuMemCfg.eROMMode = AM_HAL_PWRCTRL_ROM_AUTO;
    }
    am_hal_pwrctrl_mcu_memory_config((am_hal_pwrctrl_mcu_memory_config_t *)&g_DefaultMcuMemCfg);
    am_hal_pwrctrl_sram_config((am_hal_pwrctrl_sram_memcfg_t *)&g_DefaultSRAMCfg);

    //
    // Enable clock gate optimizations.
    //
    ui32RegVal = CLKGEN->MISC;
    ui32RegVal |= _VAL2FLD(CLKGEN_MISC_GFXCLKCLKGATEEN, 1)        |   // [19] GFX CLK
                  _VAL2FLD(CLKGEN_MISC_GFXAXICLKCLKGATEEN, 1)     |   // [20] GFX AXI CLK
                  _VAL2FLD(CLKGEN_MISC_APBDMACPUCLKCLKGATEEN, 1)  |   // [21] APB DMA CPU CLK
                  _VAL2FLD(CLKGEN_MISC_ETMTRACECLKCLKGATEEN, 1)   |   // [22] ETM TRACE CLK
                  _VAL2FLD(CLKGEN_MISC_HFRCFUNCCLKGATEEN, 1);         // [23] HFRC_FUNC_CLK
    //
    //
    ui32RegVal |= _VAL2FLD(CLKGEN_MISC_PWRONCLKENDISP, 1)          |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENDISPPHY, 1)       |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENGFX, 1)           |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENUSB, 1)           |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENSDIO, 1)          |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENCRYPTO, 1)        |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENI2S0, 1)          |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENI2S1, 1)          |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENI2S0REFCLK, 1)    |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENI2S1REFCLK, 1)    |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENUSBREFCLK, 1);
    ui32RegVal &= ~_VAL2FLD(CLKGEN_MISC_AXIXACLKENOVRRIDE, 1);
    CLKGEN->MISC = ui32RegVal;

    //
    //
    CLKGEN->CLKCTRL = 0x0; // Disable all unneccesary clocks including display controller clock

    // Increases the reference recovery time between scans in LPMODE1 from 5us to 10us.
    MCUCTRL->AUDADCPWRDLY_b.AUDADCPWR1 = 4;

    //
    // Store the factory values for various trims.
    //
    if ( g_bOrigTrimsStored == false )
    {
        g_orig_ACTTRIMVDDF          = MCUCTRL->SIMOBUCK12_b.ACTTRIMVDDF;
        g_orig_MEMLDOACTIVETRIM     = MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM;
        g_orig_LPTRIMVDDF           = MCUCTRL->SIMOBUCK12_b.LPTRIMVDDF;
        g_orig_MEMLPLDOTRIM         = MCUCTRL->LDOREG2_b.MEMLPLDOTRIM;
        g_orig_TVRGCVREFTRIM        = MCUCTRL->VREFGEN2_b.TVRGCVREFTRIM;
        g_orig_TVRGFVREFTRIM        = MCUCTRL->VREFGEN4_b.TVRGFVREFTRIM;
        g_orig_VDDCLKGTRIM          = MCUCTRL->SIMOBUCK14_b.VDDCLKGTRIM;
        g_orig_CORELDOTEMPCOTRIM    = MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM;
        g_orig_CORELDOACTIVETRIM    = MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM;
        //
        // In order to store the correct g_orig_CORELDOACTIVETRIM, add
        // CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT to g_orig_CORELDOACTIVETRIM
        // if SIMOBUCK was already enabled.
        //
        if ((PWRCTRL->VRSTATUS_b.SIMOBUCKST == PWRCTRL_VRSTATUS_SIMOBUCKST_ACT) && (APOLLO5_B0_GE_PCM0P7 || APOLLO5_GE_B1))
        {
            if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0 )
            {
                g_orig_CORELDOACTIVETRIM += CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT_B0TRIM3_B1TRIM1;
            }
            else if ( APOLLO5_B0_LE_PCM1P0 || APOLLO5_B1_PCM1P1 )
            {
                g_orig_CORELDOACTIVETRIM += CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT;
            }
        }
        g_orig_D2ASPARE             = MCUCTRL->D2ASPARE_b.D2ASPARE;
        g_orig_VDDCACTLOWTONTRIM    = MCUCTRL->SIMOBUCK2_b.VDDCACTLOWTONTRIM;
        g_orig_VDDCACTHIGHTONTRIM   = MCUCTRL->SIMOBUCK2_b.VDDCACTHIGHTONTRIM;
        g_orig_VDDCLVACTLOWTONTRIM  = MCUCTRL->SIMOBUCK4_b.VDDCLVACTLOWTONTRIM;
        g_orig_VDDCLVACTHIGHTONTRIM = MCUCTRL->SIMOBUCK4_b.VDDCLVACTHIGHTONTRIM;
        g_orig_VDDFACTLOWTONTRIM    = MCUCTRL->SIMOBUCK7_b.VDDFACTLOWTONTRIM;
        g_orig_VDDFACTHIGHTONTRIM   = MCUCTRL->SIMOBUCK6_b.VDDFACTHIGHTONTRIM;
        g_bOrigTrimsStored          = true;
    }

    //
    // Set SSRAM/DTCM/NVM retain comes from VDDS
    //
    MCUCTRL->PWRSW0_b.PWRSWVDDRCPUSTATSEL = 0x1;
    MCUCTRL->PWRSW1_b.PWRSWVDDRMSTATSEL   = 0x1;
    MCUCTRL->PWRSW1_b.PWRSWVDDRLSTATSEL   = 0x1;


    AM_CRITICAL_BEGIN
    if (APOLLO5_B0_LE_PCM0P7)
    {
        //
        //  Initialize SIMOBUCK TON
        //
        am_hal_sysctrl_ton_config_init();
        am_hal_sysctrl_ton_config_update(false, g_eCurGpuPwrMode);
    }

    AM_CRITICAL_END

#if AM_HAL_PWRCTRL_SIMOLP_AUTOSWITCH
    if (APOLLO5_B0_GE_PCM1P0 || APOLLO5_B1_LE_PCM2P0 || APOLLO5_B2_PCM2P0)
    {
        // These settings decide at what loadcurrent Buck transitions from LP to active automatically.
        PWRCTRL->TONCNTRCTRL_b.CLKDIV = 0;
        PWRCTRL->TONCNTRCTRL_b.FCNT = 1000;
        PWRCTRL->LPOVRHYSTCNT_b.LPOVRHYSTCNT = 0;
        PWRCTRL->LPOVRTHRESHVDDS_b.LPOVRTHRESHVDDS = 800;
        PWRCTRL->LPOVRTHRESHVDDF_b.LPOVRTHRESHVDDF = 450;
        PWRCTRL->LPOVRTHRESHVDDC_b.LPOVRTHRESHVDDC = 600;
        PWRCTRL->LPOVRTHRESHVDDCLV_b.LPOVRTHRESHVDDCLV = 250;
        // This setting ensures that if buck is in LP mode upon entry to deepsleep, it will transition to active
        // if the load increases slowly and stay in active for the rest of the time
        PWRCTRL->TONCNTRCTRL_b.LPMODESWOVR = 0;
        PWRCTRL->TONCNTRCTRL_b.ENABLELPOVR = 0;
    }
#endif

    // Execute clock mux reset
    am_hal_sysctrl_clkmuxrst_low_power_init();

    if (APOLLO5_GE_B1)
    {
        //
        // For B1 and later, interpretation of MRAMEXTCTRL_b.MRAMAUTOWAKEUPCTRL[1] is inverted
        // Set it here to disable autowakeup to reduce MRAM wakeup latency
        //
        PWRCTRL->MRAMEXTCTRL_b.MRAMAUTOWAKEUPCTRL = 0x3;
    }

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_low_power_init()

void
buck_ldo_override_init(void)
{
    //
    // Force SIMOBUCK into active mode. SIMOBUCKOVER must be set last.
    // This override, even though enabled - is not effective till we go to DeepSleep
    //
    MCUCTRL->VRCTRL_b.SIMOBUCKPDNB   = 1;
    MCUCTRL->VRCTRL_b.SIMOBUCKRSTB   = 1;
    MCUCTRL->VRCTRL_b.SIMOBUCKACTIVE = 1;
    MCUCTRL->VRCTRL_b.SIMOBUCKOVER   = 1;

#if AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL
    //
    // Force LDOs into active mode and to run in parallel with SIMO.
    //
    //
    // Core LDO. Set CORELDOOVER last
    //
    MCUCTRL->VRCTRL_b.CORELDOCOLDSTARTEN  = 0;
    MCUCTRL->VRCTRL |=
        MCUCTRL_VRCTRL_CORELDOACTIVE_Msk        |
        MCUCTRL_VRCTRL_CORELDOACTIVEEARLY_Msk   |
        MCUCTRL_VRCTRL_CORELDOPDNB_Msk;
    MCUCTRL->VRCTRL_b.CORELDOOVER         = 1;
    //
    // Mem LDO. Set MEMLDOOVER last
    //
    MCUCTRL->VRCTRL_b.MEMLDOCOLDSTARTEN   = 0;
    MCUCTRL->VRCTRL |=
        MCUCTRL_VRCTRL_MEMLDOACTIVE_Msk         |
        MCUCTRL_VRCTRL_MEMLDOACTIVEEARLY_Msk    |
        MCUCTRL_VRCTRL_MEMLDOPDNB_Msk;
    MCUCTRL->VRCTRL_b.MEMLDOOVER          = 1;
#endif // AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL
} // buck_ldo_override_init()


// Dynamically turn on and off the overrides for buck and LDO
// Override configs are already set once in buck_ldo_override_init
void
buck_ldo_update_override(bool bEnable)
{
    MCUCTRL->VRCTRL_b.SIMOBUCKOVER   = bEnable;
#if AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL
    MCUCTRL->VRCTRL_b.CORELDOOVER    = bEnable;
    MCUCTRL->VRCTRL_b.MEMLDOOVER     = bEnable;
#endif // AM_HAL_PWRCTL_SET_CORELDO_MEMLDO_IN_PARALLEL
} // buck_ldo_update_override()

// ****************************************************************************
//
//  am_hal_pwrctrl_control()
//  Additional miscellaneous power controls.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_control(am_hal_pwrctrl_control_e eControl, void *pArgs)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    switch ( eControl )
    {
        case AM_HAL_PWRCTRL_CONTROL_SIMOBUCK_INIT:
            {
                if (PWRCTRL->VRSTATUS_b.SIMOBUCKST == PWRCTRL_VRSTATUS_SIMOBUCKST_ACT)
                {
                    return AM_HAL_STATUS_SUCCESS;
                }
                if ( APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2 ) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                {
                    MCUCTRL->SIMOBUCK4_b.VDDCLVACTLOWTONTRIM = VDDCLVACTLOWTONTRIM_DEFAULT;
                }
                buck_ldo_override_init();
                MCUCTRL->SIMOBUCK15_b.TRIMLATCHOVER = 1;

                //
                // Enable SIMOBUCK compensations
                //
                MCUCTRL->SIMOBUCK0 |=
                    MCUCTRL_SIMOBUCK0_VDDCRXCOMPEN_Msk |
                    MCUCTRL_SIMOBUCK0_VDDFRXCOMPEN_Msk |
                    MCUCTRL_SIMOBUCK0_VDDSRXCOMPEN_Msk |
                    MCUCTRL_SIMOBUCK0_VDDCLVRXCOMPEN_Msk;

                if ((APOLLO5_B0_GE_PCM0P7 || APOLLO5_GE_B1))
                {
                    //
                    // Reduce coreldo output voltage
                    //
                    if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0 )
                    {
                        if (g_bOrigTrimsStored)
                        {
                            FINAL_VAL_UDF_CAP_BASE(g_orig_CORELDOACTIVETRIM, CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT_B0TRIM3_B1TRIM1, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
                        }
                    }
                    else if ( APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2 ) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                    {
                        if ( g_sSpotMgrINFO1regs.ui32SpotMgrINFO1GlobalValid == INFO1GLOBALVALID )
                        {
                            //
                            // Reduce CORELDOACTIVETRIM
                            //
                            MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM = g_sSpotMgrINFO1regs.sPowerStateArray[7].PWRSTATE_b.CORELDOACTTRIM;
                        }
                    }
                    else
                    {
                        if (g_bOrigTrimsStored)
                        {
                            FINAL_VAL_UDF_CAP_BASE(g_orig_CORELDOACTIVETRIM, CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
                        }
                    }
                    //
                    // Enable the SIMOBUCK
                    //
                    PWRCTRL->VRCTRL_b.SIMOBUCKEN = 1;
                    //
                    // Change memldo trim to support switching memldo reference to tvrgf
                    // Switch memldo reference to tvrgf
                    //
                    if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0)
                    {
                        MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 1;
                        MCUCTRL->D2ASPARE = (MCUCTRL->D2ASPARE & ~(0x3UL << 15)) | (0x2UL << 15);
                    }
                    else if ( APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2 ) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                    {
                        if ( g_sSpotMgrINFO1regs.ui32SpotMgrINFO1GlobalValid == INFO1GLOBALVALID )
                        {
                            //
                            // Change memldo trim to support switching memldo reference to tvrgf
                            //
                            MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = g_sSpotMgrINFO1regs.sMemldoCfg.MEMLDOCONFIG_b.MEMLDOACTTRIM;
                            MCUCTRL->D2ASPARE = (MCUCTRL->D2ASPARE & ~(0x3UL << 15)) | (g_sSpotMgrINFO1regs.sMemldoCfg.MEMLDOCONFIG_b.MEMLDOD2ASPARE << 15);
                        }
                    }
                    else
                    {
                        MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = 0;
                        MCUCTRL->D2ASPARE = (MCUCTRL->D2ASPARE & ~(0x3UL << 15)) | (0x1UL << 15);
                    }
                }
                else
                {
                    //
                    // Enable the SIMOBUCK
                    //
                    PWRCTRL->VRCTRL_b.SIMOBUCKEN = 1;
                }
            }
            break;

            // Use am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN) if using crypto
        case AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN:
            {
                bool        bEnabled;

                //
                // Check if CRYPTO block is powered on.
                //
                bEnabled = false;
                am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_CRYPTO, &bEnabled);
                if ( bEnabled )
                {
                    //
                    // Prepare Crypto for PowerDown & wait for it to be ready
                    //
                    ui32Status = crypto_quiesce();

                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        return ui32Status;
                    }

                    //
                    // Power down the crypto block
                    //
                    ui32Status = am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        return ui32Status;
                    }
                }
            }
            break;

        case AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP:
            //
            // This optimization is optional. Enable it IFF the 32KHz crystal is
            // not required during deep sleep. If enabled it will save ~0.8uA.
            //
            MCUCTRL->XTALGENCTRL_b.XTALBIASTRIM   = 0x20;

            MCUCTRL->XTALCTRL =
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALICOMPTRIM,  0 )                                       |
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALIBUFTRIM,   0 )                                       |
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOMPPDNB,   MCUCTRL_XTALCTRL_XTALCOMPPDNB_PWRDNCOMP ) |
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALPDNB,       MCUCTRL_XTALCTRL_XTALPDNB_PWRDNCORE )     |
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOMPBYPASS, MCUCTRL_XTALCTRL_XTALCOMPBYPASS_USECOMP ) |
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOREDISFB,  MCUCTRL_XTALCTRL_XTALCOREDISFB_EN )       |
                _VAL2FLD(MCUCTRL_XTALCTRL_XTALSWE,        MCUCTRL_XTALCTRL_XTALSWE_OVERRIDE_EN);
            break;

        case AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL:
            //
            // TRCENA must be off in order to disable the power domain.
            //
            DCB->DEMCR &= ~DCB_DEMCR_TRCENA_Msk;

            //
            // Disable the CM55 TPIU clock source in MCU control.
            //
            MCUCTRL->DBGCTRL_b.DBGTPIUTRACEENABLE = MCUCTRL_DBGCTRL_DBGTPIUTRACEENABLE_DIS;
            MCUCTRL->DBGCTRL_b.DBGTPIUCLKSEL      = MCUCTRL_DBGCTRL_DBGTPIUCLKSEL_OFF;
            PWRCTRL->DEVPWREN = 0;
            PWRCTRL->AUDSSPWREN = 0;
            ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                                      (uint32_t) &(PWRCTRL->DEVPWREN),
                                                      DEVPWRST_ALL_PERIPH_MASK,
                                                      0,
                                                      true);
            //
            // Check for success.
            //
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
            }
            else
            {
                if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                {
                    am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_DEVPWR, false, NULL);
                }
            }
            ui32Status = am_hal_delay_us_status_check(AM_HAL_PWRCTRL_MAX_WAIT_US,
                                                      (uint32_t) &(PWRCTRL->AUDSSPWREN),
                                                      AUDSSPWRST_ALL_PERIPH_MASK,
                                                      0,
                                                      true);
            //
            // Check for success.
            //
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
            }
            else
            {
                if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
                {
                    am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_AUDSSPWR, false, NULL);
                }
            }

            break;


        default:
            ui32Status = AM_HAL_STATUS_INVALID_ARG;
            break;
    }

    //
    // Return success status.
    //
    return ui32Status;

} // am_hal_pwrctrl_control()


//*****************************************************************************
//
// This function no longer restores MCU to its boot state; instead, it prepares
// MCU for the next image to start in the correct state.
//
// For PCM2.1 and later versions,
// this function checks CPU and GPU status and switches MCU to default power state 7.
//
// For PCM2.0 and previous versions,
// this function only checks CPU and GPU status.
//
// Important:
//
// - For PCM2.1 and later trim versions, this function must be called before
//   transition to a new application, such as the case of a secondary bootloader
//   transitioning to an application. Before calling this function, users
//   should switch CPU to LP if it is in HP mode, and turned off GPU if it was
//   turned on.
//
// - For PCM2.0 and previous versions, it is not a must to call this function.
//   But users should guarantee the following:
//     1. GPU is never turned on and am_hal_pwrctrl_temp_update is never called, if
//        am_bsp_low_power_init is called in customers bootloader.
//     2. If deepsleep is called in bootloader,
//        am_hal_sysctrl_force_buck_active_in_deepsleep(true) should be called
//        before calling deepsleep.
//
// - For all trim versions,
//   before calling this function, in order to release all clocks when exiting
//   second boot loader, we suggest users to disable peripherals which are used
//   in second bootloader and already requested clocks through clkmgr. Users
//   should disable peripherals by calling the separated API for peripheral
//   disable/deinit/powerdown, but not by writing to the power enable registers
//   directly.
//
//*****************************************************************************
uint32_t
am_hal_pwrctrl_settings_restore(void)
{
    bool bEnabled = false;
    am_hal_pwrctrl_temp_thresh_t dummy;

    //
    // Check CPU status, if it is HP, return failure.
    //
    if (( PWRCTRL->MCUPERFREQ_b.MCUPERFSTATUS == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE ))
    {
        return AM_HAL_STATUS_FAIL;
    }
    //
    // Check GPU status, if it is ON, return failure.
    //
    if (am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_GFX, &bEnabled) == AM_HAL_STATUS_SUCCESS)
    {
        if (bEnabled)
        {
            return AM_HAL_STATUS_FAIL;
        }
    }
    else
    {
        return AM_HAL_STATUS_FAIL;
    }
    //
    // For PCM2.1 and later versions, switches MCU to default power state 7.
    //
    if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
    {
        //
        // Turn on OTP if all peripherals are all off
        //
        if ((PWRCTRL->DEVPWRSTATUS == 0) &&
            (PWRCTRL->AUDSSPWRSTATUS == 0 ))
        {
            if (am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP) != AM_HAL_STATUS_SUCCESS)
            {
                return AM_HAL_STATUS_FAIL;
            }
        }
        else
        {
            return AM_HAL_STATUS_FAIL;
        }
        //
        // Report the lowest temperature to spotmgr
        //
        if (am_hal_pwrctrl_temp_update(-40.0f, &dummy) != AM_HAL_STATUS_SUCCESS)
        {
            return AM_HAL_STATUS_FAIL;
        }
        //
        // Polling the timer AM_HAL_INTERNAL_TIMER_NUM_A status until it is disabled,
        // to guarantee all power state updating operations are completed.
        //
        if (am_hal_delay_us_status_change(TIMER_A_DISABLE_WAIT_IN_US,
                                          (uint32_t)&TIMERn(AM_HAL_INTERNAL_TIMER_NUM_A)->CTRL0,
                                          TIMER_CTRL0_TMR0EN_Msk,
                                          TIMER_CTRL0_TMR0EN_DIS)
            != AM_HAL_STATUS_SUCCESS )
        {
            return AM_HAL_STATUS_TIMEOUT;
        }
    }

    return AM_HAL_STATUS_SUCCESS;
}


// ****************************************************************************
//
//  am_hal_pwrctrl_pwrmodctl_cpdlp_config()
//  sCpdlpConfig - CPDLPSTATE config.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_pwrmodctl_cpdlp_config(am_hal_pwrctrl_pwrmodctl_cpdlp_t sCpdlpConfig)
{
    uint32_t ui32CpdlpConfig = 0;

    //
    // If cache is in use, powering off cache is not allowed.
    //
    if (((SCB->CCR & SCB_CCR_IC_Msk) || (SCB->CCR & SCB_CCR_DC_Msk))
        && (sCpdlpConfig.eRlpConfig == AM_HAL_PWRCTRL_RLP_OFF))
    {
        return AM_HAL_STATUS_FAIL;
    }

    ui32CpdlpConfig |= (sCpdlpConfig.eRlpConfig << PWRMODCTL_CPDLPSTATE_RLPSTATE_Pos);
    ui32CpdlpConfig |= (sCpdlpConfig.eElpConfig << PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos);
    ui32CpdlpConfig |= (sCpdlpConfig.eClpConfig << PWRMODCTL_CPDLPSTATE_CLPSTATE_Pos);

    PWRMODCTL->CPDLPSTATE = ui32CpdlpConfig;

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_pwrmodctl_cpdlp_config()

// ****************************************************************************
//
//  am_hal_pwrctrl_pwrmodctl_cpdlp_get()
//  psCpdlpConfig - CPDLPSTATE config.
//
// ****************************************************************************
void
am_hal_pwrctrl_pwrmodctl_cpdlp_get(am_hal_pwrctrl_pwrmodctl_cpdlp_t * psCpdlpConfig)
{
    uint32_t ui32CpdlpConfig = 0;

    ui32CpdlpConfig = PWRMODCTL->CPDLPSTATE;

    psCpdlpConfig->eRlpConfig = (am_hal_pwrctrl_pwrmodctl_rlp_e) ((ui32CpdlpConfig & PWRMODCTL_CPDLPSTATE_RLPSTATE_Msk) >> PWRMODCTL_CPDLPSTATE_RLPSTATE_Pos);
    psCpdlpConfig->eElpConfig = (am_hal_pwrctrl_pwrmodctl_elp_e) ((ui32CpdlpConfig & PWRMODCTL_CPDLPSTATE_ELPSTATE_Msk) >> PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos);
    psCpdlpConfig->eClpConfig = (am_hal_pwrctrl_pwrmodctl_clp_e) ((ui32CpdlpConfig & PWRMODCTL_CPDLPSTATE_CLPSTATE_Msk) >> PWRMODCTL_CPDLPSTATE_CLPSTATE_Pos);
} // am_hal_pwrctrl_pwrmodctl_cpdlp_get()

// ****************************************************************************
//
//  vddc_vddf_tempco
//
// ****************************************************************************
static inline void
vddc_vddf_tempco(am_hal_pwrctrl_tempco_range_e eTempCoRange)
{
#if AM_HAL_ENABLE_TEMPCO_VDDC
    int32_t i32TVRGCDiff = 0;
    int32_t i32CORELDOReduce = 0;
    int32_t i32TVRGCReduce = 0;
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
    int32_t i32TVRGFDiff = 0;
    int32_t i32TVRGFReduce = 0;
#endif
    int32_t i32PosVal = 0;

    if (g_bOrigTrimsStored && (PWRCTRL->VRSTATUS_b.SIMOBUCKST == PWRCTRL_VRSTATUS_SIMOBUCKST_ACT))
    {
        if (PWRCTRL->DEVPWREN_b.PWRENGFX)
        {
#if AM_HAL_ENABLE_TEMPCO_VDDC
            i32TVRGCDiff = 9;
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
            i32TVRGFDiff = 15;
#endif
        }

        switch (eTempCoRange)
        {
            case AM_HAL_PWRCTRL_TEMPCO_RANGE_LOW:
#if AM_HAL_ENABLE_TEMPCO_VDDC
                i32CORELDOReduce = 17;
                i32TVRGCReduce = 35;
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
                i32TVRGFReduce = 10;
#endif
                break;

            case AM_HAL_PWRCTRL_TEMPCO_RANGE_MID:
#if AM_HAL_ENABLE_TEMPCO_VDDC
                i32CORELDOReduce = 12;
                i32TVRGCReduce = 20;
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
                i32TVRGFReduce = 10;
#endif
                break;

            case AM_HAL_PWRCTRL_TEMPCO_RANGE_HIGH:
#if AM_HAL_ENABLE_TEMPCO_VDDC
                if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0 )
                {
                    i32CORELDOReduce = CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT_B0TRIM3_B1TRIM1;
                }
                else
                {
                    i32CORELDOReduce = CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT;
                }
                i32TVRGCReduce = 0;
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
                i32TVRGFReduce = 0;
#endif
                break;

            default:
#if AM_HAL_ENABLE_TEMPCO_VDDC
                i32CORELDOReduce = 0;
                i32TVRGCReduce = 0;
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
                i32TVRGFReduce = 0;
#endif
                break;
        }

#if AM_HAL_ENABLE_TEMPCO_VDDC
        if (i32CORELDOReduce > 0)
        {
            FINAL_VAL_UDF_CAP_BASE(g_orig_CORELDOACTIVETRIM, i32CORELDOReduce, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
        }
        else
        {
            i32PosVal = -i32CORELDOReduce;
            FINAL_VAL_OVF_CAP_BASE(g_orig_CORELDOACTIVETRIM, i32PosVal, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
        }
        if (i32TVRGCReduce - i32TVRGCDiff > 0)
        {
            i32PosVal = i32TVRGCReduce - i32TVRGCDiff;
            FINAL_VAL_UDF_CAP_BASE(g_orig_TVRGCVREFTRIM, i32PosVal, MCUCTRL, VREFGEN2, TVRGCVREFTRIM);
        }
        else
        {
            i32PosVal = -(i32TVRGCReduce - i32TVRGCDiff);
            FINAL_VAL_OVF_CAP_BASE(g_orig_TVRGCVREFTRIM, i32PosVal, MCUCTRL, VREFGEN2, TVRGCVREFTRIM);
        }
#endif
#if AM_HAL_ENABLE_TEMPCO_VDDF
        if (i32TVRGFReduce - i32TVRGFDiff > 0)
        {
            i32PosVal = i32TVRGFReduce - i32TVRGFDiff;
            FINAL_VAL_UDF_CAP_BASE(g_orig_TVRGFVREFTRIM, i32PosVal, MCUCTRL, VREFGEN4, TVRGFVREFTRIM);
        }
        else
        {
            i32PosVal = -(i32TVRGFReduce - i32TVRGFDiff);
            FINAL_VAL_OVF_CAP_BASE(g_orig_TVRGFVREFTRIM, i32PosVal, MCUCTRL, VREFGEN4, TVRGFVREFTRIM);
        }
#endif
    }
}

// ****************************************************************************
//
//  vddc_vddf_tempco_top
//
// ****************************************************************************
static inline void
vddc_vddf_tempco_top(am_hal_pwrctrl_tempco_range_e eTempCoRange)
{
    AM_CRITICAL_BEGIN

    if (g_bPostponeTempco)
    {
        g_eTempCoRange = eTempCoRange;
        g_bTempcoPending = true;
    }
    else
    {
        vddc_vddf_tempco(eTempCoRange);
    }

    AM_CRITICAL_END
}

// ****************************************************************************
//
//  am_hal_pwrctrl_temp_update()
//  fCurTemp - Current temperature in Celsius.
//  psTempThresh - This API returns this structure for notifying of thresholds
//                 when application should make this call again (When temp goes
//                 lower than fLowThresh or rises above fHighThresh).
//
//  If current temperature is higher than BUCK_LP_TEMP_THRESHOLD(e.g. 50c),
//  bFrcBuckAct must be set to true. Otherwise, set bFrcBuckAct to false.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_temp_update(float fCurTemp, am_hal_pwrctrl_temp_thresh_t * psTempThresh)
{
    am_hal_pwrctrl_tempco_range_e eTempRange;
    am_hal_spotmgr_tempco_range_e eTempRange1;

    if (APOLLO5_B0_LE_PCM0P7)
    {
        psTempThresh->fLowThresh = LOW_LIMIT;
        psTempThresh->fHighThresh = HIGH_LIMIT;
        return AM_HAL_STATUS_SUCCESS;
    }

    if (APOLLO5_B1_GE_PCM2P1 || APOLLO5_B2_GE_PCM2P1 || APOLLO5_GT_B2) // B1 PCM2.1 and B1 later trim versions, B2 PCM2.1 and B2 later trim versions, and later silicon after B2.
    {
        if ((fCurTemp < VDDC_VDDF_TEMPCO_THRESHOLD_LOW) && (fCurTemp >= LOW_LIMIT))
        {
            eTempRange1 = AM_HAL_SPOTMGR_TEMPCO_RANGE_VERY_LOW;
        }
        else if ((fCurTemp >= VDDC_VDDF_TEMPCO_THRESHOLD_LOW) && (fCurTemp < VDDC_VDDF_TEMPCO_THRESHOLD_MID))
        {
            eTempRange1 = AM_HAL_SPOTMGR_TEMPCO_RANGE_LOW;
        }
        else if ((fCurTemp >= VDDC_VDDF_TEMPCO_THRESHOLD_MID) && (fCurTemp < VDDC_VDDF_TEMPCO_THRESHOLD_HIGH))
        {
            eTempRange1 = AM_HAL_SPOTMGR_TEMPCO_RANGE_MID;
        }
        else if ((fCurTemp >= VDDC_VDDF_TEMPCO_THRESHOLD_HIGH) && (fCurTemp < HIGH_LIMIT))
        {
            eTempRange1 = AM_HAL_SPOTMGR_TEMPCO_RANGE_HIGH;
        }
        else
        {
            eTempRange1 = AM_HAL_SPOTMGR_TEMPCO_OUT_OF_RANGE;
        }
        //
        // Report temperature range change, set or clear g_bFrcBuckAct, assign the numbers to returned temperature threshold.
        //
        switch (eTempRange1)
        {
            case AM_HAL_SPOTMGR_TEMPCO_RANGE_VERY_LOW:
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_TEMP, false, (void *) &eTempRange1);
                psTempThresh->fLowThresh = LOW_LIMIT;
                psTempThresh->fHighThresh = VDDC_VDDF_TEMPCO_THRESHOLD_LOW;
                break;
            case AM_HAL_SPOTMGR_TEMPCO_RANGE_LOW:
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_TEMP, false, (void *) &eTempRange1);
                psTempThresh->fLowThresh = VDDC_VDDF_TEMPCO_THRESHOLD_LOW - TEMP_HYSTERESIS;
                psTempThresh->fHighThresh = VDDC_VDDF_TEMPCO_THRESHOLD_MID;
                break;
            case AM_HAL_SPOTMGR_TEMPCO_RANGE_MID:
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_TEMP, false, (void *) &eTempRange1);
                psTempThresh->fLowThresh = VDDC_VDDF_TEMPCO_THRESHOLD_MID - TEMP_HYSTERESIS;
                psTempThresh->fHighThresh = VDDC_VDDF_TEMPCO_THRESHOLD_HIGH;
                break;
            case AM_HAL_SPOTMGR_TEMPCO_RANGE_HIGH:
                am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_TEMP, false, (void *) &eTempRange1);
                psTempThresh->fLowThresh = VDDC_VDDF_TEMPCO_THRESHOLD_HIGH - TEMP_HYSTERESIS;
                psTempThresh->fHighThresh = HIGH_LIMIT;
                break;
            default:
                psTempThresh->fLowThresh = 0.0f;
                psTempThresh->fHighThresh = 0.0f;
                return AM_HAL_STATUS_FAIL;
        }
    }
    else
    {
        if ((fCurTemp < VDDC_VDDF_TEMPCO_THRESHOLD) && (fCurTemp >= LOW_LIMIT))
        {
            eTempRange = AM_HAL_PWRCTRL_TEMPCO_RANGE_LOW;
        }
        else if ((fCurTemp >= VDDC_VDDF_TEMPCO_THRESHOLD) && (fCurTemp < BUCK_LP_TEMP_THRESHOLD))
        {
            eTempRange = AM_HAL_PWRCTRL_TEMPCO_RANGE_MID;
        }
        else if ((fCurTemp >= BUCK_LP_TEMP_THRESHOLD) && (fCurTemp < HIGH_LIMIT))
        {
            eTempRange = AM_HAL_PWRCTRL_TEMPCO_RANGE_HIGH;
        }
        else
        {
            eTempRange = AM_HAL_PWRCTRL_TEMPCO_OUT_OF_RANGE;
        }

        switch (eTempRange)
        {
            case AM_HAL_PWRCTRL_TEMPCO_RANGE_LOW:
                g_bFrcBuckAct = false;
                vddc_vddf_tempco_top(AM_HAL_PWRCTRL_TEMPCO_RANGE_LOW);
                psTempThresh->fLowThresh = LOW_LIMIT;
                psTempThresh->fHighThresh = VDDC_VDDF_TEMPCO_THRESHOLD;
                break;
            case AM_HAL_PWRCTRL_TEMPCO_RANGE_MID:
                g_bFrcBuckAct = false;
                vddc_vddf_tempco_top(AM_HAL_PWRCTRL_TEMPCO_RANGE_MID);
                psTempThresh->fLowThresh = VDDC_VDDF_TEMPCO_THRESHOLD - TEMP_HYSTERESIS;
                psTempThresh->fHighThresh = BUCK_LP_TEMP_THRESHOLD;
                break;
            case AM_HAL_PWRCTRL_TEMPCO_RANGE_HIGH:
                g_bFrcBuckAct = true;
                vddc_vddf_tempco_top(AM_HAL_PWRCTRL_TEMPCO_RANGE_HIGH);
                psTempThresh->fLowThresh = BUCK_LP_TEMP_THRESHOLD - TEMP_HYSTERESIS;
                psTempThresh->fHighThresh = HIGH_LIMIT;
                break;
            default:
                psTempThresh->fLowThresh = 0.0f;
                psTempThresh->fHighThresh = 0.0f;
                return AM_HAL_STATUS_FAIL;
        }
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Restore VDDC and VDDF settings before deepsleep to allow application to
// suspend tempsensing during deeplseep.
//
//*****************************************************************************

#if NO_TEMPSENSE_IN_DEEPSLEEP
void
vddc_vddf_setting_restore(void)
{
    if (g_bOrigTrimsStored && (PWRCTRL->VRSTATUS_b.SIMOBUCKST == PWRCTRL_VRSTATUS_SIMOBUCKST_ACT))
    {
        if (PWRCTRL->DEVPWREN_b.PWRENGFX)
        {
            //
            // Restore VDDF, and boost 15 codes for GPU
            //
            FINAL_VAL_OVF_CAP_BASE(g_orig_TVRGFVREFTRIM, 15, MCUCTRL, VREFGEN4, TVRGFVREFTRIM);
            //
            // Restore VDDC, and boost 9 codes for GPU
            //
            FINAL_VAL_OVF_CAP_BASE(g_orig_TVRGCVREFTRIM, 9, MCUCTRL, VREFGEN2, TVRGCVREFTRIM);
            //
            // Delay 15 us
            //
            am_hal_delay_us(15);
        }
        else
        {
            //
            // Restore VDDF
            //
            MCUCTRL->VREFGEN4_b.TVRGFVREFTRIM = g_orig_TVRGFVREFTRIM;
            //
            // Restore VDDC
            //
            MCUCTRL->VREFGEN2_b.TVRGCVREFTRIM = g_orig_TVRGCVREFTRIM;
            //
            // Set coreldo below simobuck VDDC
            //
            if ( APOLLO5_B0_GE_PCM1P1 || APOLLO5_B1_PCM2P0 || APOLLO5_B2_PCM2P0 )
            {
                FINAL_VAL_UDF_CAP_BASE(g_orig_CORELDOACTIVETRIM, CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT_B0TRIM3_B1TRIM1, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
            }
            else
            {
                FINAL_VAL_UDF_CAP_BASE(g_orig_CORELDOACTIVETRIM, CORELDOACTIVETRIM_REDUCE_IN_SIMOBUCK_INIT, MCUCTRL, LDOREG1, CORELDOACTIVETRIM);
            }
            //
            // Delay 15 us
            //
            am_hal_delay_us(15);
        }
    }
}
#endif

//*****************************************************************************
//
// Enable power to system PLL.
//
//*****************************************************************************
uint32_t
am_hal_pwrctrl_syspll_enable()
{
    AM_CRITICAL_BEGIN
    if (APOLLO5_GE_B1)
    {
        MCUCTRL->D2ASPARE_b.D2ASPARE &= ~(0x1UL << 21);
        am_hal_delay_us(1);
    }

    MCUCTRL->PLLCTL0_b.SYSPLLVDDFPDNB = MCUCTRL_PLLCTL0_SYSPLLVDDFPDNB_ENABLE;
    MCUCTRL->PLLCTL0_b.SYSPLLVDDHPDNB = MCUCTRL_PLLCTL0_SYSPLLVDDHPDNB_ENABLE;

    AM_CRITICAL_END

    // Give some time for power to stable
    am_hal_delay_us(5);

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Disable power to system PLL.
//
//*****************************************************************************
uint32_t
am_hal_pwrctrl_syspll_disable()
{
    AM_CRITICAL_BEGIN

    MCUCTRL->PLLCTL0_b.SYSPLLVDDFPDNB = MCUCTRL_PLLCTL0_SYSPLLVDDFPDNB_DISABLE;
    MCUCTRL->PLLCTL0_b.SYSPLLVDDHPDNB = MCUCTRL_PLLCTL0_SYSPLLVDDHPDNB_DISABLE;

    if (APOLLO5_GE_B1)
    {
        am_hal_delay_us(1);
        MCUCTRL->D2ASPARE_b.D2ASPARE |= (0x1UL << 21);
    }
    AM_CRITICAL_END

    return AM_HAL_STATUS_SUCCESS;
}

// ****************************************************************************
//
//  Determine whether system PLL is currently enabled.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_syspll_enabled(bool *bEnabled)
{
    *bEnabled =  (MCUCTRL->PLLCTL0_b.SYSPLLVDDFPDNB == MCUCTRL_PLLCTL0_SYSPLLVDDFPDNB_ENABLE);

    return AM_HAL_STATUS_SUCCESS;
}



//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
