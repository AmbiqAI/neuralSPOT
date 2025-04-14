/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/******************************************************************************
    See http://www.freertos.org/a00110.html for an explanation of the
    definitions contained in this file.
******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 * https://www.FreeRTOS.org/a00110.html
 *----------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif
// #include "task.h"

extern uint32_t SystemCoreClock;


#ifndef NS_MALLOC_HEAP_SIZE_IN_K
#define NS_MALLOC_HEAP_SIZE_IN_K 16
#endif

/* Cortex M55 port configuration. */
#define configENABLE_MPU                                0
#define configENABLE_FPU                                1
#define configENABLE_TRUSTZONE                          0
#define configENABLE_MVE                                0

/* This part has 16 MPU regions. */
#define configTOTAL_MPU_REGIONS                         16

/* Run FreeRTOS on the secure side and never jump to the non-secure side. */
#define configRUN_FREERTOS_SECURE_ONLY                  1

/* Constants related to the behaviour or the scheduler. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         0
#define configUSE_PREEMPTION                            1
#define configUSE_TIME_SLICING                          0
#define configMAX_PRIORITIES                            ( 8 )
#define configIDLE_SHOULD_YIELD                         1
#define configUSE_16_BIT_TICKS                          0 /* Only for 8 and 16-bit hardware. */

/* Constants that describe the hardware and memory usage. */
#if defined(APOLLO5_FPGA)
    #define configCPU_CLOCK_HZ                          (APOLLO5_FPGA*1000000UL)
#else
    #define configCPU_CLOCK_HZ                          SystemCoreClock
#endif
// #if defined(APOLLO5_FPGA)
//   #if ENABLE_CPU_HP_250  
// 	#define configCPU_CLOCK_HZ                          (20*1000000UL)  //set to 20 to measure actual time, since FPGA is running at 20MHz emulating 250MHz
//   #else	
// 	#define configCPU_CLOCK_HZ                          (8*1000000UL)  //set to 8 to measure actual time, since FPGA is running at 8MHz emulating 96MHz	
//   #endif
// #else
//   #if ENABLE_CPU_HP_250  
// 	#define configCPU_CLOCK_HZ                          (250*1000000UL)
//   #else	
// 	#define configCPU_CLOCK_HZ                          (96*1000000UL)
//   #endif
// #endif
#define configMINIMAL_STACK_SIZE                        ( ( uint16_t ) 256 )
#define configMINIMAL_SECURE_STACK_SIZE                 ( 1024 )
#define configMAX_TASK_NAME_LEN                         ( 12 )
#ifndef configTOTAL_HEAP_SIZE
// #error Missing Heap Size Warning
// #define configTOTAL_HEAP_SIZE                   (NS_MALLOC_HEAP_SIZE_IN_K * 1024)
// #define configTOTAL_HEAP_SIZE                           ( ( size_t ) ( 288 * 1024 ) )
#endif

/* Constants that build features in or out. */
#define configUSE_MUTEXES                               1
#define configOVERRIDE_DEFAULT_TICK_CONFIGURATION       0 //1 // Enable non-SysTick based Tick
#define configUSE_TICKLESS_IDLE                         0 //2 // Ambiq specific implementation for Tickless
#define configUSE_APPLICATION_TASK_TAG                  1
#define configUSE_NEWLIB_REENTRANT                      0
#define configUSE_COUNTING_SEMAPHORES                   1
#define configUSE_RECURSIVE_MUTEXES                     1
#define configUSE_QUEUE_SETS                            0
#define configUSE_TASK_NOTIFICATIONS                    1
#define configUSE_TRACE_FACILITY                        0

/* Constants that define which hook (callback) functions should be used. */
#define configUSE_IDLE_HOOK                             0
#define configUSE_TICK_HOOK                             0
#define configUSE_MALLOC_FAILED_HOOK                    1

/* Constants provided for debugging and optimisation assistance. */
#define configCHECK_FOR_STACK_OVERFLOW                  2
#define configASSERT( x )                               if( ( x ) == 0 ) { portDISABLE_INTERRUPTS(); for( ;; ); }
#define configQUEUE_REGISTRY_SIZE                       0

/* Software timer definitions. */
#define configUSE_TIMERS                                1
#define configTIMER_TASK_PRIORITY                       ( 3 )
#define configTIMER_QUEUE_LENGTH                        20
#define configTIMER_TASK_STACK_DEPTH                    ( configMINIMAL_STACK_SIZE * 2 )

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.  NOTE:  Setting an INCLUDE_ parameter to 0 is
 * only necessary if the linker does not automatically remove functions that are
 * not referenced anyway. */
#define INCLUDE_vTaskPrioritySet                        1
#define INCLUDE_uxTaskPriorityGet                       1
#define INCLUDE_vTaskDelete                             1
#define INCLUDE_vTaskCleanUpResources                   0
#define INCLUDE_vTaskSuspend                            1
#define INCLUDE_vTaskDelayUntil                         1
#define INCLUDE_vTaskDelay                              1
#define INCLUDE_uxTaskGetStackHighWaterMark             0
#define INCLUDE_xTaskGetIdleTaskHandle                  1
#define INCLUDE_eTaskGetState                           1
#define INCLUDE_xTaskResumeFromISR                      0
#define INCLUDE_xTaskGetCurrentTaskHandle               1
#define INCLUDE_xTaskGetSchedulerState                  0
#define INCLUDE_xSemaphoreGetMutexHolder                1
#define INCLUDE_xTimerPendFunctionCall                  1

/* Interrupt priority configuration follows...................... */

/* Use the system definition, if there is one. */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS                             __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS                             4     /* 8 priority levels. */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
 * function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x0F

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT
 * CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A
 * HIGHER PRIORITY THAN THIS! (higher priorities are lower numeric values). */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    3

/* Interrupt priorities used by the kernel port layer itself.  These are generic
 * to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY                 ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* Constants related to the generation of run time stats. */
#define configGENERATE_RUN_TIME_STATS                   1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()                xTaskGetTickCount()
#define configTICK_RATE_HZ                              ( ( TickType_t ) 1000 )


/* Allow RTOS objects to be created using RAM provided by the application writer. */
#define configSUPPORT_STATIC_ALLOCATION         1

/*  Create RTOS objects using dynamically allocated RAM */
#define configSUPPORT_DYNAMIC_ALLOCATION        1

/*Define FreeRTOS+POSIX errno*/
#define configUSE_POSIX_ERRNO                           1

// /* Enable non-SysTick based Tick, this will use STIMER to generate the system tick */
// #define configOVERRIDE_DEFAULT_TICK_CONFIGURATION 0
// /* Ambiq specific implementation for Tickless */
// #define configUSE_TICKLESS_IDLE                   0


extern uint32_t am_freertos_sleep(uint32_t);
extern void am_freertos_wakeup(uint32_t);
#define configPRE_SLEEP_PROCESSING( time ) \
    do { \
        (time) = am_freertos_sleep(time); \
    } while (0);

#define configPOST_SLEEP_PROCESSING(time)    am_freertos_wakeup(time)

#ifdef __cplusplus
}
#endif
#endif /* FREERTOS_CONFIG_H */
