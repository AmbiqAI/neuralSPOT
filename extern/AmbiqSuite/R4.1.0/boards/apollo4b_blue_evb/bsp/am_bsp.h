//*****************************************************************************
//
//  am_bsp.h
//! @file
//!
//! @brief Functions to aid with configuring the GPIOs.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @addtogroup apollo4_sip_evb_bsp BSP for the Apollo4 SIP EVB
//! @ingroup BSP
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
// This is part of revision release_sdk_4_1_0-8020bdf229 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_BSP_H
#define AM_BSP_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp_pins.h"
#include "am_devices_led.h"
#include "am_devices_button.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// BLE Controller reset pin
//
//*****************************************************************************
#define AM_DEVICES_BLECTRLR_RESET_PIN   42

//*****************************************************************************
//
// Print interface type
//
//*****************************************************************************
#define AM_BSP_UART_PRINT_INST  0

//*****************************************************************************
//
// PWM_LED peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_PWM_LED_TIMER                0
#define AM_BSP_PWM_LED_TIMER_SEG            AM_HAL_CTIMER_TIMERB
#define AM_BSP_PWM_LED_TIMER_INT            AM_HAL_CTIMER_INT_TIMERB0

//*****************************************************************************
//
// Button definitions.
//
//*****************************************************************************
#define AM_BSP_NUM_BUTTONS                  2
#if AM_BSP_NUM_BUTTONS
extern am_devices_button_t am_bsp_psButtons[AM_BSP_NUM_BUTTONS];
#endif

//*****************************************************************************
//
// LED definitions.
//
//*****************************************************************************
#define AM_BSP_NUM_LEDS                     3
#if AM_BSP_NUM_LEDS
extern am_devices_led_t am_bsp_psLEDs[AM_BSP_NUM_LEDS];
#endif

//*****************************************************************************
//
// Power Mode - if SIMOBUCK should be enabled, or stay in LDO mode
//
//*****************************************************************************
#ifndef AM_BSP_ENABLE_SIMOBUCK
#define AM_BSP_ENABLE_SIMOBUCK   1
#endif


#define SDIF_BUS_WIDTH_1 1
#define SDIF_BUS_WIDTH_4 4
#define SDIF_BUS_WIDTH_8 8

//*****************************************************************************
//
// Display interface.
//
//*****************************************************************************
typedef enum
{
    IF_SPI4,
    IF_DSPI,
    IF_QSPI,
    IF_DSI,
} am_bsp_disp_if_e;

//*****************************************************************************
//
// Display interface, resolution and flipping configurations.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32PanelResX;
    uint32_t ui32PanelResY;
    bool     bFlip;
    bool     bUseDPHYPLL;
    am_bsp_disp_if_e        eInterface;
} am_bsp_display_config_t;

//*****************************************************************************
//
// Display types.
//
//*****************************************************************************
typedef enum
{
    RM67162_SPI4 = 0,
    RM67162_DSPI,
    RM67162_DSI,
    RM69330_DSPI,
    RM69330_QSPI,
    RM69330_DSI,
    BOE_DSI,
} am_bsp_display_type_e;

//*****************************************************************************
//
// Display interface, resolution and flipping configurations.
//
//*****************************************************************************
typedef struct
{
    am_hal_dsi_freq_trim_e  eDsiFreq;
    am_hal_dsi_dbi_width_e  eDbiWidth;
    uint8_t ui8NumLanes;
} am_bsp_dsi_config_t;

//*****************************************************************************
//
// External global variables.
//
//*****************************************************************************
extern am_bsp_display_type_e g_eDispType;
extern am_bsp_display_config_t g_sDispCfg[7];
extern am_bsp_dsi_config_t g_sDsiCfg;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern void am_bsp_low_power_init(void);
extern void am_bsp_iom_pins_enable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_iom_pins_disable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_ios_pins_enable(uint32_t ui32Module, uint32_t ui32IOSMode);
extern void am_bsp_ios_pins_disable(uint32_t ui32Module, uint32_t ui32IOSMode);
extern void am_bsp_mspi_pins_enable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice);
extern void am_bsp_mspi_pins_disable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice);

extern void am_bsp_sdio_pins_enable(uint8_t ui8BusWidth);
extern void am_bsp_sdio_pins_disable(uint8_t ui8BusWidth);

extern int32_t am_bsp_debug_printf_enable(void);
extern void am_bsp_debug_printf_disable(void);

extern void am_bsp_itm_string_print(char *pcString);
extern int32_t am_bsp_itm_printf_enable(void);
extern void am_bsp_itm_printf_disable(void);

extern void am_bsp_uart_string_print(char *pcString);

extern int32_t am_bsp_uart_printf_enable(void);
extern void am_bsp_uart_printf_disable(void);

extern int32_t am_bsp_buffered_uart_printf_enable(void);
extern void am_bsp_buffered_uart_printf_disable(void);
extern void am_bsp_buffered_uart_service(void);


extern void am_bsp_disp_spi_pins_enable(void);
extern void am_bsp_disp_qspi_pins_enable(void);
extern void am_bsp_disp_dspi_pins_enable(void);
extern void am_bsp_disp_dsi_pins_enable(void);
extern void am_bsp_external_pwr_on(void);

extern void am_bsp_external_vdd18_switch(bool bEnable);

extern void am_bsp_external_vddusb33_switch(bool bEnable);
extern void am_bsp_external_vddusb0p9_switch(bool bEnable);

#ifdef __cplusplus
}
#endif

#endif // AM_BSP_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
