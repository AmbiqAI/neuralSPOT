//*****************************************************************************
//
//! @file startup_iar.c
//!
//! @brief Definitions for interrupt handlers, the vector table, and the stack.
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

#include <stdint.h>

//*****************************************************************************
//
// Enable the IAR extensions for this source file.
//
//*****************************************************************************
#pragma language = extended

//*****************************************************************************
//
// Weak function links.
//
//*****************************************************************************
#pragma weak MemManage_Handler      = HardFault_Handler
#pragma weak BusFault_Handler       = HardFault_Handler
#pragma weak UsageFault_Handler     = HardFault_Handler
#pragma weak SVC_Handler            = am_default_isr
#pragma weak DebugMon_Handler       = am_default_isr
#pragma weak PendSV_Handler         = am_default_isr
#pragma weak SysTick_Handler        = am_default_isr

#pragma weak am_brownout_isr        = am_default_isr
#pragma weak am_watchdog_isr        = am_default_isr
#pragma weak am_rtc_isr             = am_default_isr
#pragma weak am_vcomp_isr           = am_default_isr
#pragma weak am_ioslave_ios_isr     = am_default_isr
#pragma weak am_ioslave_acc_isr     = am_default_isr
#pragma weak am_iomaster0_isr       = am_default_isr
#pragma weak am_iomaster1_isr       = am_default_isr
#pragma weak am_iomaster2_isr       = am_default_isr
#pragma weak am_iomaster3_isr       = am_default_isr
#pragma weak am_iomaster4_isr       = am_default_isr
#pragma weak am_iomaster5_isr       = am_default_isr
#pragma weak am_iomaster6_isr       = am_default_isr
#pragma weak am_iomaster7_isr       = am_default_isr
#pragma weak am_ctimer_isr          = am_default_isr
#pragma weak am_uart_isr            = am_default_isr
#pragma weak am_uart1_isr           = am_default_isr
#pragma weak am_uart2_isr           = am_default_isr
#pragma weak am_uart3_isr           = am_default_isr
#pragma weak am_adc_isr             = am_default_isr
#pragma weak am_mspi0_isr           = am_default_isr
#pragma weak am_mspi1_isr           = am_default_isr
#pragma weak am_mspi2_isr           = am_default_isr
#pragma weak am_clkgen_isr          = am_default_isr
#pragma weak am_cryptosec_isr       = am_default_isr
#pragma weak am_sdio_isr            = am_default_isr
#pragma weak am_usb_isr             = am_default_isr
#pragma weak am_gpu_isr             = am_default_isr
#pragma weak am_disp_isr            = am_default_isr
#pragma weak am_dsi_isr             = am_default_isr
#pragma weak am_stimer_cmpr0_isr    = am_default_isr
#pragma weak am_stimer_cmpr1_isr    = am_default_isr
#pragma weak am_stimer_cmpr2_isr    = am_default_isr
#pragma weak am_stimer_cmpr3_isr    = am_default_isr
#pragma weak am_stimer_cmpr4_isr    = am_default_isr
#pragma weak am_stimer_cmpr5_isr    = am_default_isr
#pragma weak am_stimer_cmpr6_isr    = am_default_isr
#pragma weak am_stimer_cmpr7_isr    = am_default_isr
#pragma weak am_stimerof_isr        = am_default_isr
#pragma weak am_audadc0_isr         = am_default_isr
#pragma weak am_dspi2s0_isr         = am_default_isr
#pragma weak am_dspi2s1_isr         = am_default_isr
#pragma weak am_dspi2s2_isr         = am_default_isr
#pragma weak am_dspi2s3_isr         = am_default_isr
#pragma weak am_pdm0_isr            = am_default_isr
#pragma weak am_pdm1_isr            = am_default_isr
#pragma weak am_pdm2_isr            = am_default_isr
#pragma weak am_pdm3_isr            = am_default_isr
#pragma weak am_gpio0_001f_isr      = am_default_isr
#pragma weak am_gpio0_203f_isr      = am_default_isr
#pragma weak am_gpio0_405f_isr      = am_default_isr
#pragma weak am_gpio0_607f_isr      = am_default_isr
#pragma weak am_gpio1_001f_isr      = am_default_isr
#pragma weak am_gpio1_203f_isr      = am_default_isr
#pragma weak am_gpio1_405f_isr      = am_default_isr
#pragma weak am_gpio1_607f_isr      = am_default_isr
#pragma weak am_timer00_isr         = am_default_isr
#pragma weak am_timer01_isr         = am_default_isr
#pragma weak am_timer02_isr         = am_default_isr
#pragma weak am_timer03_isr         = am_default_isr
#pragma weak am_timer04_isr         = am_default_isr
#pragma weak am_timer05_isr         = am_default_isr
#pragma weak am_timer06_isr         = am_default_isr
#pragma weak am_timer07_isr         = am_default_isr
#pragma weak am_timer08_isr         = am_default_isr
#pragma weak am_timer09_isr         = am_default_isr
#pragma weak am_timer10_isr         = am_default_isr
#pragma weak am_timer11_isr         = am_default_isr
#pragma weak am_timer12_isr         = am_default_isr
#pragma weak am_timer13_isr         = am_default_isr
#pragma weak am_timer14_isr         = am_default_isr
#pragma weak am_timer15_isr         = am_default_isr
#pragma weak am_cachecpu_isr        = am_default_isr


//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
extern __stackless void Reset_Handler(void);
extern __weak void NMI_Handler(void);
extern __weak void HardFault_Handler(void);
extern        void MemManage_Handler(void);
extern        void BusFault_Handler(void);
extern        void UsageFault_Handler(void);
extern        void SVC_Handler(void);
extern        void DebugMon_Handler(void);
extern        void PendSV_Handler(void);
extern        void SysTick_Handler(void);

extern void am_brownout_isr(void);
extern void am_watchdog_isr(void);
extern void am_rtc_isr(void);
extern void am_vcomp_isr(void);
extern void am_ioslave_ios_isr(void);
extern void am_ioslave_acc_isr(void);
extern void am_iomaster0_isr(void);
extern void am_iomaster1_isr(void);
extern void am_iomaster2_isr(void);
extern void am_iomaster3_isr(void);
extern void am_iomaster4_isr(void);
extern void am_iomaster5_isr(void);
extern void am_iomaster6_isr(void);
extern void am_iomaster7_isr(void);
extern void am_ctimer_isr(void);
extern void am_uart_isr(void);
extern void am_uart1_isr(void);
extern void am_uart2_isr(void);
extern void am_uart3_isr(void);
extern void am_adc_isr(void);
extern void am_mspi0_isr(void);
extern void am_mspi1_isr(void);
extern void am_mspi2_isr(void);
extern void am_clkgen_isr(void);
extern void am_cryptosec_isr(void);
extern void am_sdio_isr(void);
extern void am_usb_isr(void);
extern void am_gpu_isr(void);
extern void am_disp_isr(void);
extern void am_dsi_isr(void);
extern void am_stimer_cmpr0_isr(void);
extern void am_stimer_cmpr1_isr(void);
extern void am_stimer_cmpr2_isr(void);
extern void am_stimer_cmpr3_isr(void);
extern void am_stimer_cmpr4_isr(void);
extern void am_stimer_cmpr5_isr(void);
extern void am_stimer_cmpr6_isr(void);
extern void am_stimer_cmpr7_isr(void);
extern void am_stimerof_isr(void);
extern void am_audadc0_isr(void);
extern void am_dspi2s0_isr(void);
extern void am_dspi2s1_isr(void);
extern void am_dspi2s2_isr(void);
extern void am_dspi2s3_isr(void);
extern void am_pdm0_isr(void);
extern void am_pdm1_isr(void);
extern void am_pdm2_isr(void);
extern void am_pdm3_isr(void);
extern void am_gpio0_001f_isr(void);
extern void am_gpio0_203f_isr(void);
extern void am_gpio0_405f_isr(void);
extern void am_gpio0_607f_isr(void);
extern void am_gpio1_001f_isr(void);
extern void am_gpio1_203f_isr(void);
extern void am_gpio1_405f_isr(void);
extern void am_gpio1_607f_isr(void);
extern void am_timer00_isr(void);
extern void am_timer01_isr(void);
extern void am_timer02_isr(void);
extern void am_timer03_isr(void);
extern void am_timer04_isr(void);
extern void am_timer05_isr(void);
extern void am_timer06_isr(void);
extern void am_timer07_isr(void);
extern void am_timer08_isr(void);
extern void am_timer09_isr(void);
extern void am_timer10_isr(void);
extern void am_timer11_isr(void);
extern void am_timer12_isr(void);
extern void am_timer13_isr(void);
extern void am_timer14_isr(void);
extern void am_timer15_isr(void);
extern void am_cachecpu_isr(void);

extern void am_default_isr(void);

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void __iar_program_start(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static uint32_t pui32Stack[256] @ ".noinit";

//*****************************************************************************
//
// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
//
//*****************************************************************************
typedef union
{
    void (*pfnHandler)(void);
    uint32_t ui32Ptr;
}
uVectorEntry;

//*****************************************************************************
//
// The vector table.
//
// Proper alignment of the vector table is dependent on the number of
// external (peripheral) interrupts, see the following table for proper
// vectorbaseaddress alignment.
//     0-16      32-word
//    17-48      64-word
//    49-112    128-word  (Apollo4)
//   113-240    256-word
//
// The Apollo4 vector table must be located on a 512 byte boundary.
//
// Note: Aliasing and weakly exporting am_mpufault_isr, am_busfault_isr, and
// am_usagefault_isr does not work if am_fault_isr is defined externally.
// Therefore, we'll explicitly use am_fault_isr in the table for those vectors.
//
//*****************************************************************************
__root const uVectorEntry __vector_table[] @ ".intvec" =
{
    { .ui32Ptr = (uint32_t)pui32Stack + sizeof(pui32Stack) },
                                            // The initial stack pointer
    Reset_Handler,                          // The reset handler
    NMI_Handler,                            // The NMI handler
    HardFault_Handler,                      // The hard fault handler
    MemManage_Handler,                      // The MemManage_Handler
    BusFault_Handler,                       // The BusFault_Handler
    UsageFault_Handler,                     // The UsageFault_Handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVC_Handler,                            // SVCall handler
    DebugMon_Handler,                       // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                         // The PendSV handler
    SysTick_Handler,                        // The SysTick handler

    //
    // Peripheral Interrupts
    //
    am_brownout_isr,                        //  0: Brownout (rstgen)
    am_watchdog_isr,                        //  1: Watchdog (WDT)
    am_rtc_isr,                             //  2: RTC
    am_vcomp_isr,                           //  3: Voltage Comparator
    am_ioslave_ios_isr,                     //  4: I/O Slave general
    am_ioslave_acc_isr,                     //  5: I/O Slave access
    am_iomaster0_isr,                       //  6: I/O Master 0
    am_iomaster1_isr,                       //  7: I/O Master 1
    am_iomaster2_isr,                       //  8: I/O Master 2
    am_iomaster3_isr,                       //  9: I/O Master 3
    am_iomaster4_isr,                       // 10: I/O Master 4
    am_iomaster5_isr,                       // 11: I/O Master 5
    am_iomaster6_isr,                       // 12: I/O Master 6 (I3C/I2C/SPI)
    am_iomaster7_isr,                       // 13: I/O Master 7 (I3C/I2C/SPI)
    am_ctimer_isr,                          // 14: OR of all timerX interrupts
    am_uart_isr,                            // 15: UART0
    am_uart1_isr,                           // 16: UART1
    am_uart2_isr,                           // 17: UART2
    am_uart3_isr,                           // 18: UART3
    am_adc_isr,                             // 19: ADC
    am_mspi0_isr,                           // 20: MSPI0
    am_mspi1_isr,                           // 21: MSPI1
    am_mspi2_isr,                           // 22: MSPI2
    am_clkgen_isr,                          // 23: ClkGen
    am_cryptosec_isr,                       // 24: Crypto Secure
    am_default_isr,                         // 25: Reserved
    am_sdio_isr,                            // 26: SDIO
    am_usb_isr,                             // 27: USB
    am_gpu_isr,                             // 28: GPU
    am_disp_isr,                            // 29: DISP
    am_dsi_isr,                             // 30: DSI
    am_default_isr,                         // 31: Reserved
    am_stimer_cmpr0_isr,                    // 32: System Timer Compare0
    am_stimer_cmpr1_isr,                    // 33: System Timer Compare1
    am_stimer_cmpr2_isr,                    // 34: System Timer Compare2
    am_stimer_cmpr3_isr,                    // 35: System Timer Compare3
    am_stimer_cmpr4_isr,                    // 36: System Timer Compare4
    am_stimer_cmpr5_isr,                    // 37: System Timer Compare5
    am_stimer_cmpr6_isr,                    // 38: System Timer Compare6
    am_stimer_cmpr7_isr,                    // 39: System Timer Compare7
    am_stimerof_isr,                        // 40: System Timer Cap Overflow
    am_default_isr,                         // 41: Reserved
    am_audadc0_isr,                         // 42: Audio ADC
    am_default_isr,                         // 43: Reserved
    am_dspi2s0_isr,                         // 44: I2S0
    am_dspi2s1_isr,                         // 45: I2S1
    am_dspi2s2_isr,                         // 46: I2S2
    am_dspi2s3_isr,                         // 47: I2S3
    am_pdm0_isr,                            // 48: PDM0
    am_pdm1_isr,                            // 49: PDM1
    am_pdm2_isr,                            // 50: PDM2
    am_pdm3_isr,                            // 51: PDM3
    am_default_isr,                         // 52: Reserved
    am_default_isr,                         // 53: Reserved
    am_default_isr,                         // 54: Reserved
    am_default_isr,                         // 55: Reserved
    am_gpio0_001f_isr,                      // 56: GPIO N0 pins  0-31
    am_gpio0_203f_isr,                      // 57: GPIO N0 pins 32-63
    am_gpio0_405f_isr,                      // 58: GPIO N0 pins 64-95
    am_gpio0_607f_isr,                      // 59: GPIO N0 pins 96-104, virtual 105-127
    am_gpio1_001f_isr,                      // 60: GPIO N1 pins  0-31
    am_gpio1_203f_isr,                      // 61: GPIO N1 pins 32-63
    am_gpio1_405f_isr,                      // 62: GPIO N1 pins 64-95
    am_gpio1_607f_isr,                      // 63: GPIO N1 pins 96-104, virtual 105-127
    am_default_isr,                         // 64: Reserved
    am_default_isr,                         // 65: Reserved
    am_default_isr,                         // 66: Reserved
    am_timer00_isr,                         // 67: timer0
    am_timer01_isr,                         // 68: timer1
    am_timer02_isr,                         // 69: timer2
    am_timer03_isr,                         // 70: timer3
    am_timer04_isr,                         // 71: timer4
    am_timer05_isr,                         // 72: timer5
    am_timer06_isr,                         // 73: timer6
    am_timer07_isr,                         // 74: timer7
    am_timer08_isr,                         // 75: timer8
    am_timer09_isr,                         // 76: timer9
    am_timer10_isr,                         // 77: timer10
    am_timer11_isr,                         // 78: timer11
    am_timer12_isr,                         // 79: timer12
    am_timer13_isr,                         // 80: timer13
    am_timer14_isr,                         // 81: timer14
    am_timer15_isr,                         // 82: timer15
    am_cachecpu_isr                         // 83: CPU cache
};

//******************************************************************************
//
// Place code immediately following vector table.
//
//******************************************************************************
//******************************************************************************
//
// The Patch table.
//
// The patch table should pad the vector table size to a total of 128 entries
// such that the code begins at 0x200.
// In other words, the final peripheral IRQ is always IRQ 111 (0-based).
//
//******************************************************************************
__root const uint32_t __Patchable[] @ ".patch" =
{
                0, 0, 0, 0, 0, 0,           //  84-89
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           //  90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 100-109
    0, 0                                    // 110-111
};

//*****************************************************************************
//
// Note - The template for this function is originally found in IAR's module,
//        low_level_init.c. As supplied by IAR, it is an empty function.
//
// This module contains the function `__low_level_init', a function
// that is called before the `main' function of the program.  Normally
// low-level initializations - such as setting the prefered interrupt
// level or setting the watchdog - can be performed here.
//
// Note that this function is called before the data segments are
// initialized, this means that this function cannot rely on the
// values of global or static variables.
//
// When this function returns zero, the startup code will inhibit the
// initialization of the data segments. The result is faster startup,
// the drawback is that neither global nor static data will be
// initialized.
//
// Copyright 1999-2017 IAR Systems AB.
//
// $Revision: 112610 $
//
//
//
//
//*****************************************************************************
#define AM_REGVAL(x)               (*((volatile uint32_t *)(x)))
#define VTOR_ADDR                   0xE000ED08

__interwork int __low_level_init(void)
{

    AM_REGVAL(VTOR_ADDR) = (uint32_t)&__vector_table;

    /*==================================*/
    /* Choose if segment initialization */
    /* should be done or not.           */
    /* Return: 0 to omit seg_init       */
    /*         1 to run seg_init        */
    /*==================================*/
    return 1;
}

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.
//
//*****************************************************************************
void
Reset_Handler(void)
{
    //
    // Call the application's entry point.
    //
    __iar_program_start();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
__weak void
NMI_Handler(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
__weak void
HardFault_Handler(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
void
am_default_isr(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}

