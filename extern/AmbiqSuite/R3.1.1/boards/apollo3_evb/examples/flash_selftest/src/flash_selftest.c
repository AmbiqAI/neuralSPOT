//*****************************************************************************
//
//! @file flash_selftest.c
//!
//! @brief An example to test all onboard flash.
//!
//! Purpose:
//! This example runs a series of test patterns on all instances of the device
//! flash.  It performs many of the same tests that the hardware BIST (built
//! in self test) uses at production test.
//!
//! Results are saved in coded form to a defined data word (g_result) which
//! tracks any failure.  Further, g_result can be given an absolute address
//! location so that an outside system will know where to find the results.
//!
//! Results output is also configurable such that the simplified results
//! (pass/fail, done, etc.) can be output to GPIO bits.
//!
//! The test must be loaded and executed in SRAM.  Therefore a J-Link Commander
//! batch file is provided here to assist with that.
//! Alternatively the program can be loaded with a debugger and run from there.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! Using the J-Link Commander batch file on an Apollo3 Blue EVB:
//! - The Commander script file is one of either selftest_commander_gcc.jlink,
//!   selftest_commander_iar.jlink, or selftest_commander_keil.jlink.
//! - Requires Segger J-Link v6.60 or later.
//! - As shipped in the SDK, the J-Link Commander scripts should be correctly
//!   configured to run the given binary. If the test is modified, the commander
//!   script may need an update of the SP and PC.  The first two word values in
//!   the vector table of your compiled binary determine the required values.
//! - Use the following command line at a DOS prompt.
//!   jlink -CommanderScript selftest_commander_xxx.jlink
//! - The flash self test stores results to address 0x10030000.
//!   0xFAE00000 = Pass, the flash tested good.
//!   0xFAE0xxxx = Fail, where xxxx is a failure code.
//! - If USE_TIMER is enabled, the run time of the selftest is stored in two
//!   words at 0x10030004 and 0x1003008.  The first word is the whole number
//!   of seconds, the second is the fractional part to 3 decimals.  Therefore
//!   the two values show the total run time in the form:  ss.fff
//!
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
#include "am_util_stdio.h"


//*****************************************************************************
//
// Test configuration options
//
//*****************************************************************************
//
// Specify an absolute address for the result.
//
#define RESULT_ADDR             0x10030000

//
// Print the results via ITM/SWO.
// Printing may not be desired in a production environment.
//
#define PRINT_RESULTS           1

//
// Use GPIOs to report run/done/pass status.
//
#define ENABLE_GPIO_STATUS      1

//
// Go ahead and time how long it takes to test the flash.
//
#define USE_TIMER               1


#if ENABLE_GPIO_STATUS
//
// Pins to use for GPIO for output status
//
#define GPIO_PIN_RUN            10
#define GPIO_PIN_DONE           30
#define GPIO_PIN_PASS           15

//
// SA0=Stuck at Zero (can't erase) SA1=Stuck at 1 (can't program)
//#define GPIO_SA0_BIT            14
//#define GPIO_SA1_BIT            17
//
#endif // ENABLE_GPIO_STATUS

//*****************************************************************************
//
// Device configuration macros
//
//*****************************************************************************
#define START_PAGE_INST_0       6
#define START_ADDR_INST_0       0xC000

//
// Macros to describe the flash
//
#define FLASH_ADDR              AM_HAL_FLASH_ADDR
#define FLASH_INST_SIZE         AM_HAL_FLASH_INSTANCE_SIZE
#define FLASH_INST_WORDS        (AM_HAL_FLASH_INSTANCE_SIZE / 4)
#define FLASH_PAGE_SIZE         AM_HAL_FLASH_PAGE_SIZE
#define FLASH_INST_NUM          AM_HAL_FLASH_NUM_INSTANCES

//
// Flash row macros (specific to the particular flash architecture).
//
#define FLASH_ROW_WIDTH_BYTES   AM_HAL_FLASH_ROW_WIDTH_BYTES
#define FLASH_ROW_WIDTH_WDS     (AM_HAL_FLASH_ROW_WIDTH_BYTES / 4)
#define INFO_ROW_WIDTH_WDS      FLASH_ROW_WIDTH_WDS
#define FLASH_INST_ROWS_NUM     (FLASH_INST_WORDS / FLASH_ROW_WIDTH_WDS)

//
// General
//
#if 0   // 0=No printing
#define ERRMSG(x)       { am_util_stdio_printf x; }
#else
#define ERRMSG(x)
#endif


//*****************************************************************************
//
// Globals
//
//*****************************************************************************
#if USE_TIMER
//
// Select the CTIMER number to use for timing.
// The entire 32-bit timer is used.
//
#define SELFTEST_TIMERNUM       0

//
// Timer configuration.
//
static am_hal_ctimer_config_t g_sContTimer =
{
    // Create 32-bit timer
    1,

    // Set up TimerA.
    (AM_HAL_CTIMER_FN_CONTINUOUS    |
     AM_HAL_CTIMER_HFRC_47KHZ),

    // Set up Timer0B.
    0
};
#endif // USE_TIMER

//
// Buffer for programming 1 row at a time
//
uint32_t g_ui32RowBuffer[FLASH_ROW_WIDTH_WDS];
uint32_t g_ui32RowBufferNot[FLASH_ROW_WIDTH_WDS];

//
// Result variables
//
uint32_t g_TestFail;
uint32_t g_TestSA0, g_TestSA1;

//
// Cache configuration
//
const am_hal_cachectrl_config_t flash_selftest_cachectrl_config =
{
    .eDescript                  = AM_HAL_CACHECTRL_DESCR_2WAY_128B_512E,
    .eMode                      = AM_HAL_CACHECTRL_CONFIG_MODE_INSTR_DATA,
    .bLRU                       = 0,
};

//
// Specify an absolute address for the result.
//
#define RESULT_ADDR         0x10030000
volatile uint32_t *g_pui32Result = (uint32_t*)(RESULT_ADDR + 0x00);

#if USE_TIMER
//
// Timer results.
//
volatile uint32_t *pui32_timer_elapsed_sec   = (uint32_t*)(RESULT_ADDR + 0x04);
volatile uint32_t *pui32_timer_elapsed_frac  = (uint32_t*)(RESULT_ADDR + 0x08);
volatile uint32_t *pui32_timer_elapsed_ticks = (uint32_t*)(RESULT_ADDR + 0x0C);
#endif // USE_TIMER

//*****************************************************************************
//
//  am_flash_gpio_init()
//  Initialize the GPIO pins used for reporting test status.
//
//*****************************************************************************
static void
am_flash_gpio_init(void)
{
#if ENABLE_GPIO_STATUS
    //
    // Initialize the GPIO values before configuring the pins.
    //

    //
    // Configure the GPIOs
    // Clear all the GPIOs before configuring them for output.
    //
    am_hal_gpio_output_clear(GPIO_PIN_RUN);
    am_hal_gpio_output_clear(GPIO_PIN_DONE);
    am_hal_gpio_output_clear(GPIO_PIN_PASS);

    //
    // Configure the GPIOs for max output.
    //
    am_hal_gpio_pinconfig(GPIO_PIN_RUN,  g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(GPIO_PIN_DONE, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(GPIO_PIN_PASS, g_AM_HAL_GPIO_OUTPUT_12);

#if (GPIO_SA0_BIT && GPIO_SA1_BIT)
    am_hal_gpio_output_clear(GPIO_SA0_BIT);
    am_hal_gpio_output_clear(GPIO_SA1_BIT);
    am_hal_gpio_pinconfig(GPIO_SA0_BIT,  g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_pinconfig(GPIO_SA1_BIT,  g_AM_HAL_GPIO_OUTPUT_12);
#endif // GPIO_SA0_BIT && GPIO_SA1_BIT

    am_hal_gpio_output_set(GPIO_PIN_RUN);

#endif // ENABLE_GPIO_STATUS
} // am_flash_gpio_init()

//*****************************************************************************
//
//  am_flash_gpio_complete()
//  On completion of the test, set the GPIOs appropriately.
//
//*****************************************************************************
static void
am_flash_gpio_complete(void)
{
#if ENABLE_GPIO_STATUS

    //
    // There is a specific order to setting the GPIOs.
    //  1. PASS - set/clear appropriately.
    //  2. RUN low.
    //  3. DONE high.
    //

    if ( !g_TestFail )
    {
        am_hal_gpio_output_set(GPIO_PIN_PASS);
    }
#if (GPIO_SA0_BIT && GPIO_SA1_BIT)
    if ( !g_TestSA0 )
    {
        am_hal_gpio_output_clear(GPIO_SA0_BIT);
    }

    if ( !g_TestSA1 )
    {
        am_hal_gpio_output_clear(GPIO_SA1_BIT);
    }
#endif // GPIO_SA0_BIT && GPIO_SA1_BIT

    am_hal_gpio_output_clear(GPIO_PIN_RUN);
    am_hal_gpio_output_set(GPIO_PIN_DONE);

#endif // ENABLE_GPIO_STATUS
} // am_flash_gpio_complete()


//*****************************************************************************
//
// am_flash_erase() - Erase main flash memory.
//
//  ui32InstErase
//      0 - FLASH_INST_NUM for an individual instance, e.g. 0, 1, 2, or 3.
//      0xA11 for ALL instances.
//
//      Optionally OR the following bits:
//          bit15 (0x8000) to erase only alternating pages, then
//          bit14 determines whether to erase odd or even pages (0=even, 1=odd)
//
//*****************************************************************************
static uint32_t
am_flash_erase(uint32_t ui32InstErase)
{
    uint32_t ui32RC, ui32Inst;
    uint32_t ui32Pg, ui32Ret = 0;
    bool bAlt, bOdd;

    bAlt  = ui32InstErase & 0x0008000 ? true : false;
    bOdd  = ui32InstErase & 0x0004000 ? true : false;
    ui32InstErase &= ~0x0000C000;

    if ( ui32InstErase == 0xA11 )
    {
        //
        // Create a test mask to cover ALL instances.
        //  e.g. 0x3 = Test 2 instances.
        //       0xF = Test 4 instances.
        //
        ui32InstErase = (1 << FLASH_INST_NUM) - 1;
    }
    else if ( ui32InstErase < FLASH_INST_NUM )
    {
        //
        // Valid instance number provided, convert it into a test mask.
        //
        ui32InstErase = 1 << ui32InstErase;
    }
    else
    {
        ui32Ret = 0x001;
        return ui32Ret;
    }

    //
    // Check both instances (0 and 1) of INFO (0 or 1).
    //
    if ( bAlt )
    {
        //
        // Erase alternating pages.
        //
        ui32Inst = 0;
        while ( ui32InstErase )
        {
            //
            // Start with page 6 in instance 0.
            // Start with page 0 in instance 1.
            //
            ui32Pg  = ( ui32Inst == 0 ) ? START_PAGE_INST_0 : 0;
            ui32Pg += bOdd ? 1 : 0;

            for ( ; ui32Pg < (FLASH_INST_SIZE / FLASH_PAGE_SIZE); ui32Pg += 2)
            {
                ui32RC = g_am_hal_flash.flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY, ui32Inst, ui32Pg);
                if ( ui32RC != 0 )
                {
                    ERRMSG(("   *** ERROR - Mass erase failed with rc=%d\n", ui32RC));
                    g_TestFail = 1;
                }
            } // for ()
            ui32InstErase >>= 1;
            ui32Inst++;
        }
    }
    else
    {
        ui32Inst = 0;
        while ( ui32InstErase )
        {
            if ( ui32InstErase & 1 )
            {
                //
                // If SBL instance 0, skip the SBL pages.
                //
                if ( ui32Inst == 0 )
                {
                    for (ui32Pg = START_PAGE_INST_0; ui32Pg < (FLASH_INST_SIZE / FLASH_PAGE_SIZE); ui32Pg++)
                    {
                        ui32RC = g_am_hal_flash.flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY, 0, ui32Pg);
                        if ( ui32RC != 0 )
                        {
                            ERRMSG(("   *** ERROR - Mass erase failed with rc=%d\n", ui32RC));
                            g_TestFail = 1;
                        }
                    }
                }
                else
                {
                    ui32RC = g_am_hal_flash.flash_mass_erase(AM_HAL_FLASH_PROGRAM_KEY, ui32Inst);
                    if ( ui32RC )
                    {
                        ERRMSG(("FAIL: am_flash_erase() instance %d, return code=0x%X\n",
                            ui32Inst, ui32RC));
                        ui32Ret = 0x8;
                    }
                }
            } // if (ui32InstErase)
            ui32InstErase >>= 1;
            ui32Inst++;
        }
    }

    //
    // Test has completed.
    //
    return ui32Ret;

} // am_flash_erase()

//*****************************************************************************
//
// am_flash_selftest() - advanced test of the FLASH space.
//
// This test will perform the following programming exercises on FLASH:
//  - Program all 00s in given instance and check.
//  - Program all AAs in given instance and check.
//  - Program all 55s in given instance and check.
//
// ui32InstNum:
//  Individual instance value of: 0 - AM_HAL_FLASH_NUM_INSTANCES, or
//  0xA11 = Test ALL instances.
//
// Returns:
//  0     = PASS.
//  If not zero, an error was detected.
//      Bits [15:14] = Instance tested. 1=inst 0, 2=inst 1, 3=both.
//      Low 12-bits error code:
//      0x0?? = A data error occurred.
//              (Currently 0x010, 0x020-0x027, 0x030-0x031, 0x040)
//              0x010 = Diagonal error.
//              0x02? = Checkboard error.
//              0x03? = Page erase error (adjacent page erased).
//              0x040 = Also bits did not end up as 0.
//      0x1?? = A helper function error occurred.
//          (Currently 0x101 - 0x113)
//
//*****************************************************************************
static uint32_t
am_flash_selftest(uint32_t ui32InstNum)
{
    uint32_t *pui32Addr;
    uint32_t ui32DataWd, ui32RowOffset, ui32Ret;
    uint32_t ui32FlashAddr, ui32RC;
    int32_t i32NumRows, i32firstRow;
    int32_t  i32Row, i32Col;
    bool     bCheckit;
    bool     bToggle4;
    uint32_t ui32DataWds[2];
    uint32_t ux;

    if ( ui32InstNum == 0 )
    {
        ui32FlashAddr = START_ADDR_INST_0;
        i32firstRow   = START_PAGE_INST_0 * (AM_HAL_FLASH_PAGE_SIZE / AM_HAL_FLASH_ROW_WIDTH_BYTES);
        i32NumRows    = FLASH_INST_ROWS_NUM - i32firstRow;
    }
    else if ( ui32InstNum < FLASH_INST_NUM )
    {
        ui32FlashAddr = FLASH_ADDR + (FLASH_INST_SIZE * ui32InstNum);
        i32firstRow   = 0;
        i32NumRows    = FLASH_INST_ROWS_NUM;
    }
    else if ( ui32InstNum == 0xA11 )
    {
        ui32FlashAddr = START_ADDR_INST_0;
        i32firstRow   = START_PAGE_INST_0 * (AM_HAL_FLASH_PAGE_SIZE / AM_HAL_FLASH_ROW_WIDTH_BYTES);
        i32NumRows    = FLASH_INST_ROWS_NUM - i32firstRow;
        i32NumRows    = (FLASH_INST_ROWS_NUM * FLASH_INST_NUM) - i32firstRow;
    }
    else
    {
        return 0xFFFFFFFF;
    }

    ui32Ret = (ui32InstNum + 1) << 14;

    //
    // Begin testing with an erased flash.
    //
    ui32RC = am_flash_erase(ui32InstNum);
    if ( ui32RC )
    {
        return ui32Ret | 0x101;
    }

    //
    // Write and check a diagonal pattern.
    // Create multiple diagonal stripes every 32 bits.
    //
    pui32Addr = (uint32_t*)(ui32FlashAddr);
    for ( i32Row = i32firstRow; i32Row < (i32NumRows + i32firstRow); i32Row++ )
    {
        //
        // Compute the starting pattern for this row.
        //
        //ui32DataWd = ~((uint32_t)0x80000000 >> (i32Row & 0x1f));
        ui32DataWd = ~((uint32_t)0x40000000 >> (i32Row & 0x1f));

        //
        // Fill the row buffer
        //
        for (i32Col = 0; i32Col < FLASH_ROW_WIDTH_WDS; i32Col++ )
        {
            g_ui32RowBuffer[i32Col] = ui32DataWd;

            //
            // Get next data value
            //
            if ( ui32DataWd == 0xFFFFFFFE )
            {
                ui32DataWd = 0x7FFFFFFF;
            }
            else
            {
                ui32DataWd >>= 1;
                ui32DataWd |= 0x80000000;
            }
        }

        //
        // Program the new row of values
        //
        ui32RC = g_am_hal_flash.flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                    &g_ui32RowBuffer[0], pui32Addr, FLASH_ROW_WIDTH_WDS);

        if ( ui32RC )
        {
            return ui32Ret | 0x102;
        }

        //
        // Bump pointer to next row.
        //
        pui32Addr += FLASH_ROW_WIDTH_WDS;
    }

    //
    // Now, check the diagonals just programmed
    //
    ui32RowOffset = 0;
    pui32Addr = (uint32_t*)(ui32FlashAddr + ui32RowOffset);
    for ( i32Row = i32firstRow; i32Row < (i32NumRows + i32firstRow); i32Row++ )
    {
        //
        // Compute the starting pattern for this row.
        //
        //ui32DataWd = ~((uint32_t)0x80000000 >> (i32Row & 0x1f));
        ui32DataWd = ~((uint32_t)0x40000000 >> (i32Row & 0x1f));
        for (i32Col = 0; i32Col < FLASH_ROW_WIDTH_WDS; i32Col++ )
        {
            //
            // Check this word.
            //
            if ( *pui32Addr != ui32DataWd )
            {
                return ui32Ret | 0x010;
            }

            //
            // Get next data value
            //
            if ( ui32DataWd == 0xFFFFFFFE )
            {
                ui32DataWd = 0x7FFFFFFF;
            }
            else
            {
                ui32DataWd >>= 1;
                ui32DataWd |= 0x80000000;
            }

            //
            // Bump pointer
            //
            pui32Addr++;
        }
    }

    ui32RC = am_flash_erase(ui32InstNum);
    if ( ui32RC )
    {
        return ui32Ret | 0x103;
    }


    //
    // Initialize the state machine for the first pass.
    //
    ui32DataWds[0] = 0xAAAAAAAA;
    ui32DataWds[1] = ~ui32DataWds[0];
    bToggle4 = true;
    bCheckit = true;

    while ( ui32DataWds[0] != 0x12345678 )
    {
        //
        // Fill the row buffer
        // To properly checkerboard the physical array, we need to invert the
        // pattern every 4 words.
        // Then every row, we need to invert that pattern.
        //
        for (i32Col = 0; i32Col < FLASH_ROW_WIDTH_WDS; i32Col += 8 )
        {
            if ( bToggle4 )
            {
                g_ui32RowBuffer[i32Col + 0]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 1]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 2]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 3]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 4]    = ui32DataWds[1];
                g_ui32RowBuffer[i32Col + 5]    = ui32DataWds[1];
                g_ui32RowBuffer[i32Col + 6]    = ui32DataWds[1];
                g_ui32RowBuffer[i32Col + 7]    = ui32DataWds[1];

                g_ui32RowBufferNot[i32Col + 0] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 1] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 2] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 3] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 4] = ui32DataWds[0];
                g_ui32RowBufferNot[i32Col + 5] = ui32DataWds[0];
                g_ui32RowBufferNot[i32Col + 6] = ui32DataWds[0];
                g_ui32RowBufferNot[i32Col + 7] = ui32DataWds[0];
            }
            else
            {
                g_ui32RowBuffer[i32Col + 0]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 1]    = ui32DataWds[1];
                g_ui32RowBuffer[i32Col + 2]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 3]    = ui32DataWds[1];
                g_ui32RowBuffer[i32Col + 4]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 5]    = ui32DataWds[1];
                g_ui32RowBuffer[i32Col + 6]    = ui32DataWds[0];
                g_ui32RowBuffer[i32Col + 7]    = ui32DataWds[1];

                g_ui32RowBufferNot[i32Col + 0] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 1] = ui32DataWds[0];
                g_ui32RowBufferNot[i32Col + 2] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 3] = ui32DataWds[0];
                g_ui32RowBufferNot[i32Col + 4] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 5] = ui32DataWds[0];
                g_ui32RowBufferNot[i32Col + 6] = ui32DataWds[1];
                g_ui32RowBufferNot[i32Col + 7] = ui32DataWds[0];
            }
        }

        //
        // Do the alternating rows from low to high addresses.
        //
        for ( i32Row = i32firstRow; i32Row < (i32NumRows + i32firstRow); i32Row += 2 )
        {
            ui32RowOffset = (i32Row - i32firstRow) * FLASH_ROW_WIDTH_WDS * 4;
            pui32Addr = (uint32_t*)(ui32FlashAddr + ui32RowOffset);

            //
            // Program the new row of values
            //
            ui32RC = g_am_hal_flash.flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                        &g_ui32RowBuffer[0], pui32Addr, FLASH_ROW_WIDTH_WDS);

            if ( ui32RC )
            {
                return ui32Ret | 0x104;
            }
        }

        //
        // Do the alternating rows from high to low addresses.
        //
        for ( i32Row = (i32NumRows + i32firstRow) - 1; i32Row > i32firstRow; i32Row -= 2 )
        {
            ui32RowOffset = (i32Row - i32firstRow) * FLASH_ROW_WIDTH_WDS * 4;
            pui32Addr = (uint32_t*)(ui32FlashAddr + ui32RowOffset);

            //
            // Program the new row of values
            //
            ui32RC = g_am_hal_flash.flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                        &g_ui32RowBufferNot[0], pui32Addr, FLASH_ROW_WIDTH_WDS);

            if ( ui32RC )
            {
                return ui32Ret | 0x105;
            }
        }

        if ( bCheckit )
        {
            //
            // Now check all of the data just programmed
            //
            ui32RowOffset = 0;
            pui32Addr = (uint32_t*)(ui32FlashAddr + ui32RowOffset);

            for (i32Row = i32firstRow; i32Row < (i32NumRows + i32firstRow); i32Row++ )
            {
                for ( i32Col = 0; i32Col < FLASH_ROW_WIDTH_WDS; i32Col++ )
                {
                    // Check the data
                    if ( bToggle4 )
                    {
                        if ( i32Row & 0x1 )
                        {
                            if ( i32Col & 0x4 )
                            {
                                if ( *pui32Addr != ui32DataWds[0] )
                                {
                                    return ui32Ret | 0x020;
                                }
                            }
                            else
                            {
                                if ( *pui32Addr != ui32DataWds[1] )
                                {
                                    return ui32Ret | 0x021;
                                }
                            }
                        }
                        else
                        {
                            if ( i32Col & 0x4 )
                            {
                                if ( *pui32Addr != ui32DataWds[1] )
                                {
                                    return ui32Ret | 0x022;
                                }
                            }
                            else
                            {
                                if ( *pui32Addr != ui32DataWds[0] )
                                {
                                    return ui32Ret | 0x023;
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( i32Row & 0x1 )
                        {
                            if ( i32Col & 0x1 )
                            {
                                if ( *pui32Addr != ui32DataWds[0] )
                                {
                                    return ui32Ret | 0x024;
                                }
                            }
                            else
                            {
                                if ( *pui32Addr != ui32DataWds[1] )
                                {
                                    return ui32Ret | 0x025;
                                }
                            }
                        }
                        else
                        {
                            if ( i32Col & 0x1 )
                            {
                                if ( *pui32Addr != ui32DataWds[1] )
                                {
                                    return ui32Ret | 0x026;
                                }
                            }
                            else
                            {
                                if ( *pui32Addr != ui32DataWds[0] )
                                {
                                    return ui32Ret | 0x027;
                                }
                            }
                        }
                    }
                    pui32Addr++;
                }
            }

            //
            // Erase and prepare for next test (or for exiting)
            //
            if ( ui32DataWds[0]  == 0xAAAAAAAA )
            {
                //
                // Erase alternating odd pages.
                // None of the words in any even pages should be 0xFFFFFFFF.
                //
                ui32RC = am_flash_erase(ui32InstNum | 0xC000);
                if ( ui32RC )
                {
                    return ui32Ret | 0x106;
                }

                //
                // Check one word in each even page, none should be erased.
                //
                for ( ux = ui32FlashAddr; ux < i32NumRows * FLASH_ROW_WIDTH_BYTES; ux += (8192 * 2) )
                {
                    if ( AM_REGVAL(ux) == 0xFFFFFFFF )
                    {
                        return ui32Ret | 0x030;
                    }
                }

                //
                // Now erase the even pages to prepare for the next test.
                //
                ui32RC = am_flash_erase(ui32InstNum | 0x8000);
                if ( ui32RC )
                {
                    return ui32Ret | 0x107;
                }
            }
            else if ( ui32DataWds[0]  == 0x55555555 )
            {
                //
                // This time we're going to erase alternating even pages.
                // None of the words in any odd pages should be 0xFFFFFFFF.
                //
                ui32RC = am_flash_erase(ui32InstNum | 0x8000);
                if ( ui32RC )
                {
                    return ui32Ret | 0x108;
                }

                //
                // Check one word in each odd page, none should be erased.
                //
                for ( ux = ui32FlashAddr + (8 * 1024); ux < i32NumRows * FLASH_ROW_WIDTH_BYTES; ux += (8192 * 2) )
                {
                    if ( AM_REGVAL(ux) == 0xFFFFFFFF )
                    {
                        return ui32Ret | 0x031;
                    }
                }

                //
                // Now erase the odd pages to prepare for the next test.
                //
                ui32RC = am_flash_erase(ui32InstNum | 0xC000);
                if ( ui32RC )
                {
                    return ui32Ret | 0x109;
                }
            }
            else
            {
                ui32RC = am_flash_erase(ui32InstNum);
                if ( ui32RC )
                {
                    return ui32Ret | 0x110;
                }
            }
        }

        //
        // State machine to determine the next data value to test.
        //
        bCheckit = true;
        bToggle4 = false;
        if ( ui32DataWds[0] == 0xAAAAAAAA )
        {
            ui32DataWds[0]  = 0x55555555;
            ui32DataWds[1]  = 0xAAAAAAAA;
            bToggle4 = true;
        }
        else if ( ui32DataWds[0] == 0x55555555 )
        {
            ui32DataWds[0]  = 0xFFFFFFFF;
            ui32DataWds[1]  = 0x00000000;
        }
        else if ( ui32DataWds[0] == 0xFFFFFFFF )
        {
            ui32DataWds[0]  = 0x00000000;
            ui32DataWds[1]  = 0xFFFFFFFF;
        }
        else
        {
            ui32DataWds[0]  = 0x12345678;
        }

    } // while(

    //
    // Finally, before exiting, we need to check programming current by
    // programming a row of 00s (00s being the worst case for programming).
    // One row is adequate for this quick check just to make sure the
    // power path is holding up.
    //

    //
    // Fill the row buffer
    //
    for (i32Col = 0; i32Col < FLASH_ROW_WIDTH_WDS; i32Col++ )
    {
        g_ui32RowBuffer[i32Col] = 0x00000000;
    }

    //
    // Program the row of values
    //
    ui32RowOffset = 10 * FLASH_ROW_WIDTH_WDS * 4; // Arbitrary row in instance 0
    pui32Addr = (uint32_t*)(ui32FlashAddr + ui32RowOffset);
    ui32RC = g_am_hal_flash.flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                &g_ui32RowBuffer[0], pui32Addr, FLASH_ROW_WIDTH_WDS);

    if ( ui32RC )
    {
        return ui32Ret | 0x112;
    }

    //
    // Check the programmed row
    //
    for ( i32Row = 0; i32Row < FLASH_ROW_WIDTH_WDS; i32Row++ )
    {
        if ( *pui32Addr != 0x00000000 )
        {
            return ui32Ret | 0x040;
        }
        pui32Addr++;
    }

    //
    // Erase instance 0
    //
    ui32RC = am_flash_erase(0);
    if ( ui32RC )
    {
        return ui32Ret | 0x113;
    }


    return 0;

} // am_flash_selftest()

#if USE_TIMER
void
selftest_timer_stop(void)
{
    uint32_t timer_elapsed_ticks;

    am_hal_ctimer_stop(SELFTEST_TIMERNUM, AM_HAL_CTIMER_BOTH);

    timer_elapsed_ticks = am_hal_ctimer_read(SELFTEST_TIMERNUM, AM_HAL_CTIMER_BOTH);

    //
    // Compute the integer and decimal portions of the total run time such that
    // we end up with:  <int>.<frac>
    // e.g. If int=15 and frac=175, total run time is 15.175.
    // Caveat: If device is not yet trimmed, the run speed is not exactly 48MHz.
    //
    *pui32_timer_elapsed_sec  = timer_elapsed_ticks / (48000000 / 1024);
    *pui32_timer_elapsed_frac = timer_elapsed_ticks % (48000000 / 1024);
    *pui32_timer_elapsed_frac = *pui32_timer_elapsed_frac * 1000 / (48000000 / 1024);
} // selftest_timer_stop()
#endif // USE_TIMER

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the cache configuration and enable.
    //
    am_hal_cachectrl_config(&flash_selftest_cachectrl_config);
    am_hal_cachectrl_enable();

    //
    // Make sure all memory and flash is powered up.
    //
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_ALL);

#if PRINT_RESULTS
    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("flash_selftest\n");
#endif // PRINT_RESULTS

    //
    // Initialize the result variables
    //
    *g_pui32Result = 0xdeadbeef;
    g_TestFail = g_TestSA0 = g_TestSA1 = 0;

    am_flash_gpio_init();

#if USE_TIMER
    //
    // Set up and start the timer.
    //
    am_hal_ctimer_stop(SELFTEST_TIMERNUM, AM_HAL_CTIMER_BOTH);
    am_hal_ctimer_clear(SELFTEST_TIMERNUM, AM_HAL_CTIMER_BOTH);
    am_hal_ctimer_config(SELFTEST_TIMERNUM, &g_sContTimer);
    am_hal_ctimer_start(SELFTEST_TIMERNUM, AM_HAL_CTIMER_TIMERA);
#endif // USE_TIMER

    //
    // Run the test on both instances.
    //
    *g_pui32Result = am_flash_selftest(0xA11);

#if USE_TIMER
    //
    // Stop the timer and save off the run times.
    // The run time is stored as 2 whole numbers: secs.frac.
    //  g_ui32timer_elapsed_sec:g_ui32timer_elapsed_frac
    //
    selftest_timer_stop();

#endif // USE_TIMER

    //
    // The upper bits of the results value are set with a value that
    // makes the result easily distinguishable.
    //
    *g_pui32Result |= 0xFAE00000;

    //
    // Set pass/fail.
    // Note that some tests may have already set this variable to 1.
    //
    g_TestFail += (*g_pui32Result & 0x000FFFFF) ? 1 : 0;

#if PRINT_RESULTS
    am_util_stdio_printf("Result = 0x%08X --- Flash Selftest ", *g_pui32Result);

    if ( ((*g_pui32Result & 0x000FFFFF) != 0x00000000) ||
         ((*g_pui32Result & 0xFFF00000) != 0xFAE00000) )
    {
        am_util_stdio_printf("FAILED.\n");
    }
    else
    {
        am_util_stdio_printf("PASSED.\n");
    }

#if USE_TIMER
    //
    // Print out the run time
    //
    uint32_t sec, frac;
    sec = *pui32_timer_elapsed_sec;
    frac = *pui32_timer_elapsed_frac;
    am_util_stdio_printf("flash_selftest completed in %d.%d seconds.\n", sec, frac);
#endif // USE_TIMER

    //
    // We are done printing.
    // Disable debug printf messages on ITM.
    //
    am_bsp_debug_printf_disable();
#endif // PRINT_RESULTS

    //
    // Signal completion.
    //
    am_flash_gpio_complete();

    //
    // Done
    //
    while(1);

} // main()



