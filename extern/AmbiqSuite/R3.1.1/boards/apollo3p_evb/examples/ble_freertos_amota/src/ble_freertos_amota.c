//*****************************************************************************
//
//! @file ble_freertos_amota.c
//!
//! @brief ARM Cordio BLE - Ambiq Micro Over the Air (AMOTA) Example.
//!
//! Purpose: This example implements Ambiq Micro Over-the-Air (OTA) slave.  This
//! example is designed to allow loading of a binary software update from either
//! and iOS or Android phone running Ambiq's application.  This example works
//! with the Apollo3 Secure Bootloader (SBL) to place the image in flash and then
//! reset the Apollo3 to allow SBL to validate and install the image.
//!
//! AM_DEBUG_PRINTF
//! WSF_TRACE_ENABLED=1
//!
//! If enabled, debug messages will be sent over ITM at 1M Baud.
//!
//! Additional Information:
//! The directory \tools\apollo3_amota\scripts contains a Makefile which will
//! build the OTA binary.
//!
//! The directory \docs\app_notes\amota explains how to use the Ambiq iOS and
//! Android applications.
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

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include "ble_freertos_amota.h"
#include "rtos.h"
#include "amota_profile_config.h"

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

#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
static char *otaStatusMessage[] =
    {
        "Success",
        "Error",
        "Failure",
        "Pending"
    };
static void
dump_ota_status(void)
{
    uint32_t *pOtaDesc = (uint32_t *)(OTA_POINTER_LOCATION & ~(AM_HAL_FLASH_PAGE_SIZE - 1));
    uint32_t i;

    // Check if the current content at OTA descriptor is valid
    for (i = 0; i < AM_HAL_SECURE_OTA_MAX_OTA + 1; i++)
    {
        // Make sure the image address looks okay
        if (pOtaDesc[i] == 0xFFFFFFFF)
        {
            break;
        }
        if (((pOtaDesc[i] & 0x3) == AM_HAL_OTA_STATUS_ERROR) || ((pOtaDesc[i] & ~0x3) >= 0x100000))
        {
            break;
        }
    }
    if (pOtaDesc[i] == 0xFFFFFFFF)
    {
        am_util_debug_printf("Valid Previous OTA state\n");
        // It seems in last boot this was used as OTA descriptor
        // Dump previous OTA information
        am_hal_ota_status_t otaStatus[AM_HAL_SECURE_OTA_MAX_OTA];
        am_hal_get_ota_status(pOtaDesc, AM_HAL_SECURE_OTA_MAX_OTA, otaStatus);
        for ( uint32_t i = 0; i < AM_HAL_SECURE_OTA_MAX_OTA; i++ )
        {
            if ((uint32_t)otaStatus[i].pImage == 0xFFFFFFFF)
            {
                break;
            }
            {
                am_util_debug_printf("Previous OTA: Blob Addr: 0x%x - Result %s\n",
                                     otaStatus[i].pImage, otaStatusMessage[otaStatus[i].status]);
            }
        }
    }
    else
    {
        am_util_debug_printf("No Previous OTA state\n");
    }
}
#endif

//*****************************************************************************
//
// Main Function
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

    // Turn off unused Flash & SRAM

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

    //
    // Initialize plotting interface.
    //
    am_util_debug_printf("FreeRTOS Fit AMOTA Example\n");

#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    dump_ota_status();
#endif

    //
    // Run the application.
    //
    run_tasks();

    //
    // We shouldn't ever get here.
    //
    while (1)
    {
    }

}

