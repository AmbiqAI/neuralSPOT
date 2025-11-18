/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file    freertos/mqueue.c
 * @brief   FreeRTOS message queue handling.
 */

#include <FreeRTOS.h>
#include <queue.h>
#include "../../mqueue.h"


void metal_mqueue_init(metal_mqueue_t* mq, uint32_t queue_len, uint32_t item_size)
{
    mq->queue = xQueueCreate( queue_len, item_size);
}

void metal_mqueue_deinit(metal_mqueue_t* mq)
{
    vQueueDelete( (QueueHandle_t)(mq->queue) );
}

int metal_mqueue_send(metal_mqueue_t* mq, const void* item_to_queue, uint32_t timeout_ms)
{
    TickType_t xTicksToWait = timeout_ms == METAL_MQUEUE_INF_TIMEOUT ? portMAX_DELAY  : pdMS_TO_TICKS( timeout_ms );
    BaseType_t ret = xQueueSendToBack( (QueueHandle_t)(mq->queue), item_to_queue, xTicksToWait );
    return (ret == pdTRUE) ? 0 : -1;
}

int metal_mqueue_receive(metal_mqueue_t* mq, void* received_item, uint32_t timeout_ms)
{
    TickType_t xTicksToWait = timeout_ms == METAL_MQUEUE_INF_TIMEOUT ? portMAX_DELAY  : pdMS_TO_TICKS( timeout_ms );
    BaseType_t ret = xQueueReceive( (QueueHandle_t)(mq->queue), received_item, xTicksToWait );
    return (ret == pdTRUE) ? 0 : -1;
}

int metal_mqueue_reset(metal_mqueue_t* mq)
{
    (void)xQueueReset( (QueueHandle_t)(mq->queue) );
    return 0;
}


uint32_t metal_mqueue_get_free(metal_mqueue_t* mq)
{
    return uxQueueSpacesAvailable( (QueueHandle_t)(mq->queue) );
}

uint32_t metal_mqueue_get_used(metal_mqueue_t* mq)
{
    return uxQueueMessagesWaiting( (QueueHandle_t)(mq->queue) );
}
