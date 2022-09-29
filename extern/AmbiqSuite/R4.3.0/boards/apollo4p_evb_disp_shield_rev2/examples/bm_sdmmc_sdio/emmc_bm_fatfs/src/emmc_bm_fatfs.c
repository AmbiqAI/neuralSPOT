//*****************************************************************************
//
//! @file emmc_bm_fatfs.c
//!
//! @brief eMMC bare-metal FatFs example.
//!
//! Purpose: This example demonstrates how to use file system with eMMC device
//! based on the eMMC bare-metal HAL.
//!
//! Additional Information:
//! To enable debug printing, add the following project-level macro definitions.
//!
//! AM_DEBUG_PRINTF
//!
//! When defined, debug messages will be sent over ITM/SWO at 1M Baud.
//!
//! Note that when these macros are defined, the device will never achieve deep
//! sleep, only normal sleep, due to the ITM (and thus the HFRC) being enabled.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "mmc_apollo4.h"

FRESULT res;      /* File function return code */
FATFS eMMCFatFs;  /* Filesystem object */
FIL TestFile;     /* File object */
char eMMCPath[4]; /* eMMC logical drive path */
static uint8_t work_buf[FF_MAX_SS]; /* working buffer for format process */
static uint8_t read_buf[256]; /* buffer for file read */
static uint8_t write_buf[256] = "This is the content insided the TXT\nApollo4 eMMC File System Example!!!\n"; /* buffer for file write */
static uint32_t write_cnt, read_cnt;  /* numbers of bytes to write and read */

//*****************************************************************************
//
// Check whether read data match with write data.
//
//*****************************************************************************
static void check_if_rwbuf_match(uint8_t *rdbuf, uint8_t *wrbuf, uint32_t len)
{
    int i;
    for ( i = 0; i < len; i++ )
    {
        if (*(wrbuf + i) != *(rdbuf + i) )
        {
            am_util_stdio_printf("Test Fail: read and write buffer does not match from %d\n", i);
            break;
        }
    }

    if (i == len)
    {
        am_util_stdio_printf("\nRead write data matched!\neMMC FatFs Test Pass\n");
    }
}

//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    //
    // Configure SDIO PINs.
    //
    am_bsp_sdio_pins_enable(AM_HAL_HOST_BUS_WIDTH_8);

#ifdef FMC_BOARD_EMMC_TEST
    //
    // FPGA level shift control
    //
    am_hal_gpio_pinconfig(76, am_hal_gpio_pincfg_output);
    am_hal_gpio_output_clear(76);
#endif

    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    //
    // Enable printing to the console.
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_printf("\nApollo4 eMMC FatFs example\n");

    res = f_mount(&eMMCFatFs, (TCHAR const*)eMMCPath, 0);
    if ( res == FR_OK )
    {
        am_util_stdio_printf("\nFatFs is initialized\n");

        res = f_mkfs((TCHAR const*)eMMCPath, 0, work_buf, sizeof(work_buf));
        if ( res == FR_OK )
        {
            am_util_stdio_printf("\nVolume is created on the logical drive\n");
            res = f_open(&TestFile, "FatFs.TXT", FA_CREATE_ALWAYS | FA_WRITE);
            if ( res == FR_OK )
            {
                am_util_stdio_printf("File FatFs.TXT is opened for write\n");
                res = f_write(&TestFile, write_buf, sizeof(write_buf), (void *)&write_cnt);
                if ( (res != FR_OK) || (write_cnt == 0) )
                {
                    am_util_stdio_printf("File Write Error!\n");
                }
                else
                {
                    res = f_close(&TestFile); //close the file to flush data into device
                    if ( res == FR_OK )
                    {
                        am_util_stdio_printf("File is closed\n");
                    }
                    else
                    {
                        am_util_stdio_printf("Fail to close file\n");
                    }

                    res = f_open(&TestFile, "FatFs.TXT", FA_READ);
                    if ( res == FR_OK )
                    {
                        am_util_stdio_printf("File FatFs.TXT is opened for read\n");
                        res = f_read(&TestFile, read_buf, sizeof(read_buf), (UINT*)&read_cnt);
                        if ( (res != FR_OK) || (read_cnt == 0) )
                        {
                            am_util_stdio_printf("File Read Error!\n");
                        }
                        else
                        {
                            res = f_close(&TestFile);
                            if ( res != FR_OK )
                            {
                                am_util_stdio_printf("Fail to close file\n");
                            }

                            //
                            // Check whether read data match with write data.
                            //
                            check_if_rwbuf_match(read_buf, write_buf, sizeof(write_buf));
                        }
                    }
                    else
                    {
                        am_util_stdio_printf("Fail to open file for read\n");
                    }
                }
            }
            else
            {
                am_util_stdio_printf("Fail to open file for write\n");
            }
        }
        else
        {
            am_util_stdio_printf("FatFs Format Fail\n");
        }
    }
    else
    {
        am_util_stdio_printf("FatFs Initialization Fail\n");
    }

    //
    // End banner.
    //
    am_util_stdio_printf("\nApollo4 eMMC FatFs example complete\n");

    //
    // Sleep forever
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
//      am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}

