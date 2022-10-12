/**
 * @file ns_timer.c
 * @author Carlos Morales
 * @brief Simple timer facility
 * @version 0.1
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_timer.h"

uint32_t ns_timer_init(uint32_t ui32TimerNum)
{
    am_hal_timer_config_t       TimerConfig;
    uint32_t ui32Status         = AM_HAL_STATUS_SUCCESS;


    //
    // Set the timer configuration
    // The default timer configuration is HFRC_DIV16, EDGE, compares=0, no trig.
    //
    am_hal_timer_default_config_set(&TimerConfig);
    ui32Status = am_hal_timer_config(ui32TimerNum, &TimerConfig);
    if ( ui32Status != AM_HAL_STATUS_SUCCESS )
    {
        am_util_stdio_printf("Failed to configure TIMER%d, return value=%d\r\n", ui32TimerNum, ui32Status);
        return ui32Status;
    }

    //
    // Stop and clear the timer.
    //
    am_hal_timer_clear(ui32TimerNum);

    //
    // Timer interrupt not needed for this purpose.
    //

    return ui32Status;

}

uint32_t ns_us_ticker_read(uint32_t ui32TimerNum)
{
    return am_hal_timer_read(ui32TimerNum) / 6;
}