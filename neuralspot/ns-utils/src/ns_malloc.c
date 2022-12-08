/**
 * @file ns-malloc.c
 * @author Carlos Morales
 * @brief Malloc-like wrapper for FreeRTOS memory allocator.
 * @version 0.1
 * @date 2022-08-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_malloc.h"

uint8_t ucHeap[NS_MALLOC_HEAP_SIZE_IN_K * 1024];

/// Empty for now, but placeholder in case we need multi-heap support
uint8_t
ns_malloc_init() {
    return 0;
};

/// Simple pvPortMalloc wrapper
void *
ns_malloc(size_t size) {
    void *ptr = NULL;

    if (size > 0) {
        ptr = pvTasklessPortMalloc(size);
    } // else NULL if there was an error

    return ptr;
}

/// Simple vPortFree wrapper
void
ns_free(void *ptr) {
    if (ptr) {
        vTasklessPortFree(ptr);
    }
}
