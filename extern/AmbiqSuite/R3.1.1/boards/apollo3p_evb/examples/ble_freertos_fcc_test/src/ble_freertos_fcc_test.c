//*****************************************************************************
//
//! @file ble_freertos_fcc_test.c
//!
//! @brief ARM Cordio BLE - FCC test example
//!
//! Purpose: This example is used to put Bluetooth radio in Apollo3 into various
//! test mode on different channels on pressing BTN3 on the Apollo3 EVB.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
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

#include <stdint.h>
#include <stdbool.h>

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_buf.h"

#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_api.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"

#include "am_mcu_apollo.h"
#include "am_util.h"
#include "am_bsp.h"


#include "app_ui.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

//*****************************************************************************
//
// Forward declarations.
//
//*****************************************************************************
void button_timer_handler(TimerHandle_t xTimer);

extern void AppUiBtnTest(uint8_t btn);

//*****************************************************************************
//
// Timer for buttons.
//
//*****************************************************************************

TimerHandle_t xButtonTimer;

// Indicates which test case to start with when button is pressed.
uint8_t current_test_case  = 0;

//*****************************************************************************
//
// Tracking variable for the scheduler timer.
//
//*****************************************************************************

//*****************************************************************************
//
// Enable printing to the console.
//
//*****************************************************************************
void
enable_print_interface(void)
{
    //
    // Initialize a debug printing interface.
    //
    am_bsp_itm_printf_enable();
}

//*****************************************************************************
//
// Poll the buttons.
//
//*****************************************************************************
void
button_timer_handler(TimerHandle_t xTimer)
{
#if AM_BSP_NUM_BUTTONS
    //
    // Every time we get a button timer tick, check all of our buttons.
    //
    am_devices_button_array_tick(am_bsp_psButtons, AM_BSP_NUM_BUTTONS);

    //
    // If we got a a press, do something with it.
    //
    if ( am_devices_button_released(am_bsp_psButtons[0]) )
    {
        am_util_debug_printf("Got Button 0 Press\n");
        AppUiBtnTest(APP_UI_BTN_1_SHORT);
    }

    if ( am_devices_button_released(am_bsp_psButtons[1]) )
    {
      AppUiBtnTest(APP_UI_BTN_1_SHORT);
#endif
      switch ( current_test_case++ )
      {
        case 0:
            HciDrvRadioShutdown();
            HciDrvRadioBoot(0);
            HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

            HciVscCarrierWaveMode((2402 - 2402) / 2);

            am_util_debug_printf("Continuous Wave on Channel 2402 MHz\n");
            break;
        case 1:
            HciDrvRadioShutdown();
            HciDrvRadioBoot(0);
            HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

            HciVscCarrierWaveMode((2440 - 2402) / 2);
            am_util_debug_printf("Continuous Wave on Channel 2440 MHz\n");
            break;
        case 2:
            HciDrvRadioShutdown();
            HciDrvRadioBoot(0);
            HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

            HciVscCarrierWaveMode((2480 - 2402) / 2);
            am_util_debug_printf("Continuous Wave on Channel 2480 MHz\n");
            break;
        case 3:
            HciDrvRadioShutdown();
            HciDrvRadioBoot(0);
            HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

            am_util_debug_printf("Continuous Modulation on Channel 2402 MHz\n");
            HciVscConstantTransmission((2402 - 2402) / 2);

            break;
        case 4:
            HciDrvRadioShutdown();
            HciDrvRadioBoot(0);
            HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

            am_util_debug_printf("Continuous Modulation on Channel 2440 MHz\n");

            HciVscConstantTransmission((2440 - 2402) / 2);

            break;
        case 5:
            HciDrvRadioShutdown();
            HciDrvRadioBoot(0);
            HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

            am_util_debug_printf("Continuous Modulation on Channel 2480 MHz\n");
            HciVscConstantTransmission((2480 - 2402) / 2);

            break;

        default:
            break;
      }

      if ( current_test_case >= 6 )
      {
        current_test_case = 0;
      }
#if AM_BSP_NUM_BUTTONS
    }

    if ( am_devices_button_released(am_bsp_psButtons[2]) )
    {
        AppUiBtnTest(APP_UI_BTN_2_SHORT);
    }
#endif
}

void
setup_buttons(void)
{
    uint32_t ui32ButtonTimer = 20 * 100;

#if AM_BSP_NUM_BUTTONS
    //
    // Enable the buttons for user interaction.
    //
    am_devices_button_array_init(am_bsp_psButtons, AM_BSP_NUM_BUTTONS);
    ui32ButtonTimer = 20;
#endif
    //
    // Start a timer.
    //

    // Create a FreeRTOS Timer
    xButtonTimer = xTimerCreate("Button Timer", pdMS_TO_TICKS(ui32ButtonTimer),
            pdTRUE, NULL, button_timer_handler);

    if (xButtonTimer != NULL)
    {
        xTimerStart(xButtonTimer, 100);
    }
}


void am_ble_isr(void)
{
    HciDrvIntService();

}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clock frequency
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

#ifndef NOFPU
    //
    // Enable the floating point module, and configure the core for lazy
    // stacking.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);
#else
    am_hal_sysctrl_fpu_disable();
#endif

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

#if 1
    //
    // Turn off unneeded Flash & SRAM
    //
#if defined(AM_PART_APOLLO3P)
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_SRAM_192K);
#else
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_SRAM_96K);
#endif
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_MIN);
#endif

#ifdef AM_PART_APOLLO
    //
    // SRAM bank power setting.
    // Need to match up with actual SRAM usage for the program
    // Current usage is between 32K and 40K - so disabling upper 3 banks
    //
    am_hal_mcuctrl_sram_power_set(AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7,
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7);

#if 0 // Not turning off the Flash as it may be needed to download the image
    //
    // Flash bank power set.
    //
    am_hal_mcuctrl_flash_power_set(AM_HAL_MCUCTRL_FLASH_POWER_DOWN_1);
#endif
#endif // AM_PART_APOLLO

#ifdef AM_PART_APOLLO2
#if 0 // Not turning off the Flash as it may be needed to download the image
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_FLASH512K);
#endif
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_SRAM64K);
#endif // AM_PART_APOLLO2

    //
    // Enable printing to the console.
    //
#ifdef AM_DEBUG_PRINTF
    enable_print_interface();
#endif

    am_util_debug_printf("Apollo3 BLE Cordio FCC Test Example\n");

    //
    // Boot the radio.
    //
    HciDrvRadioBoot(1);

#if 0
    //
    // Start the BLE interface.
    //
    am_hal_ble_initialize(0, &g_pvBLEHandle);
    am_hal_ble_power_control(g_pvBLEHandle, AM_HAL_BLE_POWER_ACTIVE);
    am_hal_ble_config(g_pvBLEHandle, &am_hal_ble_default_config);

    /*delay 1s for 32768Hz clock stability*/
    am_util_delay_ms(1000);



    if (APOLLO3_A0 || APOLLO3_A1)  //for B0 chip, don't load copy patch
    {
        am_hal_ble_default_copy_patch_apply(g_pvBLEHandle);
    }



    am_hal_ble_default_trim_set_ramcode(g_pvBLEHandle);
    am_hal_ble_default_patch_apply(g_pvBLEHandle);
    am_hal_ble_patch_complete(g_pvBLEHandle);

    //
    // Setting the TX power to the highest power value.
    //
    am_hal_ble_tx_power_set(g_pvBLEHandle, 0xf);

    am_hal_ble_int_clear(g_pvBLEHandle, BLEIF_INTSTAT_BLECIRQ_Msk);

    HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);

#endif

    //
    // Prep the buttons for use
    //
    setup_buttons();


    am_util_debug_printf("Usage as below:\n");
    am_util_debug_printf("Short press Button 1 to go through different test modes\n");
    am_util_debug_printf("TX power is set to +3.0 dBm (max) \n");


    am_hal_interrupt_master_enable();

    //
    // Start the scheduler.
    //
    vTaskStartScheduler();

    while (TRUE)
    {
        ;
    }
}


//*****************************************************************************
//
// Sleep function called from FreeRTOS IDLE task.
// Do necessary application specific Power down operations here
// Return 0 if this function also incorporates the WFI, else return value same
// as idleTime
//
//*****************************************************************************
uint32_t am_freertos_sleep(uint32_t idleTime)
{
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    return 0;
}

//*****************************************************************************
//
// Recovery function called from FreeRTOS IDLE task, after waking up from Sleep
// Do necessary 'wakeup' operations here, e.g. to power up/enable peripherals etc.
//
//*****************************************************************************
void am_freertos_wakeup(uint32_t idleTime)
{
    return;
}


//*****************************************************************************
//
// FreeRTOS debugging functions.
//
//*****************************************************************************
void
vApplicationMallocFailedHook(void)
{
    //
    // Called if a call to pvPortMalloc() fails because there is insufficient
    // free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    // internally by FreeRTOS API functions that create tasks, queues, software
    // timers, and semaphores.  The size of the FreeRTOS heap is set by the
    // configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
    //
    while (1);
}

void
vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void) pcTaskName;
    (void) pxTask;

    //
    // Run time stack overflow checking is performed if
    // configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    // function is called if a stack overflow is detected.
    //
    while (1)
    {
        __asm("BKPT #0\n") ; // Break into the debugger
    }
}

