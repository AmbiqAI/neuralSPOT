/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file    freertos/thread.c
 * @brief   FreeRTOS thread implementation.
 */

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "../../thread.h"
#include "../../utilities.h"

static metal_stack_overflow_handler stack_overflow_handler = NULL;

int metal_thread_init(metal_thread_t* thread,
                      metal_thread_func func,
                      const char* const name,
                      uint32_t stack_size,
                      void* param,
                      uint32_t priority)
{
    BaseType_t res = xTaskCreate( func,
                                  name,
                                  stack_size / sizeof(StackType_t),
                                  param,
                                  priority,
                                  (TaskHandle_t*)&thread->tid
                                );
    return (res == pdPASS) ? 0 : -1;
}

void metal_thread_terminate(metal_thread_t* thread)
{
    vTaskDelete( thread->tid );
}

int metal_thread_join(metal_thread_t* thread)
{
    TaskHandle_t task_handle = (TaskHandle_t)(thread->tid);
    eTaskState ts;
    if (task_handle == NULL)
    {
        return -1;
    }
    do
    {
        metal_thread_yield();
        ts = eTaskGetState(task_handle);
    } while (ts != eDeleted);
    return 0;
}


void metal_thread_set_priority(metal_thread_t* thread, uint32_t priority)
{
    vTaskPrioritySet( thread->tid, priority );
}

uint32_t metal_thread_get_priority(metal_thread_t* thread)
{
    return uxTaskPriorityGet( thread->tid );
}

void metal_thread_suspend(metal_thread_t* thread)
{
    vTaskSuspend( thread->tid );
}

void metal_thread_resume(metal_thread_t* thread)
{
    vTaskResume( thread->tid );
}

void metal_thread_yield()
{
    taskYIELD();
}

void metal_thread_delay(metal_thread_t* thread, uint32_t duration_ms)
{
    vTaskDelay( pdMS_TO_TICKS(duration_ms) );
}

void metal_thread_wakeup(metal_thread_t* thread)
{
    (void)xTaskAbortDelay( thread->tid );
}

void metal_scheduler_start()
{
    vTaskStartScheduler();
}

void metal_scheduler_suspend()
{
    vTaskSuspendAll();
}

void metal_scheduler_resume()
{
    (void)xTaskResumeAll();
}

void metal_set_stack_overflow_handler(metal_stack_overflow_handler func)
{
    stack_overflow_handler = func;
}

void metal_thread_notify_take_and_clear(void)
{
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

void metal_thread_notify_take(void)
{
    (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
}

void metal_thread_notify_give(metal_thread_t *thread)
{
    if (thread == NULL || thread->tid == NULL)
    {
        return;
    }

    portDISABLE_INTERRUPTS();
    if (pdTRUE == xPortIsInsideInterrupt())
    {

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        vTaskNotifyGiveFromISR(thread->tid, &xHigherPriorityTaskWoken);

        /* Note the use of portYIELD_FROM_ISR(). This is required when waking a task
         from an interrupt handler. If vTaskNotifyGiveFromISR indicates that a higher
         priority task is being woken, the portYIELD_FROM_ISR() routine will context switch
         to that task after returning from the ISR.
         Failure to use this function will result in execution will resuming at the previous
         point rather than switching to the new context.
         */

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        xTaskNotifyGive(thread->tid);
    }
    portENABLE_INTERRUPTS();
}
