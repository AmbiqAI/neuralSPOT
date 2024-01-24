//*****************************************************************************
//
//! @file fast_gpio.c
//!
//! @brief Example that demonstrates how to use the Fast GPIO feature of Apollo3.
//!
//! Purpose: This example demonstrates how to use Fast GPIO on Apollo3.  The example
//! updates the LEDs with waveforms that can be observed with a logic analyzer.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Macros
//
//*****************************************************************************
#define     FASTGPIO_PIN_A      0
#define     FASTGPIO_PIN_B      1
#define     FASTGPIO_PIN_C      2
#define     FASTGPIO_PIN_D      3
#define     FASTGPIO_PIN_E      4
#define     FASTGPIO_PIN_F      5
#define     FASTGPIO_PIN_G      6
#define     FASTGPIO_PIN_H      7


/*
#ifdef AM_BSP_NUM_LEDS
#define NUM_LEDS    AM_BSP_NUM_LEDS
#else
#define NUM_LEDS    5       // Make up an arbitrary number of LEDs
#endif
*/
//*****************************************************************************
//
// Globals
//
//*****************************************************************************
const uint32_t g_ui32LED_pattern[][2] =
{
    // Flash LED0 10 times
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},
    {0x00, 100},
    {0x01, 100},

    // Walk the LEDs sequentially
    {0x00, 200},
    {0x01, 200},
    {0x02, 200},
    {0x04, 200},
    {0x08, 200},
    {0x00, 200},
    {0x08, 200},
    {0x04, 200},
    {0x02, 200},
    {0x01, 200},
    {0x00, 200},

    // Flash all of the LEDs on and off.
    {0xA5000001, 0xA5A5A5A5},   // Flag that we need to do something here
    {0x0F, 300},
    {0x00, 300},
    {0x0F, 300},
    {0x00, 300},
    {0x0F, 300},
    {0x00, 300},
    {0xFFFFFFFF, 0xFFFFFFFF}
};

//
// Use Fast GPIO to set the LED pattern.
// LED1 and 3 are on the same row, we can only do one of them.
//   LED0: 10 = row 2
//   LED1: 30 = row 6
//   LED2: 15 = row 7
//   LED3: 14 = row 6
//   LED4: 17 = row 1
//
#define NLEDS   4
static uint32_t g_ui32LEDs[NLEDS] =
{
    AM_BSP_GPIO_LED0,
    AM_BSP_GPIO_LED1,
    AM_BSP_GPIO_LED2,
    /* AM_BSP_GPIO_LED3, */
    AM_BSP_GPIO_LED4
};

static void set_leds(uint32_t ui32Value, uint32_t ui32delayms)
{
    uint32_t ux, ui32Led;

    ux = 0;
    while ( ux < NLEDS )
    {
        ui32Led = g_ui32LEDs[ux];

        if ( ui32Value & 1 )
        {
            am_hal_gpio_fastgpio_set(ui32Led);
        }
        else
        {
            am_hal_gpio_fastgpio_clr(ui32Led);
        }
        ui32Value >>= 1;
        ux++;
    }
    am_util_delay_ms(ui32delayms);
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Ret, ux;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();


    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Fast GPIO Example\n");
    am_util_stdio_printf("When run on an Apollo3 EVB:\n");
    am_util_stdio_printf(" - An ~24MHz waveform (after instructions have cached) is output on Pin %d.\n", FASTGPIO_PIN_B);
    am_util_stdio_printf(" - Then the Apollo3 EVB LEDs display a short pattern.\n");
    am_util_stdio_printf(" - LED0=pin %d, LED1=%d, LED2=%d, LED3=%d, LED4=%d.\n", AM_BSP_GPIO_LED0, AM_BSP_GPIO_LED1,
                         AM_BSP_GPIO_LED2, AM_BSP_GPIO_LED3, AM_BSP_GPIO_LED4 );
    am_util_stdio_printf(" - Note that LED1 and LED3 of the EVB are located on the same Fast GPIO row (%d and %d).\n",
                         AM_BSP_GPIO_LED1 & 7, AM_BSP_GPIO_LED3 & 7);
    am_util_stdio_printf("   Because of that, LED3 is disabled during the walk sequence and is thus skipped in the\n");
    am_util_stdio_printf("   sequence. Then it is enabled during the all-blink and blinks along with LED1.\n");
    am_util_stdio_printf("\n");
    am_util_delay_ms(100);

    //
    // Configure the pins that are to be used for Fast GPIO.
    //
    am_hal_gpio_fastgpio_disable(FASTGPIO_PIN_B);
    am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
    AM_HAL_GPIO_MASKCREATE(sFastGpioMask);
    ui32Ret = am_hal_gpio_fast_pinconfig(AM_HAL_GPIO_MASKBIT(psFastGpioMask, FASTGPIO_PIN_B),
                                         g_AM_HAL_GPIO_OUTPUT_12, 0);
    if ( ui32Ret )
    {
        am_util_stdio_printf("Error returned from am_hal_gpio_fast_pinconfig() = .\n", ui32Ret);
    }

    //
    // We are done printing. Disable debug printf messages on ITM.
    //
    am_bsp_debug_printf_disable();

    //
    //
    // Note that we won't get to a steady 24MHz until everything is cached,
    // hence the loop.
    //
    ux = 10;
    while (ux--)
    {
        //
        // Do 20 iterations in a loop so we can achieve a 24MHz waveform.
        // With -O3, each macro call compiles into a single instruction.
        //
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 0
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 1
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 2
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 3
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 4
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 5
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 6
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 7
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 8
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 9
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 10
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 11
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 12
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 13
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 14
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 15
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 16
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 17
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 18
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
        am_hal_gpio_fastgpio_set(FASTGPIO_PIN_B); // 19
        am_hal_gpio_fastgpio_clr(FASTGPIO_PIN_B);
    }

    APBDMA->BBSETCLEAR = _VAL2FLD(APBDMA_BBSETCLEAR_CLEAR, 0xff);

    am_hal_gpio_fastgpio_disable(FASTGPIO_PIN_B);


    //
    // Let's flash some LED patterns using Fast GPIO.
    // Before configuring the pin, disable and initialize the value.
    //
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED0);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED1);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED2);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED3);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED4);
    am_hal_gpio_fastgpio_clr(AM_BSP_GPIO_LED0);
    am_hal_gpio_fastgpio_clr(AM_BSP_GPIO_LED1);
    am_hal_gpio_fastgpio_clr(AM_BSP_GPIO_LED2);
    am_hal_gpio_fastgpio_clr(AM_BSP_GPIO_LED3);
    am_hal_gpio_fastgpio_clr(AM_BSP_GPIO_LED4);

    //
    // Clear the GPIO bitmask, then set the needed bits.
    //
    AM_HAL_GPIO_MASKCLR(psFastGpioMask);

    //
    // Must config each pin on its own
    //
    AM_HAL_GPIO_MASKBIT(psFastGpioMask, AM_BSP_GPIO_LED0);
    ui32Ret = am_hal_gpio_fast_pinconfig(psFastGpioMask, g_AM_HAL_GPIO_OUTPUT_12, 0);
    if ( ui32Ret )
    {
        am_util_stdio_printf("Error returned from LED am_hal_gpio_fast_pinconfig() = 0x%X.\n", ui32Ret);
    }

    AM_HAL_GPIO_MASKBIT(psFastGpioMask, AM_BSP_GPIO_LED1);
    ui32Ret = am_hal_gpio_fast_pinconfig(psFastGpioMask, g_AM_HAL_GPIO_OUTPUT_12, 0);
    if ( ui32Ret )
    {
        am_util_stdio_printf("Error returned from LED am_hal_gpio_fast_pinconfig() = 0x%X.\n", ui32Ret);
    }

    AM_HAL_GPIO_MASKBIT(psFastGpioMask, AM_BSP_GPIO_LED2);
    ui32Ret = am_hal_gpio_fast_pinconfig(psFastGpioMask, g_AM_HAL_GPIO_OUTPUT_12, 0);
    if ( ui32Ret )
    {
        am_util_stdio_printf("Error returned from LED am_hal_gpio_fast_pinconfig() = 0x%X.\n", ui32Ret);
    }

    AM_HAL_GPIO_MASKBIT(psFastGpioMask, AM_BSP_GPIO_LED3);
    ui32Ret = am_hal_gpio_fast_pinconfig(psFastGpioMask, g_AM_HAL_GPIO_OUTPUT_12, 0);
    if ( ui32Ret )
    {
        am_util_stdio_printf("Error returned from LED am_hal_gpio_fast_pinconfig() = 0x%X.\n", ui32Ret);
    }

    AM_HAL_GPIO_MASKBIT(psFastGpioMask, AM_BSP_GPIO_LED4);
    ui32Ret = am_hal_gpio_fast_pinconfig(psFastGpioMask, g_AM_HAL_GPIO_OUTPUT_12, 0);
    if ( ui32Ret )
    {
        am_util_stdio_printf("Error returned from LED am_hal_gpio_fast_pinconfig() = 0x%X.\n", ui32Ret);
    }

    //
    // We know for Apollo3 EVB that LED1 and 3 are on the same row. Knowing
    // that, we can demonstrate how the 2 pins will display the same value.
    // For the 4 bit sequence, we'll disable LED3 so it doesn't turn on with
    // LED1.
    //
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED3);

    ux = 0;
    while ( g_ui32LED_pattern[ux][0] != 0xFFFFFFFF )
    {
        if ( g_ui32LED_pattern[ux][0] == 0xA5000001 )
        {
            // Enable LED3. After this, it will react the same as LED1.
            am_hal_gpio_fastgpio_enable(AM_BSP_GPIO_LED3);
        }
        else
        {
            set_leds(g_ui32LED_pattern[ux][0], g_ui32LED_pattern[ux][1]);
        }
        ux++;
    }

    //
    // Disable everything.
    //
    APBDMA->BBSETCLEAR = _VAL2FLD(APBDMA_BBSETCLEAR_CLEAR, 0xff);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED0);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED1);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED2);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED3);
    am_hal_gpio_fastgpio_disable(AM_BSP_GPIO_LED4);
}
