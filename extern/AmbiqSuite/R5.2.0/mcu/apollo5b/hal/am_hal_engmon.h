//*****************************************************************************
//
//! @file am_hal_engmon.h
//!
//! @brief Functions for using the Energy Monitor functions of the PWRCTRL
//!
//! @addtogroup engmon ENGMON - Energy Monitor
//! @ingroup apollo5b_hal
//! @{

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

#ifndef AM_HAL_ENGMON_H
#define AM_HAL_ENGMON_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Energy monitor settings.
//
//*****************************************************************************
typedef enum
{
    //! Never increment the counter
    AM_HAL_ENGMON_MONITOR_NEVER        = PWRCTRL_EMONCFG0_EMONSEL0_NEVER,

    //! Always increment the counter
    AM_HAL_ENGMON_MONITOR_ALWAYS       = PWRCTRL_EMONCFG0_EMONSEL0_ALWAYS,

    //! Increment the counter for MCU sleep mode
    AM_HAL_ENGMON_MONITOR_MCUSLEEP     = PWRCTRL_EMONCFG0_EMONSEL0_MCUSLEEP,

    //! Increment the counter for MCU deepsleep mode
    AM_HAL_ENGMON_MONITOR_MCUDEEPSLEEP = PWRCTRL_EMONCFG0_EMONSEL0_MCUDEEPSLEEP,

    //! Increment the counter for DSP0 active mode
    AM_HAL_ENGMON_MONITOR_DSP0ON       = PWRCTRL_EMONCFG0_EMONSEL0_DSP0ON,

    //! Increment the counter for DSP1 active mode
    AM_HAL_ENGMON_MONITOR_DSP1ON       = PWRCTRL_EMONCFG0_EMONSEL0_DSP1ON,

    //! Increment the counter when ADC is powered on
    AM_HAL_ENGMON_MONITOR_ADCON        = PWRCTRL_EMONCFG0_EMONSEL0_ADCON,

    //! Increment the counter when AUDPB is powered on
    AM_HAL_ENGMON_MONITOR_AUDPBON        = PWRCTRL_EMONCFG0_EMONSEL0_AUDPBON,

    //! Increment the counter when AUDREC is powered on
    AM_HAL_ENGMON_MONITOR_ADRECON        = PWRCTRL_EMONCFG0_EMONSEL0_AUDRECON,

    //! Increment the counter when I2S0 is powered on
    AM_HAL_ENGMON_MONITOR_I2S0ON        = PWRCTRL_EMONCFG0_EMONSEL0_I2S0ON,

    //! Increment the counter when I2S1 is powered on
    AM_HAL_ENGMON_MONITOR_I2S1ON        = PWRCTRL_EMONCFG0_EMONSEL0_I2S1ON,

    //! Increment the counter when I2S2 is powered on
    AM_HAL_ENGMON_MONITOR_I2S2ON        = PWRCTRL_EMONCFG0_EMONSEL0_I2S2ON,

    //! Increment the counter when I2S3 is powered on
    AM_HAL_ENGMON_MONITOR_I2S3ON        = PWRCTRL_EMONCFG0_EMONSEL0_I2S3ON,

    //! Increment the counter when PDM0 is powered on
    AM_HAL_ENGMON_MONITOR_PDM0ON        = PWRCTRL_EMONCFG0_EMONSEL0_PDM0ON,

    //! Increment the counter when PDM1 is powered on
    AM_HAL_ENGMON_MONITOR_PDM1ON        = PWRCTRL_EMONCFG0_EMONSEL0_PDM1ON,

    //! Increment the counter when PDM2 is powered on
    AM_HAL_ENGMON_MONITOR_PDM2ON        = PWRCTRL_EMONCFG0_EMONSEL0_PDM2ON,

    //! Increment the counter when PDM3 is powered on
    AM_HAL_ENGMON_MONITOR_PDM3ON        = PWRCTRL_EMONCFG0_EMONSEL0_PDM3ON,

    //! Increment the counter when AUDADC is powered on
    AM_HAL_ENGMON_MONITOR_AUDADCON     = PWRCTRL_EMONCFG0_EMONSEL0_AUDADCON,

    //! Increment the counter when CRYPTO is powered on
    AM_HAL_ENGMON_MONITOR_CRYPTOON     = PWRCTRL_EMONCFG0_EMONSEL0_CRYPTOON,

    //! Increment the counter when DBG is powered on
    AM_HAL_ENGMON_MONITOR_DBGON        = PWRCTRL_EMONCFG0_EMONSEL0_DBGON,

    //! Increment the counter when DISP is powered on
    AM_HAL_ENGMON_MONITOR_DISPON       = PWRCTRL_EMONCFG0_EMONSEL0_DISPON,

    //! Increment the counter when DSPA is powered on
    AM_HAL_ENGMON_MONITOR_DSPAON       = PWRCTRL_EMONCFG0_EMONSEL0_DSPAON,

    //! Increment the counter when GFX is powered on
    AM_HAL_ENGMON_MONITOR_GFXON        = PWRCTRL_EMONCFG0_EMONSEL0_GFXON,

    //! Increment the counter when UART0 is powered on
    AM_HAL_ENGMON_MONITOR_UART0ON      = PWRCTRL_EMONCFG0_EMONSEL0_UART0ON,

    //! Increment the counter when UART1 is powered on
    AM_HAL_ENGMON_MONITOR_UART1ON      = PWRCTRL_EMONCFG0_EMONSEL0_UART1ON,

    //! Increment the counter when IOM0 is powered on
    AM_HAL_ENGMON_MONITOR_IOM0ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM0ON,

    //! Increment the counter when IOM1 is powered on
    AM_HAL_ENGMON_MONITOR_IOM1ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM1ON,

    //! Increment the counter when IOM2 is powered on
    AM_HAL_ENGMON_MONITOR_IOM2ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM2ON,

    //! Increment the counter when IOM3 is powered on
    AM_HAL_ENGMON_MONITOR_IOM3ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM3ON,

    //! Increment the counter when IOM4 is powered on
    AM_HAL_ENGMON_MONITOR_IOM4ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM4ON,

    //! Increment the counter when IOM5 is powered on
    AM_HAL_ENGMON_MONITOR_IOM5ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM5ON,

    //! Increment the counter when UART2 is powered on
    AM_HAL_ENGMON_MONITOR_UART2ON      = PWRCTRL_EMONCFG0_EMONSEL0_UART2ON,

    //! Increment the counter when UART3 is powered on
    AM_HAL_ENGMON_MONITOR_UART3ON      = PWRCTRL_EMONCFG0_EMONSEL0_UART3ON,

    //! Increment the counter when IOM6 is powered on
    AM_HAL_ENGMON_MONITOR_IOM6ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM6ON,

    //! Increment the counter when IOM7 is powered on
    AM_HAL_ENGMON_MONITOR_IOM7ON       = PWRCTRL_EMONCFG0_EMONSEL0_IOM7ON,

    //! Increment the counter when IOS is powered on
    AM_HAL_ENGMON_MONITOR_IOSON        = PWRCTRL_EMONCFG0_EMONSEL0_IOSON,

    //! Increment the counter when MSPI0 is powered on
    AM_HAL_ENGMON_MONITOR_MSPI0ON      = PWRCTRL_EMONCFG0_EMONSEL0_MSPI0ON,

    //! Increment the counter when MSPI1 is powered on
    AM_HAL_ENGMON_MONITOR_MSPI1ON      = PWRCTRL_EMONCFG0_EMONSEL0_MSPI1ON,

    //! Increment the counter when SDIO is powered on
    AM_HAL_ENGMON_MONITOR_SDIOON       = PWRCTRL_EMONCFG0_EMONSEL0_SDIOON,

    //! Increment the counter when USB is powered on
    AM_HAL_ENGMON_MONITOR_USBON        = PWRCTRL_EMONCFG0_EMONSEL0_USBON,

    //! Increment the counter when USBPHY is powered on
    AM_HAL_ENGMON_MONITOR_VADON        = PWRCTRL_EMONCFG0_EMONSEL0_USBPHYON,
}
am_hal_engmon_monitor_config_e;

//*****************************************************************************
//
//! Energy monitor selector.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_ENGMON_MONITOR_0,
    AM_HAL_ENGMON_MONITOR_1,
    AM_HAL_ENGMON_MONITOR_2,
    AM_HAL_ENGMON_MONITOR_3,
    AM_HAL_ENGMON_MONITOR_4,
    AM_HAL_ENGMON_MONITOR_5,
    AM_HAL_ENGMON_MONITOR_6,
    AM_HAL_ENGMON_MONITOR_7,
    AM_HAL_ENGMON_MONITOR_MAX = AM_HAL_ENGMON_MONITOR_7
}
am_hal_engmon_monitor_select_e;

//*****************************************************************************
//
//! @brief Configure an energy monitor.
//!
//! @param eMonitor selects the energy monitor to operate on.
//! @param eConfig selects the condition under which the monitor will increase.
//!
//! Configure a power monitor to operate with a particular energy monitoring
//! criteria.
//!
//! @return status code.
//
//*****************************************************************************
extern uint32_t am_hal_engmon_monitor_config(am_hal_engmon_monitor_select_e eMonitor,
                                           am_hal_engmon_monitor_config_e eConfig);

//*****************************************************************************
//
//! @brief Read the configuration of an energy monitor.
//!
//! @param eMonitor selects the energy monitor to use.
//! @param pui32Config returns the current configuration of that monitor.
//!
//! Read the current configuration of an energy monitor.
//!
//! @return status code.
//
//*****************************************************************************
extern uint32_t am_hal_engmon_monitor_config_read(am_hal_engmon_monitor_select_e eMonitor,
                                                uint32_t *pui32Config);

//*****************************************************************************
//
//! @brief Read the count value of the energy monitor.
//!
//! @param eMonitor selects the energy monitor to use.
//! @param ui32Count returns the current count value of the energy monitor.
//!
//! Read the current count value of an energy monitor.
//!
//! @return status code.
//
//*****************************************************************************
extern uint32_t am_hal_powerctrl_monitor_read(am_hal_engmon_monitor_select_e eMonitor,
                                            uint32_t *ui32Count);

//*****************************************************************************
//
//! @brief Reset an energy monitor count back to zero.
//!
//! @param eMonitor selects the energy monitor to use.
//!
//! This function resets the selected energy monitor back to zero.
//!
//! @return status code.
//
//*****************************************************************************
extern uint32_t am_hal_powerctrl_monitor_reset(am_hal_engmon_monitor_select_e eMonitor);

//*****************************************************************************
//
//! @brief Stop an energy monitor counter, preserving the current count.
//!
//! @param eMonitor selects the energy monitor to use.
//!
//! Stop the selected energy monitor counter without changing the current count
//! value.
//!
//! @return status code.
//
//*****************************************************************************
extern uint32_t am_hal_powerctrl_monitor_stop(am_hal_engmon_monitor_select_e eMonitor);

//*****************************************************************************
//
//! @brief Start an energy monitor counter.
//!
//! @param eMonitor selects the energy monitor to use.
//!
//! Start the energy monitor. Once started the selected energy monitor will
//! increment any time its specific, configured condition is true.
//!
//! @return status code.
//
//*****************************************************************************
extern uint32_t am_hal_powerctrl_monitor_start(am_hal_engmon_monitor_select_e eMonitor);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_ENGMON_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

