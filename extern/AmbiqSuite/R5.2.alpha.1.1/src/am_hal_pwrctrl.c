//*****************************************************************************
//
//! @file am_hal_pwrctrl.c
//!
//! @brief Functions for enabling and disabling power domains.
//!
//! @addtogroup pwrctrl4_ap510L PWRCTRL - Power Control
//! @ingroup apollo510L_hal
//! @{
//!
//! Purpose: This module provides comprehensive power control functions for
//!          Apollo5 devices, managing power domains, MCU/GPU modes, memory
//!          configurations, and temperature compensation. It enables efficient power
//!          management for optimal performance and energy efficiency across all system
//!          components.
//!
//! @section hal_pwrctrl_features Key Features
//!
//! 1. @b Power @b Domain @b Control: Enable/disable and configure power domains.
//! 2. @b MCU/GPU @b Modes: Select and query MCU/GPU power modes.
//! 3. @b Memory @b Configuration: Configure and query memory power states.
//! 4. @b Temperature @b Compensation: Support for TempCo and trim adjustments.
//! 5. @b Peripheral @b Management: Enable/disable and monitor peripheral power.
//!
//! @section hal_pwrctrl_functionality Functionality
//!
//! - Enable/disable power domains and peripherals
//! - Select and query MCU/GPU power modes
//! - Configure memory and retention settings
//! - Handle temperature compensation and trim updates
//! - Query and restore power control settings
//!
//! @section hal_pwrctrl_usage Usage
//!
//! 1. Select MCU/GPU power mode using am_hal_pwrctrl_mcu_mode_select()/am_hal_pwrctrl_gpu_mode_select()
//! 2. Configure memory using am_hal_pwrctrl_mcu_memory_config()
//! 3. Enable/disable peripherals with am_hal_pwrctrl_periph_enable()/am_hal_pwrctrl_periph_disable()
//! 4. Use temperature compensation APIs for trim adjustments
//! 5. Restore power settings before transitioning to new application images
//!
//! @section hal_pwrctrl_configuration Configuration
//!
//! - Configure power domains and memory retention
//! - Set up temperature compensation intervals and thresholds
//! - Manage peripheral power and clock gating
//! - Restore factory trims and default power settings
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
#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_hal_clkmgr_private.h"
#include "mcu/am_hal_crm_private.h"

//*****************************************************************************
//
// Local defines
//
//*****************************************************************************

//
//! Maximum number of checks to memory power status before declaring error
// (5 x 1usec = 5usec).
//
#define AM_HAL_PWRCTRL_MAX_WAIT_US          5
#define AM_HAL_PWRCTRL_MAX_WAIT_OTP_US      100000
#define AM_HAL_PWRCTRL_MAX_BOOTROM_COUNT    10000
#define AM_HAL_PWRCTRL_MAX_WAIT_CM4_WAKEUP_US    10000
#define AM_HAL_PWRCTRL_MAX_WAIT_CM4_MBOX_INIT_US 200000

#define AM_HAL_PWRCTRL_MEMPWRSTATUS_MASK    ( PWRCTRL_MEMPWRSTATUS_PWRSTTCM_Msk        |    \
                                              PWRCTRL_MEMPWRSTATUS_PWRSTNVM0_Msk       |    \
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
bool     g_bMcuSpotmgrInitSuccess = false;

//*****************************************************************************
// Temporarily suppress variable set but not used warnings
//*****************************************************************************
DIAG_SUPPRESS_SETNOTUSED_VAR

uint32_t g_orig_TVRGCVREFTRIM               = 0;
uint32_t g_orig_TVRGFVREFTRIM               = 0;
uint32_t g_orig_VDDCLKGTRIM                 = 0;
uint32_t g_orig_CORELDOTEMPCOTRIM           = 0;
uint32_t g_orig_CORELDOACTIVETRIM           = 0;
static uint32_t g_orig_D2ASPARE             = 0;
static uint32_t g_orig_MEMLDOACTIVETRIM     = 0;
DIAG_SUPPRESS_SETNOTUSED_VAR_DEFAULT
//! @}

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
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENI3C, PWRCTRL_DEVPWREN_PWRENI3C_EN))

#define PWRCTRL_HCPA_DEVPWREN_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART0, PWRCTRL_DEVPWREN_PWRENUART0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRENUART1, PWRCTRL_DEVPWREN_PWRENUART1_EN))

#define PWRCTRL_IOS_DEVPWREN_MASK       ( \
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
    PWRCTRL_DEVPWRSTATUS_PWRSTI3C_Msk)

#define PWRCTRL_HCPA_DEVPWRSTATUS_MASK          ( \
    PWRCTRL_DEVPWRSTATUS_PWRSTUART0_Msk | \
    PWRCTRL_DEVPWRSTATUS_PWRSTUART1_Msk)

#define PWRCTRL_IOS_DEVPWRSTATUS_MASK      ( \
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
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENI3CPHY, PWRCTRL_DEVPWREN_PWRENI3CPHY_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTI3CPHY_Msk
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
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENI3C, PWRCTRL_DEVPWREN_PWRENI3C_EN),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_HCPC_DEVPWRSTATUS_MASK
    },
    {
        AM_REGADDR(PWRCTRL, DEVPWREN),
        _VAL2FLD(PWRCTRL_DEVPWREN_PWRENNETAOL, PWRCTRL_DEVPWREN_PWRENNETAOL_ON),
        AM_REGADDR(PWRCTRL, DEVPWRSTATUS),
        PWRCTRL_DEVPWRSTATUS_PWRSTNETAOL_Msk
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
        PWRCTRL_AUDSSPWRSTATUS_PWRSTI2S0_Msk
    },
};

//*****************************************************************************
//
//! @brief  Return the pwr_ctrl entry for a given ePeripheral.
//!         This function does not contain the entry for SYSPLL, please use
//!         am_hal_pwrctrl_syspll_enable, am_hal_pwrctrl_syspll_disable and
//!         am_hal_pwrctrl_syspll_enabled for SYSPLL power control and status
//!         check.
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
//! @brief  Return the pwr_ctrl entry for a given ePeripheral.
//!         This function does not contain the entry for SYSPLL, please use
//!         am_hal_pwrctrl_syspll_enable, am_hal_pwrctrl_syspll_disable and
//!         am_hal_pwrctrl_syspll_enabled for SYSPLL power control and status
//!         check.
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
        shift_pos = (ePeripheral - AM_HAL_PWRCTRL_PERIPH_IOM0) + 1;
        if (ePeripheral > AM_HAL_PWRCTRL_PERIPH_I3CPHY)
        {
            shift_pos += 1;
        }
        if (ePeripheral > AM_HAL_PWRCTRL_PERIPH_UART1)
        {
            shift_pos += 2;
        }
        if (ePeripheral > AM_HAL_PWRCTRL_PERIPH_MSPI2)
        {
            shift_pos += 1;
        }
        pwr_ctrl->ui32PwrEnRegAddr = AM_REGADDR(PWRCTRL, DEVPWREN);
        pwr_ctrl->ui32PwrStatReqAddr = AM_REGADDR(PWRCTRL, DEVPWRSTATUS);
        pwr_ctrl->ui32PeriphEnable = 1 << shift_pos;
        if ((ePeripheral >= AM_HAL_PWRCTRL_PERIPH_IOM0) && (ePeripheral <= AM_HAL_PWRCTRL_PERIPH_IOM3))
        {
            pwr_ctrl->ui32PeriphStatus = PWRCTRL_HCPB_DEVPWRSTATUS_MASK;
        }
        else if (((ePeripheral >= AM_HAL_PWRCTRL_PERIPH_IOM4) && (ePeripheral <= AM_HAL_PWRCTRL_PERIPH_IOM5)) || (ePeripheral == AM_HAL_PWRCTRL_PERIPH_I3C))
        {
            pwr_ctrl->ui32PeriphStatus = PWRCTRL_HCPC_DEVPWRSTATUS_MASK;
        }
        else if ((ePeripheral >= AM_HAL_PWRCTRL_PERIPH_UART0) && (ePeripheral <= AM_HAL_PWRCTRL_PERIPH_UART1))
        {
            pwr_ctrl->ui32PeriphStatus = PWRCTRL_HCPA_DEVPWRSTATUS_MASK;
        }
        else if ((ePeripheral >= AM_HAL_PWRCTRL_PERIPH_IOSFD0) && (ePeripheral <= AM_HAL_PWRCTRL_PERIPH_IOSFD1))
        {
            pwr_ctrl->ui32PeriphStatus = PWRCTRL_IOS_DEVPWRSTATUS_MASK;
        }
        else
        {
            pwr_ctrl->ui32PeriphStatus = 1 << shift_pos;
        }
    }
    else
    {
        shift_pos = (ePeripheral - AM_HAL_PWRCTRL_PERIPH_PDM0) + 2;
        if (ePeripheral > AM_HAL_PWRCTRL_PERIPH_PDM0)
        {
            shift_pos += 3;
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
const am_hal_pwrctrl_mcu_memory_config_t    g_DefaultMcuMemCfg =
{
    .eROMMode           = AM_HAL_PWRCTRL_ROM_AUTO,
    .eDTCMCfg           = AM_HAL_PWRCTRL_DTCM256K,
    .eRetainDTCM        = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN,
    .eNVMCfg            = AM_HAL_PWRCTRL_NVM,
    .eNVMPowerDown      = AM_HAL_PWRCTRL_NVM_DEEP_POWER_DOWN,
    .bKeepNVMOnInDeepSleep         = false,
};

const am_hal_pwrctrl_sram_memcfg_t          g_DefaultSRAMCfg =
{
    .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_1P75M,
    .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_NONE,
    .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_NONE,
    .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_1P75M
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
//  Wait for CM4 IPC init event
//
// ****************************************************************************
static uint32_t
am_hal_pwrctrl_rss_mbox_init_wait()
{
    uint32_t ui32Status = AM_HAL_STATUS_TIMEOUT;
    uint32_t ui32LoopCnt = AM_HAL_PWRCTRL_MAX_WAIT_CM4_MBOX_INIT_US / 10;

    for (uint32_t ui32Loop = 0; ui32Loop < ui32LoopCnt; ui32Loop++)
    {
        if (__NVIC_GetPendingIRQ(IPC_ERR_IRQn))
        {
            if (AM_HAL_IPC_MBOX_ERROR_IPCINIT)
            {
                ui32Status = AM_HAL_STATUS_SUCCESS;
                break;
            }
        }
        am_hal_delay_us(10);
    }

    //
    // Initialize MBOX HAL
    //
    if (ui32Status == AM_HAL_STATUS_SUCCESS)
    {
        am_hal_ipc_mbox_init_state_set(AM_HAL_IPC_MBOX_INIT_STATE_IPCINIT_RECEIVED);
        ui32Status = am_hal_ipc_mbox_init();
    }

    //
    // Enable MBOX IRQ
    //
    #ifdef AM_IRQ_PRIORITY_DEFAULT
    NVIC_SetPriority(IPC_PEND_MSG_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_SetPriority(IPC_ERR_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    #endif // AM_IRQ_PRIORITY_DEFAULT
    NVIC_EnableIRQ(IPC_PEND_MSG_IRQn);
    NVIC_EnableIRQ(IPC_ERR_IRQn);

    return ui32Status;
} // am_hal_pwrctrl_rss_mbox_init_wait()

// ****************************************************************************
//
//  Start the RSS Power Up sequence
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_rss_bootup(void)
{
    uint32_t ui32Status;

    if ( (PWRCTRL->DEVPWRSTATUS_b.PWRSTNETAOL == PWRCTRL_DEVPWRSTATUS_PWRSTNETAOL_ON)
        && (am_hal_ipc_mbox_init_state_get() != AM_HAL_IPC_MBOX_INIT_STATE_NOT_READY))
    {
        ui32Status = AM_HAL_STATUS_IN_USE;
    }
    else
    {

        MCUCTRL->BODCTRL_b.BODRFPWD = 0;
        am_hal_delay_us(1);
        MCUCTRL->BODCTRL_b.BODRFPWD = 1;

        //
        // Turn on RSS power
        //
        ui32Status = am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_NETAOL);
        if ( ui32Status != AM_HAL_STATUS_SUCCESS )
        {
            return ui32Status;
        }

        //
        // Trigger RSS Wakeup
        //
        ui32Status = am_hal_pwrctrl_cm4_wakeup_req();
        if ( ui32Status != AM_HAL_STATUS_SUCCESS )
        {
            return ui32Status;
        }

        //
        // Wait for Radio Subsystem MBOX init
        //
        ui32Status = am_hal_pwrctrl_rss_mbox_init_wait();
        if ( ui32Status != AM_HAL_STATUS_SUCCESS )
        {
            return ui32Status;
        }

        //
        // Send the RFXTAL configuration to radio subsystem via mailbox
        //
        ui32Status = am_hal_clkmgr_private_rfxtal_config_send();
    }

    return ui32Status;
} // am_hal_pwrctrl_rss_bootup()

// ****************************************************************************
//
// Power off the RSS
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_rss_pwroff(void)
{
    uint32_t ui32Status;

    if ( PWRCTRL->DEVPWRSTATUS_b.PWRSTNETAOL == PWRCTRL_DEVPWRSTATUS_PWRSTNETAOL_ON )
    {
        //
        // Mailbox cannot be accessed now
        //
        am_hal_ipc_mbox_init_state_set(AM_HAL_IPC_MBOX_INIT_STATE_NOT_READY);
        //
        // Power off RSS
        //
        ui32Status = am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_NETAOL);

        //
        // Disable MBOX IRQ
        //
        NVIC_DisableIRQ(IPC_PEND_MSG_IRQn);
        NVIC_DisableIRQ(IPC_ERR_IRQn);
        NVIC_ClearPendingIRQ(IPC_PEND_MSG_IRQn);
        NVIC_ClearPendingIRQ(IPC_ERR_IRQn);
    }
    else
    {
        ui32Status = AM_HAL_STATUS_SUCCESS;
    }

    return ui32Status;
} // am_hal_pwrctrl_rss_pwroff()

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
    if ( (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1) ||
         (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2) )
    {
        //
        // Report CPU state change
        //
        am_hal_spotmgr_cpu_state_e eCpuSt;
        if (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1)
        {
            eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_HP1;
        }
        else
        {
            eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_HP2;
        }
        ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_CPU_STATE, false, (void *) &eCpuSt);
        if (AM_HAL_STATUS_SUCCESS == ui32Status)
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
        if ( (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1) &&
             (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2) )
        {
            //
            // Report CPU state change
            //
            am_hal_spotmgr_cpu_state_e eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_LP;
            ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_CPU_STATE, false, (void *) &eCpuSt);
        }
    }
    else
    {
        if ( (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1) ||
             (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2) )
        {
            //
            // Need to revert the power state
            //
            am_hal_spotmgr_cpu_state_e eCpuSt = AM_HAL_SPOTMGR_CPUSTATE_ACTIVE_LP;
            am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_CPU_STATE, false, (void *) &eCpuSt); // ui32Status is FAIL already, do not update ui32Status with the return value of am_hal_spotmgr_power_state_update.
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
         (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1) &&
         (ePowerMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    //
    // We must be using SIMOBUCK in order to go to HP mode.
    //
    if ( ( (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1) ||
           (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2) ) &&
         (PWRCTRL->VRSTATUS_b.SIMOBUCKST != PWRCTRL_VRSTATUS_SIMOBUCKST_ACT) )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Check if the SKU supports HP1 or HP2
    //
    if ( ( (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1) &&
           (MCUCTRL->SKU_b.SKUTURBOSPOT < MCUCTRL_SKU_SKUTURBOSPOT_HP1) ) ||
         ( (ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2) &&
           (MCUCTRL->SKU_b.SKUTURBOSPOT < MCUCTRL_SKU_SKUTURBOSPOT_HP2) ) )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }


    //
    // Check if the request mode is already selected.  If so, return SUCCESS.
    //
    if ( ePowerMode == g_eCurMcuPwrMode )
    {
        return AM_HAL_STATUS_SUCCESS;
    }

    if ( ePowerMode == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2)
    {
        uint32_t ui32PLLVCOFreq = 0;
        am_hal_clkmgr_clock_config_get(AM_HAL_CLKMGR_CLK_ID_PLLVCO, &ui32PLLVCOFreq, NULL);
        if ( ui32PLLVCOFreq < 192000000 )
        {
            //
            // We require that SW NOT ask for HP2 if it has set VCO < 192Mhz
            //
            return AM_HAL_STATUS_FAIL;
        }
        ui32Status = am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_PLLVCO, AM_HAL_CLKMGR_USER_ID_CPU);
        if ( ui32Status )
        {
            return ui32Status;
        }
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
        am_hal_pwrctrl_mcu_mode_e ePWrMode = g_eCurMcuPwrMode;
        g_eCurMcuPwrMode = ePowerMode;
        if (ePWrMode != AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2)
        {
            return am_hal_clkmgr_clock_release(AM_HAL_CLKMGR_CLK_ID_PLLVCO, AM_HAL_CLKMGR_USER_ID_CPU);
        }

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
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM, 0);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM0, 0);
            break;
        case AM_HAL_PWRCTRL_NVM:
            //
            // Power up NVM0, power down NVM1.
            //
            ui32PwrEn |= _VAL2FLD(PWRCTRL_MEMPWREN_PWRENNVM, 1);
            ui32PwrStatus |= _VAL2FLD(PWRCTRL_MEMPWRSTATUS_PWRSTNVM0, 1);
            break;
    }

    //
    // Configure the Non-Volatile Memory Power Down.
    //
    PWRCTRL->MEMRETCFG_b.NVMPWDCFG = psConfig->eNVMPowerDown;


    DIAG_SUPPRESS_VOLATILE_ORDER()

    //
    // Special logic needed, when powering on NVM1 by itself
    // Need to force AXI Clock during the transition
    // If powering on both banks "at same time", no special processing needed
    //
    if ((psConfig->eNVMCfg == AM_HAL_PWRCTRL_NVM) &&
        ((PWRCTRL->MEMPWRSTATUS & PWRCTRL_MEMPWRSTATUS_PWRSTNVM0_Msk) == PWRCTRL_MEMPWRSTATUS_PWRSTNVM0_Msk))
    {
        MCUCTRL->FORCEAXICLKEN_b.FRCAXICLKEN = 1;
        bForceAxiClkEn = true;
    }

    if (ui32PwrStatus != PWRCTRL->MEMPWRSTATUS)
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
            //
            // Report MEM power status
            //
            ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_MEMPWR, true, (void *) &ui32PwrStatus);
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
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
        PWRCTRL->MEMRETCFG_b.TCMPWDSLP = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN;
    }
    else if ( psConfig->eRetainDTCM == AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN )
    {
        PWRCTRL->MEMRETCFG_b.TCMPWDSLP = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN;
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
        (am_hal_pwrctrl_nvm_select_e)(PWRCTRL->MEMPWREN_b.PWRENNVM);

    //
    // Configure the Non-Volatile Memory retention.
    //
    psConfig->bKeepNVMOnInDeepSleep =
        (PWRCTRL->MEMRETCFG_b.NVMPWDSLP == PWRCTRL_MEMRETCFG_NVMPWDSLP_DIS);

    //
    // Configure the MCU Tightly Coupled Memory retention.
    //
    if (PWRCTRL->MEMRETCFG_b.TCMPWDSLP == AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN)
    {
        psConfig->eRetainDTCM = AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_RETAIN;
    }
    else if (PWRCTRL->MEMRETCFG_b.TCMPWDSLP == AM_HAL_PWRCTRL_MEMRETCFG_TCMPWDSLP_NORETAIN)
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
    uint32_t count;
    uint32_t ui32Status;

    if ( AM_HAL_PWRCTRL_ROM_AUTO == g_eCurROMPwrMode )
    {
        //
        // Report rom power change
        //
        uint32_t ui32MemSt = PWRCTRL_MEMPWRSTATUS_PWRSTROM_Msk | PWRCTRL->MEMPWRSTATUS;
        ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_MEMPWR, true, (void *) &ui32MemSt);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
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
    uint32_t ui32Count;
    uint32_t ui32Status;

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
        uint32_t ui32MemSt = PWRCTRL->MEMPWRSTATUS;
        ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_MEMPWR, false, (void *) &ui32MemSt);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
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
        //Check if turning on more memory.
        //
        if ( ((psConfig->eSRAMCfg > PWRCTRL->SSRAMPWRST_b.SSRAMPWRST) &&
             !((psConfig->eSRAMCfg == 2) && (PWRCTRL->SSRAMPWRST_b.SSRAMPWRST == 1))) ||
             (((psConfig->eSRAMCfg == 1) && (PWRCTRL->SSRAMPWRST_b.SSRAMPWRST == 2))) )
        {
            //
            // Report SSRAM power change
            //
            ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_SSRAMPWR, true, (void *) &(psConfig->eSRAMCfg));
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
            }
            bUpdateLater = false;
        }
        else
        {
            bUpdateLater = true;
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

        if (bUpdateLater)
        {
            //
            // Report SSRAM power change
            //
            ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_SSRAMPWR, false, NULL);
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
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
        case AM_HAL_PWRCTRL_SRAM_1M:       // Retain lower 1M, pwr dwn upper 0.75M
            PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP = PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP1;
            break;
        case AM_HAL_PWRCTRL_SRAM_0P75M:    // Retain upper 0.75M, pwr dwn lower 1M
            PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP = PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP0;
            break;
        case AM_HAL_PWRCTRL_SRAM_1P75M:    // Retain all SSRAM, power down none
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
        psConfig->eSRAMRetain = AM_HAL_PWRCTRL_SRAM_1P75M;
    }
    else if (PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP == PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP0)
    {
        psConfig->eSRAMRetain = AM_HAL_PWRCTRL_SRAM_0P75M;
    }
    else if (PWRCTRL->SSRAMRETCFG_b.SSRAMPWDSLP == PWRCTRL_SSRAMRETCFG_SSRAMPWDSLP_GROUP1)
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
//  am_hal_pwrctrl_periph_enable()
//  Enable power for a peripheral.
//  This function does not support SYSPLL power control, please use
//  am_hal_pwrctrl_syspll_enable and am_hal_pwrctrl_syspll_disable for
//  SYSPLL power control.
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

    //
    // For GPU enable, execute GPU power on sequence
    //
    if (ePeripheral == AM_HAL_PWRCTRL_PERIPH_GFX)
    {
        am_hal_spotmgr_gpu_state_e eGpuSt;

        am_hal_clkmgr_clock_request(AM_HAL_CLKMGR_CLK_ID_HFRC, AM_HAL_CLKMGR_USER_ID_GFX);

        //
        // Report GPU state change to SPOTmanager
        //
        eGpuSt = AM_HAL_SPOTMGR_GPUSTATE_ACTIVE;
        ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_GPU_STATE, true, (void *) &eGpuSt);
        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return ui32Status;
        }
    }
    else
    {
        //
        // Update power state
        //
        if ( (pwr_ctrl.ui32PeriphEnable & DEVPWRST_MONITOR_PERIPH_MASK) && (ePeripheral < AM_HAL_PWRCTRL_PERIPH_PDM0) )
        {
            ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_DEVPWR, true, (void *) &(pwr_ctrl.ui32PeriphStatus));
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
            }
        }
        if ( (pwr_ctrl.ui32PeriphEnable & AUDSSPWRST_MONITOR_PERIPH_MASK) && (ePeripheral >= AM_HAL_PWRCTRL_PERIPH_PDM0) )
        {
            ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_AUDSSPWR, true, (void *) &(pwr_ctrl.ui32PeriphStatus));
            if (AM_HAL_STATUS_SUCCESS != ui32Status)
            {
                return ui32Status;
            }
        }
    }

    AM_CRITICAL_BEGIN

    //
    // Enable power control for the given device.
    //
    AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) |= pwr_ctrl.ui32PeriphEnable;
    AM_CRITICAL_END

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
//      will fail when more than one enable in the same domain is set and the
//      user tries disable only one.
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
//  This function does not support SYSPLL power control, please use
//  am_hal_pwrctrl_syspll_enable and am_hal_pwrctrl_syspll_disable for
//  SYSPLL power control.
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
        if (PWRCTRL->DEVPWRSTATUS_b.PWRSTCRYPTO)
        {
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

    //
    // Disable power domain for the given device.
    //
    AM_CRITICAL_BEGIN
    AM_REGVAL(pwr_ctrl.ui32PwrEnRegAddr) &= ~pwr_ctrl.ui32PeriphEnable;
    AM_CRITICAL_END

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
                am_hal_spotmgr_gpu_state_e eGpuSt = AM_HAL_SPOTMGR_GPUSTATE_OFF;

                //
                // Report GPU state change to SPOTmanager
                //
                ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_GPU_STATE, true, (void *) &eGpuSt);
                if (AM_HAL_STATUS_SUCCESS != ui32Status)
                {
                    return ui32Status;
                }

                //
                // Release GPU clocks to clock manager
                //
                am_hal_clkmgr_clock_release_all(AM_HAL_CLKMGR_USER_ID_GFX);
            }
            else
            {
                //
                // Update power state
                //
                if ((ePeripheral < AM_HAL_PWRCTRL_PERIPH_PDM0) && (pwr_ctrl.ui32PeriphEnable & DEVPWRST_MONITOR_PERIPH_MASK))
                {
                    ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_DEVPWR, false, (void *) &(pwr_ctrl.ui32PeriphStatus));
                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        return ui32Status;
                    }
                }
                if ((ePeripheral >= AM_HAL_PWRCTRL_PERIPH_PDM0) && (pwr_ctrl.ui32PeriphEnable & AUDSSPWRST_MONITOR_PERIPH_MASK))
                {
                    ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_AUDSSPWR, false, (void *) &(pwr_ctrl.ui32PeriphStatus));
                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        return ui32Status;
                    }
                }
            }
        }
    }

    return ui32Status;

} // am_hal_pwrctrl_periph_disable()

// ****************************************************************************
//
//  am_hal_pwrctrl_periph_enabled()
//  Determine whether a peripheral is currently enabled.
//  This function does not support SYSPLL power status check, please use
//  am_hal_pwrctrl_syspll_enabled to check SYSPLL power status.
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
    // To optimize power, we'll capture the INFO1 registers of interest here
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
//  am_hal_pwrctrl_mcu_and_spotmgr_init()
//  Initialize MCU and Spotmgr
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_mcu_and_spotmgr_init(void)
{
    uint32_t ui32Ret;
    uint32_t ui32TrimVer = 0;
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
    // Initialise SPOT manager
    //
    ui32Ret = am_hal_spotmgr_init();
    if (ui32Ret == AM_HAL_STATUS_SUCCESS)
    {
        g_bMcuSpotmgrInitSuccess = true;
    }
    return ui32Ret;
}

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
    uint32_t ui32RegVal = 0;

    //
    // Initialize Spotmgr and MCU
    //
    if (!g_bMcuSpotmgrInitSuccess)
    {
        am_hal_pwrctrl_mcu_and_spotmgr_init();
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
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENGFX, 1)           |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENUSB, 1)           |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENSDIO, 1)          |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENCRYPTO, 1)        |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENI2S0, 1)          |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENI2S0REFCLK, 1)    |
                  _VAL2FLD(CLKGEN_MISC_PWRONCLKENUSBREFCLK, 1);
    ui32RegVal &= ~_VAL2FLD(CLKGEN_MISC_AXIXACLKENOVRRIDE, 1);
    CLKGEN->MISC = ui32RegVal;

    //
    // Additional required settings
    //
    //
    CLKGEN->CLKCTRL = 0x0; // Disable all unneccesary clocks including display controller clock


    //
    // Store the factory values for various trims.
    //
    if ( g_bOrigTrimsStored == false )
    {
        g_orig_MEMLDOACTIVETRIM     = MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM;
        g_orig_TVRGCVREFTRIM        = MCUCTRL->VREFGEN2_b.TVRGCVREFTRIM;
        g_orig_TVRGFVREFTRIM        = MCUCTRL->VREFGEN4_b.TVRGFVREFTRIM;
        g_orig_CORELDOTEMPCOTRIM    = MCUCTRL->LDOREG1_b.CORELDOTEMPCOTRIM;
        g_orig_CORELDOACTIVETRIM    = MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM;
        g_orig_D2ASPARE             = MCUCTRL->D2ASPARE;
        g_bOrigTrimsStored          = true;
    }

    //
    // Set SSRAM/DTCM/NVM retain comes from VDDS
    //
    MCUCTRL->PWRSW0_b.PWRSWVDDRCPUSTATSEL = 0x1;
    MCUCTRL->PWRSW1_b.PWRSWVDDRMSTATSEL   = 0x1;
    MCUCTRL->PWRSW1_b.PWRSWVDDRLSTATSEL   = 0x1;


#if AM_HAL_PWRCTRL_SIMOLP_AUTOSWITCH
    am_hal_spotmgr_simobuck_lp_autosw_init();
#endif

    //
    // Set it here to disable autowakeup to reduce MRAM wakeup latency
    //
    PWRCTRL->MRAMEXTCTRL_b.MRAMAUTOWAKEUPCTRL = 0x1;

    //
    // Always initialize CRM
    //
    am_hal_crm_initialize();

    //
    // Initialize Clock Manager
    //
    am_hal_clkmgr_initialize();

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
                //
                // For A0 trimver2 and newer trims, and silicons newer than A0,
                // initialise power trims before enabling simobuck.
                //
                if (g_bIsTrimver2OrNewer)
                {
                    ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_INIT_STATE, false, NULL);
                    if (AM_HAL_STATUS_SUCCESS != ui32Status)
                    {
                        return ui32Status;
                    }
                }
                //
                // Set hfrcsimobucken to 1
                //
                MCUCTRL->HFRC_b.HFRCSIMOBUCKCLKEN = 1;
                //
                // Enable SIMOBUCK compensations
                // MCUCTRL->SIMOBUCK0 = 0x0007FFBF;
                //
                MCUCTRL->SIMOBUCK0 =
                    MCUCTRL_SIMOBUCK0_VDDCRXCOMPEN_Msk   |
                    MCUCTRL_SIMOBUCK0_VDDFRXCOMPEN_Msk   |
                    MCUCTRL_SIMOBUCK0_VDDSRXCOMPEN_Msk   |
                    MCUCTRL_SIMOBUCK0_VDDCLVRXCOMPEN_Msk |
                    MCUCTRL_SIMOBUCK0_VDDRFRXCOMPEN_Msk  |
                    MCUCTRL_SIMOBUCK0_CCMEN_Msk          |
                    MCUCTRL_SIMOBUCK0_PMOSDRVSTRTRIM_Msk |
                    MCUCTRL_SIMOBUCK0_NMOSDRVSTRTRIM_Msk |
                    MCUCTRL_SIMOBUCK0_TPNDELAYTRIM_Msk   |
                    MCUCTRL_SIMOBUCK0_TNPDELAYTRIM_Msk;
                //
                // Enable the SIMOBUCK
                //
                am_hal_spotmgr_simobuck_init_bfr_enable();
                PWRCTRL->VRCTRL_b.SIMOBUCKEN = 1;
                buck_ldo_override_init();
                am_hal_spotmgr_simobuck_init_aft_enable();
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
                uint32_t ui32AllDisabledStatus = 0;
                ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_DEVPWR, false, &ui32AllDisabledStatus);
                if (AM_HAL_STATUS_SUCCESS != ui32Status)
                {
                    return ui32Status;
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
                uint32_t ui32AllDisabledStatus = 0;
                ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_AUDSSPWR, false, &ui32AllDisabledStatus);
                if (AM_HAL_STATUS_SUCCESS != ui32Status)
                {
                    return ui32Status;
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
// Important:
//
// - This function must be called before
//   transition to a new application, such as the case of a secondary bootloader
//   transitioning to an application. Before calling this function, users
//   should switch CPU to LP if it is in HP mode, and turned off GPU and all other
//   peripherals if anyone was turned on, except OTP.
//   Before calling this function, in order to release all clocks when exiting
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
    //
    // Check CPU status, if it is HP, return failure.
    //
    if (( PWRCTRL->MCUPERFREQ_b.MCUPERFSTATUS == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE1 ) ||
        ( PWRCTRL->MCUPERFREQ_b.MCUPERFSTATUS == AM_HAL_PWRCTRL_MCU_MODE_HIGH_PERFORMANCE2 ))
    {
        return AM_HAL_STATUS_FAIL;
    }

    //
    // Reset power status to default except the temperature, let SPOT manager
    // determine the power state according to the actual temperature.
    //
    return am_hal_spotmgr_default_reset();
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
//  am_hal_pwrctrl_temp_update()
//  fCurTemp - Current temperature in Celsius.
//  psTempThresh - This API returns this structure for notifying of thresholds
//                 when application should make this call again (When temp goes
//                 lower than fLowThresh or rises above fHighThresh).
//
//  If current temperature is higher than BUCK_LP_TEMP_THRESHOLD(e.g. 50c),
//  bFrcBuckAct must be set to true. Otherwise, set bFrcBuckAct to false.
//
//  Note: For Apollo330_510L trimver 2 or newer versions, it is not needed to
//  report temperature.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_temp_update(float fCurTemp, am_hal_pwrctrl_temp_thresh_t * psTempThresh)
{
    uint32_t ui32Status;
    am_hal_spotmgr_tempco_param_t sTempCo;
    sTempCo.fTemperature = fCurTemp;

    if (!g_bMcuSpotmgrInitSuccess)
    {
        //
        // Initialize Spotmgr and MCU
        //
        ui32Status = am_hal_pwrctrl_mcu_and_spotmgr_init();
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }
    }

    //
    // Update SPOTmanager on the temperature
    //
    ui32Status = am_hal_spotmgr_power_state_update(AM_HAL_SPOTMGR_STIM_TEMP, false, (void *) &sTempCo);

    if (ui32Status == AM_HAL_STATUS_SUCCESS)
    {
        psTempThresh->fLowThresh = sTempCo.fRangeLower;
        psTempThresh->fHighThresh = sTempCo.fRangeHigher;
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        psTempThresh->fLowThresh = 0.0f;
        psTempThresh->fHighThresh = 0.0f;
        return AM_HAL_STATUS_FAIL;
    }
}

// ****************************************************************************
//
//  am_hal_pwrctrl_wait_pll_lock_for_hp2()
//
// ****************************************************************************
void
am_hal_pwrctrl_wait_pll_lock_for_hp2(bool bWaitPllockForHp2)
{
    //
    // wait for hp2 or not.
    //
    PWRCTRL->MCUPERFREQ_b.WAITFORHP2 = bWaitPllockForHp2;
} // am_hal_pwrctrl_wait_pll_lock_for_hp2()

//*****************************************************************************
//
// Enable power to system PLL.
//
//*****************************************************************************
uint32_t
am_hal_pwrctrl_syspll_enable()
{
    AM_CRITICAL_BEGIN

    MCUCTRL->PLLCTL0_b.SYSPLLVDDFPDN = MCUCTRL_PLLCTL0_SYSPLLVDDFPDN_ENABLE;
    MCUCTRL->PLLCTL0_b.SYSPLLVDDHPDN = MCUCTRL_PLLCTL0_SYSPLLVDDHPDN_ENABLE;

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

    MCUCTRL->PLLCTL0_b.SYSPLLVDDFPDN = MCUCTRL_PLLCTL0_SYSPLLVDDFPDN_DISABLE;
    MCUCTRL->PLLCTL0_b.SYSPLLVDDHPDN = MCUCTRL_PLLCTL0_SYSPLLVDDHPDN_DISABLE;

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
    *bEnabled = (PWRCTRL->DEVPWRSTATUS_b.PWRSTPLL == PWRCTRL_DEVPWRSTATUS_PWRSTPLL_ON);

    return AM_HAL_STATUS_SUCCESS;
}

// ****************************************************************************
//
//  Get the CM4 power state.
//
// ****************************************************************************
uint32_t am_hal_pwrctrl_get_cm4_pwrstate(am_hal_pwrctrl_cm4_pwr_state_e * pCm4pwrstate)
 {
    *pCm4pwrstate = PWRCTRL->CM4PWRSTATE_b.CM4PWRSTATUS;

    return AM_HAL_STATUS_SUCCESS;
 }

// ****************************************************************************
//
//  Wake up the CM4 .
//
// ****************************************************************************
uint32_t am_hal_pwrctrl_cm4_wakeup_req(void)
{
    uint32_t count;

    PWRCTRL->CM4PWRCTRL_b.CM4POWERONREQ = PWRCTRL_CM4PWRCTRL_CM4POWERONREQ_ON;

    //
    // Wait for Status
    //
    for (count = 0; count < AM_HAL_PWRCTRL_MAX_WAIT_CM4_WAKEUP_US; count++)
    {
        if (PWRCTRL->CM4PWRSTATE_b.CM4PWRSTATUS !=
            PWRCTRL_CM4PWRSTATE_CM4PWRSTATUS_OFF)
        {
            break;
        }
        am_hal_delay_us(1);
        PWRCTRL->CM4PWRCTRL_b.CM4POWERONREQ = PWRCTRL_CM4PWRCTRL_CM4POWERONREQ_ON;
    }

    //
    // Check for timeout.
    //
    if (count == AM_HAL_PWRCTRL_MAX_WAIT_CM4_WAKEUP_US)
    {
        return AM_HAL_STATUS_TIMEOUT;
    }

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_cm4_wakeup_req()


//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
