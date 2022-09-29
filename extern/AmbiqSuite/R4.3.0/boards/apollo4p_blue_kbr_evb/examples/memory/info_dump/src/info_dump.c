//*****************************************************************************
//
//! @file info_dump.c
//!
//! @brief A simple program to dump the INFOspace to SWO
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

uint32_t info1Start = AM_HAL_INFO1_VISIBLE_OFFSET;
uint32_t info1Size = AM_HAL_INFO1_SIZE_BYTES - AM_HAL_INFO1_VISIBLE_OFFSET;

uint32_t gInfo0[AM_HAL_INFO0_SIZE_BYTES];
uint32_t gInfo1[AM_HAL_INFO1_SIZE_BYTES];

void
mem_dump(uint32_t startOffset, uint32_t *pBuf, uint32_t size)
{
    for ( uint32_t i = 0; i < size / 16; i++ )
    {
        am_util_stdio_printf("0x%08X: ", startOffset + i * 16);
        for (uint32_t j = 0; j < 4; j++)
        {
            am_util_stdio_printf("0x%08X ", pBuf[i * 4 + j]);
        }
        am_util_stdio_printf("\n");
    }
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

#if defined AM_PART_APOLLO4B || AM_PART_APOLLO4P
    uint32_t ui32dcuVal;

    if ((PWRCTRL->DEVPWRSTATUS_b.PWRSTCRYPTO == 1) && (CRYPTO->HOSTCCISIDLE_b.HOSTCCISIDLE == 1))
    {
        am_hal_dcu_get(&ui32dcuVal);
        // Enable SWO
        if ( !(ui32dcuVal & AM_HAL_DCU_CPUTRC_TPIU_SWO) &&
             (am_hal_dcu_update(true, AM_HAL_DCU_CPUTRC_TPIU_SWO) != AM_HAL_STATUS_SUCCESS) )
        {
            // Cannot enable SWO
            while(1);
        }
    }
#endif

    //
    // Initialize the printf interface for ITM output
    //
    am_bsp_debug_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Apollo4 Info Dump!\n\n");

#if defined AM_PART_APOLLO4B || AM_PART_APOLLO4P
    am_hal_mram_info_read(0, 0, AM_HAL_INFO0_SIZE_BYTES / 4, gInfo0);
    am_util_stdio_printf("INFO0\n");
    am_util_stdio_printf("=====\n");
    mem_dump(0, gInfo0, AM_HAL_INFO0_SIZE_BYTES);

    am_hal_mram_info_read(1, info1Start / 4, info1Size / 4, &gInfo1[info1Start]);

    am_util_stdio_printf("\nINFO1\n");
    am_util_stdio_printf("=====\n");
    mem_dump(info1Start, &gInfo1[info1Start], info1Size);

    am_util_stdio_printf("\nEnd of dump\n");
#else
    am_util_stdio_printf("This program is not valid for this chip.\n\n");
#endif

    //
    // We are done printing.
    // Disable debug printf messages on ITM.
    //
    am_bsp_debug_printf_disable();

    //
    // Loop forever while sleeping.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_NORMAL);
    }
}
