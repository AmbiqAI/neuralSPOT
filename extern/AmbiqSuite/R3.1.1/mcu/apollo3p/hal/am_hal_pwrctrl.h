//*****************************************************************************
//
//! @file am_hal_pwrctrl.h
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

#ifndef AM_HAL_PWRCTRL_H
#define AM_HAL_PWRCTRL_H

//
//! Designate this peripheral.
//
#define AM_APOLLO3_PWRCTRL  1

//
//! Peripheral Type
//
typedef enum
{
    AM_HAL_PWRCTRL_PERIPH_NONE,
    AM_HAL_PWRCTRL_PERIPH_IOS,
    AM_HAL_PWRCTRL_PERIPH_IOM0,
    AM_HAL_PWRCTRL_PERIPH_IOM1,
    AM_HAL_PWRCTRL_PERIPH_IOM2,
    AM_HAL_PWRCTRL_PERIPH_IOM3,
    AM_HAL_PWRCTRL_PERIPH_IOM4,
    AM_HAL_PWRCTRL_PERIPH_IOM5,
    AM_HAL_PWRCTRL_PERIPH_UART0,
    AM_HAL_PWRCTRL_PERIPH_UART1,
    AM_HAL_PWRCTRL_PERIPH_ADC,
    AM_HAL_PWRCTRL_PERIPH_SCARD,
    AM_HAL_PWRCTRL_PERIPH_MSPI0,
    AM_HAL_PWRCTRL_PERIPH_MSPI1,
    AM_HAL_PWRCTRL_PERIPH_MSPI2,
    AM_HAL_PWRCTRL_PERIPH_PDM,
    AM_HAL_PWRCTRL_PERIPH_BLEL,
    AM_HAL_PWRCTRL_PERIPH_MAX
} am_hal_pwrctrl_periph_e;

//
//! Momory Type
//
typedef enum
{
    AM_HAL_PWRCTRL_MEM_NONE,
    AM_HAL_PWRCTRL_MEM_SRAM_8K_DTCM,
    AM_HAL_PWRCTRL_MEM_SRAM_32K_DTCM,
    AM_HAL_PWRCTRL_MEM_SRAM_64K_DTCM,
    AM_HAL_PWRCTRL_MEM_SRAM_128K,
    AM_HAL_PWRCTRL_MEM_SRAM_192K,
    AM_HAL_PWRCTRL_MEM_SRAM_256K,
    AM_HAL_PWRCTRL_MEM_SRAM_320K,
    AM_HAL_PWRCTRL_MEM_SRAM_384K,
    AM_HAL_PWRCTRL_MEM_SRAM_448K,
    AM_HAL_PWRCTRL_MEM_SRAM_512K,
    AM_HAL_PWRCTRL_MEM_SRAM_576K,
    AM_HAL_PWRCTRL_MEM_SRAM_672K,
    AM_HAL_PWRCTRL_MEM_SRAM_768K,
    AM_HAL_PWRCTRL_MEM_FLASH_1M,
    AM_HAL_PWRCTRL_MEM_FLASH_2M,
    AM_HAL_PWRCTRL_MEM_CACHE,
    AM_HAL_PWRCTRL_MEM_ALL,
    AM_HAL_PWRCTRL_MEM_MAX
} am_hal_pwrctrl_mem_e;

#define AM_HAL_PWRCTRL_MEM_FLASH_MIN    AM_HAL_PWRCTRL_MEM_FLASH_1M
#define AM_HAL_PWRCTRL_MEM_FLASH_MAX    AM_HAL_PWRCTRL_MEM_FLASH_2M

#define AM_HAL_PWRCTRL_MEM_SRAM_MIN     AM_HAL_PWRCTRL_MEM_SRAM_8K_DTCM
#define AM_HAL_PWRCTRL_MEM_SRAM_MAX     AM_HAL_PWRCTRL_MEM_SRAM_768K

//*****************************************************************************
//
//! @name Macros to check whether Apollo3 bucks are enabled.
//!
//! @{
//
//*****************************************************************************
#define am_hal_pwrctrl_simobuck_enabled_check()                             \
        (PWRCTRL->SUPPLYSTATUS_b.SIMOBUCKON)

#define am_hal_pwrctrl_blebuck_enabled_check()                              \
        (PWRCTRL->SUPPLYSTATUS_b.BLEBUCKON)

//
//! @}
//

//*****************************************************************************
//
//! @brief Option to Enable vddf Boost workaound for APB Hang issue
//!
//! APB bus will hang with
//!
//! AM_HAL_PWRCTRL_VDDF_BOOST_WA:
//!  Default: Defined
//
//*****************************************************************************
#define AM_HAL_PWRCTRL_VDDF_BOOST_WA

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Enable power to a peripheral.
//!
//! @param ePeripheral - The peripheral to enable.
//!
//! This function enables power to the peripheral and waits for a
//! confirmation from the hardware.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_periph_enable(am_hal_pwrctrl_periph_e ePeripheral);

//*****************************************************************************
//
//! @brief Disable power to a peripheral.
//!
//! @param ePeripheral - The peripheral to disable.
//!
//! This function disables power to the peripheral and waits for a
//! confirmation from the hardware.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_periph_disable(am_hal_pwrctrl_periph_e ePeripheral);

//*****************************************************************************
//
//! @brief Determine whether a peripheral is currently enabled.
//!
//! @param ePeripheral  - The peripheral to enable.
//! @param pui32Enabled - Pointer to a ui32 that will return as 1 or 0.
//!
//! This function determines to the caller whether a given peripheral is
//! currently enabled or disabled.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_periph_enabled(
                am_hal_pwrctrl_periph_e ePeripheral, uint32_t *pui32Enabled);

//*****************************************************************************
//
//! @brief Enable a configuration of memory.
//!
//! @param eMemConfig - The memory configuration.
//!
//! This function establishes the desired configuration of flash, SRAM, ICache,
//! and DCache (DTCM) according to the desired Memory Configuration mask.
//!
//! @note Only the type of memory specified is affected. Therefore separate calls
//! are required to affect power settings for FLASH, SRAM, or CACHE.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_memory_enable(am_hal_pwrctrl_mem_e eMemConfig);

//*****************************************************************************
//
//! @brief Power down respective memory.
//!
//! @param eMemConfig - The memory power down enum.
//!
//! This function establishes the desired power down of flash, SRAM, ICache,
//! and DCache (DTCM) according to the desired enum.
//!
//! @note Only the type of memory specified is affected. Therefore separate calls
//! are required to affect power settings for FLASH, SRAM, or CACHE.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_memory_deepsleep_powerdown(am_hal_pwrctrl_mem_e eMemConfig);

//*****************************************************************************
//
//! @brief Apply retention voltage to respective memory.
//!
//! @param eMemConfig - The memory power down enum.
//!
//! This function establishes the desired power retain of flash, SRAM, ICache,
//! and DCache (DTCM) according to the desired enum.
//!
//! @note Only the type of memory specified is affected. Therefore separate calls
//! are required to affect power settings for FLASH, SRAM, or CACHE.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_memory_deepsleep_retain(am_hal_pwrctrl_mem_e eMemConfig);

//*****************************************************************************
//
//! @brief Initialize system for low power configuration.
//!
//! This function handles low power initialization.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_pwrctrl_low_power_init(void);

//*****************************************************************************
//
//! @brief Initialize BLE Buck Trims for Lowest Power.
//
//*****************************************************************************
extern void am_hal_pwrctrl_blebuck_trim(void);

#if AM_HAL_BURST_LDO_WORKAROUND

typedef enum
{
    AM_HAL_BURST_VDDC,
    AM_HAL_BURST_VDDF
} am_hal_burst_voltage_wa_e;

//****************************************************************************
//
//! @brief Adjust the simobuck vddc/vddf active trim value with saturation.
//!         based on current register setting
//!
//! @param vddx      - Voltage to be trimmed:
//!                    AM_HAL_BURST_VDDC,
//!                    AM_HAL_BURST_VDDF
//!
//! @param vddx_code - Current Value to trim
//!
//! @return int32_t actual adjusted trim code
//
//****************************************************************************
extern int32_t am_hal_pwrctrl_simobuck_vddx_active_trim_adj(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code);

//****************************************************************************
//
//! @brief Adjust the ldo vddc/vddf active trim values with saturation.
//!         based on current register setting
//!
//! @param vddx      - Voltage to be trimmed:
//!                    AM_HAL_BURST_VDDC,
//!                    AM_HAL_BURST_VDDF
//!
//! @return int32_t actual adjusted trim code
//
//****************************************************************************
extern int32_t am_hal_pwrctrl_ldo_vddx_active_trim_adj(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code);

//****************************************************************************
//
//! @brief Adjust the simobuck vddc/vddf active trim values with saturation.
//!         based on current register setting
//!
//! @note Requires Info1 patch5
//!
//! @param vddx      - Voltage to be trimmed:
//!                    AM_HAL_BURST_VDDC,
//!                    AM_HAL_BURST_VDDF
//!
//! @return int32_t actual adjusted trim code
//
//****************************************************************************
extern int32_t am_hal_pwrctrl_simobuck_vddx_active_trim_adj_default(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code);

//****************************************************************************
//
//! @brief Adjust the LDO vddc/vddf active trim values with saturation.
//!         based on current register setting
//!
//! @note Requires Info1 patch5
//!
//! @param vddx      - Voltage to be trimmed:
//!                    AM_HAL_BURST_VDDC,
//!                    AM_HAL_BURST_VDDF
//!
//! @return int32_t actual adjusted trim code
//
//****************************************************************************
extern int32_t am_hal_pwrctrl_ldo_vddx_active_trim_adj_default(am_hal_burst_voltage_wa_e vddx, int32_t vddx_code);

//*****************************************************************************
//
//! @brief Boost VDDF if CV Patch applied
//
//*****************************************************************************
extern void am_hal_pwrctrl_wa_vddf_boost(void);

//*****************************************************************************
//
//! @brief Restore VDDF if CV Patch applied
//
//*****************************************************************************
extern void am_hal_pwrctrl_wa_vddf_restore(void);

#endif // AM_HAL_BURST_LDO_WORKAROUND

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_PWRCTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
