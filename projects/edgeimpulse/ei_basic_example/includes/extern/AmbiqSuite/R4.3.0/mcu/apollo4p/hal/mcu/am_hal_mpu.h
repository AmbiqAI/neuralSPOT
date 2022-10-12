//*****************************************************************************
//
//! @file am_hal_mpu.h
//!
//! @brief Hardware abstraction for the MPU.
//!
//! @addtogroup mpu MPU - Memory Protection Unit
//! @ingroup apollo4p_hal
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

#ifndef AM_HAL_MPU_H
#define AM_HAL_MPU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
//*****************************************************************************
//
// Macro definitions for MPU Registers
//
//*****************************************************************************
#define MPU_TYPE                0xE000ED90 // MPU Type
#define MPU_CTRL                0xE000ED94 // MPU Control
#define MPU_RNR                 0xE000ED98 // MPU Region Number
#define MPU_RBAR                0xE000ED9C // MPU Region Base Address
#define MPU_RASR                0xE000EDA0 // MPU Region AttrSize

// MPU Alias registers.
#define MPU_RBAR_A1             0xE000EDA4
#define MPU_RASR_A1             0xE000EDA8
#define MPU_RBAR_A2             0xE000EDAC
#define MPU_RASR_A2             0xE000EDB0
#define MPU_RBAR_A3             0xE000EDB4
#define MPU_RASR_A3             0xE000EDB8

//! Default value to use for TEX,S,C,B
#define MPU_DEFAULT_TEXSCB      0x00050000

// Macro definitions for system control block registers
#define AM_REG_SYSCTRL_MMFSR    0xE000ED28
#define AM_REG_SYSCTRL_MMFAR    0xE000ED34

//! Simple macro for accessing registers
#define REGVAL(x)               (*(volatile uint32_t *)(x))

//*****************************************************************************
//
// External variable definitions
//
//*****************************************************************************

//*****************************************************************************
//
// Structure definitions.
//
//*****************************************************************************

//
//! @brief Enum type for specifying memory access priveleges for an MPU region.
//
typedef enum
{
    NO_ACCESS       = 0x0,
    PRIV_RW         = 0x1,
    PRIV_RW_PUB_RO  = 0x2,
    PRIV_RW_PUB_RW  = 0x3,
    PRIV_RO         = 0x5,
    PRIV_RO_PUB_RO  = 0x6
}
tAccessPermission;

//
//! @brief Configuration structure for MPU regions.
//
typedef struct
{
    uint8_t ui8RegionNumber;
    uint32_t ui32BaseAddress;
    uint8_t ui8Size;
    tAccessPermission eAccessPermission;
    bool bExecuteNever;
    uint16_t ui16SubRegionDisable;
}
tMPURegion;

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

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
extern uint32_t mpu_type_get(void);

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
extern void mpu_global_configure(bool bMPUEnable, bool bPrivelegedDefault,
                                 bool bFaultNMIProtect);

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
extern void mpu_region_configure(tMPURegion *psConfig, bool bEnableNow);

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
extern void mpu_region_enable(uint8_t ui8RegionNumber);

//*****************************************************************************
//
//! @brief Disable an MPU region.
//!
//! @param ui8RegionNumber
//!
//! Disable the MPU region referred to by \e ui8RegionNumber.
//
//*****************************************************************************
extern void mpu_region_disable(uint8_t ui8RegionNumber);

//*****************************************************************************
//
//! @brief Get the MPU region number.
//!
//! Get the MPU region number from MPU_RNR register.
//!
//! @return MPU_RNR register value.
//
//*****************************************************************************
extern uint32_t mpu_get_region_number(void);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_MPU_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

