//*****************************************************************************
//
//! @file am_hal_burst.c
//!
//! @brief Functions for Controlling Burst Mode Operation.
//!
//! @addtogroup burstmode3p Burst - Burst Mode Functionality
//! @ingroup apollo3p_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"


#if AM_HAL_BURST_LDO_WORKAROUND
//
//! @name Series of defines for the LDO Burst Workaround
//!
//! @{
//
#define DELAY_VDDBH_CHARGE_US           10000
#define DELAY_BURSTLDO_ACT_US           2
#define DELAY_VDDS_CHARGE_US            2000
#define DELAY_BLEBUCK_ON_US             10
#define DELAY_POWER_DOMAIN_SWITCH_US    1
#define DELAY_BLEBUCK_OVER_REMOVAL_US   25
//
//! @}
//
#endif // AM_HAL_BURST_LDO_WORKAROUND

//
//! Globals.
//
bool    g_am_hal_burst_mode_available = false;
#if AM_HAL_BURST_LDO_WORKAROUND
static bool g_bEnFromManual = false;
static bool g_bDisFromManual = false;
#endif // AM_HAL_BURST_LDO_WORKAROUND


// ****************************************************************************
//
//  am_hal_burst_mode_initialize()
//  Burst mode initialization function
//
// ****************************************************************************
uint32_t
am_hal_burst_mode_initialize(am_hal_burst_avail_e *peBurstAvail)
{
    uint32_t    ui32Status;

#if AM_HAL_BURST_LDO_WORKAROUND
    //
    // Global VDDF adjustment for VDDF+30mV
    // Make sure there is no other place in the project VDDF is adjusted!
    // Make sure SIMOBUCK/LDO mode is already selected before calling this function!
    // Make sure info1 patch5 is applied, otherwise this action will not work!
    //
    if (PWRCTRL->MISC_b.SIMOBUCKEN == 1)
    {
        am_hal_pwrctrl_simobuck_vddx_active_trim_adj_default(AM_HAL_BURST_VDDF, AM_HAL_BURST_BUCK_VDDF_ADJ_CODE_30MV);
    }
    else
    {
        am_hal_pwrctrl_ldo_vddx_active_trim_adj_default(AM_HAL_BURST_VDDF, AM_HAL_BURST_LDO_VDDF_ADJ_CODE_30MV);
    }
#endif // AM_HAL_BURST_LDO_WORKAROUND

    //
    // Check if the Burst Mode feature is available based on the SKU.
    //
    if ( 0 == MCUCTRL->SKU_b.ALLOWBURST )
    {
        //
        // Burst mode is not available.
        //
        g_am_hal_burst_mode_available = false;
        *peBurstAvail = AM_HAL_BURST_NOTAVAIL;
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    //
    // Enable the Burst Feature Event (DEVPWREVENTEN).
    //
    PWRCTRL->DEVPWREVENTEN_b.BURSTEVEN = 1;

    //
    // Check if CLKGEN->FREQCTRL_b.BURSTREQ is already set.
    //
    if ( CLKGEN->FREQCTRL_b.BURSTREQ == 1 )
    {
        //
        // Burst mode is already enabled, we need to turn it off first
        //
        AM_CRITICAL_BEGIN
        am_hal_flash_store_ui32((uint32_t*)&CLKGEN->FREQCTRL, CLKGEN_FREQCTRL_BURSTREQ_DIS);
        AM_CRITICAL_END
        ui32Status = am_hal_flash_delay_status_check(10000,
                        (uint32_t)&CLKGEN->FREQCTRL,
                        CLKGEN_FREQCTRL_BURSTSTATUS_Msk,
                        0,
                        true);
        //
        // Something went wrong, return the error.
        //
        if ( ui32Status != AM_HAL_STATUS_SUCCESS )
        {
            return ui32Status;
        }
    }

    //
    // Enable the Burst Functionality (FEATUREENABLE).
    //
    MCUCTRL->FEATUREENABLE_b.BURSTREQ = 1;

    ui32Status = am_hal_flash_delay_status_check(10000,
                            (uint32_t)&MCUCTRL->FEATUREENABLE,
                            MCUCTRL_FEATUREENABLE_BURSTACK_Msk,
                            MCUCTRL_FEATUREENABLE_BURSTACK_Msk,
                            true);

    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        g_am_hal_burst_mode_available = false;
        *peBurstAvail = AM_HAL_BURST_NOTAVAIL;
        return ui32Status;
    }

    if ( 0 == MCUCTRL->FEATUREENABLE_b.BURSTAVAIL )
    {
        //
        // Burst mode is not available.
        //
        g_am_hal_burst_mode_available = false;
        *peBurstAvail = AM_HAL_BURST_NOTAVAIL;
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    //
    // Check the ACK for the Burst Functionality.
    //
    if ( MCUCTRL->FEATUREENABLE_b.BURSTACK == 0 )
    {
        //
        // If NACK, return status.
        //
        g_am_hal_burst_mode_available = false;
        *peBurstAvail = AM_HAL_BURST_NOTAVAIL;
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    //
    // Return Availability
    //
    g_am_hal_burst_mode_available = true;
    *peBurstAvail = AM_HAL_BURST_AVAIL;
    return AM_HAL_STATUS_SUCCESS;
}

// ****************************************************************************
//
//  am_hal_burst_mode_enable()
//  Burst mode enable function
//
// ****************************************************************************
uint32_t
am_hal_burst_mode_enable(am_hal_burst_mode_e *peBurstStatus)
{
    uint32_t    ui32Status;

#if AM_HAL_BURST_LDO_WORKAROUND
    //
    // Burst LDO into Active mode and delay before enabling 96MHz CLK
    //
    if ( !g_bEnFromManual )
    {
        //
        // First call the workaround function, then return. (In turn, the
        // workaround will set the flag and call this enable function.)
        //
        return am_hal_burst_mode_enable_w_manual_sequence(peBurstStatus);
    }
#endif // AM_HAL_BURST_LDO_WORKAROUND

    //
    // Check if Burst Mode is allowed and return status if it is not.
    //
    if (!g_am_hal_burst_mode_available)
    {
        *peBurstStatus = AM_HAL_NORMAL_MODE;
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    //
    // Request Burst Mode Enable (FREQCTRL)
    //
    CLKGEN->FREQCTRL_b.BURSTREQ = CLKGEN_FREQCTRL_BURSTREQ_EN;

    ui32Status = am_hal_flash_delay_status_check(10000,
                    (uint32_t)&CLKGEN->FREQCTRL,
                    CLKGEN_FREQCTRL_BURSTSTATUS_Msk,
                    CLKGEN_FREQCTRL_BURSTSTATUS_Msk,
                    true);

    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        *peBurstStatus = AM_HAL_NORMAL_MODE;
        return ui32Status;
    }

    //
    // Check that the Burst Request was ACK'd.
    //
    if ( 0 == CLKGEN->FREQCTRL_b.BURSTACK )
    {
        *peBurstStatus = AM_HAL_NORMAL_MODE;
        return AM_HAL_STATUS_FAIL;
    }

    //
    // Check the Burst Mode Status (FREQCTRL)
    //
    if ( CLKGEN->FREQCTRL_b.BURSTSTATUS > 0)
    {
        *peBurstStatus =  AM_HAL_BURST_MODE;
    }
    else
    {
        *peBurstStatus =  AM_HAL_NORMAL_MODE;
    }

    return AM_HAL_STATUS_SUCCESS;
}

// ****************************************************************************
//
//  am_hal_burst_mode_disable()
//  Burst mode disable function
//
// ****************************************************************************
uint32_t
am_hal_burst_mode_disable(am_hal_burst_mode_e *peBurstStatus)
{
    uint32_t    ui32Status;

#if AM_HAL_BURST_LDO_WORKAROUND
    //
    // Burst LDO into Active mode and delay before enabling 96MHz CLK
    //
    if ( !g_bDisFromManual )
    {
        //
        // First call the workaround function, then return. (In turn, the
        // workaround will set the flag and call this disable function.)
        //
        return am_hal_burst_mode_disable_w_manual_sequence(peBurstStatus);
    }
#endif // AM_HAL_BURST_LDO_WORKAROUND

    //
    // Request Burst Mode Enable (FREQCTRL)
    //
    //
    // Safely disable burst mode.
    //
    AM_CRITICAL_BEGIN
    am_hal_flash_store_ui32((uint32_t*)&CLKGEN->FREQCTRL, CLKGEN_FREQCTRL_BURSTREQ_DIS);
    AM_CRITICAL_END

    //
    // Disable the Burst Feature Event (DEVPWREVENTEN).
    //
    PWRCTRL->DEVPWREVENTEN_b.BURSTEVEN = 0;

    ui32Status = am_hal_flash_delay_status_check(10000,
                            (uint32_t)&CLKGEN->FREQCTRL,
                            CLKGEN_FREQCTRL_BURSTSTATUS_Msk,
                            0,
                            true);

    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        *peBurstStatus = AM_HAL_NORMAL_MODE;
        return ui32Status;
    }

    //
    // Check the Burst Mode Status (FREQCTRL)
    //
    if ( CLKGEN->FREQCTRL_b.BURSTSTATUS > 0 )
    {
        *peBurstStatus = AM_HAL_BURST_MODE;
    }
    else
    {
        *peBurstStatus = AM_HAL_NORMAL_MODE;
    }

    return AM_HAL_STATUS_SUCCESS;
}

#if AM_HAL_BURST_LDO_WORKAROUND
//****************************************************************************
//
//  am_hal_burst_ldo_patch_check()
//
//****************************************************************************
bool
am_hal_burst_ldo_patch_check(void)
{
    //
    // patch tracker bit3 for CV patch
    //
    if (AM_REGVAL(0x50023838) & 0x8)
    {
        //
        // cv patch not applied
        //
        return false;
    }

    //
    // cv patch applied
    //
    return true;
} // am_hal_burst_ldo_patch_check

//****************************************************************************
//
//  Burst LDO to charge VDDS cap
//
//****************************************************************************
void
am_hal_burst_ldo_charge(void)
{
    //
    // If patch is not applied, the code after this check will FAIL.
    //
    if (!am_hal_burst_ldo_patch_check())
    {
        return;
    }

    //
    // Burst LDO power init to charge VDDS cap and put Burst LDO into LP mode after chip power up
    // Put the BLE buck in strong pull-up mode
    //
    MCUCTRL->VRCTRL1_b.BLEBUCKPDNB            = 0;
    MCUCTRL->VRCTRL1_b.BLEBUCKRSTB            = 0;
    MCUCTRL->VRCTRL1_b.BLEBUCKACTIVE          = 1;
    MCUCTRL->VRCTRL1_b.BLEBUCKOVER            = 1;

    //
    // Delay 10ms for VDDBH rail to charge => we might be able to reduce this time dependent on ramp rate
    //
    am_hal_flash_delay(FLASH_CYCLES_US(DELAY_VDDBH_CHARGE_US));

    //
    // Power up the Burst LDO to charge VDDS cap
    //
    MCUCTRL->VRCTRL2_b.BURSTLDOPDNB           = 1;
    MCUCTRL->VRCTRL2_b.BURSTLDOCOLDSTARTEN    = 1;
    MCUCTRL->VRCTRL2_b.BURSTLDOACTIVEEARLY    = 1;
    MCUCTRL->VRCTRL2_b.BURSTLDOACTIVE         = 0;
    MCUCTRL->VRCTRL2_b.BURSTLDOOVER           = 1;

    //
    // Delay 2us before asserting burst LDO active
    //
    am_hal_flash_delay(FLASH_CYCLES_US(DELAY_BURSTLDO_ACT_US));
    MCUCTRL->VRCTRL2_b.BURSTLDOACTIVE         = 1;

    //
    // Delay 2ms for burst LDO to charge VDDS rail
    //
    am_hal_flash_delay(FLASH_CYCLES_US(DELAY_VDDS_CHARGE_US));

    //
    // Put Burst LDO into LP mode and keep VDDS cap charged
    //
    MCUCTRL->VRCTRL2_b.BURSTLDOCOLDSTARTEN    = 0;
    MCUCTRL->VRCTRL2_b.BURSTLDOACTIVEEARLY    = 0;
    MCUCTRL->VRCTRL2_b.BURSTLDOACTIVE         = 0;

    //
    // Switch BLE buck to weak pull-up mode and disable the override
    //
    MCUCTRL->VRCTRL1_b.BLEBUCKACTIVE          = 0;
    MCUCTRL->VRCTRL1_b.BLEBUCKOVER            = 0;
} //am_hal_burst_ldo_charge

//****************************************************************************
//
//  Enable Burst LDO workaround if CV Patch applied
//
//****************************************************************************
uint32_t
am_hal_burst_mode_enable_w_manual_sequence(am_hal_burst_mode_e *peBurstStatus)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    //
    // Check if Burst Mode is allowed and return status if it is not.
    //
    if (!g_am_hal_burst_mode_available)
    {
        *peBurstStatus = AM_HAL_NORMAL_MODE;
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    //
    // Check if patch applied
    //
    if ( !am_hal_burst_ldo_patch_check() )
    {
        //
        // The patch is not installed. Enable burst mode (96MHz) normally without workaround
        //
        g_bEnFromManual = true;
        ui32Status = am_hal_burst_mode_enable(peBurstStatus);
        g_bEnFromManual = false;
    }
    else
    {
        //
        // enter burst mode with workaround
        //
        AM_CRITICAL_BEGIN

        //
        // restore VDDF setting
        //
        am_hal_pwrctrl_wa_vddf_restore();

        //
        // Burst mode entry sequence
        // Turn on BLE buck and release from reset
        //
        MCUCTRL->VRCTRL1_b.BLEBUCKPDNB    = 1;
        MCUCTRL->VRCTRL1_b.BLEBUCKACTIVE  = 1;
        MCUCTRL->VRCTRL1_b.BLEBUCKRSTB = PWRCTRL->SUPPLYSTATUS_b.BLEBUCKON ? 1 : 0;
        MCUCTRL->VRCTRL1_b.BLEBUCKOVER    = 1;

        //
        // Delay 10us for analog to settle before turning on BLE buck
        //
        am_hal_flash_delay(FLASH_CYCLES_US(DELAY_BLEBUCK_ON_US));

        //
        // BLEBUCKRSTB = 1 => release BLE buck from reset to start buck operation
        //
        MCUCTRL->VRCTRL1_b.BLEBUCKRSTB    = 1;

        //
        // Force BLE buck into active mode
        //
        PWRCTRL->MISC_b.FORCEBLEBUCKACT   = 1;
        PWRCTRL->SUPPLYSRC_b.BLEBUCKEN      = 1;


        if (MCUCTRL->VRCTRL2_b.BURSTLDOOVER == 0)
        {
            //
            // Burstldo override is not set
            //
            MCUCTRL->VRCTRL2_b.BURSTLDOPDNB         = 1;
            MCUCTRL->VRCTRL2_b.BURSTLDOCOLDSTARTEN  = 1;
            MCUCTRL->VRCTRL2_b.BURSTLDOACTIVEEARLY  = 1;
            MCUCTRL->VRCTRL2_b.BURSTLDOACTIVE       = 0;
            MCUCTRL->VRCTRL2_b.BURSTLDOOVER         = 1;
        }
        else
        {
            //
            // Burstldo override is set
            // Put Burst LDO into active mode
            //
            MCUCTRL->VRCTRL2_b.BURSTLDOACTIVEEARLY  = 1;
        }

        //
        // Delay 2us before asserting burst LDO active
        //
        am_hal_flash_delay(FLASH_CYCLES_US(DELAY_BURSTLDO_ACT_US));
        MCUCTRL->VRCTRL2_b.BURSTLDOACTIVE         = 1;

        //
        // Provide delay for power domain switch
        //
        am_hal_flash_delay(FLASH_CYCLES_US(DELAY_POWER_DOMAIN_SWITCH_US));

        //
        // Enable burst mode, switch to 96MHz clock
        //
        g_bEnFromManual = true;
        ui32Status = am_hal_burst_mode_enable(peBurstStatus);
        g_bEnFromManual = false;

        //
        // Provide delay for Burst Mode enable (must be 25us or longer)
        //
        am_hal_flash_delay(FLASH_CYCLES_US(DELAY_BLEBUCK_OVER_REMOVAL_US));

        //
        // Remove BLEBUCK active overrides after entering burst mode
        //
        MCUCTRL->VRCTRL1_b.BLEBUCKOVER            = 0;
        MCUCTRL->VRCTRL1_b.BLEBUCKPDNB            = 0;
        MCUCTRL->VRCTRL1_b.BLEBUCKACTIVE          = 0;
        MCUCTRL->VRCTRL1_b.BLEBUCKRSTB            = 0;

        //
        // Remove Burstldo coldstart setting
        //
        if (MCUCTRL->VRCTRL2_b.BURSTLDOCOLDSTARTEN == 1)
        {
            MCUCTRL->VRCTRL2_b.BURSTLDOCOLDSTARTEN  = 0;
        }

        // boost vddf again
        am_hal_pwrctrl_wa_vddf_boost();

        AM_CRITICAL_END
    }

    return ui32Status;
} // am_hal_burst_mode_enable_w_manual_sequence

//****************************************************************************
//
//  Disable Burst LDO workaround if CV Patch applied
//
//****************************************************************************
uint32_t
am_hal_burst_mode_disable_w_manual_sequence(am_hal_burst_mode_e *peBurstStatus)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    //
    // Disable burst mode, switch to 48MHz clock
    //
    if (peBurstStatus != NULL)
    {
        g_bDisFromManual = true;
        ui32Status = am_hal_burst_mode_disable(peBurstStatus);
        g_bDisFromManual = false;
    }
    else
    {
        ui32Status = AM_HAL_STATUS_INVALID_HANDLE;
    }


    if (am_hal_burst_ldo_patch_check())
    {
        //
        // Put Burst LDO back into LP mode => keeps VDDS cap charged and
        // ready for next burst mode entry
        //
        MCUCTRL->VRCTRL2_b.BURSTLDOACTIVEEARLY    = 0;
        MCUCTRL->VRCTRL2_b.BURSTLDOACTIVE         = 0;

        //
        // remove force BLE buck active mode
        //
        PWRCTRL->MISC_b.FORCEBLEBUCKACT           = 0;
    }

    return ui32Status;
} // am_hal_burst_mode_disable_w_manual_sequence

#endif //AM_HAL_BURST_LDO_WORKAROUND

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
