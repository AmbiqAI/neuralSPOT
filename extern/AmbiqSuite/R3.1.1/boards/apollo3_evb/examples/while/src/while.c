//*****************************************************************************
//
//! @file while.c
//!
//! @brief Example to emulate a polling loop.
//!
//! Purpose: This example provides a demonstration of the power required while
//! executing in a tight loop on the Apollo3 MCU.
//!
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
// Print option
//
//*****************************************************************************
#define PRINT_UART  1

//*****************************************************************************
//
// Cache configuration
//
//*****************************************************************************
const am_hal_cachectrl_config_t am_hal_cachectrl_benchmark =
{
    .bLRU                       = 0,
    .eDescript                  = AM_HAL_CACHECTRL_DESCR_1WAY_128B_512E,
    .eMode                      = AM_HAL_CACHECTRL_CONFIG_MODE_INSTR,
};

//*****************************************************************************
//
// Minimize power
//
//*****************************************************************************
void
set_for_min_power(void)
{
    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Turn OFF unneeded flash
    //
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_MIN);

    //
    // Turn off SRAMs above 8K.
    // Note - assumes a 4KB stack (the usual example stack size).
    //
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_SRAM_8K_DTCM);

    //
    // Disable the XTAL.
    //
    CLKGEN->OCTRL_b.OSEL = 1;

    //
    // Turn off the voltage comparator.
    //
    VCOMP->PWDKEY = _VAL2FLD(VCOMP_PWDKEY_PWDKEY, VCOMP_PWDKEY_PWDKEY_Key);

#if AM_PART_APOLLO2
    //
    // Powerdown the BOD and PDR logic.
    //
    MCUCTRL->BODPORCTRL = 0x2;  // 3=disable both 2=disable BOD 1=disable PDR

    //
    // Turn off all peripheral power domains.
    //
    PWRCTRL->DEVICEEN = 0;
#endif // AM_PART_APOLLO2

#ifdef AM_PART_APOLLO3
    //
    // Enable the cache for LPMMODE and aggressive settings.
    // This must be done after am_hal_cachectrl_enable().
    //
    if ( am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_LPMMODE_AGGRESSIVE, 0) )
    {
#if (PRINT_UART == 1)
        am_bsp_uart_printf_enable();
        am_util_stdio_printf("ERROR: The setting of LPMMODE failed! Halting program...\n");
        am_bsp_uart_printf_disable();
#endif // (PRINT_UART == 1)
        while(1);
    }
#endif // AM_PART_APOLLO3
} // set_for_min_power()

//*****************************************************************************
//
// Main Function.
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
    am_hal_cachectrl_config(&am_hal_cachectrl_benchmark);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#if (PRINT_UART == 1)
    am_bsp_uart_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Ambiq Micro 'while' example.\n\n");

    //
    // Brief description
    //
    am_util_stdio_printf("Used for measuring power in an infinite while loop.\n");

    //
    // Print the compiler version.
    //
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
    am_util_stdio_printf("HAL Compiler:    %s\n", g_ui8HALcompiler);
    am_util_stdio_printf("HAL SDK version: %d.%d.%d\n",
                         g_ui32HALversion.s.Major,
                         g_ui32HALversion.s.Minor,
                         g_ui32HALversion.s.Revision);
    am_util_stdio_printf("HAL compiled with %s-style registers\n",
                         g_ui32HALversion.s.bAMREGS ? "AM_REG" : "CMSIS");

    am_util_stdio_printf("\nEntering while loop...\n");

    //
    // To minimize power during the run, disable the UART.
    //
    am_hal_flash_delay(FLASH_CYCLES_US(10000));
    am_bsp_uart_printf_disable();
#endif // PRINT_UART

    //
    // Set MCU for minimal power
    //
    set_for_min_power();

    //
    // Enter the while loop
    //
    while(1)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}
