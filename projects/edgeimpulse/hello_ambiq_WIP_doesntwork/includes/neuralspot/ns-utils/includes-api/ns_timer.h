/**
 * @file ns_timer.h
 * @author Carlos Morales
 * @brief Simple timer facility
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef NS_TIMER
#define NS_TIMER

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

extern uint32_t ns_timer_init(uint32_t ui32TimerNum);
extern uint32_t ns_us_ticker_read(uint32_t ui32TimerNum);

#ifdef __cplusplus
}
#endif

#endif