//*****************************************************************************
//
//! @file startup_gcc.c
//!
//! @brief Definitions for interrupt handlers, the vector table, and the stack.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
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
// This is part of revision release_a5b_sdk2-748191cd0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "apollo510.h"

//*****************************************************************************
//
// Forward declaration of interrupt handlers.
//
//*****************************************************************************
extern void Reset_Handler(void)             __attribute ((naked));
extern void NMI_Handler(void)               __attribute ((weak));
extern void HardFault_Handler(void)         __attribute ((weak));
extern void MemManage_Handler(void)         __attribute ((weak, alias ("HardFault_Handler")));
extern void BusFault_Handler(void)          __attribute ((weak, alias ("HardFault_Handler")));
extern void UsageFault_Handler(void)        __attribute ((weak, alias ("HardFault_Handler")));
extern void SecureFault_Handler(void)       __attribute ((weak, alias ("am_default_isr")));
extern void SVC_Handler(void)               __attribute ((weak, alias ("am_default_isr")));
extern void DebugMon_Handler(void)          __attribute ((weak, alias ("am_default_isr")));
extern void PendSV_Handler(void)            __attribute ((weak, alias ("am_default_isr")));
extern void SysTick_Handler(void)           __attribute ((weak, alias ("am_default_isr")));
extern void FloatingPoint_Handler(void)     __attribute ((weak, alias ("am_default_isr")));

extern void am_brownout_isr(void)           __attribute ((weak, alias ("am_default_isr")));
extern void am_watchdog_isr(void)           __attribute ((weak, alias ("am_default_isr")));
extern void am_rtc_isr(void)                __attribute ((weak, alias ("am_default_isr")));
extern void am_vcomp_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_ioslave_ios_isr(void)        __attribute ((weak, alias ("am_default_isr")));
extern void am_ioslave_acc_isr(void)        __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster0_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster1_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster2_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster3_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster4_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster5_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster6_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_iomaster7_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_ctimer_isr(void)             __attribute ((weak, alias ("am_default_isr")));
extern void am_uart_isr(void)               __attribute ((weak, alias ("am_default_isr")));
extern void am_uart1_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_uart2_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_uart3_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_adc_isr(void)                __attribute ((weak, alias ("am_default_isr")));
extern void am_mspi0_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_mspi1_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_mspi2_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_mspi3_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_clkgen_isr(void)             __attribute ((weak, alias ("am_default_isr")));
extern void am_cryptosec_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_sdio0_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_sdio1_isr(void)              __attribute ((weak, alias ("am_default_isr")));
extern void am_usb_isr(void)                __attribute ((weak, alias ("am_default_isr")));
extern void am_gpu_isr(void)                __attribute ((weak, alias ("am_default_isr")));
extern void am_disp_isr(void)               __attribute ((weak, alias ("am_default_isr")));
extern void am_dsi_isr(void)                __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr0_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr1_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr2_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr3_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr4_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr5_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr6_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimer_cmpr7_isr(void)       __attribute ((weak, alias ("am_default_isr")));
extern void am_stimerof_isr(void)           __attribute ((weak, alias ("am_default_isr")));
extern void am_audadc0_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_dspi2s0_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_dspi2s1_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_pdm0_isr(void)               __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_001f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_203f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_405f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_607f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_809f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_a0bf_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_c0df_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio0_e0ff_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_001f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_203f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_405f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_607f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_809f_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_a0bf_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_gpio1_c0df_isr(void)         __attribute ((weak, alias ("am_default_isr")));
extern void am_timer00_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer01_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer02_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer03_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer04_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer05_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer06_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer07_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer08_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer09_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer10_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer11_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer12_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer13_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer14_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_timer15_isr(void)            __attribute ((weak, alias ("am_default_isr")));
extern void am_cachecpu_isr(void)           __attribute ((weak, alias ("am_default_isr")));
extern void am_software0_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_software1_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_software2_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_software3_isr(void)          __attribute ((weak, alias ("am_default_isr")));
extern void am_otp_isr(void)                __attribute ((weak, alias ("am_default_isr")));
extern void am_ioslave_fd0_isr(void)        __attribute ((weak, alias ("am_default_isr")));
extern void am_ioslave_fd0_acc_isr(void)    __attribute ((weak, alias ("am_default_isr")));
extern void am_ioslave_fd1_isr(void)        __attribute ((weak, alias ("am_default_isr")));
extern void am_ioslave_fd1_acc_isr(void)    __attribute ((weak, alias ("am_default_isr")));
extern void am_default_isr(void)            __attribute ((weak));

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
__attribute__ ((section(".stack")))
static uint32_t g_pui32Stack[STACK_SIZE];

#ifndef HEAP_SIZE
    #define HEAP_SIZE 1024
#endif

__attribute__ ((section(".heap"))) __attribute__ ((__used__))
static uint32_t g_pui32Heap[HEAP_SIZE];

#define AM_STACK_SIZE   (sizeof(g_pui32Stack))
#define AM_STACK_LIMIT  ((uint32_t)&g_pui32Stack)
#define AM_HEAP_SIZE    (sizeof(g_pui32Heap))

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
__attribute__ ((section(".isr_vector")))
void (* const __Vectors[])(void) =
{
    (void (*)(void))((uint32_t)g_pui32Stack + sizeof(g_pui32Stack)),
                                            // The initial stack pointer
    Reset_Handler,                          // The reset handler
    NMI_Handler,                            // The NMI handler
    HardFault_Handler,                      // The hard fault handler
    MemManage_Handler,                      // The MemManage_Handler
    BusFault_Handler,                       // The BusFault_Handler
    UsageFault_Handler,                     // The UsageFault_Handler
    SecureFault_Handler,                    // The Secure Fault Handler
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
    am_sdio0_isr,                           // 26: SDIO0
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
    am_default_isr,                         // 46: Reserved
    am_default_isr,                         // 47: Reserved
    am_pdm0_isr,                            // 48: PDM0
    am_default_isr,                         // 49: Reserved
    am_default_isr,                         // 50: Reserved
    am_default_isr,                         // 51: Reserved
    am_default_isr,                         // 52: Reserved
    am_default_isr,                         // 53: Reserved
    am_mspi3_isr,                           // 54: MSPI3
    am_default_isr,                         // 55: Reserved
    am_gpio0_001f_isr,                      // 56: GPIO N0 pins  0-31
    am_gpio0_203f_isr,                      // 57: GPIO N0 pins 32-63
    am_gpio0_405f_isr,                      // 58: GPIO N0 pins 64-95
    am_gpio0_607f_isr,                      // 59: GPIO N0 pins 96-127
    am_gpio0_809f_isr,                      // 60: GPIO N0 pins 128-159
    am_gpio0_a0bf_isr,                      // 61: GPIO N0 pins 160-191
    am_gpio0_c0df_isr,                      // 62: GPIO N0 pins 192-223
    am_gpio0_e0ff_isr,                      // 63: GPIO N0 pins 224-256
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
    am_default_isr,                         // 83: Reserved
    am_sdio1_isr,                           // 84: SDIO1
    am_default_isr,                         // 85: Reserved
    am_default_isr,                         // 86: Reserved
    am_default_isr,                         // 87: Reserved
    am_default_isr,                         // 88: Reserved
    am_default_isr,                         // 89: Reserved
    am_default_isr,                         // 90: Reserved
    am_default_isr,                         // 91: Reserved
    am_software0_isr,                       // 92: SOFTWARE0
    am_software1_isr,                       // 93: SOFTWARE1
    am_software2_isr,                       // 94: SOFTWARE2
    am_software3_isr,                       // 95: SOFTWARE3
    am_ioslave_fd0_isr,                     // 96: Reserved
    am_ioslave_fd0_acc_isr,                 // 97: Reserved
    am_ioslave_fd1_isr,                     // 98: Reserved
    am_ioslave_fd1_acc_isr,                 // 99: Reserved
    am_default_isr,                         // 100: Reserved
    am_default_isr,                         // 101: Reserved
    am_default_isr,                         // 102: Reserved
    am_default_isr,                         // 103: Reserved
    am_default_isr,                         // 104: Reserved
    am_default_isr,                         // 105: Reserved
    am_default_isr,                         // 106: Reserved
    am_default_isr,                         // 107: Reserved
    am_default_isr,                         // 108: Reserved
    am_default_isr,                         // 109: Reserved
    am_default_isr,                         // 110: Reserved
    am_default_isr,                         // 111: Reserved
    am_default_isr,                         // 112: Reserved
    am_default_isr,                         // 113: Reserved
    am_default_isr,                         // 114: Reserved
    am_default_isr,                         // 115: Reserved
    am_default_isr,                         // 116: Reserved
    am_default_isr,                         // 117: Reserved
    am_default_isr,                         // 118: Reserved
    am_default_isr,                         // 119: Reserved
    am_default_isr,                         // 120: Reserved
    am_default_isr,                         // 121: Reserved
    am_default_isr,                         // 122: Reserved
    am_default_isr,                         // 123: Reserved
    am_default_isr,                         // 124: Reserved
    am_gpio1_001f_isr,                      // 125: GPIO N1 pins  0-31
    am_gpio1_203f_isr,                      // 126: GPIO N1 pins 32-63
    am_gpio1_405f_isr,                      // 127: GPIO N1 pins 64-95
    am_gpio1_607f_isr,                      // 128: GPIO N1 pins 96-127
    am_gpio1_809f_isr,                      // 129: GPIO N1 pins 128-159
    am_gpio1_a0bf_isr,                      // 130: GPIO N1 pins 160-191
    am_gpio1_c0df_isr,                      // 131: GPIO N1 pins 192-223
    am_default_isr,                         // 132: Reserved
    FloatingPoint_Handler,                  // 133: Floating Point Exception
    am_otp_isr                              // 134: OTP
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
__attribute__ ((section(".patch")))
uint32_t const __Patchable[] =
{
                   0, 0, 0, 0, 0,           // 135-139
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 140-149
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 150-159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 160-169
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 170-179
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 180-189
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 190-199
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 200-209
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 210-219
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,           // 220-229
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0            // 230-239
};

// define the start of the patch table - at what would be vector 135
const uint32_t  * const __pPatchable =  (uint32_t *) __Patchable;

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.
//
//*****************************************************************************
#if defined(__GNUC_STDC_INLINE__)

// Stub out some CPP init-related functions
extern void _init(void) { ; }
extern void _fini(void) { ; }
void *__dso_handle = 0;
extern void __libc_init_array(void);

void
Reset_Handler(void)
{
    //
    // Set the vector table pointer.
    //
    __asm("    ldr    r0, =0xE000ED08\n"
          "    ldr    r1, =__Vectors\n"
          "    str    r1, [r0]");

    //
    // Set the stack pointer.
    //
    __asm("    ldr    sp, [r1]");
#ifndef NOFPU
    //
    // Enable the FPU.
    //
    __asm("ldr  r0, =0xE000ED88\n"
          "ldr  r1,[r0]\n"
          "orr  r1,#(0xF << 20)\n"
          "str  r1,[r0]\n"
          "dsb\n"
          "isb\n");
#endif

    //
    // Set the stack limits
    //
    __set_MSPLIM(AM_STACK_LIMIT);
    __set_PSPLIM(AM_STACK_LIMIT);

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    __asm("    ldr     r0, =_init_data\n"
          "    ldr     r1, =_sdata\n"
          "    ldr     r2, =_edata\n"
          "copy_loop:\n"
          "        ldr   r3, [r0], #4\n"
          "        str   r3, [r1], #4\n"
          "        cmp     r1, r2\n"
          "        blt     copy_loop\n");

    __asm("    ldr     r0, =_init_data_sram\n"
          "    ldr     r1, =_ssdata\n"
          "    ldr     r2, =_sedata\n"
          "copy_loop_sram:\n"
          "        ldr   r3, [r0], #4\n"
          "        str   r3, [r1], #4\n"
          "        cmp     r1, r2\n"
          "        blt     copy_loop_sram\n");

    //
    // Copy the ITCM text from flash to ITCM.
    //
    __asm("    ldr     r0, =_init_itcm_text\n"
          "    ldr     r1, =_s_itcm_text\n"
          "    ldr     r2, =_e_itcm_text\n"
          "copy_loop_itcm:\n"
          "    ldr     r3, [r0], #4\n"
          "    str     r3, [r1], #4\n"
          "    cmp     r1, r2\n"
          "    blt     copy_loop_itcm\n");

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =_sbss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

   //
   // Zero fill the shared‐SRAM BSS region.
   //
   __asm("    ldr     r0, =_ssram_bss\n"
         "    ldr     r1, =_esram_bss\n"
         "    mov     r2, #0\n"
         "zero_sram_loop:\n"
         "        cmp     r0, r1\n"
         "        it      lt\n"
         "        strlt   r2, [r0], #4\n"
         "        blt     zero_sram_loop\n");   

    //
    // CMSIS System Initialization
    //
    SystemInit();

    __libc_init_array();

    //
    // Call the application's entry point.
    //
    main();

    //
    // If main returns then execute a break point instruction
    //
    __asm("    bkpt     ");
}
#else
#error GNU STDC inline not supported.
#endif

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
void
NMI_Handler(void)
{
    //
    // Go into an infinite loop.
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
// void
// HardFault_Handler(void)
// {
//     //
//     // Go into an infinite loop.
//     //
//     while(1)
//     {
//     }
// }

// Pull in CMSIS core defs (SCB, register names)
#include "core_cm55.h"   // or simply "cmsis.h" depending on your tree
#include "ns_ambiqsuite_harness.h"
#include <stdbool.h>

// ---- CONFIG ----
// Heuristic code range for "is this a code address?" check.
// Adjust to your SoC memory map (Flash, XIP, ITCM, etc.)
#ifndef CODE_START
#define CODE_START   (0x00410000UL)
#endif
#ifndef CODE_END
#define CODE_END     (0x00800000UL)  
#endif

// How many 32-bit words of stack to scan for potential return addresses
#ifndef BT_SCAN_WORDS
#define BT_SCAN_WORDS  256
#endif

// Maximum number of backtrace entries to print
#ifndef BT_MAX_FRAMES
#define BT_MAX_FRAMES  16
#endif

// Weak linker symbols for stack bounds (typical CMSIS names).
// Provide these in your linker script; we fall back gracefully if missing.
__attribute__((weak)) extern uint32_t __StackTop;
__attribute__((weak)) extern uint32_t __StackLimit;

// Structure of the basic (non-FP) stacked frame pushed on exception entry.
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;   // return address into the interrupted code
    uint32_t pc;   // program counter at the time of fault
    uint32_t xpsr;
} stacked_regs_t;

static inline const char *which_stack(uint32_t exc_return) {
    // EXC_RETURN bit[2]: 0=MSP, 1=PSP
    return (exc_return & (1UL << 2)) ? "PSP" : "MSP";
}

static inline bool exc_frame_has_fp(uint32_t exc_return) {
    // EXC_RETURN bit[4]: 0 = FP context stacked, 1 = no FP context
    return ((exc_return & (1UL << 4)) == 0U);
}

static inline bool is_thumb_code(uint32_t addr) {
    // On Cortex-M, code addresses are Thumb; return addresses typically have bit0=1.
    return (addr & 0x1U) != 0;
}

static inline bool is_code_addr(uint32_t addr) {
    return (addr >= CODE_START) && (addr < CODE_END) && is_thumb_code(addr);
}

static void print_cfsr_details(uint32_t cfsr) {
    uint32_t mmfsr =  cfsr        & 0xFFU;       // MemManage Faults
    uint32_t bfsr  = (cfsr >> 8)  & 0xFFU;       // Bus Faults
    uint32_t ufsr  = (cfsr >> 16) & 0xFFFFU;     // Usage Faults

    if (mmfsr) {
        ns_lp_printf("  MMFSR: 0x%02lx", (unsigned long)mmfsr);
        if (mmfsr & (1U << 0)) ns_lp_printf("  [IACCVIOL]");
        if (mmfsr & (1U << 1)) ns_lp_printf("  [DACCVIOL]");
        if (mmfsr & (1U << 3)) ns_lp_printf("  [MUNSTKERR]");
        if (mmfsr & (1U << 4)) ns_lp_printf("  [MSTKERR]");
        if (mmfsr & (1U << 5)) ns_lp_printf("  [MLSPERR]");
        if (mmfsr & (1U << 7)) ns_lp_printf("  [MMARVALID]");
        ns_lp_printf("\r\n");
    }

    if (bfsr) {
        ns_lp_printf("  BFSR:  0x%02lx", (unsigned long)bfsr);
        if (bfsr & (1U << 0)) ns_lp_printf("  [IBUSERR]");
        if (bfsr & (1U << 1)) ns_lp_printf("  [PRECISERR]");
        if (bfsr & (1U << 2)) ns_lp_printf("  [IMPRECISERR]");
        if (bfsr & (1U << 3)) ns_lp_printf("  [UNSTKERR]");
        if (bfsr & (1U << 4)) ns_lp_printf("  [STKERR]");
        if (bfsr & (1U << 5)) ns_lp_printf("  [LSPERR]");
        if (bfsr & (1U << 7)) ns_lp_printf("  [BFARVALID]");
        ns_lp_printf("\r\n");
    }

    if (ufsr) {
        ns_lp_printf("  UFSR:  0x%04lx", (unsigned long)ufsr);
        if (ufsr & (1U << 0))  ns_lp_printf("  [UNDEFINSTR]");
        if (ufsr & (1U << 1))  ns_lp_printf("  [INVSTATE]");
        if (ufsr & (1U << 2))  ns_lp_printf("  [INVPC]");
        if (ufsr & (1U << 3))  ns_lp_printf("  [NOCP]");
        if (ufsr & (1U << 8))  ns_lp_printf("  [UNALIGNED]");
        if (ufsr & (1U << 9))  ns_lp_printf("  [DIVBYZERO]");
        ns_lp_printf("\r\n");
    }
}

// --- Armclang/Keil stack symbols (scatter file) ---------------------------
// We alias the linker symbol names (with $$ in them) to C identifiers.
__attribute__((weak)) extern const uint32_t __arm_stack_base  __asm("Image$$ARM_LIB_STACK$$Base");
__attribute__((weak)) extern const uint32_t __arm_stack_limit __asm("Image$$ARM_LIB_STACK$$Limit");
// Some projects emit ZI variants; declare them too (weak), harmless if absent.
__attribute__((weak)) extern const uint32_t __arm_stack_zi_base  __asm("Image$$ARM_LIB_STACK$$ZI$$Base");
__attribute__((weak)) extern const uint32_t __arm_stack_zi_limit __asm("Image$$ARM_LIB_STACK$$ZI$$Limit");

// Optional Keil startup symbol for the initial MSP (top of main stack)
__attribute__((weak)) extern const uint32_t __initial_sp;

// If you want a hard-coded emergency fallback for your exact .sct (from your post):
// ARM_LIB_STACK 0x20080000 EMPTY - 0x00003000
#ifndef MANUAL_MAIN_STACK_TOP
#define MANUAL_MAIN_STACK_TOP   ((uint32_t *)0x20080000UL)
#define MANUAL_MAIN_STACK_LIMIT ((uint32_t *)(0x20080000UL - 0x00003000UL))
#endif

// Helper: resolve main stack (MSP) bounds for Armclang/Keil.
static void resolve_main_stack_bounds(uint32_t **out_limit, uint32_t **out_top) {
    uint32_t *limit = 0, *top = 0;

    // Prefer non-ZI armclang symbols
    if (&__arm_stack_base)  limit = (uint32_t *)&__arm_stack_base;   // lower address
    if (&__arm_stack_limit) top   = (uint32_t *)&__arm_stack_limit;  // higher address (initial SP)

    // Try ZI variants if needed
    if ((!limit || !top) && &__arm_stack_zi_base)
        limit = (uint32_t *)&__arm_stack_zi_base;
    if ((!limit || !top) && &__arm_stack_zi_limit)
        top   = (uint32_t *)&__arm_stack_zi_limit;

    // Try Keil startup symbol
    if (!top && &__initial_sp) {
        top = (uint32_t *)&__initial_sp;
    }

    // As a robust fallback, read the initial MSP from the vector table (word 0 at VTOR)
    if (!top) {
        uint32_t vtor = SCB->VTOR ? SCB->VTOR : 0x00000000UL;
        top = (uint32_t *)(*(uint32_t *)vtor);
    }

    // If we *still* don’t have a limit, use the manual constants (from your .sct)
    if (!limit) {
        limit = MANUAL_MAIN_STACK_LIMIT;
    }
    if (!top) {
        top = MANUAL_MAIN_STACK_TOP;
    }

    *out_limit = limit;
    *out_top   = top;
}

// Replace your old dump_callstack() with this safer version.
// It will scan up to BT_SCAN_WORDS but never past stack_top if known.
// If bounds are unknown (e.g., PSP under an RTOS), it still scans a small window.
static void dump_callstack(uint32_t *sp, uint32_t *stack_limit, uint32_t *stack_top) {
    uint32_t *start = sp;
    uint32_t *end;

    if (stack_top && (sp < stack_top)) {
        // Clamp to the top of the (down-growing) stack region
        uint32_t max_words = (uint32_t)(stack_top - sp);
        if (max_words > BT_SCAN_WORDS) max_words = BT_SCAN_WORDS;
        end = sp + max_words;
    } else {
        // Bounds unknown: do a short, conservative scan forward
        end = sp + BT_SCAN_WORDS;
    }

    if (!stack_limit || !stack_top) {
        ns_lp_printf("Call stack: bounds unknown; scanning %u words\r\n", (unsigned)(end - start));
    } else if (sp < stack_limit || sp >= stack_top) {
        ns_lp_printf("Call stack: SP (0x%08lx) outside [0x%08lx..0x%08lx), scanning %u words anyway\r\n",
                     (unsigned long)(uintptr_t)sp,
                     (unsigned long)(uintptr_t)stack_limit,
                     (unsigned long)(uintptr_t)stack_top,
                     (unsigned)(end - start));
    } else {
        ns_lp_printf("Call stack (scan %u words within stack bounds):\r\n", (unsigned)(end - start));
    }

    int found = 0;
    for (uint32_t *p = start; p < end && found < BT_MAX_FRAMES; ++p) {
        uint32_t addr = *p;
        if (is_code_addr(addr)) {
            ns_lp_printf("  #%02d: 0x%08lx\r\n", found, (unsigned long)addr);
            found++;
        }
    }
    if (found == 0) {
        ns_lp_printf("  (no plausible return addresses found)\r\n");
    }
}

// C side of the handler. 'sp' is the base of the stacked frame (FP part removed if present).
void hardfault_handler_c(uint32_t *sp, uint32_t exc_return) {
    // If FP context was stacked, the basic regs are after 16 S regs + FPSCR + reserved = 18 words
    if (exc_frame_has_fp(exc_return)) {
        sp += 18U;
    }

    stacked_regs_t *frame = (stacked_regs_t *)sp;

    // Read SCB fault diagnostics
    uint32_t hfsr  = SCB->HFSR;
    uint32_t cfsr  = SCB->CFSR;
    uint32_t bfar  = SCB->BFAR;
    uint32_t mmfar = SCB->MMFAR;
    uint32_t shcsr = SCB->SHCSR;
    uint32_t afsr  = SCB->AFSR;

    // Some extra context (optional). Use CMSIS intrinsics with armclang.
    // These compile on GCC/Clang/Arm Compiler 6.
    uint32_t control   = __get_CONTROL();
    uint32_t primask   = __get_PRIMASK();
    uint32_t basepri   = __get_BASEPRI();
    uint32_t faultmask = __get_FAULTMASK();

    ns_lp_printf("\r\n========== HARD FAULT ==========\r\n");
    ns_lp_printf("EXC_RETURN:  0x%08lx  (%s, %s frame)\r\n",
                 (unsigned long)exc_return,
                 which_stack(exc_return),
                 exc_frame_has_fp(exc_return) ? "with FP" : "no FP");

    ns_lp_printf("Stacked registers:\r\n");
    ns_lp_printf("  R0  = 0x%08lx  R1  = 0x%08lx  R2  = 0x%08lx  R3  = 0x%08lx\r\n",
                 (unsigned long)frame->r0, (unsigned long)frame->r1,
                 (unsigned long)frame->r2, (unsigned long)frame->r3);
    ns_lp_printf("  R12 = 0x%08lx  LR  = 0x%08lx  PC  = 0x%08lx  xPSR= 0x%08lx\r\n",
                 (unsigned long)frame->r12, (unsigned long)frame->lr,
                 (unsigned long)frame->pc,  (unsigned long)frame->xpsr);

    ns_lp_printf("SCB: HFSR=0x%08lx  CFSR=0x%08lx  SHCSR=0x%08lx  AFSR=0x%08lx\r\n",
                 (unsigned long)hfsr, (unsigned long)cfsr,
                 (unsigned long)shcsr, (unsigned long)afsr);

    // Decode key HFSR bits
    if (hfsr) {
        ns_lp_printf("  HFSR bits:");
        if (hfsr & (1UL << 1))  ns_lp_printf(" [VECTTBL]");   // Bus fault on vector table read
        if (hfsr & (1UL << 30)) ns_lp_printf(" [FORCED]");     // Escalated fault
        if (hfsr & (1UL << 31)) ns_lp_printf(" [DEBUGEVT]");   // Debug event
        ns_lp_printf("\r\n");
    }

    // Decode CFSR breakdown
    if (cfsr) {
        print_cfsr_details(cfsr);
    }

    // BFAR/MMFAR valid bits
    if (cfsr & (1U << 15)) {
        ns_lp_printf("  BFAR (valid)  = 0x%08lx\r\n", (unsigned long)bfar);
    }
    if (cfsr & (1U << 7)) {
        ns_lp_printf("  MMFAR (valid) = 0x%08lx\r\n", (unsigned long)mmfar);
    }

    ns_lp_printf("CONTROL=0x%08lx  PRIMASK=0x%08lx  BASEPRI=0x%08lx  FAULTMASK=0x%08lx\r\n",
                 (unsigned long)control, (unsigned long)primask,
                 (unsigned long)basepri, (unsigned long)faultmask);

    // Stack bounds from linker, if available
    uint32_t *stack_top   = 0;
    uint32_t *stack_limit = 0;

    // Resolve **main stack (MSP)** bounds from scatter file / VTOR
    resolve_main_stack_bounds(&stack_limit, &stack_top);

    // If faulted context used PSP (EXC_RETURN bit2==1), we likely *don’t* know PSP bounds
    // unless you integrate with your RTOS to query the current thread’s stack.
    // We keep stack_limit/top for logging, but the scanner will safely limit itself.
    const bool used_psp = (exc_return & (1UL << 2)) != 0;

    ns_lp_printf("Active %s @ SP=0x%08lx  MainStackLimit=0x%08lx  MainStackTop=0x%08lx\r\n",
                 used_psp ? "PSP" : "MSP",
                 (unsigned long)(uintptr_t)sp,
                 (unsigned long)(uintptr_t)stack_limit,
                 (unsigned long)(uintptr_t)stack_top);

    dump_callstack(sp, used_psp ? 0 : stack_limit, used_psp ? 0 : stack_top);

    ns_lp_printf("========== END HARD FAULT ========\r\n");

    // Optional: loop here, or trigger system reset
    // NVIC_SystemReset();
    while (1) { __WFI(); }
}

// Naked wrapper to capture the right SP (MSP/PSP) and EXC_RETURN (LR)
__attribute__((naked)) void HardFault_Handler(void) {
    __asm volatile(
        "tst   lr, #4          \n" // Test EXC_RETURN bit 2: 0=MSP, 1=PSP
        "ite   eq              \n"
        "mrseq r0, msp         \n"
        "mrsne r0, psp         \n"
        "mov   r1, lr          \n" // r1 = EXC_RETURN
        "b     hardfault_handler_c \n"
    );
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

