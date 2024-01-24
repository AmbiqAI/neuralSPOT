//*****************************************************************************
//
//! @file startup_iar.c
//!
//! @brief Definitions for interrupt handlers, the vector table, and the stack.
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
#pragma weak am_ble_isr             = am_default_isr
#pragma weak am_gpio_isr            = am_default_isr
#pragma weak am_ctimer_isr          = am_default_isr
#pragma weak am_uart_isr            = am_default_isr
#pragma weak am_uart1_isr           = am_default_isr
#pragma weak am_scard_isr           = am_default_isr
#pragma weak am_adc_isr             = am_default_isr
#pragma weak am_pdm0_isr            = am_default_isr
#pragma weak am_mspi0_isr           = am_default_isr
#pragma weak am_software0_isr       = am_default_isr
#pragma weak am_stimer_isr          = am_default_isr
#pragma weak am_stimer_cmpr0_isr    = am_default_isr
#pragma weak am_stimer_cmpr1_isr    = am_default_isr
#pragma weak am_stimer_cmpr2_isr    = am_default_isr
#pragma weak am_stimer_cmpr3_isr    = am_default_isr
#pragma weak am_stimer_cmpr4_isr    = am_default_isr
#pragma weak am_stimer_cmpr5_isr    = am_default_isr
#pragma weak am_stimer_cmpr6_isr    = am_default_isr
#pragma weak am_stimer_cmpr7_isr    = am_default_isr
#pragma weak am_flash_isr           = am_default_isr
#pragma weak am_clkgen_isr          = am_default_isr
#pragma weak am_mspi1_isr           = am_default_isr
#pragma weak am_mspi2_isr           = am_default_isr


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
extern void am_ble_isr(void);
extern void am_gpio_isr(void);
extern void am_ctimer_isr(void);
extern void am_uart_isr(void);
extern void am_uart1_isr(void);
extern void am_scard_isr(void);
extern void am_adc_isr(void);
extern void am_pdm0_isr(void);
extern void am_mspi0_isr(void);
extern void am_software0_isr(void);
extern void am_stimer_isr(void);
extern void am_stimer_cmpr0_isr(void);
extern void am_stimer_cmpr1_isr(void);
extern void am_stimer_cmpr2_isr(void);
extern void am_stimer_cmpr3_isr(void);
extern void am_stimer_cmpr4_isr(void);
extern void am_stimer_cmpr5_isr(void);
extern void am_stimer_cmpr6_isr(void);
extern void am_stimer_cmpr7_isr(void);
extern void am_flash_isr(void);
extern void am_clkgen_isr(void);
extern void am_mspi1_isr(void);
extern void am_mspi2_isr(void);

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
static uint32_t pui32Stack[2560] @ ".stack";

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
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
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
    am_watchdog_isr,                        //  1: Watchdog
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
    am_ble_isr,                             // 12: BLEIF
    am_gpio_isr,                            // 13: GPIO
    am_ctimer_isr,                          // 14: CTIMER
    am_uart_isr,                            // 15: UART0
    am_uart1_isr,                           // 16: UART1
    am_scard_isr,                           // 17: SCARD
    am_adc_isr,                             // 18: ADC
    am_pdm0_isr,                            // 19: PDM
    am_mspi0_isr,                           // 20: MSPI0
    am_software0_isr,                       // 21: SOFTWARE0
    am_stimer_isr,                          // 22: SYSTEM TIMER
    am_stimer_cmpr0_isr,                    // 23: SYSTEM TIMER COMPARE0
    am_stimer_cmpr1_isr,                    // 24: SYSTEM TIMER COMPARE1
    am_stimer_cmpr2_isr,                    // 25: SYSTEM TIMER COMPARE2
    am_stimer_cmpr3_isr,                    // 26: SYSTEM TIMER COMPARE3
    am_stimer_cmpr4_isr,                    // 27: SYSTEM TIMER COMPARE4
    am_stimer_cmpr5_isr,                    // 28: SYSTEM TIMER COMPARE5
    am_stimer_cmpr6_isr,                    // 29: SYSTEM TIMER COMPARE6
    am_stimer_cmpr7_isr,                    // 30: SYSTEM TIMER COMPARE7
    am_clkgen_isr,                          // 31: CLKGEN
    am_mspi1_isr,                           // 32: MSPI1
    am_mspi2_isr,                           // 33: MSPI2
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
// The patch table should pad the vector table size to a total of 64 entries
// (16 core + 48 periph) such that code begins at offset 0x100.
//
//******************************************************************************
__root const uint32_t __Patchable[] @ ".patch" =
{
    0,                                      // 34
    0,                                      // 35
    0,                                      // 36
    0,                                      // 37
    0,                                      // 38
    0,                                      // 39
    0,                                      // 40
    0,                                      // 41
    0,                                      // 42
    0,                                      // 43
    0,                                      // 44
    0,                                      // 45
    0,                                      // 46
    0,                                      // 47
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

