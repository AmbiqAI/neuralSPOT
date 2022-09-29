//*****************************************************************************
//
//! @file am_hal_mpu.c
//!
//! @brief Hardware abstraction for the MPU.
//!
//! @addtogroup mpu_4b MPU - Memory Protection Unit
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
#include "am_hal_mpu.h"

//*****************************************************************************
//
//! @brief Returns the contents of the MPU_TYPE register
//!
//! This function accesses the ARM MPU_TYPE register. It can be used to check
//! for the presence of an MPU, and to obtain basic information about the
//! implementation of the MPU.
//!
//! @return 32-bit unsigned integer representing the contents of MPU_TYPE
//
//*****************************************************************************
uint32_t
mpu_type_get(void)
{
    return REGVAL(MPU_TYPE);
}

//*****************************************************************************
//
//! @brief Sets the global configuration of the MPU
//!
//! @param bMPUEnable - Enable the MPU
//! @param bPrivelegedDefault - Enable the default priveleged memory map
//! @param bFaultNMIProtect - Enable the MPU during fault handlers
//!
//! This function is a wrapper for the MPU_CTRL register, which controls the
//! global configuration of the MPU. This function can enable or disable the
//! MPU overall with the \e bMPUEnable parameter, and also controls how fault
//! handlers, NMI service routines, and priveleged-mode execution is handled by
//! the MPU.
//!
//! Setting \e bPrivelegedDefault will enable the default memory map for
//! priveleged accesses. If the MPU is enabled with this value set, only
//! priveleged code can execute from the system address map
//!
//! Setting \e bFaultNMIProtect leaves the MPU active during the execution of
//! NMI and Hard Fault handlers. Clearing this value will disable the MPU
//! during these procedures.
//
//*****************************************************************************
void
mpu_global_configure(bool bMPUEnable, bool bPrivelegedDefault,
                     bool bFaultNMIProtect)
{
    __DMB();
    REGVAL(MPU_CTRL) = ((bMPUEnable * 0x1) |
                        (bFaultNMIProtect * 0x2) |
                        (bPrivelegedDefault * 0x4));

    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    __DSB();
    __ISB();
}

//*****************************************************************************
//
//! @brief Configures an MPU region.
//!
//! @param psConfig
//! @param bEnableNow
//!
//! This function performs the necessary configuration for the MPU region
//! described by the \e psConfig structure, and will also enable the region if
//! the \e bEnableNow option is true.
//
//*****************************************************************************
void
mpu_region_configure(tMPURegion *psConfig, bool bEnableNow)
{
    //
    // Set the new base address for the specified region.
    //
    REGVAL(MPU_RBAR) = ((psConfig->ui32BaseAddress & 0xFFFFFFE0) |
                        (psConfig->ui8RegionNumber) |
                        (0x00000010));

    //
    // Set the attributes for this region based on the input structure.
    //
    REGVAL(MPU_RASR) = ((psConfig->bExecuteNever * 0x10000000) |
                        (psConfig->eAccessPermission << 24) |
                        (psConfig->ui16SubRegionDisable << 8) |
                        (psConfig->ui8Size << 1) |
                        (bEnableNow) |
                        (MPU_DEFAULT_TEXSCB));

}

//*****************************************************************************
//
//! @brief Enable an MPU region.
//!
//! @param ui8RegionNumber
//!
//! Enable the MPU region referred to by \e ui8RegionNumber.
//!
//! @note This function should only be called after the desired region has
//! already been configured.
//
//*****************************************************************************
void
mpu_region_enable(uint8_t ui8RegionNumber)
{
    //
    // Set the region number in the MPU_RNR register, and set the enable bit.
    //
    REGVAL(MPU_RNR) = ui8RegionNumber;
    REGVAL(MPU_RASR) |= 0x1;
}

//*****************************************************************************
//
//! @brief Disable an MPU region.
//!
//! @param ui8RegionNumber
//!
//! Disable the MPU region referred to by \e ui8RegionNumber.
//
//*****************************************************************************
void
mpu_region_disable(uint8_t ui8RegionNumber)
{
    //
    // Set the region number in the MPU_RNR register, and clear the enable bit.
    //
    REGVAL(MPU_RNR) = ui8RegionNumber;
    REGVAL(MPU_RASR) &= ~(0x1);
}

//*****************************************************************************
//
//! @brief Get the MPU region number.
//!
//! Get the MPU region number from MPU_RNR register.
//!
//! @return MPU_RNR register value.
//
//*****************************************************************************
uint32_t
mpu_get_region_number(void)
{
    return REGVAL(MPU_RNR);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

