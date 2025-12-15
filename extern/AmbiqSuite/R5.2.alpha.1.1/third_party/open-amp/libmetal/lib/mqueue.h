/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	mqueue.h
 * @brief	Message queue for libmetal.
 */

#ifndef __METAL_MQUEUE__H__
#define __METAL_MQUEUE__H__

#include "utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup queue Message Queue Interfaces
 *  @{ */

/** Opaque libmetal message queue data structure. */
struct metal_mqueue {
	void *queue; /**< Message queue handle		    */
};

typedef struct metal_mqueue metal_mqueue_t;

/** Constant for infinite time-out. */
#define METAL_MQUEUE_INF_TIMEOUT (0xFFFFFFFFUL)


/**
 * @brief        Initializes a libmetal message queue.
 * @param[in]	 mq	message queue to initialize.
 * @param[in]	 queue_len the maximum number of items the queue can hold at any one time.
 * @param[in]	 item_size	the size, in bytes, required to hold each item in the queue.
 */
void metal_mqueue_init(metal_mqueue_t *mq, uint32_t queue_len, uint32_t item_size);


/**
 * @brief        Destroys a libmetal message queue.
 * @param[in]	 mq	message queue to deinitialize.
 */
void metal_mqueue_deinit(metal_mqueue_t *mq);


/**
 * @brief        Sends an item to the queue.
 * @param[in]    mq	message message queue.
 * @param[in]    item_to_queue a pointer to the item that is to be placed on the queue.
 * @param[in]    timeout_ms the maximum amount of time (in milli-seconds) the task should block if the queue is full,
 *               may be set to METAL_MQUEUE_INF_TIMEOUT.
 * @return       zero on no errors, non-zero on errors
 */
int metal_mqueue_send(metal_mqueue_t *mq, const void * item_to_queue, uint32_t timeout_ms);

/**
 * @brief        Receives an item from the queue.
 * @param[in]    mq	message message queue.
 * @param[in]    received_item pointer to the buffer into which the received item will be copied.
 * @param[in]    timeout_ms the maximum amount of time (in milli-seconds) the task should block if the queue is empty,
 *               may be set to METAL_MQUEUE_INF_TIMEOUT.
 * @return       zero on no errors, non-zero on errors
 */
int metal_mqueue_receive(metal_mqueue_t *mq, void * received_item, uint32_t timeout_ms);

/**
 * @brief        Resets a queue to its original empty state.
 * @param[in]    mq	message message queue.
 * @return       zero on no errors, non-zero on errors
 */
int metal_mqueue_reset(metal_mqueue_t *mq);


/**
 * @brief        Returns the number of free spaces in a queue.
 * @param[in]    mq	message message queue.
 * @return       the number of free spaces available in the queue
 */
uint32_t metal_mqueue_get_free(metal_mqueue_t *mq);

/**
 * @brief        Return the number of items stored in a queue.
 * @param[in]    mq	message message queue.
 * @return       the number of items available in the queue
 */
uint32_t metal_mqueue_get_used(metal_mqueue_t *mq);


#include "./system/freertos/mqueue.h"

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_MQUEUE__H__ */
