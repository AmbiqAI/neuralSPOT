//*****************************************************************************
//
//! @file am_hal_mcuctrl.c
//!
//! @brief Functions for interfacing with the MCUCTRL.
//!
//! @addtogroup mcuctrl4_4b MCUCTRL - MCU Control
//! @ingroup apollo4b_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

//*****************************************************************************
//
// Global Variables.
//
//*****************************************************************************
//
// Lookup table for TCM and SSRAM sizes as derived from the CHIPPN register.
//
static const uint16_t
g_am_hal_mcuctrl_sram_size[AM_HAL_MCUCTRL_CHIPPN_SRAM_SIZE_N][3] =
{
    // 0: TCM, 1: SSRAM, 2: Extended mem
    {384, 512, 384},        // 0x0  896KB: 384 TCM +  512 SRAM +   0 Ext
    {384, 1024, 0},         // 0x1 1408KB: 384 TCM + 1024 SRAM +   0 Ext                // Apollo4b/c
    {384, 1024, 480},       // 0x2 1888KB: 384 TCM + 1024 SRAM + 480 Ext                // Apollo4b
    {384, 1024, 384},       // 0x3 1792KB: 384 TCM + 1024 SRAM + 384 Ext +    0 SRAM    // Apollo4c
    {384, 2048, 384},       // 0x4 2816KB: 384 TCM + 1024 SRAM + 384 Ext + 1024 SRAM    // Apollo4c
    {384, 2048, 384},       // 0x5 TBD
    {384, 2048, 384},       // 0x6 TBD
    {384, 2048, 384},       // 0x7 TBD
    {384, 2048, 384},       // 0x8 TBD
    {384, 2048, 384},       // 0x9 TBD
    {384, 2048, 384},       // 0xA TBD
    {384, 2048, 384},       // 0xB TBD
    {384, 2048, 384},       // 0xC TBD
    {384, 2048, 384},       // 0xD TBD
    {384, 2048, 384},       // 0xE TBD
    {384, 2048, 384},       // 0xF TBD
};

//
// Lookup table for MRAM sizes as derived from the SKU register.
//
static const uint32_t
g_am_hal_mcuctrl_mram_size[AM_HAL_MCUCTRL_SKU_MRAM_SIZE_N] =
{
     512  * 1024,
     1024 * 1024,
     1536 * 1024,
     2048 * 1024
};

// ****************************************************************************
// MCUCTRL XTALHSCAP Globals for Cooper Device
// Refer to App Note Apollo4B Blue 32MHz Crystal Calibration
// ****************************************************************************
uint32_t g_ui32xtalhscap2trim = XTALHSCAP2TRIM_DEFAULT;
uint32_t g_ui32xtalhscaptrim = XTALHSCAPTRIM_DEFAULT;

// ****************************************************************************
//
//  device_info_get()
//  Gets all relevant device information.
//
// ****************************************************************************
static void
device_info_get(am_hal_mcuctrl_device_t *psDevice)
{
    //
    // Read the Part Number.
    //
    psDevice->ui32ChipPN = MCUCTRL->CHIPPN;

    //
    // Read the Chip ID0.
    //
    psDevice->ui32ChipID0 = MCUCTRL->CHIPID0;

    //
    // Read the Chip ID1.
    //
    psDevice->ui32ChipID1 = MCUCTRL->CHIPID1;

    //
    // Read the Chip Revision.
    //
    psDevice->ui32ChipRev = MCUCTRL->CHIPREV;

    //
    // Read the Chip VENDOR ID.
    //
    psDevice->ui32VendorID = MCUCTRL->VENDORID;

    //
    // Read the SKU.
    //
    psDevice->ui32SKU = MCUCTRL->SKU;

    //
    // Qualified from Part Number.
    //
    psDevice->ui32Qualified = 1;

    //
    // TCM size as derived from CHIPPN register.
    // Note: Total onboard SRAM is computed as ui32DTCMSize + ui32SSRAMSize.
    //
    psDevice->ui32DTCMSize = g_am_hal_mcuctrl_sram_size[
            (psDevice->ui32ChipPN & MCUCTRL_CHIPPN_PARTNUM_SRAMSIZE_M) >>
             MCUCTRL_CHIPPN_PARTNUM_SRAMSIZE_S][0] * 1024;

    //
    // Non-TCM SRAM size as derived from the CHIPPN register.
    //
    psDevice->ui32SSRAMSize =
        (g_am_hal_mcuctrl_sram_size[(psDevice->ui32ChipPN & MCUCTRL_CHIPPN_PARTNUM_SRAMSIZE_M) >> MCUCTRL_CHIPPN_PARTNUM_SRAMSIZE_S][1] +
         g_am_hal_mcuctrl_sram_size[(psDevice->ui32ChipPN & MCUCTRL_CHIPPN_PARTNUM_SRAMSIZE_M) >> MCUCTRL_CHIPPN_PARTNUM_SRAMSIZE_S][2]) * 1024;

    //
    // MRAM size as derived from the SKU register.
    //
    psDevice->ui32MRAMSize = g_am_hal_mcuctrl_mram_size[MCUCTRL->SKU_b.SKUMRAMSIZE];

    // Now, let's look at the JEDEC info.
    // The full partnumber is 12 bits total, but is scattered across 2 registers.
    // Bits [11:8] are 0xE.
    // Bits [7:4] are 0xE for Apollo, 0xD for Apollo2.
    // Bits [3:0] are defined differently for Apollo and Apollo2.
    //   For Apollo, the low nibble is 0x0.
    //   For Apollo2, the low nibble indicates flash and SRAM size.
    //
    psDevice->ui32JedecPN  = JEDEC->PID0_b.PNL8 << 0;
    psDevice->ui32JedecPN |= JEDEC->PID1_b.PNH4 << 8;

    //
    // JEPID is the JEP-106 Manufacturer ID Code, which is assigned to Ambiq as
    //  0x1B, with parity bit is 0x9B.  It is 8 bits located across 2 registers.
    //
    psDevice->ui32JedecJEPID  = JEDEC->PID1_b.JEPIDL << 0;
    psDevice->ui32JedecJEPID |= JEDEC->PID2_b.JEPIDH << 4;

    //
    // CHIPREV is 8 bits located across 2 registers.
    //
    psDevice->ui32JedecCHIPREV  = JEDEC->PID2_b.CHIPREVH4 << 4;
    psDevice->ui32JedecCHIPREV |= JEDEC->PID3_b.CHIPREVL4 << 0;

    //
    // Let's get the Coresight ID (32-bits across 4 registers)
    // For Apollo and Apollo2, it's expected to be 0xB105100D.
    //
    psDevice->ui32JedecCID  = JEDEC->CID3_b.CID << 24;
    psDevice->ui32JedecCID |= JEDEC->CID2_b.CID << 16;
    psDevice->ui32JedecCID |= JEDEC->CID1_b.CID <<  8;
    psDevice->ui32JedecCID |= JEDEC->CID0_b.CID <<  0;

} // device_info_get()

// ****************************************************************************
//
//  am_hal_mcuctrl_control()
//  Apply various specific commands/controls on the MCUCTRL module.
//
// ****************************************************************************
uint32_t
am_hal_mcuctrl_control(am_hal_mcuctrl_control_e eControl, void *pArgs)
{
    volatile uint32_t ui32Reg;

    switch ( eControl )
    {
        case AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_ENABLE:
            //
            // Configure the bits in XTALCTRL that enable external 32KHz clock.
            //
            ui32Reg  = MCUCTRL->XTALCTRL;
            ui32Reg &= ~(MCUCTRL_XTALCTRL_XTALPDNB_Msk                      |
                         MCUCTRL_XTALCTRL_XTALCOMPPDNB_Msk                  |
                         MCUCTRL_XTALCTRL_XTALCOMPBYPASS_Msk                |
                         MCUCTRL_XTALCTRL_XTALCOREDISFB_Msk                 |
                         MCUCTRL_XTALCTRL_XTALSWE_Msk);
            ui32Reg |= _VAL2FLD(MCUCTRL_XTALCTRL_XTALPDNB,       MCUCTRL_XTALCTRL_XTALPDNB_PWRUPCORE)       |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOMPPDNB,   MCUCTRL_XTALCTRL_XTALCOMPPDNB_PWRUPCOMP)   |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOMPBYPASS, MCUCTRL_XTALCTRL_XTALCOMPBYPASS_USECOMP)   |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOREDISFB,  MCUCTRL_XTALCTRL_XTALCOREDISFB_EN)         |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALSWE,        MCUCTRL_XTALCTRL_XTALSWE_OVERRIDE_EN);
            MCUCTRL->XTALCTRL = ui32Reg;
            break;

        case AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_DISABLE:
            //
            // Configure the bits in XTALCTRL that disable external 32KHz
            // clock, thus re-configuring for the crystal.
            //
            ui32Reg  = MCUCTRL->XTALCTRL;
            ui32Reg &= ~(MCUCTRL_XTALCTRL_XTALPDNB_Msk                      |
                         MCUCTRL_XTALCTRL_XTALCOMPPDNB_Msk                  |
                         MCUCTRL_XTALCTRL_XTALCOMPBYPASS_Msk                |
                         MCUCTRL_XTALCTRL_XTALCOREDISFB_Msk                 |
                         MCUCTRL_XTALCTRL_XTALSWE_Msk);
            ui32Reg |= _VAL2FLD(MCUCTRL_XTALCTRL_XTALPDNB,       MCUCTRL_XTALCTRL_XTALPDNB_PWRUPCORE)       |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOMPPDNB,   MCUCTRL_XTALCTRL_XTALCOMPPDNB_PWRUPCOMP)   |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOMPBYPASS, MCUCTRL_XTALCTRL_XTALCOMPBYPASS_USECOMP)   |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALCOREDISFB,  MCUCTRL_XTALCTRL_XTALCOREDISFB_EN)         |
                       _VAL2FLD(MCUCTRL_XTALCTRL_XTALSWE,        MCUCTRL_XTALCTRL_XTALSWE_OVERRIDE_DIS);
            MCUCTRL->XTALCTRL = ui32Reg;
            break;

        case AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START:

// Set the default trim code for CAP1/CAP2, it impacts frequency accuracy and should be retrimmed
            ui32Reg = _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSCAP2TRIM, g_ui32xtalhscap2trim)    |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSCAPTRIM, g_ui32xtalhscaptrim)      |
// Set the transconductance of crystal to maximum, it accelerate the startup sequence
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSDRIVETRIM, 3)                      |
// Choose the power of clock driver to be the cleanest one
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSDRIVERSTRENGTH, 0)                 |
// Tune the bias generator
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASCOMP2TRIM, 3)                 |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASCOMPTRIM, 15)                 |
// Set the bias of crystal to maximum
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASTRIM, 127)                    |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSRSTRIM, 0)                         |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSSPARE, 0);
            MCUCTRL->XTALHSTRIMS = ui32Reg;

            ui32Reg  = MCUCTRL->XTALHSCTRL;
            ui32Reg &= ~(MCUCTRL_XTALHSCTRL_XTALHSCOMPPDNB_Msk              |
                         MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE_Msk          |
                         MCUCTRL_XTALHSCTRL_XTALHSPADOUTEN_Msk);
// Enable the option to output clock on PAD GPIO46 connecting to Cooper crystal input
            ui32Reg |= _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSCOMPPDNB, 1)       |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE, 0)   |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPADOUTEN, 1);
            MCUCTRL->XTALHSCTRL = ui32Reg;
            //
            // Turn on xtalhs_pdnb & xtalhs_injection_enable
            //

            // Turn on crystal oscillator
            MCUCTRL->XTALHSCTRL_b.XTALHSPDNB = 1;
            // inject HFRC clock to accelerate the startup sequence
            MCUCTRL->XTALHSCTRL_b.XTALHSINJECTIONENABLE = 1;

            //
            // Turn on xtalhs_ibst_enable
            //
            // Maximize the bias current to accelerate the startup sequence
            MCUCTRL->XTALHSCTRL_b.XTALHSIBSTENABLE = 1;

            //
            // Turn off xtalhs_injection_enable
            //
            // Wait 5us to make the setting above effective
            am_hal_delay_us(5);
            // Turn off the clock injection
            MCUCTRL->XTALHSCTRL_b.XTALHSINJECTIONENABLE = 0;

            // Apply external source
            if ( (pArgs) && (*((bool *)pArgs) == true) )
            {
                ui32Reg = MCUCTRL->XTALHSCTRL;
                ui32Reg &=  ~(MCUCTRL_XTALHSCTRL_XTALHSPDNB_Msk                 |
                              MCUCTRL_XTALHSCTRL_XTALHSIBSTENABLE_Msk           |
                              MCUCTRL_XTALHSCTRL_XTALHSEXTERNALCLOCK_Msk);
                ui32Reg |=  _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNB, 0)          |
                            _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSIBSTENABLE, 0)    |
                            _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSEXTERNALCLOCK, 1);
                MCUCTRL->XTALHSCTRL = ui32Reg;
            }

            break;

        case AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL:
            ui32Reg = _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSCAP2TRIM, g_ui32xtalhscap2trim)    |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSCAPTRIM, g_ui32xtalhscaptrim)      |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSDRIVETRIM, 3)                      |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSDRIVERSTRENGTH, 0)                 |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASCOMP2TRIM, 3)                 |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASCOMPTRIM, 15)                 |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASTRIM, 127)                    |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSRSTRIM, 0)                         |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSSPARE, 0);
            MCUCTRL->XTALHSTRIMS = ui32Reg;

            ui32Reg  = MCUCTRL->XTALHSCTRL;
            ui32Reg &= ~(MCUCTRL_XTALHSCTRL_XTALHSCOMPPDNB_Msk                  |
                         MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE_Msk              |
                         MCUCTRL_XTALHSCTRL_XTALHSPADOUTEN_Msk);
            ui32Reg |= _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSCOMPPDNB, 1)           |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE, 1)       |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPADOUTEN, 1);
            MCUCTRL->XTALHSCTRL = ui32Reg;
            //
            // Turn on xtalhs_pdnb
            //
            MCUCTRL->XTALHSCTRL_b.XTALHSPDNB = 1;
            // Apply external source
            ui32Reg = MCUCTRL->XTALHSCTRL;
            // Apply external source
            if ( (pArgs) && (*((bool *)pArgs) == true) )
            {
                ui32Reg &=  ~(MCUCTRL_XTALHSCTRL_XTALHSPDNB_Msk                 |
                              MCUCTRL_XTALHSCTRL_XTALHSEXTERNALCLOCK_Msk);
                ui32Reg |=  _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNB, 0)          |
                            _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSEXTERNALCLOCK, 1);
            }
            else
            {
                ui32Reg &=  ~(MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE_Msk         |
                              MCUCTRL_XTALHSCTRL_XTALHSIBSTENABLE_Msk);
                ui32Reg |=  _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE, 0)  |
                            _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSIBSTENABLE, 1);
            }

            MCUCTRL->XTALHSCTRL = ui32Reg;
            break;

        case AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_DISABLE:
            ui32Reg = _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSCAP2TRIM, g_ui32xtalhscap2trim)    |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSCAPTRIM, g_ui32xtalhscaptrim)      |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSDRIVETRIM, 0)                      |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSDRIVERSTRENGTH, 7)                 |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASCOMP2TRIM, 3)                 |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASCOMPTRIM, 8)                  |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSIBIASTRIM, 24)                     |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSRSTRIM, 0)                         |
                      _VAL2FLD(MCUCTRL_XTALHSTRIMS_XTALHSSPARE, 0);
            MCUCTRL->XTALHSTRIMS = ui32Reg;

            ui32Reg  = MCUCTRL->XTALHSCTRL;
            ui32Reg &= ~(MCUCTRL_XTALHSCTRL_XTALHSPDNB_Msk                  |
                         MCUCTRL_XTALHSCTRL_XTALHSEXTERNALCLOCK_Msk         |
                         MCUCTRL_XTALHSCTRL_XTALHSCOMPPDNB_Msk              |
                         MCUCTRL_XTALHSCTRL_XTALHSIBSTENABLE_Msk            |
                         MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE_Msk          |
                         MCUCTRL_XTALHSCTRL_XTALHSPADOUTEN_Msk);
            ui32Reg |= _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNB, 0)           |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSEXTERNALCLOCK, 0)  |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSCOMPPDNB, 1)       |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSIBSTENABLE, 0)     |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPDNPNIMPROVE, 0)   |
                       _VAL2FLD(MCUCTRL_XTALHSCTRL_XTALHSPADOUTEN, 1);
            MCUCTRL->XTALHSCTRL = ui32Reg;
            break;

        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Return success status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_mcuctrl_control()

// ****************************************************************************
//
//  am_hal_mcuctrl_status_get()
//! This function returns  current status of the MCUCTRL as obtained from
//! various registers of the MCUCTRL block.
//
// ****************************************************************************
uint32_t
am_hal_mcuctrl_status_get(am_hal_mcuctrl_status_t *psStatus)
{
    uint32_t ui32Status;

    if ( psStatus == NULL )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    psStatus->bDebuggerLockout =
        _FLD2VAL(MCUCTRL_DEBUGGER_LOCKOUT, MCUCTRL->DEBUGGER);

    psStatus->bADCcalibrated =
        _FLD2VAL(MCUCTRL_ADCCAL_ADCCALIBRATED, MCUCTRL->ADCCAL);

    psStatus->bBattLoadEnabled =
        _FLD2VAL(MCUCTRL_ADCBATTLOAD_BATTLOAD, MCUCTRL->ADCBATTLOAD);

    ui32Status = MCUCTRL->BOOTLOADER;

    psStatus->bSecBootOnColdRst =
        (_FLD2VAL(MCUCTRL_BOOTLOADER_SECBOOT, ui32Status) != MCUCTRL_BOOTLOADER_SECBOOT_ERROR);
    psStatus->bSecBootOnWarmRst =
        (_FLD2VAL(MCUCTRL_BOOTLOADER_SECBOOTONRST, ui32Status) != MCUCTRL_BOOTLOADER_SECBOOTONRST_ERROR);

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_mcuctrl_status_get()

// ****************************************************************************
//
//  am_hal_mcuctrl_info_get()
//  Get information of the given MCUCTRL item.
//
// ****************************************************************************
uint32_t
am_hal_mcuctrl_info_get(am_hal_mcuctrl_infoget_e eInfoGet, void *pInfo)
{
    am_hal_mcuctrl_feature_t *psFeature;

    if ( pInfo == NULL )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    switch ( eInfoGet )
    {
        case AM_HAL_MCUCTRL_INFO_FEATURES_AVAIL:
            psFeature = (am_hal_mcuctrl_feature_t*)pInfo;
            psFeature->eDTCMSize = AM_HAL_MCUCTRL_DTCM_384K;
            psFeature->eSharedSRAMSize = (am_hal_mcuctrl_ssram_e)MCUCTRL->SKU_b.SKUMRAMSIZE;
            psFeature->eMRAMSize = (am_hal_mcuctrl_mram_e)MCUCTRL->SKU_b.SKUSRAMSIZE;
            psFeature->bTurboSpot = (MCUCTRL->SKU_b.SKUTURBOSPOT > 0);
            psFeature->bDisplayCtrl = (MCUCTRL->SKU_b.SKUMIPIDSI > 0);
            psFeature->bGPU = (MCUCTRL->SKU_b.SKUGFX > 0);
            psFeature->bUSB = (MCUCTRL->SKU_b.SKUUSB > 0);
            psFeature->bSecBootFeature = (MCUCTRL->SKU_b.SKUSECURESPOT > 0);
            break;

        case AM_HAL_MCUCTRL_INFO_DEVICEID:
            device_info_get((am_hal_mcuctrl_device_t *)pInfo);
            break;

        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Return success status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_mcuctrl_info_get()

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
