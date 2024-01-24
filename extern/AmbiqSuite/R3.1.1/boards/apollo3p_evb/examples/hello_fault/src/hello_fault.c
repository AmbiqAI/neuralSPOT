//*****************************************************************************
//
//! @file hello_fault.c
//!
//! @brief A simple example that causes a hard-fault.
//!
//! Purpose: This example demonstrates the extended hard fault handler which can
//! assist the user in decoding a fault condition. The handler pulls the
//! registers that the Cortex M4 automatically loads onto the stack and
//! combines them with various other fault information into a single
//! data structure saved locally.  It can optionally print out the fault
//! data structure (assuming the stdio printf has previously been enabled
//! and is still enabled at the time of the fault).
//!
//! Printing takes place over the ITM at 1M Baud.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
//
// Max SRAM for Apollo is 64KB.
// Use an offset of 1MB as the value to cause the fault.
//
#define ILLEGAL_SRAM_ADDR   (0x10000000 + (1024 * 1024))

//*****************************************************************************
//
//! @brief Function to cause a hard fault.
//!
//! @return None.
//
//*****************************************************************************
void
force_fault(void)
{
    uint32_t *pCauseFault;
    volatile uint32_t uVal;

    pCauseFault = (uint32_t*)ILLEGAL_SRAM_ADDR;
    uVal = *pCauseFault;

    //
    // Use the variable uVal in order to avoid a warning from some compilers.
    // However, the fault will prevent us from getting here.
    //
    pCauseFault = (uint32_t*)uVal;
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Hello Fault.\n\n");

    //
    // Print a message about the forthcoming hard fault.
    //
    am_util_stdio_printf(""
        "An illegal memory access will occur next, which will execute the\n"
        "extended fault handler to assist in decoding the fault.\n");
    am_util_stdio_printf("\n"
        "In order to print out the fault information for this example\n"
        "a macro, AM_UTIL_FAULTISR_PRINT, has been defined in the build\n"
        "environment.  Otherwise the fault data can only be examined\n"
        "directly in the ISR from a debugger.\n"
        "\n");
    am_util_stdio_printf(""
        "Forcing the fault now ...\n"
        "\n");

    //
    // Give the above print statements time to complete.
    //
    am_util_delay_ms(10);

    //
    // Enable the Apollo3 Fault detection.
    //
    am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_FAULT_CAPTURE_ENABLE, 0);

    //
    // Force an invalid memory address fault.
    // This function will not return, so anything after it
    //  will not be executed.
    //
    force_fault();

    //
    //
    // We are done printing. Disable debug printf messages on ITM.
    //
    am_bsp_debug_printf_disable();

    //
    // Loop forever while sleeping.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
