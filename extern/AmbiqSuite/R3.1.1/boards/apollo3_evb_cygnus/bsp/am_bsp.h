//*****************************************************************************
//
//  am_bsp.h
//! @file
//!
//! @brief Functions to aid with configuring the GPIOs.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @addtogroup apollo3_fpga_bsp BSP for the Apollo3 Hotshot FPGA
//! @ingroup BSP
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

#ifndef AM_BSP_H
#define AM_BSP_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp_pins.h"

//
// Make individual includes to not require full port before usage.
//#include "am_devices.h"
//
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
#define AM_BSP_PRINT_INFC_NONE              0
#define AM_BSP_PRINT_INFC_SWO               1
#define AM_BSP_PRINT_INFC_UART0             2
#define AM_BSP_PRINT_INFC_BUFFERED_UART0    3

//*****************************************************************************
//
// Button definitions.
//
//*****************************************************************************
#define AM_BSP_NUM_BUTTONS                  3
extern am_devices_button_t am_bsp_psButtons[AM_BSP_NUM_BUTTONS];

//*****************************************************************************
//
// LED definitions.
//
//*****************************************************************************
#define AM_BSP_NUM_LEDS                   5
extern am_devices_led_t am_bsp_psLEDs[AM_BSP_NUM_LEDS];

//*****************************************************************************
//
// PWM_LED peripheral assignments.
//
//*****************************************************************************
//
// The Apollo3 EVB LED1 is pin 30, which can use timer B2 or B4 as output.
// None of the other Apollo3 EVB LEDs can use CTx outputs.
//
#define AM_BSP_PIN_PWM_LED                  AM_BSP_GPIO_LED1
#define AM_BSP_PWM_LED_TIMER                2
#define AM_BSP_PWM_LED_TIMER_SEG            AM_HAL_CTIMER_TIMERB
#define AM_BSP_PWM_LED_TIMER_INT            AM_HAL_CTIMER_INT_TIMERB2C0

//*****************************************************************************
//
// UART definitions.
//
//*****************************************************************************
//
// Apollo3 has two UART instances.
// AM_BSP_UART_PRINT_INST should correspond to COM_UART.
//
#define AM_BSP_UART_IOS_INST                0
#define AM_BSP_UART_PRINT_INST              0
#define AM_BSP_UART_BOOTLOADER_INST         1

//*****************************************************************************
//
//! Structure containing UART configuration information while it is powered down.
//
//*****************************************************************************
typedef struct
{
    bool     bSaved;
    uint32_t ui32TxPinNum;
    uint32_t ui32TxPinCfg;
}
am_bsp_uart_pwrsave_t;

//*****************************************************************************
//
// External data definitions.
//
//*****************************************************************************
extern am_bsp_uart_pwrsave_t am_bsp_uart_pwrsave[AM_REG_UART_NUM_MODULES];

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern void am_bsp_low_power_init(void);
extern void am_bsp_iom_pins_enable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_iom_pins_disable(uint32_t ui32Module, am_hal_iom_mode_e eIOMMode);
extern void am_bsp_mspi_pins_enable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice);
extern void am_bsp_mspi_pins_disable(uint32_t ui32Module, am_hal_mspi_device_e eMSPIDevice);

extern void am_bsp_ios_pins_enable(uint32_t ui32Module, uint32_t ui32IOSMode);
extern void am_bsp_ios_pins_disable(uint32_t ui32Module, uint32_t ui32IOSMode);

extern void am_bsp_debug_printf_enable(void);
extern void am_bsp_debug_printf_disable(void);

extern void am_bsp_itm_string_print(char *pcString);
extern void am_bsp_itm_printf_enable(void);
extern void am_bsp_itm_printf_disable(void);

extern void am_bsp_uart_string_print(char *pcString);
extern void am_bsp_uart_printf_enable(void);
extern void am_bsp_uart_printf_disable(void);

extern void am_bsp_buffered_uart_printf_enable(void);
extern void am_bsp_buffered_uart_service(void);

extern uint32_t am_bsp_com_uart_transfer(const am_hal_uart_transfer_t *psTransfer);

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
