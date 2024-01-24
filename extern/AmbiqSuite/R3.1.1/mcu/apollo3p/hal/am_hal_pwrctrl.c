//*****************************************************************************
//
//! @file am_hal_pwrctrl.c
//!
//! @brief Functions for enabling and disabling power domains.
//!
//! @addtogroup pwrctrl3p Pwrctrl - Power Control
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

//*****************************************************************************
//
//  Defines
//
//*****************************************************************************
//
//! Maximum number of checks to memory power status before declaring error.
//
#define AM_HAL_PWRCTRL_MAX_WAIT     20

//*****************************************************************************
//
//  Globals
//
//*****************************************************************************
bool g_bSimobuckTrimsDone = false;

#ifdef AM_HAL_PWRCTRL_VDDF_BOOST_WA
bool g_bVddfTrimsDone = false;
#endif

//
//!    Mask of HCPA Enables from the PWRCTRL->DEVPWRSTATUS register mapped to the
//!        PWRCTRL->DEVPWREN register
//
#define HCPA_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOS, PWRCTRL_DEVPWREN_PWRIOS_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRUART0, PWRCTRL_DEVPWREN_PWRUART0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRUART1, PWRCTRL_DEVPWREN_PWRUART1_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRSCARD, PWRCTRL_DEVPWREN_PWRSCARD_EN))

//
//!    Mask of HCPB Enables from the PWRCTRL->DEVPWRSTATUS register mapped to the
//!        PWRCTRL->DEVPWREN register
//
#define HCPB_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM0, PWRCTRL_DEVPWREN_PWRIOM0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM1, PWRCTRL_DEVPWREN_PWRIOM1_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM2, PWRCTRL_DEVPWREN_PWRIOM2_EN))

//
//!    Mask of HCPC Enables from the PWRCTRL->DEVPWRSTATUS register mapped to the
//!        PWRCTRL->DEVPWREN register
//
#define HCPC_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM3, PWRCTRL_DEVPWREN_PWRIOM3_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM4, PWRCTRL_DEVPWREN_PWRIOM4_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM5, PWRCTRL_DEVPWREN_PWRIOM5_EN))

//
//!    Mask of MSPI Enables from the PWRCTRL->DEVPWRSTATUS register mapped to the
//!        PWRCTRL->DEVPWREN register
//
#define MSPI_MASK       ( \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRMSPI0, PWRCTRL_DEVPWREN_PWRMSPI0_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRMSPI1, PWRCTRL_DEVPWREN_PWRMSPI1_EN) | \
    _VAL2FLD(PWRCTRL_DEVPWREN_PWRMSPI2, PWRCTRL_DEVPWREN_PWRMSPI2_EN))

//
//! Define the peripheral control structure.
//
const struct
{
    uint32_t      ui32PeriphEnable;
    uint32_t      ui32PeriphStatus;
    uint32_t      ui32PeriphEvent;
}
am_hal_pwrctrl_peripheral_control[AM_HAL_PWRCTRL_PERIPH_MAX] =
{
    {0, 0, 0},                                  //  AM_HAL_PWRCTRL_PERIPH_NONE
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOS, PWRCTRL_DEVPWREN_PWRIOS_EN),
     PWRCTRL_DEVPWRSTATUS_HCPA_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPAEVEN, PWRCTRL_DEVPWREVENTEN_HCPAEVEN_EN)},  // AM_HAL_PWRCTRL_PERIPH_IOS
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM0, PWRCTRL_DEVPWREN_PWRIOM0_EN),
     PWRCTRL_DEVPWRSTATUS_HCPB_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPBEVEN, PWRCTRL_DEVPWREVENTEN_HCPBEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_IOM0
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM1, PWRCTRL_DEVPWREN_PWRIOM1_EN),
     PWRCTRL_DEVPWRSTATUS_HCPB_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPBEVEN, PWRCTRL_DEVPWREVENTEN_HCPBEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_IOM1
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM2, PWRCTRL_DEVPWREN_PWRIOM2_EN),
     PWRCTRL_DEVPWRSTATUS_HCPB_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPBEVEN, PWRCTRL_DEVPWREVENTEN_HCPBEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_IOM2
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM3, PWRCTRL_DEVPWREN_PWRIOM3_EN),
     PWRCTRL_DEVPWRSTATUS_HCPC_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPCEVEN, PWRCTRL_DEVPWREVENTEN_HCPCEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_IOM3
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM4, PWRCTRL_DEVPWREN_PWRIOM4_EN),
     PWRCTRL_DEVPWRSTATUS_HCPC_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPCEVEN, PWRCTRL_DEVPWREVENTEN_HCPCEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_IOM4
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRIOM5, PWRCTRL_DEVPWREN_PWRIOM5_EN),
     PWRCTRL_DEVPWRSTATUS_HCPC_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPCEVEN, PWRCTRL_DEVPWREVENTEN_HCPCEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_IOM5
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRUART0, PWRCTRL_DEVPWREN_PWRUART0_EN),
     PWRCTRL_DEVPWRSTATUS_HCPA_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPAEVEN, PWRCTRL_DEVPWREVENTEN_HCPAEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_UART0
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRUART1, PWRCTRL_DEVPWREN_PWRUART1_EN),
     PWRCTRL_DEVPWRSTATUS_HCPA_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPAEVEN, PWRCTRL_DEVPWREVENTEN_HCPAEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_UART1
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRADC, PWRCTRL_DEVPWREN_PWRADC_EN),
     PWRCTRL_DEVPWRSTATUS_PWRADC_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_ADCEVEN, PWRCTRL_DEVPWREVENTEN_ADCEVEN_EN)},    //  AM_HAL_PWRCTRL_PERIPH_ADC
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRSCARD, PWRCTRL_DEVPWREN_PWRSCARD_EN),
     PWRCTRL_DEVPWRSTATUS_HCPA_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_HCPAEVEN, PWRCTRL_DEVPWREVENTEN_HCPAEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_SCARD
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRMSPI0, PWRCTRL_DEVPWREN_PWRMSPI0_EN),
     PWRCTRL_DEVPWRSTATUS_PWRMSPI_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_MSPIEVEN, PWRCTRL_DEVPWREVENTEN_MSPIEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_MSPI0
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRMSPI1, PWRCTRL_DEVPWREN_PWRMSPI1_EN),
     PWRCTRL_DEVPWRSTATUS_PWRMSPI_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_MSPIEVEN, PWRCTRL_DEVPWREVENTEN_MSPIEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_MSPI1
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRMSPI2, PWRCTRL_DEVPWREN_PWRMSPI2_EN),
     PWRCTRL_DEVPWRSTATUS_PWRMSPI_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_MSPIEVEN, PWRCTRL_DEVPWREVENTEN_MSPIEVEN_EN)},  //  AM_HAL_PWRCTRL_PERIPH_MSPI2
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRPDM, PWRCTRL_DEVPWREN_PWRPDM_EN),
     PWRCTRL_DEVPWRSTATUS_PWRPDM_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_PDMEVEN, PWRCTRL_DEVPWREVENTEN_PDMEVEN_EN)},    //  AM_HAL_PWRCTRL_PERIPH_PDM
    {_VAL2FLD(PWRCTRL_DEVPWREN_PWRBLEL, PWRCTRL_DEVPWREN_PWRBLEL_EN),
     PWRCTRL_DEVPWRSTATUS_BLEL_Msk,
     _VAL2FLD(PWRCTRL_DEVPWREVENTEN_BLELEVEN, PWRCTRL_DEVPWREVENTEN_BLELEVEN_EN)}   //  AM_HAL_PWRCTRL_PERIPH_BLEL
};

//
//! Define the memory control structure.
//
const struct
{
    uint32_t      ui32MemoryEnable;
    uint32_t      ui32MemoryStatus;
    uint32_t      ui32MemoryEvent;
    uint32_t      ui32MemoryMask;
    uint32_t      ui32StatusMask;
    uint32_t      ui32PwdSlpEnable;
}
am_hal_pwrctrl_memory_control[AM_HAL_PWRCTRL_MEM_MAX] =
{
    {0, 0, 0, 0, 0, 0},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_8K_DTCM,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_8K_DTCM,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_8K_DTCM,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_8K_DTCM},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_32K_DTCM,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_32K_DTCM,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_32K_DTCM,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_32K_DTCM},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_64K_DTCM,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_64K_DTCM,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_64K_DTCM,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_64K_DTCM},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_128K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_128K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_128K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_128K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_192K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_192K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_192K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_192K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_256K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_256K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_256K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_256K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_320K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_320K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_320K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_320K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_384K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_384K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_384K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_384K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_448K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_448K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_448K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_448K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_512K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_512K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_512K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_512K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_576K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_576K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_576K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_576K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_672K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_672K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_672K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_672K},
    {AM_HAL_PWRCTRL_MEMEN_SRAM_768K,
     AM_HAL_PWRCTRL_PWRONSTATUS_SRAM_768K,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_SRAM_768K,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_SRAM_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_SRAM_768K},
    {AM_HAL_PWRCTRL_MEMEN_FLASH_1M,
     AM_HAL_PWRCTRL_PWRONSTATUS_FLASH_1M,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_FLASH_1M,
     AM_HAL_PWRCTRL_MEM_REGION_FLASH_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_FLASH_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_FLASH_1M},
    {AM_HAL_PWRCTRL_MEMEN_FLASH_2M,
     AM_HAL_PWRCTRL_PWRONSTATUS_FLASH_2M,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_FLASH_2M,
     AM_HAL_PWRCTRL_MEM_REGION_FLASH_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_FLASH_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_FLASH_2M},
    {AM_HAL_PWRCTRL_MEMEN_CACHE,
     0,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_CACHE,
     AM_HAL_PWRCTRL_MEM_REGION_CACHE_MASK,
     0,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_CACHE},
    {AM_HAL_PWRCTRL_MEMEN_ALL,
     AM_HAL_PWRCTRL_PWRONSTATUS_ALL,
     AM_HAL_PWRCTRL_MEMPWREVENTEN_ALL,
     AM_HAL_PWRCTRL_MEM_REGION_ALL_MASK,
     AM_HAL_PWRCTRL_MEM_REGION_ALT_ALL_MASK,
     AM_HAL_PWRCTRL_MEMPWDINSLEEP_ALL}
};

// ****************************************************************************
//
//  Enable power for a peripheral.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_periph_enable(am_hal_pwrctrl_periph_e ePeripheral)
{
    //
    // Enable power control for the given device.
    //
    AM_CRITICAL_BEGIN
    PWRCTRL->DEVPWREN |= am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable;
    AM_CRITICAL_END

    for (uint32_t wait_usecs = 0; wait_usecs < AM_HAL_PWRCTRL_MAX_WAIT; wait_usecs += 10)
    {
        am_hal_flash_delay(FLASH_CYCLES_US(10));

        if ((PWRCTRL->DEVPWRSTATUS & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphStatus) > 0)
        {
            break;
        }
    }

    //
    // Check the device status.
    //
    if ((PWRCTRL->DEVPWRSTATUS & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphStatus) > 0)
    {
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        //
        // If the Power Enable fails, make sure the DEVPWREN is Low
        //
        AM_CRITICAL_BEGIN
        PWRCTRL->DEVPWREN &= ~am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable;
        AM_CRITICAL_END

        return AM_HAL_STATUS_FAIL;
    }
} // am_hal_pwrctrl_periph_enable()

// ****************************************************************************
//
//  Function checks the PWRCTRL->DEVPWREN since the PWRCTRL->DEVPWRSTATUS
//        register alone cannot tell the user if a peripheral is enabled when
//        and HCPx register is being used.
//
// ****************************************************************************
static uint32_t
pwrctrl_periph_disable_msk_check(am_hal_pwrctrl_periph_e ePeripheral)
{
    uint32_t retVal = AM_HAL_STATUS_FAIL;
    uint32_t HCPxMSPIxMask = PWRCTRL->DEVPWREN;

    switch (am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphStatus)
    {
        case (PWRCTRL_DEVPWRSTATUS_HCPA_Msk):
            if (((HCPxMSPIxMask & HCPA_MASK) > 0) && ((HCPxMSPIxMask & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable) == 0))
            {
                retVal = AM_HAL_STATUS_SUCCESS;
            }
            break;

        case (PWRCTRL_DEVPWRSTATUS_HCPB_Msk):
            if (((HCPxMSPIxMask & HCPB_MASK) > 0) && ((HCPxMSPIxMask & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable) == 0))
            {
                retVal = AM_HAL_STATUS_SUCCESS;
            }
            break;

        case (PWRCTRL_DEVPWRSTATUS_HCPC_Msk):
            if (((HCPxMSPIxMask & HCPC_MASK) > 0) && ((HCPxMSPIxMask & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable) == 0))
            {
                retVal = AM_HAL_STATUS_SUCCESS;
            }
            break;

        case (PWRCTRL_DEVPWRSTATUS_PWRMSPI_Msk):
            if (((HCPxMSPIxMask & MSPI_MASK) > 0) && ((HCPxMSPIxMask & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable) == 0))
            {
                retVal = AM_HAL_STATUS_SUCCESS;
            }
            break;

        default:
            break;
    }

    return retVal;
}

// ****************************************************************************
//
//  Disable power for a peripheral.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_periph_disable(am_hal_pwrctrl_periph_e ePeripheral)
{
    //
    // Disable power domain for the given device.
    //
    AM_CRITICAL_BEGIN
    PWRCTRL->DEVPWREN &= ~am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable;
    AM_CRITICAL_END

    for (uint32_t wait_usecs = 0; wait_usecs < AM_HAL_PWRCTRL_MAX_WAIT; wait_usecs += 10)
    {
        am_hal_flash_delay(FLASH_CYCLES_US(10));

        if ((PWRCTRL->DEVPWRSTATUS & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphStatus) == 0)
        {
            break;
        }
    }

    //
    // Check the device status.
    //
    if ((PWRCTRL->DEVPWRSTATUS & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphStatus) == 0)
    {
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        return pwrctrl_periph_disable_msk_check(ePeripheral);
    }

} // am_hal_pwrctrl_periph_disable()

//*****************************************************************************
//
// This function determines to the caller whether a given peripheral is
// currently enabled or disabled.
//
//*****************************************************************************
uint32_t
am_hal_pwrctrl_periph_enabled(am_hal_pwrctrl_periph_e ePeripheral,
                              uint32_t *pui32Enabled)
{
    uint32_t ui32Mask = am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphStatus;
    uint32_t ui32Enabled = 0;

    if (pui32Enabled == NULL)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if (ui32Mask != 0)
    {
        ui32Enabled = PWRCTRL->DEVPWRSTATUS & ui32Mask ? 1 : 0;
        ui32Enabled = ui32Enabled && (PWRCTRL->DEVPWREN & am_hal_pwrctrl_peripheral_control[ePeripheral].ui32PeriphEnable);
    }

    *pui32Enabled = ui32Enabled;

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_periph_enabled()

// ****************************************************************************
//
//  Enable a configuration of memory.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_memory_enable(am_hal_pwrctrl_mem_e eMemConfig)
{
    uint32_t ui32MemEnMask, ui32MemDisMask, ui32MemRegionMask, ui32MemStatusMask;

    ui32MemEnMask     =  am_hal_pwrctrl_memory_control[eMemConfig].ui32MemoryEnable;
    ui32MemDisMask    = ~am_hal_pwrctrl_memory_control[eMemConfig].ui32MemoryEnable;
    ui32MemRegionMask = am_hal_pwrctrl_memory_control[eMemConfig].ui32MemoryMask;
    ui32MemStatusMask = am_hal_pwrctrl_memory_control[eMemConfig].ui32StatusMask;

    //
    // Disable unneeded memory. If nothing to be disabled, skip to save time.
    //
    // Note that a deliberate disable step using a disable mask is taken here
    // for 2 reasons: 1) To only affect the specified type of memory, and 2)
    // To avoid inadvertently disabling any memory currently being depended on.
    //
    if ( ui32MemDisMask != 0 )
    {
        PWRCTRL->MEMPWREN &=
            ~(ui32MemDisMask & ui32MemRegionMask)                                   |
             (_VAL2FLD(PWRCTRL_MEMPWREN_DTCM, PWRCTRL_MEMPWREN_DTCM_GROUP0DTCM0)    |
              _VAL2FLD(PWRCTRL_MEMPWREN_FLASH0, PWRCTRL_MEMPWREN_FLASH0_EN));
        am_hal_flash_delay(FLASH_CYCLES_US(1));
    }

    //
    // Enable the required memory.
    //
    if ( ui32MemEnMask != 0 )
    {
        PWRCTRL->MEMPWREN |= ui32MemEnMask;

        for (uint32_t wait_usecs = 0; wait_usecs < AM_HAL_PWRCTRL_MAX_WAIT; wait_usecs += 10)
        {
            am_hal_flash_delay(FLASH_CYCLES_US(10));

            if ( (PWRCTRL->MEMPWRSTATUS & ui32MemStatusMask) ==
                  am_hal_pwrctrl_memory_control[eMemConfig].ui32MemoryStatus )
            {
                break;
            }
        }
    }

    //
    // Return status based on whether the power control memory status has reached the desired state.
    //
    if ( ( PWRCTRL->MEMPWRSTATUS & ui32MemStatusMask) ==
           am_hal_pwrctrl_memory_control[eMemConfig].ui32MemoryStatus )
    {
        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        return AM_HAL_STATUS_FAIL;
    }

} // am_hal_pwrctrl_memory_enable()

// ****************************************************************************
//
//  Power down respective memory.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_memory_deepsleep_powerdown(am_hal_pwrctrl_mem_e eMemConfig)
{
    if ( eMemConfig >= AM_HAL_PWRCTRL_MEM_MAX )
    {
        return AM_HAL_STATUS_FAIL;
    }

    //
    // Power down the required memory.
    //
    PWRCTRL->MEMPWDINSLEEP |= am_hal_pwrctrl_memory_control[eMemConfig].ui32PwdSlpEnable;

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_memory_deepsleep_powerdown()

// ****************************************************************************
//
//  Apply retention voltage to respective memory.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_memory_deepsleep_retain(am_hal_pwrctrl_mem_e eMemConfig)
{
    if ( eMemConfig >= AM_HAL_PWRCTRL_MEM_MAX )
    {
        return AM_HAL_STATUS_FAIL;
    }

    //
    // Retain the required memory.
    //
    PWRCTRL->MEMPWDINSLEEP &= ~am_hal_pwrctrl_memory_control[eMemConfig].ui32PwdSlpEnable;

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_pwrctrl_memory_deepsleep_retain()


#ifdef AM_HAL_PWRCTRL_VDDF_BOOST_WA
//****************************************************************************
//
//  simobuck_memldo_patch_check()
//
//****************************************************************************
#define AM_REG_INFO1_PATCHVER6_ADDR 0x50023838
#define AM_REG_INFO1_TRIM22_ADDR    0x5002386C
#define AM_REG_INFO1_TRIM28_ADDR    0x50023870

#define TRIM22_MEMLDOACTIVETRIM_Msk 0x0000003F
#define TRIM22_MEMLDOACTIVETRIM_Pos 0
#define TRIM28_MEMACTIVETRIM_Msk    0x003F0000
#define TRIM28_MEMACTIVETRIM_Pos    16

static bool
simobuck_memldo_patch_check(void)
{
    //
    // patch checker bit3 for CV patch
    //
    return (AM_REGVAL(AM_REG_INFO1_PATCHVER6_ADDR) & (1 << 3)) ? false : true;
} // simobuck_memldo_patch_check

//****************************************************************************
//
//  Apply VDDF boost of +30mV on SIMOBUCK and MEMLDO Active Trim
//
//****************************************************************************
void
am_hal_pwrctrl_bleif_workaround(void)
{
    uint32_t ui32RegBackupVal;
    uint32_t ui32TrimVal;
    //
    // If patch is not applied, use current values for boost.
    //
    if (!simobuck_memldo_patch_check())
    {
        //
        // Apply boost based on original values in
        //   MCUCTRL->SIMOBUCK1 (for simobuck)
        //   MCUCTRL->LDOREG2   (for MEMLDO)
        //

        //
        // increase VDDF (LDOREG2_MEMLDOACTIVETRIM) by 3 counts
        //
        ui32RegBackupVal = MCUCTRL->LDOREG2;

        ui32TrimVal = _FLD2VAL(MCUCTRL_LDOREG2_MEMLDOACTIVETRIM, ui32RegBackupVal);

        //
        // increase VDDF by ui32TrimVal counts
        //
        ui32TrimVal += 3;

        //
        // check for overflow and limit
        //
        ui32TrimVal =  ui32TrimVal > ( MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk >> MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Pos ) ? ( MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk >> MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Pos ) : ui32TrimVal;

        //
        // Put boosted value back.
        //
        MCUCTRL->LDOREG2 &= ~MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk;
        MCUCTRL->LDOREG2 |= _VAL2FLD(MCUCTRL_LDOREG2_MEMLDOACTIVETRIM, ui32TrimVal);

        //
        // increase VDDF (SIMOBUCK1_MEMACTIVETRIM) by 5 counts
        //
        ui32RegBackupVal = MCUCTRL->SIMOBUCK1;

        ui32TrimVal = _FLD2VAL(MCUCTRL_SIMOBUCK1_MEMACTIVETRIM, ui32RegBackupVal);

        //
        // increase VDDF by ui32TrimVal counts
        //
        ui32TrimVal += 5;

        //
        // check for overflow and limit
        //
        ui32TrimVal =  ui32TrimVal > ( MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Pos ) ? ( MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Pos ) : ui32TrimVal;

        //
        // Put boosted value back.
        //
        MCUCTRL->SIMOBUCK1 &= ~MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk;
        MCUCTRL->SIMOBUCK1 |= _VAL2FLD(MCUCTRL_SIMOBUCK1_MEMACTIVETRIM, ui32TrimVal);
    }
    else
    {
        //
        // Apply boost based on memldo and simobuck_mem_active trims
        //   AM_REG_INFO1_TRIM28_ADDR & TRIM28_MEMACTIVETRIM_Msk (for simobuck)
        //   AM_REG_INFO1_TRIM22_ADDR & TRIM22_MEMLDOACTIVETRIM_Msk (for MEMLDO)
        //

        //
        // increase VDDF (MEMLDO) by 3 counts
        //
        ui32RegBackupVal = AM_REGVAL(AM_REG_INFO1_TRIM22_ADDR);

        ui32TrimVal = _FLD2VAL(TRIM22_MEMLDOACTIVETRIM, ui32RegBackupVal);

        //
        // increase VDDF by ui32TrimVal counts
        //
        ui32TrimVal += 3;

        //
        // check for overflow and limit
        //
        ui32TrimVal =  ui32TrimVal > (TRIM22_MEMLDOACTIVETRIM_Msk >> TRIM22_MEMLDOACTIVETRIM_Pos) ? (TRIM22_MEMLDOACTIVETRIM_Msk >> TRIM22_MEMLDOACTIVETRIM_Pos) : ui32TrimVal;

        //
        // Put boosted value back.
        //
        MCUCTRL->LDOREG2 &= ~TRIM22_MEMLDOACTIVETRIM_Msk;
        MCUCTRL->LDOREG2 |= _VAL2FLD(TRIM22_MEMLDOACTIVETRIM, ui32TrimVal);

        //
        // increase VDDF (SIMOBUCK MEM) by 5 counts
        //
        ui32RegBackupVal = AM_REGVAL(AM_REG_INFO1_TRIM28_ADDR);

        ui32TrimVal = _FLD2VAL(TRIM28_MEMACTIVETRIM, ui32RegBackupVal);

        //
        // increase VDDF by ui32TrimVal counts
        //
        ui32TrimVal += 5;

        //
        // check for overflow and limit
        //
        ui32TrimVal =  ui32TrimVal > ( TRIM28_MEMACTIVETRIM_Msk >> TRIM28_MEMACTIVETRIM_Pos ) ? ( TRIM28_MEMACTIVETRIM_Msk >> TRIM28_MEMACTIVETRIM_Pos ) : ui32TrimVal;

        //
        // Put boosted value back.
        //
        MCUCTRL->SIMOBUCK1 &= ~TRIM28_MEMACTIVETRIM_Msk;
        MCUCTRL->SIMOBUCK1 |= _VAL2FLD(TRIM28_MEMACTIVETRIM, ui32TrimVal);
    }
} //am_hal_pwrctrl_bleif_workaround

#endif //#ifdef AM_HAL_PWRCTRL_VDDF_BOOST_WA

// ****************************************************************************
//
//  Updates to the SIMOBUCK Trims
//
// ****************************************************************************
static void
simobuck_updates(void)
{
    //
    // Adjust the SIMOBUCK LP settings.
    //
    if ( APOLLO3_GE_B0 )
    {
        if ( !g_bSimobuckTrimsDone )
        {
            uint32_t ui32LPTRIM;
            g_bSimobuckTrimsDone = true;

            //
            // Update SIMOBUCK MEM LP TRIM 12 codes ~ -37mV
            //
            ui32LPTRIM = MCUCTRL->SIMOBUCK1_b.SIMOBUCKMEMLPTRIM;
            ui32LPTRIM = ui32LPTRIM > 12 ? ui32LPTRIM - 12 : 0;
            MCUCTRL->SIMOBUCK1_b.SIMOBUCKMEMLPTRIM = ui32LPTRIM;

            //
            // Update SIMOBUCK CORE LP TRIM 13 codes ~ -40mV
            //
            ui32LPTRIM = MCUCTRL->SIMOBUCK1_b.SIMOBUCKCORELPTRIM;
            ui32LPTRIM = ui32LPTRIM > 13 ? ui32LPTRIM - 13 : 0;
            MCUCTRL->SIMOBUCK1_b.SIMOBUCKCORELPTRIM = ui32LPTRIM;

            MCUCTRL->SIMOBUCK2_b.SIMOBUCKCORELPLOWTONTRIM   = 8;
            MCUCTRL->SIMOBUCK2_b.SIMOBUCKCORELPHIGHTONTRIM  = 5;
            MCUCTRL->SIMOBUCK4_b.SIMOBUCKMEMLPLOWTONTRIM    = 8;
            MCUCTRL->SIMOBUCK3_b.SIMOBUCKMEMLPHIGHTONTRIM   = 6;
            MCUCTRL->SIMOBUCK4_b.SIMOBUCKCOMP2LPEN          = 1;
            MCUCTRL->SIMOBUCK4_b.SIMOBUCKCOMP2TIMEOUTEN     = 0;
            MCUCTRL->SIMOBUCK2_b.SIMOBUCKCORELEAKAGETRIM    = 3;
            MCUCTRL->SIMOBUCK2_b.SIMOBUCKTONGENTRIM         = 31;
        }

#ifdef AM_HAL_PWRCTRL_VDDF_BOOST_WA
        //
        // Increase VDDF voltage by +30mV for both mem simobuck and ldo
        //
        if ( !g_bVddfTrimsDone )
        {
            g_bVddfTrimsDone = true;

            //
            // Perform boost of vddf by +30mV
            //
            am_hal_pwrctrl_bleif_workaround();
        }
#endif //#ifdef AM_HAL_PWRCTRL_VDDF_BOOST_WA
    }

    //
    // Adjust the SIMOBUCK Timeout settings.
    //
    if ( APOLLO3_GE_A1 )
    {
        MCUCTRL->SIMOBUCK4_b.SIMOBUCKCOMP2TIMEOUTEN = 0;
    }
} // simobuck_updates()

// ****************************************************************************
//
//  am_hal_pwrctrl_low_power_init()
//  Initialize system for low power configuration.
//
// ****************************************************************************
uint32_t
am_hal_pwrctrl_low_power_init(void)
{
    uint32_t      ui32Status;

    //
    // Take a snapshot of the reset status, if not done already
    //
    if ( !gAmHalResetStatus )
    {
        gAmHalResetStatus = RSTGEN->STAT;
    }

    //
    // Adjust the SIMOBUCK LP settings.
    //
    simobuck_updates();

    //
    // Configure cache for low power and performance.
    //
    am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_LPMMODE_RECOMMENDED, 0);

    //
    // Check if the BLE is already enabled.
    //
    if ( PWRCTRL->DEVPWRSTATUS_b.BLEL == 0)
    {
        am_hal_pwrctrl_blebuck_trim();

        //
        // First request the BLE feature and check that it was available and acknowledged.
        //
        MCUCTRL->FEATUREENABLE_b.BLEREQ = 1;
        ui32Status = am_hal_flash_delay_status_check(10000,
                                                    (uint32_t)&MCUCTRL->FEATUREENABLE,
                                                    (MCUCTRL_FEATUREENABLE_BLEAVAIL_Msk |
                                                     MCUCTRL_FEATUREENABLE_BLEACK_Msk   |
                                                     MCUCTRL_FEATUREENABLE_BLEREQ_Msk),
                                                    (MCUCTRL_FEATUREENABLE_BLEAVAIL_Msk |
                                                     MCUCTRL_FEATUREENABLE_BLEACK_Msk   |
                                                     MCUCTRL_FEATUREENABLE_BLEREQ_Msk),
                                                     true);

        if (AM_HAL_STATUS_SUCCESS != ui32Status)
        {
            return AM_HAL_STATUS_TIMEOUT;
        }

        //
        // Next, enable the BLE Buck.
        //
        PWRCTRL->SUPPLYSRC |= _VAL2FLD(PWRCTRL_SUPPLYSRC_BLEBUCKEN,
                                       PWRCTRL_SUPPLYSRC_BLEBUCKEN_EN);

        //
        // Allow the buck to go to low power mode in BLE sleep.
        //
        PWRCTRL->MISC |= _VAL2FLD(PWRCTRL_MISC_MEMVRLPBLE,
                                  PWRCTRL_MISC_MEMVRLPBLE_EN);
    }

#if AM_HAL_BURST_LDO_WORKAROUND
    //
    // This is to charge the capacitor
    //
    am_hal_burst_ldo_charge();
#endif

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_pwrctrl_low_power_init()

// ****************************************************************************
//
//  am_hal_pwrctrl_blebuck_trim()
//
// ****************************************************************************
void
am_hal_pwrctrl_blebuck_trim(void)
{
    //
    // Enable the BLE buck trim values
    //
    if ( APOLLO3_GE_A1 )
    {
        AM_CRITICAL_BEGIN
        MCUCTRL->BLEBUCK2_b.BLEBUCKTONHITRIM = 0x19;
        MCUCTRL->BLEBUCK2_b.BLEBUCKTONLOWTRIM = 0xC;
        CLKGEN->BLEBUCKTONADJ_b.TONADJUSTEN = CLKGEN_BLEBUCKTONADJ_TONADJUSTEN_DIS;
        AM_CRITICAL_END
    }
} // am_hal_pwrctrl_blebuck_trim()

#if AM_HAL_BURST_LDO_WORKAROUND

//****************************************************************************
//
// Get the simobuck vddc/vddf active trim value with saturation.
// Based on current register setting
//
//****************************************************************************
static uint32_t
simobuck_vddx_active_trim_get(am_hal_burst_voltage_wa_e vddx)
{
    if (vddx == AM_HAL_BURST_VDDC)
    {
        return MCUCTRL->SIMOBUCK1_b.COREACTIVETRIM;
    }
    else if (vddx == AM_HAL_BURST_VDDF)
    {
        return MCUCTRL->SIMOBUCK1_b.MEMACTIVETRIM;
    }

    return 0;
}

//****************************************************************************
//
// Get the simobuck vddc/vddf default trim value with saturation.
// Based on current register setting
//
//****************************************************************************
static uint32_t
simobuck_vddx_default_trim_get(am_hal_burst_voltage_wa_e vddx)
{
    if (am_hal_burst_ldo_patch_check())
    {
        if (vddx == AM_HAL_BURST_VDDC)
        {
            return (AM_REGVAL(0x50023870) & 0x3FF);
        }
        else if (vddx == AM_HAL_BURST_VDDF)
        {
            return ((AM_REGVAL(0x50023870) >> 16) & 0x3F);
        }
    }

    return 0;
}

//****************************************************************************
//
// Get the LDO vddc/vddf active trim value with saturation.
// Based on current register setting
//
//****************************************************************************
static uint32_t
ldo_vddx_active_trim_get(am_hal_burst_voltage_wa_e vddx)
{
    if (vddx == AM_HAL_BURST_VDDC)
    {
        return MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM;
    }
    else if (vddx == AM_HAL_BURST_VDDF)
    {
        return MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM;
    }

    return 0;
}

//****************************************************************************
//
// Get the LDO vddc/vddf default trim value with saturation.
// Based on current register setting
//
//****************************************************************************
static uint32_t
ldo_vddx_default_trim_get(am_hal_burst_voltage_wa_e vddx)
{
    if (am_hal_burst_ldo_patch_check())
    {
        if (vddx == AM_HAL_BURST_VDDC)
        {
            return (AM_REGVAL(0x50023868) & 0x3FF);
        }
        else if (vddx == AM_HAL_BURST_VDDF)
        {
            return (AM_REGVAL(0x5002386C) & 0x3F);
        }
    }

    return 0;
}

//****************************************************************************
//
// Adjust the simobuck vddc/vddf active trim values with saturation.
// Based on current register setting
// Returns actual adjusted amount of trim code
//
//****************************************************************************
int32_t
am_hal_pwrctrl_simobuck_vddx_active_trim_adj(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code)
{
    //
    // save original SIMOBUCK1 value, it will be restored
    //
    int32_t i32ActualCode = 0;

    AM_CRITICAL_BEGIN;
    if (vddx == AM_HAL_BURST_VDDC)
    {
        int32_t i32Vddc = MCUCTRL->SIMOBUCK1_b.COREACTIVETRIM;
        i32ActualCode = i32Vddc;
        i32Vddc += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddc < 0)
        {
            i32Vddc = 0;
        }
        else if (i32Vddc > (MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Pos))
        {
            i32Vddc = (MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddc - i32ActualCode;
        MCUCTRL->SIMOBUCK1_b.COREACTIVETRIM = i32Vddc;
    }
    else if (vddx == AM_HAL_BURST_VDDF)
    {
        int32_t i32Vddf = MCUCTRL->SIMOBUCK1_b.MEMACTIVETRIM;
        i32ActualCode = i32Vddf;
        i32Vddf += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddf < 0)
        {
            i32Vddf = 0;
        }
        else if (i32Vddf > (MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Pos))
        {
            i32Vddf = (MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddf - i32ActualCode;
        MCUCTRL->SIMOBUCK1_b.MEMACTIVETRIM = i32Vddf;
    }

    am_hal_flash_delay( FLASH_CYCLES_US(20) );
    AM_CRITICAL_END;
    return i32ActualCode;
}

//****************************************************************************
//
// Adjust the LDO vddc/vddf active trim values with saturation.
// Based on current register setting
// Returns actual adjusted amount of trim code
//
//****************************************************************************
int32_t
am_hal_pwrctrl_ldo_vddx_active_trim_adj(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code)
{
    //
    // save original LDO value, it will be restored
    //
    int32_t i32ActualCode = 0;

    AM_CRITICAL_BEGIN;
    if (vddx == AM_HAL_BURST_VDDC)
    {
        int32_t i32Vddc = MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM;
        i32ActualCode = i32Vddc;
        i32Vddc += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddc < 0)
        {
            i32Vddc = 0;
        }
        else if (i32Vddc > (MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Msk >> MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Pos))
        {
            i32Vddc = (MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Msk >> MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddc - i32ActualCode;
        MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM = i32Vddc;
    }
    else if (vddx == AM_HAL_BURST_VDDF)
    {
        int32_t i32Vddf = MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM;
        i32ActualCode = i32Vddf;
        i32Vddf += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddf < 0)
        {
            i32Vddf = 0;
        }
        else if (i32Vddf > (MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk >> MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Pos))
        {
            i32Vddf = (MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk >> MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddf - i32ActualCode;
        MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = i32Vddf;
    }

    am_hal_flash_delay( FLASH_CYCLES_US(20) );
    AM_CRITICAL_END;
    return i32ActualCode;
}

//****************************************************************************
//
// Adjust the simobuck vddc/vddf active trim values with saturation.
// Based on default trim setting
// Returns actual adjusted amount of trim code
// (Requires Info1 patch5)
//
//****************************************************************************
int32_t
am_hal_pwrctrl_simobuck_vddx_active_trim_adj_default(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code)
{
    //
    // save original SIMOBUCK1 value, it will be restored
    //
    int32_t i32ActualCode = 0;

    //
    // check if info1 patch5 is present
    //
    if (!am_hal_burst_ldo_patch_check())
    {
        return 0;
    }

    AM_CRITICAL_BEGIN;
    if (vddx == AM_HAL_BURST_VDDC)
    {
        int32_t i32Vddc = simobuck_vddx_default_trim_get(AM_HAL_BURST_VDDC);
        i32ActualCode = i32Vddc;
        i32Vddc += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddc < 0)
        {
            i32Vddc = 0;
        }
        else if (i32Vddc > (MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Pos))
        {
            i32Vddc = (MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_COREACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddc - i32ActualCode;
        MCUCTRL->SIMOBUCK1_b.COREACTIVETRIM = i32Vddc;
    }
    else if (vddx == AM_HAL_BURST_VDDF)
    {
        int32_t i32Vddf = simobuck_vddx_default_trim_get(AM_HAL_BURST_VDDF);
        i32ActualCode = i32Vddf;
        i32Vddf += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddf < 0)
        {
            i32Vddf = 0;
        }
        else if (i32Vddf > (MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Pos))
        {
            i32Vddf = (MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Msk >> MCUCTRL_SIMOBUCK1_MEMACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddf - i32ActualCode;
        MCUCTRL->SIMOBUCK1_b.MEMACTIVETRIM = i32Vddf;
    }

    am_hal_flash_delay( FLASH_CYCLES_US(20) );
    AM_CRITICAL_END;
    return i32ActualCode;
}

//****************************************************************************
//
// Adjust the LDO vddc/vddf active trim values with saturation.
// Based on default trim setting
// (Requires Info1 patch5)
// Returns actual adjusted amount of trim code
//
//****************************************************************************
int32_t
am_hal_pwrctrl_ldo_vddx_active_trim_adj_default(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code)
{
    //
    // save original LDO value, it will be restored
    //
    int32_t i32ActualCode = 0;

    AM_CRITICAL_BEGIN;
    if (vddx == AM_HAL_BURST_VDDC)
    {
        int32_t i32Vddc = ldo_vddx_default_trim_get(AM_HAL_BURST_VDDC);
        i32ActualCode = i32Vddc;
        i32Vddc += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddc < 0)
        {
            i32Vddc = 0;
        }
        else if (i32Vddc > (MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Msk >> MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Pos))
        {
            i32Vddc = (MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Msk >> MCUCTRL_LDOREG1_CORELDOACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddc - i32ActualCode;
        MCUCTRL->LDOREG1_b.CORELDOACTIVETRIM = i32Vddc;
    }
    else if (vddx == AM_HAL_BURST_VDDF)
    {
        int32_t i32Vddf = ldo_vddx_default_trim_get(AM_HAL_BURST_VDDF);
        i32ActualCode = i32Vddf;
        i32Vddf += vddx_code;

        //
        // check for overflow and limit
        //
        if (i32Vddf < 0)
        {
            i32Vddf = 0;
        }
        else if (i32Vddf > (MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk >> MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Pos))
        {
            i32Vddf = (MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Msk >> MCUCTRL_LDOREG2_MEMLDOACTIVETRIM_Pos);
        }

        i32ActualCode = i32Vddf - i32ActualCode;
        MCUCTRL->LDOREG2_b.MEMLDOACTIVETRIM = i32Vddf;
    }

    am_hal_flash_delay( FLASH_CYCLES_US(20) );
    AM_CRITICAL_END;
    return i32ActualCode;
}

//****************************************************************************
//
//  Boost VDDF if CV Patch applied
//
//****************************************************************************
void
am_hal_pwrctrl_wa_vddf_boost(void)
{
    //
    // Boost VDDF value
    //
    AM_CRITICAL_BEGIN
    if (PWRCTRL->MISC_b.SIMOBUCKEN == 1)
    {
        am_hal_pwrctrl_simobuck_vddx_active_trim_adj_default(AM_HAL_BURST_VDDF, AM_HAL_BURST_BUCK_VDDF_ADJ_CODE_30MV);
    }
    else
    {
        am_hal_pwrctrl_ldo_vddx_active_trim_adj_default(AM_HAL_BURST_VDDF, AM_HAL_BURST_LDO_VDDF_ADJ_CODE_30MV);
    }
    am_hal_flash_delay( FLASH_CYCLES_US(20) );
    AM_CRITICAL_END
}

//****************************************************************************
//
//  Restore VDDF if CV Patch applied
//
//****************************************************************************
void
am_hal_pwrctrl_wa_vddf_restore(void)
{
    //
    // Restore VDDF value
    //
    AM_CRITICAL_BEGIN
    if (PWRCTRL->MISC_b.SIMOBUCKEN == 1)
    {
        am_hal_pwrctrl_simobuck_vddx_active_trim_adj_default(AM_HAL_BURST_VDDF, 0);
    }
    else
    {
        am_hal_pwrctrl_ldo_vddx_active_trim_adj_default(AM_HAL_BURST_VDDF, 0);
    }
    am_hal_flash_delay( FLASH_CYCLES_US(20) );
    AM_CRITICAL_END
}

#endif // AM_HAL_BURST_LDO_WORKAROUND

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
