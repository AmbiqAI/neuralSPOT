//*****************************************************************************
//
//! @file am_hal_mpu.h
//!
//! @brief Hardware abstraction for the MPU.
//!
//! @addtogroup mpu MPU - Memory Protection Unit
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

#ifndef AM_HAL_MPU_H
#define AM_HAL_MPU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// MACROs.
//
//*****************************************************************************
#define APOLLO5_MPU_REGIONS 16

//*****************************************************************************
//
// Structure definitions.
//
//*****************************************************************************

//
//! @brief Enum type for specifying memory access shareable for an MPU region.
//
typedef enum
{
    NON_SHARE = ARM_MPU_SH_NON,
    OUTER_SHARE = ARM_MPU_SH_OUTER,
    INNER_SHARE = ARM_MPU_SH_INNER
}
am_hal_mpu_shareable_t;

//
//! @brief Enum type for specifying memory access priveleges for an MPU region.
//
typedef enum
{
    RW_PRIV       = ARM_MPU_AP_(0, 0),
    RW_NONPRIV    = ARM_MPU_AP_(0, 1),
    RO_PRIV       = ARM_MPU_AP_(1, 0),
    RO_NONPRIV    = ARM_MPU_AP_(1, 1)
}
am_hal_mpu_access_permission_t;

//
//! @brief Configuration structure for MPU regions.
//
typedef struct
{
    uint32_t ui32RegionNumber;
    uint32_t ui32BaseAddress;
    am_hal_mpu_shareable_t eShareable;
    am_hal_mpu_access_permission_t eAccessPermission;
    bool bExecuteNever;
    uint32_t ui32LimitAddress;
    uint32_t ui32AttrIndex;
    bool bEnable;
}
am_hal_mpu_region_config_t;

//
//! @brief Attribute for normal memory (outer and inner).
//
typedef struct
{
    bool bNonTransient;
    bool bWriteBack;
    bool bReadAllocate;
    bool bWriteAllocate;
}
am_hal_mpu_normal_mem_attr_t;

//
//! @brief Attribute for device memory.
//
typedef enum
{
    DEV_nGnRnE = ARM_MPU_ATTR_DEVICE_nGnRnE,
    DEV_nGnRE  =  ARM_MPU_ATTR_DEVICE_nGnRE,
    DEV_nGRE   =   ARM_MPU_ATTR_DEVICE_nGRE,
    DEV_GRE    =    ARM_MPU_ATTR_DEVICE_GRE
}
am_hal_mpu_device_mem_attr_t;

//
//! @brief Configuration structure for MPU attributes.
//
typedef struct
{
    uint8_t ui8AttrIndex;
    bool bNormalMem;
    am_hal_mpu_normal_mem_attr_t sOuterAttr;
    am_hal_mpu_normal_mem_attr_t sInnerAttr;
    am_hal_mpu_device_mem_attr_t eDeviceAttr;
}
am_hal_mpu_attr_t;

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Returns the amount of MPU D regions
//!
//! This function can be used to get amount of D regions
//!
//! @return Amount of D regions
//
//*****************************************************************************
extern uint32_t am_hal_mpu_dregion_amount_get(void);

//*****************************************************************************
//
//! @brief Disable the MPU
//!
//! This function can disable the MPU overall.
//!
//
//*****************************************************************************
extern uint32_t am_hal_mpu_disable(void);

//*****************************************************************************
//
//! @brief Enable the MPU
//!
//! @param bPrivelegedDefault - Enable the default priveleged memory map
//! @param bFaultNMIProtect - Enable the MPU during fault handlers
//!
//! This function is a wrapper for the MPU_CTRL register, which controls the
//! global configuration of the MPU. This function can enable the
//! MPU overall, and also controls how fault
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
extern uint32_t am_hal_mpu_enable(bool bPrivelegedDefault,
                              bool bFaultNMIProtect);

//*****************************************************************************
//
//! @brief Configure MPU attributes.
//!
//! @param psConfig - MPU attibutes
//! @param ui32Cnt  - Amount of attribute combinations
//!
//! This function performs the necessary configuration for the MPU attribute
//! described by the \e psConfig structure, and will configure multiple groups
//! of attributes if the \e ui32Cnt is greater than 1.
//
//*****************************************************************************
extern uint32_t am_hal_mpu_attr_configure(am_hal_mpu_attr_t *psConfig, uint32_t ui32Cnt);

//*****************************************************************************
//
//! @brief Configure MPU regions.
//!
//! @param psConfig - MPU region configurations
//! @param ui32Cnt  - Amount of regions to be configured
//!
//! This function performs the necessary configuration for the MPU region
//! described by the \e psConfig structure, and will configure multiple regions
//! if the \e ui32Cnt is greater than 1.
//
//*****************************************************************************
extern uint32_t am_hal_mpu_region_configure(am_hal_mpu_region_config_t *psConfig, uint32_t ui32Cnt);

//*****************************************************************************
//
//! @brief Enable an MPU region.
//!
//! @param ui8RegionNumber - Index of region to be enabled
//!
//! Enable the MPU region referred to by \e ui8RegionNumber.
//!
//! @note This function should only be called after the desired region has
//! already been configured.
//
//*****************************************************************************
extern uint32_t am_hal_mpu_region_enable(uint8_t ui8RegionNumber);

//*****************************************************************************
//
//! @brief Disable an MPU region.
//!
//! @param ui8RegionNumber - Index of region to be disabled
//!
//! Disable the MPU region referred to by \e ui8RegionNumber.
//
//*****************************************************************************
extern uint32_t am_hal_mpu_region_disable(uint8_t ui8RegionNumber);

//*****************************************************************************
//
//! @brief Clear all MPU regions
//!
//! @return None.
//
//*****************************************************************************
extern uint32_t am_hal_mpu_region_clear(void);

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

