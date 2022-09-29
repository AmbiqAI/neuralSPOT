//*****************************************************************************
//
//! @file while.c
//!
//! @brief Example to emulate a polling loop.
//!
//! Purpose: This example provides a demonstration of the power required while
//! executing in a tight loop on the Apollo4 MCU.
//!
//!
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Print option
//
//*****************************************************************************
#define PRINT_UART  1

#if 0
//
// The while loop, shown below in its C implementation, should be 32-byte aligned.
// To achieve this alignment easily across toolchains, we'll create the loop
// using an array of opcodes equivalent to the C code.
//
void while_loop(void)
{
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
#endif

//
// Prototype the assembly function.
//
typedef void (*whileloopfunc_t)(void);
#if (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION < 6000000)
__align(32)
#define WHILE_ATTRIB
#elif (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION >= 6000000)
#warning This attribute is not yet tested on ARM6.
#define WHILE_ATTRIB   __attribute__ ((aligned (32)))
#elif defined(__GNUC_STDC_INLINE__)
#define WHILE_ATTRIB   __attribute__ ((aligned (32)))
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 32
#define WHILE_ATTRIB
#else
#error Unknown compiler.
#endif

//
// This is the aligned while loop, aka while_loop().
//
static
const
uint16_t whileloop[12] WHILE_ATTRIB =
{
    0xBF00, 0xBF00,
    0xBF00, 0xBF00,
    0xBF00, 0xBF00,
    0xBF00, 0xBF00,
    0xBF00, 0xBF00,
    0xE7F4,         // E7F4: b while_loop
    0x4770,         // 4770: bx lr (not actually needed and will never get here)
};
whileloopfunc_t while_loop = (whileloopfunc_t)((uint8_t *)whileloop + 1);

//*****************************************************************************
//
// Minimize power
//
//*****************************************************************************
static void apollo4_cache_memory_config(void)
{
    am_hal_pwrctrl_sram_memcfg_t SRAMMemCfg =
    {
        .eSRAMCfg           = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithMCU     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithGFX     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDISP    = AM_HAL_PWRCTRL_SRAM_NONE,
        .eActiveWithDSP     = AM_HAL_PWRCTRL_SRAM_NONE,
        .eSRAMRetain        = AM_HAL_PWRCTRL_SRAM_NONE
    };

    am_hal_pwrctrl_mcu_memory_config_t McuMemCfg =
    {
        .eCacheCfg    = AM_HAL_PWRCTRL_CACHEB0_ONLY,
        .bRetainCache = false,
        .eDTCMCfg     = AM_HAL_PWRCTRL_DTCM_8K,
        .eRetainDTCM  = AM_HAL_PWRCTRL_DTCM_8K,
        .bEnableNVM0  = true,
        .bRetainNVM0  = true
    };

    am_hal_pwrctrl_mcu_memory_config(&McuMemCfg);

    //
    // Configure the cache
    //
    CPU->CACHECFG_b.CONFIG = CPU_CACHECFG_CONFIG_W1_128B_2048E; // Direct mapped, 128-bit linesize, 2048 entries (4 SRAMs active)

    CPU->CACHECFG_b.DENABLE = 1; // 0 = disable data cache, 1 = enable data cache
    CPU->CACHECFG_b.IENABLE = 1; // 0 = disable instr cache, 1 = enable instr cache
    CPU->CACHECFG_b.ENABLE  = 1; // 0 = disable cache, 1 = enable cache

    PWRCTRL->MEMPWREN_b.PWRENCACHEB2 = PWRCTRL_MEMPWREN_PWRENCACHEB2_DIS;
    PWRCTRL->MEMPWREN_b.PWRENCACHEB0 = PWRCTRL_MEMPWREN_PWRENCACHEB0_EN;
    PWRCTRL->MEMRETCFG_b.CACHEPWDSLP = PWRCTRL_MEMRETCFG_CACHEPWDSLP_DIS;

    //
    // Configure the MRAM for low power mode
    //
    MCUCTRL->MRAMPWRCTRL_b.MRAMLPREN = 1;
    MCUCTRL->MRAMPWRCTRL_b.MRAMSLPEN = 0;
#if defined(AM_PART_APOLLO4B)
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 0;
#elif defined(AM_PART_APOLLO4P) // AM_PART_APOLLO4B
    MCUCTRL->MRAMPWRCTRL_b.MRAMPWRCTRL = 1;
#endif

    //
    // Disable SRAM
    //
    am_hal_pwrctrl_sram_config(&SRAMMemCfg);

    MCUCTRL->PWRSW0_b.PWRSWVDDMDSP0DYNSEL = 0;    // Bit 18
    MCUCTRL->PWRSW0_b.PWRSWVDDMDSP1DYNSEL = 0;    // Bit 21
    MCUCTRL->PWRSW0_b.PWRSWVDDMLDYNSEL    = 0;    // Bit 24

} // apollo4_cache_memory_config()

//*****************************************************************************
//
// Main Function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#if defined(AM_PART_APOLLO4B)
    //
    // Note that this differs slightly from the Apollo4b HAL setting of this
    // field to 0x3E. The low bit is DAXI CLKGATE.
    //
    CLKGEN->MISC_b.CLKGENMISCSPARES = 0x3F;

#endif // AM_PART_APOLLO4B

#if defined (AM_PART_APOLLO4B)
    //
    // Power down Crypto.
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_CRYPTO_POWERDOWN, 0);

    //
    // Disable all peripherals
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_DIS_PERIPHS_ALL, 0);

    //
    // Disable XTAL
    //
    am_hal_pwrctrl_control(AM_HAL_PWRCTRL_CONTROL_XTAL_PWDN_DEEPSLEEP, 0);
#elif defined (AM_PART_APOLLO4P)
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_CRYPTO);
#endif

    //
    // Set memory configuration to minimum.
    //
    apollo4_cache_memory_config();

#ifdef AM_DEVICES_BLECTRLR_RESET_PIN
    //
    // For SiP packages, put the BLE Controller in reset.
    //
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_pinconfig(AM_DEVICES_BLECTRLR_RESET_PIN,   am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_DEVICES_BLECTRLR_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
#endif // AM_DEVICES_BLECTRLR_RESET_PIN

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
    am_hal_delay_us(10000);
    am_bsp_uart_printf_disable();
#endif // PRINT_UART

    //
    // Enter the while loop
    //
    while_loop();

} // main()
