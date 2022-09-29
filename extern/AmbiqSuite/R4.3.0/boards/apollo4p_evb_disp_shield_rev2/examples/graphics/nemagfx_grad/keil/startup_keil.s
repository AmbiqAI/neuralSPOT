;******************************************************************************
;
;! @file startup_keil.s
;!
;! @brief Definitions for Apollo4 interrupt handlers, the vector table, and the stack.
;
;******************************************************************************

;******************************************************************************
;
; Copyright (c) 2022, Ambiq Micro, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; 1. Redistributions of source code must retain the above copyright notice,
; this list of conditions and the following disclaimer.
;
; 2. Redistributions in binary form must reproduce the above copyright
; notice, this list of conditions and the following disclaimer in the
; documentation and/or other materials provided with the distribution.
;
; 3. Neither the name of the copyright holder nor the names of its
; contributors may be used to endorse or promote products derived from this
; software without specific prior written permission.
;
; Third party software included in this distribution is subject to the
; additional license terms as defined in the /docs/licenses directory.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
;
; This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
;
;******************************************************************************

;******************************************************************************
;
; <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;************************************************************************
Stack   EQU     0x00002B00

;******************************************************************************
;
; <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Heap    EQU     0x00000000

;******************************************************************************
;
; Allocate space for the stack.
;
;******************************************************************************
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack
__initial_sp

;******************************************************************************
;
; Allocate space for the heap.
;
;******************************************************************************
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
HeapMem
        SPACE   Heap
__heap_limit

;******************************************************************************
;
; Indicate that the code in this file preserves 8-byte alignment of the stack.
;
;******************************************************************************
        PRESERVE8

;******************************************************************************
;
; Place code into the reset code section.
;
;******************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;******************************************************************************
;
; The vector table.
;
; Proper alignment of the vector table is dependent on the number of
; external (peripheral) interrupts, see the following table for proper
; vectorbaseaddress alignment.
;     0-16      32-word
;    17-48      64-word
;    49-112    128-word  (Apollo4)
;   113-240    256-word
;
; The Apollo4 vector table must be located on a 512 byte boundary.
;
;******************************************************************************
;
; Note: Aliasing and weakly exporting am_mpufault_isr, am_busfault_isr, and
; am_usagefault_isr does not work if am_fault_isr is defined externally.
; Therefore, we'll explicitly use am_fault_isr in the table for those vectors.
;

        EXPORT  __Vectors
__Vectors
        DCD     StackMem + Stack            ; Top of Stack
        DCD     Reset_Handler               ; Reset Handler
        DCD     NMI_Handler                 ; NMI Handler
        DCD     HardFault_Handler           ; Hard Fault Handler
        DCD     MemManage_Handler           ; The MPU fault handler
        DCD     BusFault_Handler            ; The bus fault handler
        DCD     UsageFault_Handler          ; The usage fault handler
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     SVC_Handler                 ; SVCall handler
        DCD     DebugMon_Handler            ; Debug monitor handler
        DCD     0                           ; Reserved
        DCD     PendSV_Handler              ; The PendSV handler
        DCD     SysTick_Handler             ; The SysTick handler

        ;
        ; Peripheral Interrupts
        ;
        DCD     am_brownout_isr             ;  0: Brownout (rstgen)
        DCD     am_watchdog_isr             ;  1: Watchdog (WDT)
        DCD     am_rtc_isr                  ;  2: RTC
        DCD     am_vcomp_isr                ;  3: Voltage Comparator
        DCD     am_ioslave_ios_isr          ;  4: I/O Slave general
        DCD     am_ioslave_acc_isr          ;  5: I/O Slave access
        DCD     am_iomaster0_isr            ;  6: I/O Master 0
        DCD     am_iomaster1_isr            ;  7: I/O Master 1
        DCD     am_iomaster2_isr            ;  8: I/O Master 2
        DCD     am_iomaster3_isr            ;  9: I/O Master 3
        DCD     am_iomaster4_isr            ; 10: I/O Master 4
        DCD     am_iomaster5_isr            ; 11: I/O Master 5
        DCD     am_iomaster6_isr            ; 12: I/O Master 6 (I3C/I2C/SPI)
        DCD     am_iomaster7_isr            ; 13: I/O Master 7 (I3C/I2C/SPI)
        DCD     am_ctimer_isr               ; 14: OR of all timerX interrupts
        DCD     am_uart_isr                 ; 15: UART0
        DCD     am_uart1_isr                ; 16: UART1
        DCD     am_uart2_isr                ; 17: UART2
        DCD     am_uart3_isr                ; 18: UART3
        DCD     am_adc_isr                  ; 19: ADC
        DCD     am_mspi0_isr                ; 20: MSPI0
        DCD     am_mspi1_isr                ; 21: MSPI1
        DCD     am_mspi2_isr                ; 22: MSPI2
        DCD     am_clkgen_isr               ; 23: ClkGen
        DCD     am_cryptosec_isr            ; 24: Crypto Secure
        DCD     am_default_isr              ; 25: Reserved
        DCD     am_sdio_isr                 ; 26: SDIO
        DCD     am_usb_isr                  ; 27: USB
        DCD     am_gpu_isr                  ; 28: GPU
        DCD     am_disp_isr                 ; 29: DISP
        DCD     am_dsi_isr                  ; 30: DSI
        DCD     am_default_isr              ; 31: Reserved
        DCD     am_stimer_cmpr0_isr         ; 32: System Timer Compare0
        DCD     am_stimer_cmpr1_isr         ; 33: System Timer Compare1
        DCD     am_stimer_cmpr2_isr         ; 34: System Timer Compare2
        DCD     am_stimer_cmpr3_isr         ; 35: System Timer Compare3
        DCD     am_stimer_cmpr4_isr         ; 36: System Timer Compare4
        DCD     am_stimer_cmpr5_isr         ; 37: System Timer Compare5
        DCD     am_stimer_cmpr6_isr         ; 38: System Timer Compare6
        DCD     am_stimer_cmpr7_isr         ; 39: System Timer Compare7
        DCD     am_stimerof_isr             ; 40: System Timer Cap Overflow
        DCD     am_default_isr              ; 41: Reserved
        DCD     am_audadc0_isr              ; 42: Audio ADC
        DCD     am_default_isr              ; 43: Reserved
        DCD     am_dspi2s0_isr              ; 44: I2S0
        DCD     am_dspi2s1_isr              ; 45: I2S1
        DCD     am_dspi2s2_isr              ; 46: I2S2
        DCD     am_dspi2s3_isr              ; 47: I2S3
        DCD     am_pdm0_isr                 ; 48: PDM0
        DCD     am_pdm1_isr                 ; 49: PDM1
        DCD     am_pdm2_isr                 ; 50: PDM2
        DCD     am_pdm3_isr                 ; 51: PDM3
        DCD     am_default_isr              ; 52: Reserved
        DCD     am_default_isr              ; 53: Reserved
        DCD     am_default_isr              ; 54: Reserved
        DCD     am_default_isr              ; 55: Reserved
        DCD     am_gpio0_001f_isr           ; 56: GPIO N0 pins  0-31
        DCD     am_gpio0_203f_isr           ; 57: GPIO N0 pins 32-63
        DCD     am_gpio0_405f_isr           ; 58: GPIO N0 pins 64-95
        DCD     am_gpio0_607f_isr           ; 59: GPIO N0 pins 96-104, virtual 105-127
        DCD     am_gpio1_001f_isr           ; 60: GPIO N1 pins  0-31
        DCD     am_gpio1_203f_isr           ; 61: GPIO N1 pins 32-63
        DCD     am_gpio1_405f_isr           ; 62: GPIO N1 pins 64-95
        DCD     am_gpio1_607f_isr           ; 63: GPIO N1 pins 96-104, virtual 105-127
        DCD     am_default_isr              ; 64: Reserved
        DCD     am_default_isr              ; 65: Reserved
        DCD     am_default_isr              ; 66: Reserved
        DCD     am_timer00_isr              ; 67: timer0
        DCD     am_timer01_isr              ; 68: timer1
        DCD     am_timer02_isr              ; 69: timer2
        DCD     am_timer03_isr              ; 70: timer3
        DCD     am_timer04_isr              ; 71: timer4
        DCD     am_timer05_isr              ; 72: timer5
        DCD     am_timer06_isr              ; 73: timer6
        DCD     am_timer07_isr              ; 74: timer7
        DCD     am_timer08_isr              ; 75: timer8
        DCD     am_timer09_isr              ; 76: timer9
        DCD     am_timer10_isr              ; 77: timer10
        DCD     am_timer11_isr              ; 78: timer11
        DCD     am_timer12_isr              ; 79: timer12
        DCD     am_timer13_isr              ; 80: timer13
        DCD     am_timer14_isr              ; 81: timer14
        DCD     am_timer15_isr              ; 82: timer15
        DCD     am_cachecpu_isr             ; 83: CPU cache

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

;******************************************************************************
;
; Place code immediately following vector table.
;
; The patch table should pad the vector table size to a total of 128 entries
; such that the code begins at 0x200.
; In other words, the final peripheral IRQ is always IRQ 111 (0-based).
;
;******************************************************************************
;******************************************************************************
;
; The Patch table.
;
;******************************************************************************
        EXPORT  __Patchable
__Patchable
        DCD                 0, 0, 0, 0, 0, 0    ;  84-89
        DCD     0, 0, 0, 0, 0, 0, 0, 0, 0, 0    ;  90-99
        DCD     0, 0, 0, 0, 0, 0, 0, 0, 0, 0    ; 100-109
        DCD     0, 0                            ; 110-111



;******************************************************************************
;
; This is the code that gets called when the processor first starts execution
; following a reset event.
;
;******************************************************************************
Reset_Handler   PROC
                EXPORT  Reset_Handler               [WEAK]
                IMPORT  __main

                ;
                ; Enable the FPU.
                ;
                MOVW    R0, #0xED88
                MOVT    R0, #0xE000
                LDR     R1, [R0]
                ORR     R1, #0x00F00000
                STR     R1, [R0]
                DSB
                ISB

                ;
                ; Branch to main.
                ;
                LDR     R0, =__main
                BX      R0

                ENDP

;******************************************************************************
;
; Weak Exception Handlers.
;
;******************************************************************************

HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler           [WEAK]
                B       .
                ENDP
NMI_Handler     PROC
                EXPORT  NMI_Handler                 [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler           [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler            [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler          [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                 [WEAK]
                B       .
                ENDP
DebugMon_Handler  PROC
                EXPORT  DebugMon_Handler            [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler              [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler             [WEAK]
                B       .
                ENDP

am_default_isr\
                PROC
                EXPORT  am_brownout_isr             [WEAK]
                EXPORT  am_watchdog_isr             [WEAK]
                EXPORT  am_rtc_isr                  [WEAK]
                EXPORT  am_vcomp_isr                [WEAK]
                EXPORT  am_ioslave_ios_isr          [WEAK]
                EXPORT  am_ioslave_acc_isr          [WEAK]
                EXPORT  am_iomaster0_isr            [WEAK]
                EXPORT  am_iomaster1_isr            [WEAK]
                EXPORT  am_iomaster2_isr            [WEAK]
                EXPORT  am_iomaster3_isr            [WEAK]
                EXPORT  am_iomaster4_isr            [WEAK]
                EXPORT  am_iomaster5_isr            [WEAK]
                EXPORT  am_iomaster6_isr            [WEAK]
                EXPORT  am_iomaster7_isr            [WEAK]
                EXPORT  am_ctimer_isr               [WEAK]
                EXPORT  am_uart_isr                 [WEAK]
                EXPORT  am_uart1_isr                [WEAK]
                EXPORT  am_uart2_isr                [WEAK]
                EXPORT  am_uart3_isr                [WEAK]
                EXPORT  am_adc_isr                  [WEAK]
                EXPORT  am_mspi0_isr                [WEAK]
                EXPORT  am_mspi1_isr                [WEAK]
                EXPORT  am_mspi2_isr                [WEAK]
                EXPORT  am_clkgen_isr               [WEAK]
                EXPORT  am_sdio_isr                 [WEAK]
                EXPORT  am_usb_isr                  [WEAK]
                EXPORT  am_gpu_isr                  [WEAK]
                EXPORT  am_disp_isr                 [WEAK]
                EXPORT  am_dsi_isr                  [WEAK]
                EXPORT  am_stimer_cmpr0_isr         [WEAK]
                EXPORT  am_stimer_cmpr1_isr         [WEAK]
                EXPORT  am_stimer_cmpr2_isr         [WEAK]
                EXPORT  am_stimer_cmpr3_isr         [WEAK]
                EXPORT  am_stimer_cmpr4_isr         [WEAK]
                EXPORT  am_stimer_cmpr5_isr         [WEAK]
                EXPORT  am_stimer_cmpr6_isr         [WEAK]
                EXPORT  am_stimer_cmpr7_isr         [WEAK]
                EXPORT  am_stimerof_isr             [WEAK]
                EXPORT  am_audadc0_isr              [WEAK]
                EXPORT  am_dspi2s0_isr              [WEAK]
                EXPORT  am_dspi2s1_isr              [WEAK]
                EXPORT  am_dspi2s2_isr              [WEAK]
                EXPORT  am_dspi2s3_isr              [WEAK]
                EXPORT  am_pdm0_isr                 [WEAK]
                EXPORT  am_pdm1_isr                 [WEAK]
                EXPORT  am_pdm2_isr                 [WEAK]
                EXPORT  am_pdm3_isr                 [WEAK]
                EXPORT  am_gpio0_001f_isr           [WEAK]
                EXPORT  am_gpio0_203f_isr           [WEAK]
                EXPORT  am_gpio0_405f_isr           [WEAK]
                EXPORT  am_gpio0_607f_isr           [WEAK]
                EXPORT  am_gpio1_001f_isr           [WEAK]
                EXPORT  am_gpio1_203f_isr           [WEAK]
                EXPORT  am_gpio1_405f_isr           [WEAK]
                EXPORT  am_gpio1_607f_isr           [WEAK]
                EXPORT  am_timer00_isr              [WEAK]
                EXPORT  am_timer01_isr              [WEAK]
                EXPORT  am_timer02_isr              [WEAK]
                EXPORT  am_timer03_isr              [WEAK]
                EXPORT  am_timer04_isr              [WEAK]
                EXPORT  am_timer05_isr              [WEAK]
                EXPORT  am_timer06_isr              [WEAK]
                EXPORT  am_timer07_isr              [WEAK]
                EXPORT  am_timer08_isr              [WEAK]
                EXPORT  am_timer09_isr              [WEAK]
                EXPORT  am_timer10_isr              [WEAK]
                EXPORT  am_timer11_isr              [WEAK]
                EXPORT  am_timer12_isr              [WEAK]
                EXPORT  am_timer13_isr              [WEAK]
                EXPORT  am_timer14_isr              [WEAK]
                EXPORT  am_timer15_isr              [WEAK]
                EXPORT  am_cachecpu_isr             [WEAK]
am_brownout_isr
am_watchdog_isr
am_rtc_isr
am_vcomp_isr
am_ioslave_ios_isr
am_ioslave_acc_isr
am_iomaster0_isr
am_iomaster1_isr
am_iomaster2_isr
am_iomaster3_isr
am_iomaster4_isr
am_iomaster5_isr
am_iomaster6_isr
am_iomaster7_isr
am_ctimer_isr
am_uart_isr
am_uart1_isr
am_uart2_isr
am_uart3_isr
am_adc_isr
am_mspi0_isr
am_mspi1_isr
am_mspi2_isr
am_clkgen_isr
am_cryptosec_isr
am_sdio_isr
am_usb_isr
am_gpu_isr
am_disp_isr
am_dsi_isr
am_stimer_cmpr0_isr
am_stimer_cmpr1_isr
am_stimer_cmpr2_isr
am_stimer_cmpr3_isr
am_stimer_cmpr4_isr
am_stimer_cmpr5_isr
am_stimer_cmpr6_isr
am_stimer_cmpr7_isr
am_stimerof_isr
am_audadc0_isr
am_dspi2s0_isr
am_dspi2s1_isr
am_dspi2s2_isr
am_dspi2s3_isr
am_pdm0_isr
am_pdm1_isr
am_pdm2_isr
am_pdm3_isr
am_gpio0_001f_isr
am_gpio0_203f_isr
am_gpio0_405f_isr
am_gpio0_607f_isr
am_gpio1_001f_isr
am_gpio1_203f_isr
am_gpio1_405f_isr
am_gpio1_607f_isr
am_timer00_isr
am_timer01_isr
am_timer02_isr
am_timer03_isr
am_timer04_isr
am_timer05_isr
am_timer06_isr
am_timer07_isr
am_timer08_isr
am_timer09_isr
am_timer10_isr
am_timer11_isr
am_timer12_isr
am_timer13_isr
am_timer14_isr
am_timer15_isr
am_cachecpu_isr
                ; all device interrupts go here unless the weak label is over
                ; ridden in the linker hard spin so the debugger will know it
                ; was an unhandled interrupt request a come-from-buffer or
                ; instruction trace hardware would sure be nice if you get here
                B       .

                ENDP

;******************************************************************************
;
; Align the end of the section.
;
;******************************************************************************
                ALIGN

;******************************************************************************
;
; Initialization of the heap and stack.
;
;******************************************************************************
                AREA    |.text|, CODE, READONLY

;******************************************************************************
;
; User Initial Stack & Heap.
;
;******************************************************************************
    IF :DEF: __MICROLIB
        EXPORT  __initial_sp
        EXPORT  __heap_base
        EXPORT  __heap_limit
    ELSE
        IMPORT  __scatterload_copy
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap
__user_initial_stackheap PROC
        LDR     R0, =HeapMem
        LDR     R1, =(StackMem + Stack)
        LDR     R2, =(HeapMem + Heap)
        LDR     R3, =StackMem
        BX      LR

        ENDP

    ENDIF

;******************************************************************************
;
; Align the end of the section.
;
;******************************************************************************
                ALIGN

;******************************************************************************
;
; All Done
;
;******************************************************************************
                END


