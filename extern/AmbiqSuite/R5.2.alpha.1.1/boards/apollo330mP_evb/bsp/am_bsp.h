//*****************************************************************************
//
//  am_bsp.h
//! @file
//!
//! @brief Functions to aid with configuring the GPIOs.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @addtogroup apollo330mP_evb BSP
//! @ingroup BSP
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
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
// Definition of External Clock Sources for the board
//
//*****************************************************************************
#define AM_BSP_XTAL_HS_MODE         AM_HAL_CLKMGR_XTAL_HS_MODE_XTAL
#define AM_BSP_XTAL_HS_FREQ_HZ      (48000000U)
#define AM_BSP_XTAL_LS_MODE         AM_HAL_CLKMGR_XTAL_LS_MODE_XTAL
#define AM_BSP_XTAL_LS_FREQ_HZ      (32768U)
#define AM_BSP_EXTREF_CLK_FREQ_HZ   (0U)

// If defined, the custom configuration will be used, otherwise use the Info1 trims.
// #define AM_BSP_USE_CUSTOM_RFXTAL_CONFIG
#ifdef AM_BSP_USE_CUSTOM_RFXTAL_CONFIG
extern const am_hal_clkmgr_rfxtal_config_t g_sRfxtalCfg;
#endif

typedef struct
{
    uint32_t    ui32Designator;
    uint32_t    ui32Module;
}am_bsp_nand_flash_t;

extern const am_bsp_nand_flash_t bsp_nand_flash_devices[];

//*****************************************************************************
//
// UART specifics for this board including assigned UART modules.
//
//*****************************************************************************
#define AM_BSP_UART_PRINT_INST  0   // UART COM module

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
#define AM_BSP_NUM_LEDS                     3   // FPGAs and EBs generally have no native LEDs.
#if AM_BSP_NUM_LEDS > 0
extern am_devices_led_t am_bsp_psLEDs[AM_BSP_NUM_LEDS];
#endif

//*****************************************************************************
//
// MSPI PSRAM definitions.
//
//*****************************************************************************
#define AM_BSP_MSPI_PSRAM_DEVICE_APS25616BA
#define AM_BSP_MSPI_PSRAM_MODULE_OCTAL_DDR_CE   AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0
#define USE_APS51216BA

//*****************************************************************************
//
// IOM PSRAM definitions.
//
//*****************************************************************************
#define AM_BSP_IOM_PSRAM_MODULE            4

//*****************************************************************************
//
// IOM - IOS macro definitions for ios 2-board testing
//
//*****************************************************************************

//
// ios_device
//
#define TEST_IOS_MODULE                 0
#define SLINT_GPIO                      0
#define lram_array                      am_hal_iosfd0_pui8LRAM
#define IOS_ACC_IRQ                     IOSLAVEFDACC0_IRQn
#define IOS_IRQ                         IOSLAVEFD0_IRQn
#define ios_isr                         am_ioslave_fd0_isr
#define ios_acc_isr                     am_ioslave_fd0_acc_isr
#define SLINT_FUN_SEL                   AM_HAL_PIN_4_SLFDINT
#define IOS_PWRCTRL                     AM_HAL_PWRCTRL_PERIPH_IOSFD0
#define IOS_WRAPAROUND_MAX_LRAM_SIZE    0x40
#define IOS_FIFO_THRESHOLD              0x20

//
// ios_controller
//
#define TEST_IOS_CNTLR_MODULE           0
#define am_iomaster_isr                 am_iomaster0_isr
#define IOMSTR_IQRn                     IOMSTR0_IRQn

//
// ios_fullduplex
//
#define IOM_DUPLEX_TEST_MODULE          1

//
// ios_device and ios_controller pins
//
#define AM_BSP_GPIO_TEST_IOM_SCK        AM_BSP_GPIO_IOM0_SCK
#define AM_BSP_GPIO_TEST_IOM_MOSI       AM_BSP_GPIO_IOM0_MOSI
#define AM_BSP_GPIO_TEST_IOM_MISO       AM_BSP_GPIO_IOM0_MISO
#define AM_BSP_GPIO_TEST_IOM_CE         AM_BSP_GPIO_IOM0_CS

#define AM_BSP_GPIO_TEST_IOS_SCK        AM_BSP_GPIO_IOSFD0_SCK
#define AM_BSP_GPIO_TEST_IOS_MOSI       AM_BSP_GPIO_IOSFD0_MOSI
#define AM_BSP_GPIO_TEST_IOS_MISO       AM_BSP_GPIO_IOSFD0_MISO
#define AM_BSP_GPIO_TEST_IOS_CE         AM_BSP_GPIO_IOSFD0_CE


//*****************************************************************************
//
// UART macro definitions for UART Examples
//
//*****************************************************************************
#define TEST_UART_TX_MODULE     0   // keeping for backward compatibilty
#define TEST_UART_RX_MODULE     1


#define AM_BSP_UART_STREAM_UART  0  // used for most uart streaming examples
#define AM_BSP_UART_FULL_DUPLEX_TX 0
#define AM_BSP_UART_FULL_DUPLEX_RX 1
//*****************************************************************************
//
// The Apollo510 EVB uses external loadswitches to control USB power.
//
//*****************************************************************************
#define ENABLE_EXT_USB_PWR_RAILS

//*****************************************************************************
//
// JDI timer pins definitions.
//
//*****************************************************************************
#define AM_BSP_JDI_TIMER_VA               0
#define AM_BSP_JDI_TIMER_VCOM             1
#define AM_BSP_JDI_TIMER_VB               2

//*****************************************************************************
//
// SDIO Bus Width definitions
//
//*****************************************************************************
#define AM_BSP_SDIO_INSTANCE    (0)
#define AM_BSP_SDIO_BUS_WIDTH   AM_HAL_HOST_BUS_WIDTH_8

//*****************************************************************************
//
// Assigned to reversed pins to disable SDIO CD & WP.
//
//*****************************************************************************
#define AM_BSP_GPIO_SDIO0_WP                80
#define AM_BSP_GPIO_SDIO0_CD                89

#define AM_BSP_GPIO_SDIO1_WP                112
#define AM_BSP_GPIO_SDIO1_CD                113

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Set up the I2S pins based on module.
//!
//! @param ui32Module - I2S module
//! @param bBidirectionalData - Use Bidirectional Data for I2S Module
//
//*****************************************************************************
extern void am_bsp_i2s_pins_enable(uint32_t ui32Module, bool bBidirectionalData);

//*****************************************************************************
//
//! @brief Disable the I2S pins based on module.
//!
//! @param ui32Module - I2S module
//! @param bBidirectionalData - Use Bidirectional Data for I2S Module
//
//*****************************************************************************
extern void am_bsp_i2s_pins_disable(uint32_t ui32Module, bool bBidirectionalData);

//*****************************************************************************
//
//! @brief Set up the PDM pins based on module.
//!
//! @param ui32Module - PDM module
//
//*****************************************************************************
extern void am_bsp_pdm_pins_enable(uint32_t ui32Module);

//*****************************************************************************
//
//! @brief Disable the PDM pins based on module.
//!
//! @param ui32Module - PDM module
//
//*****************************************************************************
extern void am_bsp_pdm_pins_disable(uint32_t ui32Module);

extern void am_bsp_low_power_init(void);
extern void am_bsp_iom_pins_enable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_iom_pins_disable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_iom_3wire_pins_enable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_iom_3wire_pins_disable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
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
//! @brief Set up the MSPI pins based on the external flash device type with clock
//! on data pin 4.
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
extern void am_bsp_mspi_clkond4_pins_enable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice);

//*****************************************************************************
//
//! @brief Disable the MSPI pins based on the external flash device type with clock
//! on data pin 4.
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
extern void am_bsp_mspi_clkond4_pins_disable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice);

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
//! @brief Set up the MSPI device selection
//!
//! @param ui32Module  - MSPI module 2
//! @param ui8Select   - MSPI device 0 or 1
//!        #define AM_BSP_MSPI_FLASH_DEVICE_IS25WX064   1
//!        #define AM_BSP_MSPI_FLASH_DEVICE_GD25LB512ME 0
//!
//! This function resolves the conflict in am_bsp_mspi_pins_enable between the two
//! device when AM_HAL_MSPI_FLASH_SERIAL_CE0/1 is selected.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_mspi_device_select(uint32_t ui32Module, uint8_t ui8Select);

//*****************************************************************************
//
//! @brief Reset SDIO device via GPIO.
//!
//! This function reset SDIO device via GPIO
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_sdio_reset(uint32_t ui32Module);

//*****************************************************************************
//
//! @brief Set up the SDIF's pins.
//!
//! @param ui8BusWidth - Bus Width of SDIF
//! @param ui8SdioNum - SDIO module number (0 or 1)
//!
//! This function configures SDIf's CMD, CLK, DAT0-7 pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_sdio_pins_enable(uint8_t ui8SdioNum, uint8_t ui8BusWidth);

//*****************************************************************************
//
//! @brief Disable the SDIF's pins.
//!
//! @param ui8BusWidth - Bus Width of SDIF
//! @param ui8SdioNum - SDIO module number (0 or 1)
//!
//! This function deconfigures SDIF's CMD, CLK, DAT0-7 pins
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_sdio_pins_disable(uint8_t ui8SdioNum, uint8_t ui8BusWidth);

//*****************************************************************************
//
//! @brief  Set up the SD's CD pin.
//!
//! This function configure SD's CD pin when enable or disable sd card detection.
//
//*****************************************************************************
extern void am_bsp_sd_cd_pin_enable(uint8_t ui8SdioNum, bool bEnable);

//*****************************************************************************
//
//! @brief  Set up the SD's WP pin.
//!
//! This function configure SD's WP pin to detect sd card write protection.
//
//*****************************************************************************
extern void am_bsp_sd_wp_pin_enable(uint8_t ui8SdioNum, bool bEnable);

//*****************************************************************************
//
//! @brief Power on eMMC card.
//!
//! @param ui8SdioNum - SDIO module number (0 or 1)
//!
//! This function enable VCCQ and VCC of eMMC
//
//*****************************************************************************
extern void am_bsp_emmc_power_on(uint8_t ui8SdioNum);

//*****************************************************************************
//
//! @brief Power off eMMC card.
//!
//! @param ui8SdioNum - SDIO module number (0 or 1)
//!
//! This function disable VCCQ and VCC of eMMC
//
//*****************************************************************************
extern void am_bsp_emmc_power_off(uint8_t ui8SdioNum);

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
//! @brief Prepare for deepsleep while keeping DEBUG active.
//!
//!  When an application finds it necessary to not disable printing before
//!  deepsleep, this function should be called just before and just after
//!  deepsleep to make sure the SWO line is properly handled.
//!
//!  Important - This function is only needed if the application needs to
//!  keep SWO/ITM enabled during deepsleep. It is not needed when the
//!  print protocol to disable before deepsleep is used.
//!
//!  However, under these conditions, the deepsleep level will only reach the
//!  CORE_DEEPSLEEP level rather than the SYS_DEEPSLEEP level (see also
//!  MCUCTRL->SYSPWRSTATUS).
//
//*****************************************************************************
extern void am_bsp_debug_printf_deepsleep_prepare(bool bGoingToSleep);

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
//! @param pcString - Pointer to character array to print
//!
//! This function is used for printing a string via the UART, which for some
//! MCU devices may be multi-module.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_uart_string_print(char *pcString);

//*****************************************************************************
//
//! @brief UART-based string get function.
//!
//! @param pcString - Pointer to character array to store
//!
//! This function is used for getting a string via the UART, which for some
//! MCU devices may be multi-module.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_uart_string_get(char *pcString);

#ifndef AM_BSP_DISABLE_BUFFERED_UART
//*****************************************************************************
//
//! Initialize and configure the UART
//!
//! @param pvHandle - Pointer to UART Handle
//!
//! @return 0 on success.
//
//*****************************************************************************
extern int32_t am_bsp_buffered_uart_printf_enable(void *pvHandle);

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
//! @param bEnable - Enable or disable the external VDDUSB33 switch
//
//*****************************************************************************
extern void am_bsp_external_vddusb33_switch(bool bEnable);

//*****************************************************************************
//
//! @brief MSPI CE Pin Config Get
//!
//! @param ui32Module - MSPI module 0/1/2/3
//! @param eMSPIDevice - MSPI Device Type (HEX, OCTAL, ...)
//! @param pPinnum - MSPI pin number to return to the device driver
//! @param pPincfg - Pin configuration to return to the device driver
//!
//! This function returns the pinnum and pincfg for the CE of MSPI requested.
//
//*****************************************************************************
extern void am_bsp_mspi_ce_pincfg_get( uint32_t ui32Module,
                        am_hal_mspi_device_e eMSPIDevice,
                        uint32_t * pPinnum,
                        am_hal_gpio_pincfg_t * pPincfg );

//*****************************************************************************
//
//! @brief MSPI RESET Pin Config Get
//!
//! @param ui32Module - MSPI module 0/1/2/3
//! @param eMSPIDevice - MSPI Device Type (HEX, OCTAL, ...)
//! @param pPinnum - MSPI pin number to return to the device driver
//! @param pPincfg - Pin configuration to return to the device driver
//!
//! This function returns the pinnum and pincfg for the CE of MSPI requested.
//
//*****************************************************************************
extern void am_bsp_mspi_reset_pincfg_get(uint32_t ui32Module,
                          am_hal_mspi_device_e eMSPIDevice,
                          uint32_t * pPinnum,
                          am_hal_gpio_pincfg_t * pPincfg);

//*****************************************************************************
//
//! @brief Put the test errors to the TPIU and ITM.
//!
//! @param i32ErrorCount
//!
//! This function outputs the number of test errors to the TPIU/ITM.
//!
//! @return None.
//
//*****************************************************************************
extern void am_bsp_ckerr(int32_t i32ErrorCount);

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
