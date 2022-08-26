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

#ifdef __cplusplus
extern "C" {
#endif
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "rtos.h"
#include "FreeRTOS.h"
#include "portable.h"
#include "portmacro.h"

extern uint8_t ns_malloc_init();
extern void* ns_malloc(size_t size);
extern void ns_free(void* ptr);

#ifdef __cplusplus
}
#endif
#endif