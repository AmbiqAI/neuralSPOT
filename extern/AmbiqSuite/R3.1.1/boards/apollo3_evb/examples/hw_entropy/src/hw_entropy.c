//*****************************************************************************
//
//! @file hw_entropy.c
//!
//! @brief An example of how to collect entropy from the Apollo3 hardware.
//!
//! Purpose: This example prints a "Hello World" message with some device info.
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

//******************************************************************************
//
// Settings.
//
//******************************************************************************
#define TOTAL_OUTPUT_BYTES   1024
#define VERBOSE_PRINTF       1

//******************************************************************************
//
// Prototypes.
//
//******************************************************************************
void entropy_complete_callback(void *context);
void initial_setup(void);

//******************************************************************************
//
// Globals
//
//******************************************************************************
volatile uint32_t data_received = 0;
uint8_t output[32];
uint32_t length = 32;
#define ITERATIONS (TOTAL_OUTPUT_BYTES / length)

//******************************************************************************
//
// ISR for CTIMER.
//
//******************************************************************************
void
am_ctimer_isr(void)
{
    uint32_t ui32Status;

    //
    // Read and clear the interrupt status.
    //
    ui32Status = am_hal_ctimer_int_status_get(true);
    am_hal_ctimer_int_clear(ui32Status);

    //
    // Call the registered interrupt handlers for any interrupts we recieved.
    //
    am_hal_ctimer_int_service(ui32Status);
}

//*****************************************************************************
//
// Initial setup of:
//  - Clocks
//  - Caching
//  - Essential power functions
//
//*****************************************************************************
void
initial_setup(void)
{
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
    // Low power init
    //
    am_bsp_low_power_init();

    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

#if VERBOSE_PRINTF
    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("TIMER entropy Example.\n");
#endif // VERBOSE_PRINTF

}


//******************************************************************************
//
// Callback function for entropy generator - Called when data has been transferred.
//
//******************************************************************************
void
entropy_complete_callback(void *context)
{
    uint32_t *flag = context;
    *flag = 1;
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    initial_setup();

    //
    // Initialize the entropy hardware.
    //
    am_hal_entropy_init();

    //
    // Interrupts must be enabled, or else the entropy generator will not generate any data.
    //
    am_hal_interrupt_master_enable();

    //
    // Start the entropy generator. It won't produce anything until we ask for data, but
    //
    am_hal_entropy_enable();

    for (uint32_t i = 0; i < ITERATIONS; i++)
    {
        data_received = 0;
        am_hal_entropy_get_values(output, length, entropy_complete_callback, (void *) &data_received);

        //
        // Note: we can't sleep here because it would stop the SysTick timer,
        // which is part of the entropy generation method.
        //
        while (data_received == 0);

        for (uint32_t j = 0; j < length; j++)
        {
#if VERBOSE_PRINTF
            if (j % 32 == 0)
            {
                am_util_stdio_printf("%02d%%: ", ((i * 32) * 100) / (TOTAL_OUTPUT_BYTES));
            }
#endif

            am_util_stdio_printf("%02X", output[j]);

            if (j % 32 == 31)
            {
                am_util_stdio_printf("\n");
            }
            else
            {
                am_util_stdio_printf(" ");
            }
        }
    }

    // Disable the entropy generator.
    am_hal_entropy_disable();

#if VERBOSE_PRINTF
    am_util_stdio_printf("Done.");
#endif // VERBOSE_PRINTF

    while (1)
    {
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}
