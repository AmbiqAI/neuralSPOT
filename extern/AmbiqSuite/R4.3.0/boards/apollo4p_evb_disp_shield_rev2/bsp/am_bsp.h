//*****************************************************************************
//
//  am_bsp.h
//! @file
//!
//! @brief Functions to aid with configuring the GPIOs.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @addtogroup apollo4p_evb_disp_shield_rev2 BSP for the Apollo4p Display Shield
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
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
// MSPI FLASH definitions.
//
//*****************************************************************************
#define AM_BSP_MSPI_FLASH_DEVICE_IS25WX064
#define AM_BSP_MSPI_FLASH_MODULE            1

//*****************************************************************************
//
// MSPI PSRAM definitions.
//
//*****************************************************************************
#define AM_BSP_MSPI_PSRAM_DEVICE_APS25616N
#define AM_BSP_MSPI_PSRAM_MODULE_OCTAL_DDR_CE   AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0
#define AM_BSP_HEX_CE0	1

//*****************************************************************************
//
// Touch interface definitions.
//
//*****************************************************************************
#define AM_BSP_TP_IOM_MODULE                0

//*****************************************************************************
//
// Power Mode - if SIMOBUCK should be enabled, or stay in LDO mode
//
//*****************************************************************************
#ifndef AM_BSP_ENABLE_SIMOBUCK
#define AM_BSP_ENABLE_SIMOBUCK   1
#endif



#define SDIO_BUS_WIDTH_1 1
#define SDIO_BUS_WIDTH_4 4
#define SDIO_BUS_WIDTH_8 8

#define AM_BSP_APOLLO4B_EVB_DISP_RESX           454
#define AM_BSP_APOLLO4B_EVB_DISP_RESY           454

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
    RM69330_SPI4,
    BOE_DSI
} am_bsp_display_type_e;

//*****************************************************************************
// Display offset
//*****************************************************************************
#define AM_BSP_DISPLAY_OFFSET     (14)

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
extern am_bsp_display_config_t g_sDispCfg[];
extern am_bsp_dsi_config_t g_sDsiCfg;

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
//*****************************************************************************
//
//! @brief Prepare the MCU for low power operation.
//!
//! This function enables several power-saving features of the MCU, and
//! disables some of the less-frequently used peripherals. It also sets the
//! system clock to 24 MHz.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_low_power_init(void);

//*****************************************************************************
//
//! @brief Set up the IOM pins based on mode and module.
//!
//! @param ui32Module - MSPI module
//! @param eIOMMode - IOS mode of the device
//!
//! This function configures up to 10-pins for MSPI serial, dual, quad,
//! dual-quad, and octal operation.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_iom_pins_enable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);

//*****************************************************************************
//
//! @brief Disable the IOM pins based on mode and module.
//!
//! @param ui32Module - MSPI module
//! @param eIOMMode - IOS mode of the device
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_iom_pins_disable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);

//*****************************************************************************
//
//! @brief Set up the IOS pins based on mode and module.
//!
//! @param ui32Module - MSPI module
//! @param ui32IOSMode - IOS mode of the device
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_ios_pins_enable(uint32_t ui32Module, uint32_t ui32IOSMode);

//*****************************************************************************
//
//! @brief Disable the IOS pins based on mode and module.
//!
//! @param ui32Module - MSPI module
//! @param ui32IOSMode - IOS mode of the device
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_ios_pins_disable(uint32_t ui32Module, uint32_t ui32IOSMode);

//*****************************************************************************
//
//! @brief Set up the MSPI pins based on the external flash device type.
//!
//! @param ui32Module - MSPI module
//! @param eMSPIDevice - MSPI device type
//!
//! This function configures up to 10-pins for MSPI serial, dual, quad,
//! dual-quad, and octal operation.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_mspi_pins_enable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice );

//*****************************************************************************
//
//! @brief Disable the MSPI pins based on the external flash device type.
//!
//! @param ui32Module - MSPI module
//! @param eMSPIDevice - MSPI device type
//!
//! This function deconfigures up to 10-pins for MSPI serial, dual, quad,
//! dual-quad, and octal operation.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_mspi_pins_disable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice );

//*****************************************************************************
//
//! @brief Set up the SDIF's pins.
//!
//! @param ui8BusWidth - Bus Width of SDIF
//!
//! This function configures SDIf's CMD, CLK, DAT0-7 pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_sdio_pins_enable(uint8_t ui8BusWidth);

//*****************************************************************************
//
//! @brief Disable the SDIF's pins.
//!
//! @param ui8BusWidth - Bus Width of SDIF
//!
//! This function deconfigures SDIF's CMD, CLK, DAT0-7 pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_sdio_pins_disable(uint8_t ui8BusWidth);

//*****************************************************************************
//
//! @brief Enable the TPIU and ITM for debug printf messages.
//!
//! This function enables TPIU registers for debug printf messages and enables
//! ITM GPIO pin to SWO mode. This function should be called after reset and
//! after waking up from deep sleep.
//!
//! @return 0 on success.
//
//*****************************************************************************
extern int32_t am_bsp_debug_printf_enable(void);

//*****************************************************************************
//
//! @brief Disable the TPIU and ITM for debug printf messages.
//!
//! This function disables TPIU registers for debug printf messages and disables
//! ITM GPIO pin to SWO mode. This function should be called after reset and
//! after waking up from deep sleep.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_debug_printf_disable(void);

//*****************************************************************************
//
//! @brief Enable printing over ITM.
//
//! @return 0 on success.
//
//*****************************************************************************
extern int32_t am_bsp_itm_printf_enable(void);

//*****************************************************************************
//
// @brief Disable printing over ITM.
//
//*****************************************************************************
extern void am_bsp_itm_printf_disable(void);

//*****************************************************************************
//
//! Initialize and configure the UART
//
//! @return 0 on success.
//
//*****************************************************************************
extern int32_t am_bsp_uart_printf_enable(void);

//*****************************************************************************
//
//! Disable the UART
//
//*****************************************************************************
extern void am_bsp_uart_printf_disable(void);

//*****************************************************************************
//
//! @brief UART-based string print function.
//!
//! @param pcString - Pointer to chacter array to print
//!
//! This function is used for printing a string via the UART, which for some
//! MCU devices may be multi-module.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_uart_string_print(char *pcString);

#ifndef AM_BSP_DISABLE_BUFFERED_UART
//*****************************************************************************
//
//! Initialize and configure the UART
//
//! @return 0 on success.
//
//*****************************************************************************
extern int32_t am_bsp_buffered_uart_printf_enable(void);

//*****************************************************************************
//
//! Disable the UART
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_buffered_uart_printf_disable(void);

//*****************************************************************************
//
//! Interrupt routine for the buffered UART interface.
//
//*****************************************************************************
extern void am_bsp_buffered_uart_service(void);
#endif // AM_BSP_DISABLE_BUFFERED_UART


//*****************************************************************************
//
//! @brief Set up the DC's spi pins.
//!
//! This function configures DC's DCX, CLOCK, DATA pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_spi_pins_enable(void);

//*****************************************************************************
//
//! @brief Disable the DC's spi pins.
//!
//! This function deconfigures DC's DCX, CLOCK, DATA pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_spi_pins_disable(void);

//*****************************************************************************
//
//! @brief Set up the DC's qspi pins.
//!
//! This function configures DC's DCX, CLOCK, DATA pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_qspi_pins_enable(void);

//*****************************************************************************
//
//! @brief Disable the DC's qspi pins.
//!
//! This function deconfigures DC's DCX, CLOCK, DATA pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_qspi_pins_disable(void);

//*****************************************************************************
//
//! @brief Set up the DC's dspi pins.
//!
//! This function configures DC's DCX, CLOCK, DATA pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_dspi_pins_enable(void);

//*****************************************************************************
//
//! @brief Disable the DC's dspi pins.
//!
//! This function deconfigures DC's DCX, CLOCK, DATA pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_dspi_pins_disable(void);

//*****************************************************************************
//
//! @brief Set up the DSI pins.
//!
//! This function configures DSI reset and TE pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_dsi_pins_enable(void);

//*****************************************************************************
//
//! @brief Disable the DSI pins.
//!
//! This function deconfigures DSI reset and TE pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_disp_dsi_pins_disable(void);

//*****************************************************************************
//
//! @brief External power on.
//!
//! This function turns on external power switch
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_external_pwr_on(void);

//*****************************************************************************
//
//! @brief Display device enable on the apollo4_bga_evb_disp_shld board.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_external_display_device_enable(bool bEnable);

//*****************************************************************************
//
//! @brief Accelerometer device enable on the apollo4_bga_evb_disp_shld board.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_external_accel_device_enable(bool bEnable);

//*****************************************************************************
//
//! @brief DSI VDD18 power switch.
//!
//! @param bEnable - Enable or disable the external VDD1.8V switch
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_external_vdd18_switch(bool bEnable);

//*****************************************************************************
//
//! @brief VDDUSB33 power switch.
//!
//! @param bEnable - Enable or disable the external VDDUSB3.3V switch
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_external_vddusb33_switch(bool bEnable);

//*****************************************************************************
//
//! @brief VDDUSB0P9 power switch.
//!
//! @param bEnable - Enable or disable the external VDDUSB0.9V switch
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_external_vddusb0p9_switch(bool bEnable);

//*****************************************************************************
//
//! @brief MSPI Pin Config Get
//!
//! @param eMSPIDevice - MSPI Device Type (HEX, OCTAL, ...)
//! @param pPinnum - MSPI pin number to return to the device driver
//! @param pPincfg - Pin configuration to return to the device driver
//!
//! This function returns the pinnum and pincfg for the MSPI requested.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_mspi_pincfg_get( am_hal_mspi_device_e eMSPIDevice,
                                   uint32_t * pPinnum,
                                   am_hal_gpio_pincfg_t * pPincfg );

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
