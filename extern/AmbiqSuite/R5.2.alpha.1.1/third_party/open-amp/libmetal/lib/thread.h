/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	thread.h
 * @brief	Threads for libmetal.
 */

#ifndef __METAL_THREAD__H__
#define __METAL_THREAD__H__

#include "utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup thread Thread Interfaces
 *  @{ */

/** Opaque libmetal thread data structure. */
struct metal_thread {
	void *tid; /**< Thread handle		    */
};

typedef struct metal_thread metal_thread_t;
typedef void (*metal_thread_func)(void*);


/**
 * @brief        Initializes a libmetal thread
 * @param[in]    thread Thread to initialize
 * @param[in]    func Pointer to the thread entry function
 * @param[in]    name A descriptive name for the thread
 * @param[in]    stack_size The number of bytes to allocate for use as the thread's stack
 * @param[in]    param A value that will passed into the created thread as the thread's parameter
 * @param[in]    priority The priority at which the created task will execute
 * @return       zero on no errors, non-zero on errors (e.g. failed to allocate memory)
 */
int metal_thread_init(metal_thread_t *thread,
                      metal_thread_func func,
                      const char * const name,
                      uint32_t stack_size,
                      void* param,
                      uint32_t priority);

/**
 * @brief        Stops a libmetal thread, removes it from the scheduler management and frees memory
 * @param[in]    thread	Thread to terminate (may be NULL for the current thread)
 */
void metal_thread_terminate(metal_thread_t *thread);


/**
 * @brief        Waits for the thread to terminate
 * @param[in]    thread	Thread to wait upon
 * @return       zero on no errors, non-zero on errors
 */
int metal_thread_join(metal_thread_t *thread);


/**
 * @brief        Sets the priority of the thread
 * @param[in]    thread Thread handle
 * @param[in]    priority The new priority value
 */
void metal_thread_set_priority(metal_thread_t *thread, uint32_t priority);

/**
 * @brief        Gets the priority of the thread
 * @param[in]    thread Thread handle
 * @return       The priority value
 */
uint32_t metal_thread_get_priority(metal_thread_t *thread);

/**
 * @brief        Suspends the thread execution
 * @param[in]    thread Thread handle
 */
void metal_thread_suspend(metal_thread_t *thread);

/**
 * @brief        Resumes the thread execution
 * @param[in]    thread Thread handle
 */
void metal_thread_resume(metal_thread_t *thread);


/**
 * @brief        Yields the control to the scheduler
 */
void metal_thread_yield(void);

/**
 * @brief        Delays the current thread for the given duration
 * @param[in]    thread Thread handle
 * @param[in]    duration_ms Time duration in milliseconds
 */
void metal_thread_delay(metal_thread_t *thread, uint32_t duration_ms);

/**
 * @brief        Wakes up the thread from the waiting state
 * @param[in]    thread Thread handle
 */
void metal_thread_wakeup(metal_thread_t *thread);

/**
 * @brief        Starts the scheduler operation
 */
void metal_scheduler_start(void);


/**
 * @brief        Suspends the scheduler operation
 */
void metal_scheduler_suspend(void);

/**
 * @brief        Resumes the scheduler operation
 */
void metal_scheduler_resume(void);

/**
 * @brief        Stack overflow handler function type
 */
typedef void (*metal_stack_overflow_handler)(const char* thread_name);

/**
 * @brief        Registers a stack overflow handler function
 */
void metal_set_stack_overflow_handler(metal_stack_overflow_handler func);

/**
 * @brief        Current thread that's calling this API to wait for notify, like a binary semaphore
 */
void metal_thread_notify_take_and_clear(void);

/**
 * @brief        Current thread that's calling this API to wait for notify, like a counting semaphore
 */
void metal_thread_notify_take(void);

/**
 * @brief        Notify thread
 * @param[in]    thread Thread handle
 */
void metal_thread_notify_give(metal_thread_t *thread);

#include "./system/freertos/thread.h"

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_THREAD__H__ */
