//*****************************************************************************
//
//! @file mram_program.c
//!
//! @brief MRAM programming example.
//!
//! Purpose: This example shows how to modify the internal MRAM using HAL
//! MRAM helper functions.
//!
//! This example works on the portion of the MRAM at the 1MB boundary.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//! Arbitrary section address in MRAM
#define ARB_ADDRESS AM_HAL_MRAM_INSTANCE_SIZE

static uint32_t ui32Source[512];

static uint32_t ui32Info0[5] = {0x5B75A5FA, 0x7B9C8674, 0x869A96FE, 0xAEC90860,  // INFO0_SIGNATURE0-3
                                0x00055FF }; // INFO0_SECURITY
uint32_t ui32Info0ReadBack[5];

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    int32_t i32ReturnCode;
    int32_t i32ErrorFlag = 0;
    uint32_t *pui32Src;
    uint32_t *pui32Dst;
    int32_t ix;
    uint32_t ui32PrgmAddr;
    uint32_t ui32Info0Addr;

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Initialize the peripherals for this board.
    //
    am_bsp_low_power_init();

    //
    // Enable printing through the ITM interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("MRAM Programming Example\n");

    //
    // Select an arbitrary section address in MRAM at 1MB boundary.
    //
    ui32PrgmAddr = ARB_ADDRESS;

    //
    // Setup a pattern to write to the MRAM.
    //
    am_util_stdio_printf("  ... programming at 1M boundary of MRAM\n");

    pui32Src = ui32Source;
    for (ix = 0x100; ix < (0x100 + (512 * 4)); ix += 4)
    {
        *pui32Src++ = ix;
    }

    //
    // Program a few words in a section in the main block of instance 1.
    //
    pui32Dst = (uint32_t *) ui32PrgmAddr;
    i32ReturnCode = am_hal_mram_main_program(AM_HAL_MRAM_PROGRAM_KEY,
                                              ui32Source,
                                              pui32Dst,
                                              512);

    //
    // Check for an error from the HAL.
    //
    if (i32ReturnCode)
    {
        am_util_stdio_printf("MRAM program section at 0x%08x "
                             "i32ReturnCode = 0x%x.\n",
                             ui32PrgmAddr,
                             i32ReturnCode);
        i32ErrorFlag++;
    }

    //
    // Check the section just programmed.
    //
    am_util_stdio_printf("  ... verifying the section just programmed.\n");
    for ( ix = 0; ix < 512; ix++ )
    {
        if ( *(uint32_t*)(ui32PrgmAddr + (ix*4)) != ui32Source[ix] )
        {
            am_util_stdio_printf("ERROR: MRAM address 0x%08x did not program properly:\n"
                                 "  Expected value = 0x%08x, programmed value = 0x%08x.\n",
                                 ui32PrgmAddr + (ix * 4),
                                 ui32Source[ix],
                                 *(uint32_t*)(ui32PrgmAddr + (ix * 4)) );
        }
    }

    //
    // Fill the section just programmed.
    //
    am_util_stdio_printf("  ... filling the section just programmed.\n");
    i32ReturnCode = am_hal_mram_main_fill(AM_HAL_MRAM_PROGRAM_KEY,
                                              0x00000000,
                                              pui32Dst,
                                              512);

    //
    // Check for an error from the HAL.
    //
    if (i32ReturnCode)
    {
        am_util_stdio_printf(" MRAM filled section at 0x%08x "
                             "i32ReturnCode =  0x%x.\n",
                             ARB_ADDRESS, i32ReturnCode);
        i32ErrorFlag++;
    }

    //
    // Check that the entire section is filled.
    //
    am_util_stdio_printf("  ... verifying the section just filled.\n");
    for ( ix = 0; ix < 512; ix++ )
    {
        if ( *(uint32_t*)(ui32PrgmAddr + (ix*4)) != 0x00000000 )
        {
            am_util_stdio_printf("ERROR: MRAM address 0x%08x did not ERASE properly:\n"
                                 "  Expected value = 0x00000000, programmed value = 0x%08x.\n",
                                 ui32PrgmAddr + (ix*4),
                                 *(uint32_t*)(ui32PrgmAddr + (ix * 4)) );
        }
    }

    //
    // Program a few words in a INFO0.
    //
    am_util_stdio_printf("  ... programming INFO0\n");
    pui32Src = ui32Info0;
    ui32Info0Addr = 0x42000000;
    i32ReturnCode = am_hal_mram_info_program(AM_HAL_MRAM_INFO_KEY,
                                              pui32Src,
                                              0,
                                              (sizeof(ui32Info0) / sizeof(uint32_t)));

    //
    // Check for an error from the HAL.
    //
    if (i32ReturnCode)
    {
        am_util_stdio_printf("MRAM program Info0"
                             "i32ReturnCode = 0x%x.\n",
                             i32ReturnCode);
        i32ErrorFlag++;
    }

    //
    // Check INFO0 just programmed.
    //
    am_util_stdio_printf("  ... verifying the INFO0 just programmed.\n");
#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4L)
    if (0 != am_hal_mram_info_read(0, 0, 5, &ui32Info0ReadBack[0]))
    {
      am_util_stdio_printf("ERROR: INFO0 Read Back failed\n");
    }
#endif
    for ( ix = 0; ix < (sizeof(ui32Info0) / sizeof(uint32_t)); ix++ )
    {
#if defined(AM_PART_APOLLO4)
        if ( *(uint32_t*)(ui32Info0Addr + (ix * 4)) != ui32Info0[ix] )
        {
            am_util_stdio_printf("ERROR: INFO0 address 0x%08x did not program properly:\n"
                                 "  Expected value = 0x%08x, programmed value = 0x%08x.\n",
                                 ui32Info0Addr + (ix * 4),
                                 ui32Info0[ix],
                                 *(uint32_t*)(ui32Info0Addr + (ix * 4)) );
            break;
        }
#elif defined (AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4L)
        if ( ui32Info0ReadBack[ix] != ui32Info0[ix] )
        {
            am_util_stdio_printf("ERROR: INFO0 address 0x%08x did not program properly:\n"
                                 "  Expected value = 0x%08x, programmed value = 0x%08x.\n",
                                 ui32Info0Addr + (ix * 4),
                                 ui32Info0[ix],
                                 *(uint32_t*)(ui32Info0Addr + (ix * 4)) );
            break;
        }
#endif
    }

    //
    // Report success or any failures and exit.
    //
    if (i32ErrorFlag)
    {
        am_util_stdio_printf("ERROR: MRAM Program Write example failure %d\n",
                             i32ErrorFlag);
    }
    else
    {
        am_util_stdio_printf("MRAM Program example successfully \n");
    }

    //
    // Verify that ITM is done printing
    //
    am_hal_itm_not_busy();

    //
    // Provide return code back to the system.
    //
    return i32ErrorFlag;
}
