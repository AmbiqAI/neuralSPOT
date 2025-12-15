//*****************************************************************************
//
//! @file am_hal_vcomp.h
//!
//! @brief Functions for operating the on-chip Voltage Comparator
//!
//! @addtogroup vcomp5_ap510L Voltage Comparator (VCOMP)
//! @ingroup apollo510L_hal
//! @{
//!
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
#ifndef AM_HAL_VCOMP_H
#define AM_HAL_VCOMP_H

//*****************************************************************************
//
//! @name Interrupt bit fields
//! @brief Interrupt bit fields for interrupt related registers configuration
//!
//! These macros may be used to set and clear interrupt bits
//
//*****************************************************************************
#define AM_HAL_VCOMP_INT_OUTLOW_Msk  (1 << VCOMP_INTSTAT_OUTLOW_Pos)
#define AM_HAL_VCOMP_INT_OUTHI_Msk   (1 << VCOMP_INTSTAT_OUTHI_Pos)
#define AM_HAL_VCOMP_INT_ALL_Msk     (AM_HAL_VCOMP_INT_OUTLOW_Msk |    \
                                      AM_HAL_VCOMP_INT_OUTHI_Msk)

//*****************************************************************************
//
//! @name VCOMP Status Bits
//! @brief Status bit for the VCOM STAT register
//!
//! These macros may be used to monitor VCOM STAT register
//
//*****************************************************************************
#define AM_HAL_VCOMP_STAT_CMPOUT                1 << VCOMP_STAT_CMPOUT_Pos
#define AM_HAL_VCOMP_STAT_PWDSTAT               1 << VCOMP_STAT_PWDSTAT_Pos

//*****************************************************************************
//
// Key values.
//
//*****************************************************************************
#define AM_REG_VCOMP_PWDKEY_KEYVAL                   0x00000037

//*****************************************************************************
//
//! @name Positive Input Selection Enum
//! @brief Enum for the positive input selection to the VCOMP
//
//*****************************************************************************
typedef enum
{
    AM_HAL_VCOMP_POSSEL_VDDADJ,
    AM_HAL_VCOMP_POSSEL_VTEMP,
    AM_HAL_VCOMP_POSSEL_VEXT1,
    AM_HAL_VCOMP_POSSEL_VEXT2,
} am_hal_vcomp_possel_e;

//*****************************************************************************
//
//! @name Negative Input Selection Enum
//! @brief Enum for the negative input selection to the VCOMP
//
//*****************************************************************************
typedef enum
{
    AM_HAL_VCOMP_NEGSEL_VREFEXT1,
    AM_HAL_VCOMP_NEGSEL_VREFEXT2,
    AM_HAL_VCOMP_NEGSEL_VREFEXT3,
    AM_HAL_VCOMP_NEGSEL_DAC,
} am_hal_vcomp_negsel_e;

//*****************************************************************************
//
//! @name Negative Input DAC Selection Enum
//! @brief Enum for the DAC reference input selection if DAC is selected as
//!        the negative input selection
//
//*****************************************************************************
typedef enum
{
    AM_HAL_VCOMP_DAC_LVLSEL_0P58V,
    AM_HAL_VCOMP_DAC_LVLSEL_0P77V,
    AM_HAL_VCOMP_DAC_LVLSEL_0P97V,
    AM_HAL_VCOMP_DAC_LVLSEL_1P16V,
    AM_HAL_VCOMP_DAC_LVLSEL_1P35V,
    AM_HAL_VCOMP_DAC_LVLSEL_1P55V,
    AM_HAL_VCOMP_DAC_LVLSEL_1P74V,
    AM_HAL_VCOMP_DAC_LVLSEL_1P93V,
    AM_HAL_VCOMP_DAC_LVLSEL_2P13V,
} am_hal_vcomp_dac_lvlsel_e;

//*****************************************************************************
//
//! @brief Configuration struct
//
//*****************************************************************************
typedef struct
{
    //
    //! The DAC level setting
    //
    am_hal_vcomp_dac_lvlsel_e eLevelSelect;

    //
    //! The "positive" comparator input channel
    //! This channel is usually used as the signal to be monitored.
    //
    am_hal_vcomp_possel_e ePosInput;

    //
    //! The "negative" comparator input channel
    //! This channel is usually used as the reference signal.
    //
    am_hal_vcomp_negsel_e eNegInput;
}
am_hal_vcomp_config_t;

//*****************************************************************************
//
//! @brief Default structrue for the configuration struct
//!        Use VDDADJ for positive input
//!        Use DAC for negative input
//!        Use 0.58V as the voltage level for DAC
//
//*****************************************************************************
#define AM_HAL_VCOMP_CONFIG_DEFAULT             \
{                                               \
    AM_HAL_VCOMP_DAC_LVLSEL_0P58V,              \
    AM_HAL_VCOMP_POSSEL_VDDADJ,                 \
    AM_HAL_VCOMP_NEGSEL_DAC,                    \
}

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// External function definitions
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Configure the Voltage Comparator module.
//!
//! @param psConfig is a structure containing configuration information for the
//! voltage comparator.
//!
//! This function configures the positive and negative input signals for the
//! voltage comparator.
//!
//! @return AM_HAL_STATUS_SUCCESS or AM_HAL_STATUS_FAIL
//
//*****************************************************************************
extern am_hal_status_e am_hal_vcomp_config(const am_hal_vcomp_config_t *psConfig);

//*****************************************************************************
//
//! @brief Set the Voltage Comparator DAC Level Select in Configuration Reg.
//!
//! @param eLevelSelect - DAC voltage selector (use macros enumerations)
//!
//! This function sets the DAC level select in the configuration register.
//!
//! @return AM_HAL_STATUS_SUCCESS or AM_HAL_STATUS_FAIL
//
//*****************************************************************************
extern am_hal_status_e am_hal_vcomp_dac_level_set(am_hal_vcomp_dac_lvlsel_e eLevelSelect);

//*****************************************************************************
//
//! @brief Read the state of the voltage comparator.
//!
//! This function extracts the comparator state from the status register.
//!
//! @return the voltage comparator state
//
//*****************************************************************************
extern uint32_t am_hal_vcomp_status_get(void);

//*****************************************************************************
//
//! @brief Enable the voltage comparator.
//!
//! This function powers up the voltage comparator.
//!
//! @return None
//
//*****************************************************************************
extern void am_hal_vcomp_enable(void);

//*****************************************************************************
//
//! @brief Disable the voltage comparator.
//!
//! This function powers down the voltage comparator.
//!
//! @return None
//
//*****************************************************************************
extern void am_hal_vcomp_disable(void);

//*****************************************************************************
//
//! @brief Enable the voltage comparator interrupt status bits.
//!
//! @param ui32Interrupt - interrupts to be enabled.
//!
//! This function enables desired interrupt status bits.
//!
//! ui32Interrupt should be a logical or of:
//!
//! AM_HAL_VCOMP_INT_OUTLOW_EN
//! AM_HAL_VCOMP_INT_OUTHI_EN
//!
//! @return AM_HAL_STATUS_SUCCESS or AM_HAL_STATUS_FAIL
//
//*****************************************************************************
extern am_hal_status_e am_hal_vcomp_int_enable(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Return the enabled, voltage comparator interrupt status bits.
//!
//! This function returns the enabled interrupt status bits
//!
//! @return returns the enabled interrupt status bits. The return is a logical
//! or of:
//!
//! AM_HAL_VCOMP_INT_OUTLOW_STAT
//! AM_HAL_VCOMP_INT_OUTHI_STAT
//
//*****************************************************************************
extern uint32_t am_hal_vcomp_int_enable_get(void);

//*****************************************************************************
//
//! @brief Disable the voltage comparator interrupt status bits.
//!
//! @param ui32Interrupt - interrupts to be disabled.
//!
//! This function disables desired interrupt status bits.
//!
//! ui32Interrupt should be a logical or of:
//!
//! AM_HAL_VCOMP_INT_OUTLOW_SET
//! AM_HAL_VCOMP_INT_OUTHI_SET
//!
//! @return AM_HAL_STATUS_SUCCESS or AM_HAL_STATUS_FAIL
//
//*****************************************************************************
extern am_hal_status_e am_hal_vcomp_int_disable(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Clear the state of the voltage comparator interrupt status bits.
//!
//! @param ui32Interrupt - interrupts to be cleared.
//!
//! This function clears the specified interrupt status bits.
//!
//! ui32Interrupt should be a logical or of:
//!
//! AM_HAL_VCOMP_INT_OUTLOW_CLR
//! AM_HAL_VCOMP_INT_OUTHI_CLR
//!
//! @return AM_HAL_STATUS_SUCCESS or AM_HAL_STATUS_FAIL
//
//*****************************************************************************
extern am_hal_status_e am_hal_vcomp_int_clear(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Set the state of the voltage comparator interrupt status bits.
//!
//! @param ui32Interrupt - interrupts to be set.
//!
//! This function sets the specified interrupt status bits.
//!
//! ui32Interrupt should be a logical or of:
//!
//! AM_HAL_VCOMP_INT_OUTLOW_SET
//! AM_HAL_VCOMP_INT_OUTHI_SET
//!
//! @return AM_HAL_STATUS_SUCCESS or AM_HAL_STATUS_FAIL
//
//*****************************************************************************
extern am_hal_status_e am_hal_vcomp_int_set(uint32_t ui32Interrupt);

//*****************************************************************************
//
//! @brief Read the state of the voltage comparator interrupt status bits.
//!
//! @param bEnabledOnly - return the status of only the enabled interrupts.
//!
//! This function extracts the interrupt status bits and returns the raw or
//! only the enabled based on bEnabledOnly.
//!
//! @return Bitwise representation of the current interrupt status.
//!
//! The return value will be the logical OR of one or more of the following
//! values:
//!
//! AM_HAL_VCOMP_INT_OUTLOW_STAT
//! AM_HAL_VCOMP_INT_OUTHI_STAT
//
//*****************************************************************************
extern uint32_t am_hal_vcomp_int_status_get(bool bEnabledOnly);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_VCOMP_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
