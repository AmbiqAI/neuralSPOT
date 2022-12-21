/**
 * @file ns-malloc.h
 * @author Carlos Morales
 * @brief Malloc-like wrapper for FreeRTOS memory allocator.
 * @version 0.1
 * @date 2022-08-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef NS_MALLOC
#define NS_MALLOC

#ifndef NS_MALLOC_HEAP_SIZE_IN_K
    #define NS_MALLOC_HEAP_SIZE_IN_K 16
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "FreeRTOS.h"
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "portable.h"
#include "portmacro.h"
#include "rtos.h"

extern uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024];

extern uint8_t
ns_malloc_init();
extern void *
ns_malloc(size_t size);
extern void
ns_free(void *ptr);

#ifdef __cplusplus
}
#endif
#endif
