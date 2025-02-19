//*****************************************************************************
//
//! @file am_hal_wdt.h
//!
//! @brief Hardware abstraction layer for the Watchdog Timer module.
//!
//! @addtogroup wdt3 WDT - Watchdog Timer
//! @ingroup apollo3_hal
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
#ifndef AM_HAL_WDT_H
#define AM_HAL_WDT_H

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************

//*****************************************************************************
//
//! @name WDT Clock Divider Selections.
//! @brief Macro definitions for WDT clock frequencies.
//!
//! These macros may be used with the am_hal_wdt_config_t structure to set the
//! clock frequency of the watch dog timer.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_WDT_LFRC_CLK_DEFAULT (_VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_128HZ))
#define AM_HAL_WDT_LFRC_CLK_128HZ   (_VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_128HZ))
#define AM_HAL_WDT_LFRC_CLK_16HZ    (_VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_16HZ))
#define AM_HAL_WDT_LFRC_CLK_1HZ     (_VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_1HZ))
#define AM_HAL_WDT_LFRC_CLK_1_16HZ  (_VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_1_16HZ))
#define AM_HAL_WDT_LFRC_CLK_OFF     (_VAL2FLD(WDT_CFG_CLKSEL, WDT_CFG_CLKSEL_OFF))
//! @}

//*****************************************************************************
//
//! @name WDT Enable Reset in the WDT Configuration.
//! @brief Macro definitions for WDT Reset Enable.
//!
//! These macros may be used with the am_hal_wdt_config_t structure to enable
//! the watch dog timer to generate resets to the chip.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_WDT_ENABLE_RESET      (_VAL2FLD(WDT_CFG_RESEN, 1))
#define AM_HAL_WDT_DISABLE_RESET     (_VAL2FLD(WDT_CFG_RESEN, 0))
//! @}

//*****************************************************************************
//
//! @name WDT Enable Interrupt Generation from the WDT Configuration.
//! @brief Macro definitions for WDT Interrupt Enable.
//!
//! These macros may be used with the am_hal_wdt_config_t structure to enable
//! the watch dog timer to generate generate WDT interrupts.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_WDT_ENABLE_INTERRUPT      (_VAL2FLD(WDT_CFG_INTEN, 1))
#define AM_HAL_WDT_DISABLE_INTERRUPT     (_VAL2FLD(WDT_CFG_INTEN, 0))
//! @}

//*****************************************************************************
//
//! @brief Watchdog timer configuration structure.
//!
//! This structure is made to be used with the am_hal_wdt_init() function. It
//! describes the configuration of the watchdog timer.
//
//*****************************************************************************
typedef struct
{
    //! Configuration Values for watchdog timer
    //! event is generated.
    uint32_t ui32Config;

    //! Number of watchdog timer ticks allowed before a watchdog interrupt
    //! event is generated.
    uint16_t ui16InterruptCount;

    //! Number of watchdog timer ticks allowed before the watchdog will issue a
    //! system reset.
    uint16_t ui16ResetCount;

}
am_hal_wdt_config_t;

//*****************************************************************************
//
//! @brief Restarts the watchdog timer ("Pets" the dog)
//!
//! This function restarts the watchdog timer from the beginning, preventing
//! any interrupt or reset even from occuring until the next time the watchdog
//! timer expires.
//!
//! @return None.
//
//*****************************************************************************
#define am_hal_wdt_restart()                                                  \
    do                                                                        \
    {                                                                         \
        WDT->RSTRT = WDT_RSTRT_RSTRT_KEYVALUE;                                \
        (void)(WDT->RSTRT);                                                   \
    }                                                                         \
    while (0)

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
//! @brief Configure the watchdog timer.
//!
//! @param psConfig - pointer to a configuration structure containing the
//! desired watchdog settings.
//!
//! This function will set the watchdog configuration register based on the
//! user's desired settings listed in the structure referenced by psConfig. If
//! the structure indicates that watchdog interrupts are desired, this function
//! will also set the interrupt enable bit in the configuration register.
//!
//! @note In order to actually receive watchdog interrupt and/or watchdog reset
//! events, the caller will also need to make sure that the watchdog interrupt
//! vector is enabled in the ARM NVIC, and that watchdog resets are enabled in
//! the reset generator module. Otherwise, the watchdog-generated interrupt and
//! reset events will have no effect.
//
//*****************************************************************************
extern void am_hal_wdt_init(const am_hal_wdt_config_t *psConfig);

//*****************************************************************************
//
//! @brief Starts the watchdog timer.
//!
//! Enables the watchdog timer tick using the 'enable' bit in the watchdog
//! configuration register.  This function does not perform any locking of the
//! watchdog timer, so it can be disabled or reconfigured later.
//
//*****************************************************************************
extern void am_hal_wdt_start(void);

//*****************************************************************************
//
//! @brief Stops the watchdog timer.
//!
//! Disables the watchdog timer tick by clearing the 'enable' bit in the
//! watchdog configuration register.
//
//*****************************************************************************
extern void am_hal_wdt_halt(void);

//*****************************************************************************
//
//! @brief Locks the watchdog configuration and starts the watchdog timer.
//!
//! This function sets the watchdog "lock" register, which prevents software
//! from re-configuring the watchdog. This action will also set the enable bit
//! for the watchdog timer, so it will start counting immediately.
//
//*****************************************************************************
extern void am_hal_wdt_lock_and_start(void);
//*****************************************************************************
//
//! @brief Get the wdt counter value.
//!
//! @details This function reads the current value of watch dog timer counter register.
//
//*****************************************************************************
extern uint32_t am_hal_wdt_counter_get(void);
//*****************************************************************************
//
//! @brief Enable the wdt interrupt.
//!
//! This function enable the interrupt.
//
//*****************************************************************************
extern void am_hal_wdt_int_enable(void);

//*****************************************************************************
//
//! @brief Return the enabled WDT interrupts.
//!
//! This function returns the enabled WDT interrupts.
//!
//! @return enabled WDT interrupts.
//
//*****************************************************************************
extern uint32_t am_hal_wdt_int_enable_get(void);

//*****************************************************************************
//
//! @brief Disable the wdt interrupt.
//!
//! This function disablee the interrupt.
//
//*****************************************************************************
extern void am_hal_wdt_int_disable(void);

//*****************************************************************************
//
//! @brief Clear the state of the wdt interrupt status bit.
//!
//! This function clear the interrupt bit.
//
//*****************************************************************************
extern void am_hal_wdt_int_clear(void);

//*****************************************************************************
//
//! @brief Set the state of the wdt interrupt status bit.
//!
//! This function sets the interrupt bit.
//
//*****************************************************************************
extern void am_hal_wdt_int_set(void);

//*****************************************************************************
//
//! @brief Read the state of the wdt interrupt status.
//!
//! @param bEnabledOnly - return the status of only the enabled interrupts.
//!
//! This function extracts the interrupt status bits and returns the enabled or
//! raw based on bEnabledOnly.
//!
//! @return WDT interrupt status.
//
//*****************************************************************************
extern uint32_t am_hal_wdt_int_status_get(bool bEnabledOnly);
#ifdef __cplusplus
}
#endif

#endif // AM_HAL_WDT_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
