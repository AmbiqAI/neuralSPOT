//*****************************************************************************
//
//! @file mspi_ddr_octal_psram_example.c
//!
//! @brief Example of the MSPI operation with DDR OCTAL SPI PSRAM.
//!
//! Purpose: This example demonstrates MSPI DDR OCTAL operation using the PSRAM
//! device.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_devices_mspi_psram_aps12808l.h"
#include "am_util.h"
#define ENABLE_XIPMM
#define MSPI_INT_TIMEOUT        (100)

#define DDR_PSRAM_TARGET_ADDRESS 0
#define MSPI_BUFFER_SIZE        (4*1024)  // 4K example buffer size.

#define DEFAULT_TIMEOUT         10000

uint32_t        DMATCBBuffer[2560];
uint8_t         TestBuffer[2048];
uint8_t         DummyBuffer[1024];
uint8_t         g_TXBuffer[MSPI_BUFFER_SIZE];
uint8_t         g_RXBuffer[MSPI_BUFFER_SIZE];
void            *g_pDevHandle;
void            *g_pHandle;

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE1MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1,
    .eClockFreq               = AM_HAL_MSPI_CLK_48MHZ,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};


#define MSPI_TEST_MODULE              0

#if (MSPI_TEST_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS 0x14000000
#elif (MSPI_TEST_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS 0x18000000
#elif (MSPI_TEST_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS 0x1C000000
#endif // #if (MSPI_TEST_MODULE == 0)

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
    MSPI1_IRQn,
    MSPI2_IRQn,
};

//
// Take over the interrupt handler for whichever MSPI we're using.
//
#define psram_mspi_isr                                                          \
    am_mspi_isr1(MSPI_TEST_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

//*****************************************************************************
//
// MSPI ISRs.
//
//*****************************************************************************
void psram_mspi_isr(void)
{
    uint32_t      ui32Status;

    am_hal_mspi_interrupt_status_get(g_pHandle, &ui32Status, false);

    am_hal_mspi_interrupt_clear(g_pHandle, ui32Status);

    am_hal_mspi_interrupt_service(g_pHandle, ui32Status);
}

//*****************************************************************************
//
// Static function to be executed from external flash device
//
//*****************************************************************************
#if defined(__GNUC_STDC_INLINE__)
__attribute__((naked))
static void xip_test_function(void)
{
    __asm
    (
        "   nop\n"              // Just execute NOPs and return.
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   nop\n"
        "   bx      lr\n"
    );
}

#elif defined(__ARMCC_VERSION)
__asm static void xip_test_function(void)
{
    nop                         // Just execute NOPs and return.
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    bx      lr
}

#elif defined(__IAR_SYSTEMS_ICC__)
__stackless static void xip_test_function(void)
{
    __asm("    nop");           // Just execute NOPs and return.
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    nop");
    __asm("    bx      lr");
}
#endif

#define MSPI_XIP_FUNCTION_SIZE  72
typedef void (*mspi_xip_test_function_t)(void);

#ifdef ENABLE_XIPMM
//*****************************************************************************
//
// XIPMM check
//
//*****************************************************************************
#define MSPI_XIPMM_BASE_ADDRESS MSPI_XIP_BASE_ADDRESS

bool
run_mspi_xipmm(uint32_t block, bool bUseWordAccesses)
{
        uint32_t ix;

        // Use word accesses if scrambled.
        uint32_t *pAddr1 = (uint32_t *)(MSPI_XIPMM_BASE_ADDRESS);
        uint32_t *pAddr2 = (uint32_t *)(MSPI_XIPMM_BASE_ADDRESS  + 512);

        // Initialize a pattern
        for (ix = 0; ix < 512 / 4; ix++)
        {
            *pAddr1++ = ix;
            *pAddr2++ = ix ^ 0xFFFFFFFF;
        }
        pAddr1 = (uint32_t *)(MSPI_XIPMM_BASE_ADDRESS);
        pAddr2 = (uint32_t *)(MSPI_XIPMM_BASE_ADDRESS + 512);

        // Verify the pattern
        for (ix = 0; ix < 512 / 4; ix++)
        {
            if ( (*pAddr1++ != ix) || (*pAddr2++ != (ix ^ 0xFFFFFFFF)) )
            {
                return false;
            }
        }
    return true;
}
#endif

am_devices_mspi_psram_ddr_timing_config_t MSPIDdrTimingConfig;

//*****************************************************************************
//
// MSPI Example Main.
//
//*****************************************************************************
int
main(void)
{
    uint32_t      ui32Status;
    uint32_t      funcAddr = ((uint32_t)&xip_test_function) & 0xFFFFFFFE;
    bool bDoScrambling = false;

    //
    // Cast a pointer to the begining of the sector as the test function to call.
    //
    mspi_xip_test_function_t test_function = (mspi_xip_test_function_t)((MSPI_XIP_BASE_ADDRESS) | 0x00000001);

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
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Apollo4 Octal MSPI DDR PSRAM Example\n\n");

    am_util_debug_printf("Starting MSPI DDR Timing Scan: \n");
    if ( AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS == am_devices_mspi_psram_aps12808l_ddr_init_timing_check(MSPI_TEST_MODULE, &MSPI_PSRAM_OctalCE0MSPIConfig, &MSPIDdrTimingConfig) )
    {
        am_util_debug_printf("==== Scan Result: RXDQSDELAY0 = %d \n", MSPIDdrTimingConfig.ui32Rxdqsdelay);
    }
    else
    {
        am_util_debug_printf("==== Scan Result: Failed, no valid setting.  \n");
    }

    //
    // Configure the MSPI and PSRAM Device.
    //
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_init(MSPI_TEST_MODULE, &MSPI_PSRAM_OctalCE0MSPIConfig, &g_pDevHandle, &g_pHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
    }
    NVIC_SetPriority(mspi_interrupts[MSPI_TEST_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    am_hal_interrupt_master_enable();

    //
    //  Set the DDR timing from previous scan.
    //
    // TODO: This needs to check for error in Scan and return and check an error here!!!
    am_devices_mspi_psram_aps12808l_apply_ddr_timing(g_pDevHandle, &MSPIDdrTimingConfig);

    //
    // Generate data into the Sector Buffer
    //
    for (uint32_t i = 0; i < MSPI_BUFFER_SIZE; i++)
    {
        g_TXBuffer[i] = (i & 0xFF);
    }

    //
    // Make sure we aren't in XIP mode.
    //
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_disable_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to disable XIP mode in the MSPI!\n");
    }

    //
    // Write the TX buffer into the target sector.
    //
    am_util_stdio_printf("Writing %d Bytes to Address 0x%X\n", MSPI_BUFFER_SIZE, DDR_PSRAM_TARGET_ADDRESS);
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_write(g_pDevHandle, g_TXBuffer, DDR_PSRAM_TARGET_ADDRESS, MSPI_BUFFER_SIZE, true);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to write buffer to Flash Device!\n");
    }

    //
    // Read the data back into the RX buffer.
    //
    am_util_stdio_printf("Read %d Bytes to Address 0x%X\n", MSPI_BUFFER_SIZE, DDR_PSRAM_TARGET_ADDRESS);
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_read(g_pDevHandle, g_RXBuffer, DDR_PSRAM_TARGET_ADDRESS, MSPI_BUFFER_SIZE, true);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to read buffer to Flash Device!\n");
    }

    //
    // Compare the buffers
    //
    am_util_stdio_printf("Comparing the TX and RX Buffers\n");
    for (uint32_t i = 0; i < MSPI_BUFFER_SIZE; i++)
    {
        if (g_RXBuffer[i] != g_TXBuffer[i])
        {
            am_util_stdio_printf("TX and RX buffers failed to compare!\n");
            am_util_stdio_printf("g_TXBuffer[%d] = 0x%X\n", i, g_TXBuffer[i]);
            am_util_stdio_printf("g_RXBuffer[%d] = 0x%X\n", i, g_RXBuffer[i]);
            break;
        }
    }

    //
    // Enable XIP mode.
    //
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to enable XIP mode in the MSPI!\n");
    }

    //
    // Verify the content using XIP aperture
    //
    uint8_t    *pui8Address = (uint8_t *)(MSPI_XIP_BASE_ADDRESS + DDR_PSRAM_TARGET_ADDRESS);
    for (uint32_t i = 0; i < MSPI_BUFFER_SIZE; i++)
    {
        uint8_t val = *pui8Address;
        if (val != g_TXBuffer[i])
        {
            am_util_stdio_printf("TX and XIP failed to compare!\n");
            break;
        }
        pui8Address++;
    }

    if ( bDoScrambling )
    {
        //
        // Turn on scrambling operation.
        //
        am_util_stdio_printf("Putting the MSPI into Scrambling mode\n");
        ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_scrambling(g_pDevHandle);
        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            am_util_stdio_printf("Failed to enable MSPI scrambling!\n");
        }
    }

    //
    // Write the executable function into the target sector.
    //
    am_util_stdio_printf("Writing Executable function of %d Bytes to Sector %d\n", MSPI_XIP_FUNCTION_SIZE, 0);
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_write(g_pDevHandle, (uint8_t *)funcAddr, 0, MSPI_XIP_FUNCTION_SIZE, true);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to write executable function to Flash Device!\n");
    }

    //
    // Set up for XIP operation.
    //
    am_util_stdio_printf("Putting the MSPI and External PSRAM into XIP mode\n");
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to put the MSPI into XIP mode!\n");
    }

    if ( bDoScrambling )
    {
        //
        // Turn on scrambling operation.
        //
        am_util_stdio_printf("Putting the MSPI into Scrambling mode\n");
        ui32Status = am_devices_mspi_psram_aps12808l_ddr_enable_scrambling(g_pDevHandle);
        if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
        {
            am_util_stdio_printf("Failed to enable MSPI scrambling!\n");
        }
    }

    //
    // Execute a call to the test function in the sector.
    //
    am_util_stdio_printf("Jumping to function in External Flash\n");
    test_function();
    am_util_stdio_printf("Returned from XIP call\n");

    //
    // Shutdown XIP operation.
    //
#ifndef ENABLE_XIPMM
    am_util_stdio_printf("Disabling the MSPI and External Flash from XIP mode\n");
    ui32Status = am_devices_mspi_psram_aps12808l_ddr_disable_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to disable XIP mode in the MSPI!\n");
    }
#endif

#ifdef ENABLE_XIPMM
    //
    // If scrambling on, force word accesses in XIPMM.
    //
    if ( run_mspi_xipmm(0, bDoScrambling) )
    {
        am_util_stdio_printf("XIPMM aperature is working!\n");
    }
    else
    {
        am_util_stdio_printf("XIPMM aperature is NOT working!\n");
    }
#endif
    //
    // Clean up the MSPI before exit.
    //
    am_hal_interrupt_master_disable();
    NVIC_DisableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);

    ui32Status = am_devices_mspi_psram_aps12808l_ddr_deinit(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to shutdown the MSPI and Flash Device!\n");
    }

    //
    //  End banner.
    //
    am_util_stdio_printf("\nApollo4 Octal MSPI DDR PSRAM Example Complete\n");

    //
    // Loop forever while sleeping.
    //
    while (1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}

