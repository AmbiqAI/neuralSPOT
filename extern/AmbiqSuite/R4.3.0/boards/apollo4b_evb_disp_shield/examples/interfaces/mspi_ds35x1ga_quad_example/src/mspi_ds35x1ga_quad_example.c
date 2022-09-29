//*****************************************************************************
//
//! @file mspi_ds35x1ga_quad_example.c
//!
//! @brief Example of the MSPI operation with Quad SPI NAND Flash.
//!
//! Purpose: This example configures an MSPI connected NAND flash device in Quad mode
//! and performs various operations - verifying the correctness of the same
//! Operations include - Erase, Write, Read.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! this example can work on:
//! Apollo4l_eb
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

#include <string.h>

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include "am_devices_mspi_ds35x1ga.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define MSPI_TEST_MODULE            2    // 0 ~ 2
#define MSPI_TEST_CLKFREQ           AM_HAL_MSPI_CLK_24MHZ
#define NAND_TEST_PAGE              0x100
#define NAND_TEST_FLASH_CFG_MODE    1    // 0:serial ; 1:quad

//*****************************************************************************
//
// Variables Definition
//
//*****************************************************************************
void *g_FlashHdl;
void *g_MSPIHdl;

static uint8_t *modeinfo[] = {"SERIAL 1-1-1", "QUAD 1-1-4"};

static uint8_t write_data[AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE];
static uint8_t read_data[AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE];
static uint8_t write_oob[AM_DEVICES_MSPI_DS35X1GA_PAGE_OOB_SIZE];
static uint8_t read_oob[AM_DEVICES_MSPI_DS35X1GA_PAGE_OOB_SIZE];



//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
    MSPI1_IRQn,
    MSPI2_IRQn,
};



//*****************************************************************************
//
// MSPI ISRs.
//
//*****************************************************************************

#define flash_mspi_isr                                                         \
    am_mspi_isr1(MSPI_TEST_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi##n##_isr

void flash_mspi_isr(void)
{
    uint32_t ui32Status;

    am_hal_mspi_interrupt_status_get(g_MSPIHdl, &ui32Status, false);
    am_hal_mspi_interrupt_clear(g_MSPIHdl, ui32Status);
    am_hal_mspi_interrupt_service(g_MSPIHdl, ui32Status);
}

uint32_t DMATCBBuffer[512];

const am_devices_mspi_ds35x1ga_config_t MSPI_Serial_Flash_Config =
{
    .eDeviceConfig = AM_HAL_MSPI_FLASH_SERIAL_CE0,
    .eClockFreq = MSPI_TEST_CLKFREQ,
    .pNBTxnBuf = DMATCBBuffer,
    .ui32NBTxnBufLength = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
    .ui32ScramblingStartAddr = 0,
    .ui32ScramblingEndAddr = 0,
};

const am_devices_mspi_ds35x1ga_config_t MSPI_Quad_Flash_Config =
{
    .eDeviceConfig = AM_HAL_MSPI_FLASH_QUAD_CE0_1_1_4,
    .eClockFreq = MSPI_TEST_CLKFREQ,
    .pNBTxnBuf = DMATCBBuffer,
    .ui32NBTxnBufLength = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
    .ui32ScramblingStartAddr = 0,
    .ui32ScramblingEndAddr = 0,
};

am_devices_mspi_ds35x1ga_config_t const * MSPI_Flash_Configs[2] =
    { &MSPI_Serial_Flash_Config, &MSPI_Quad_Flash_Config};

//*****************************************************************************
//
// Set up MSPI clock for the device.
//
//*****************************************************************************
const uint8_t* clkfreq_string(am_hal_mspi_clock_e clk)
{
    switch(clk)
    {
        case AM_HAL_MSPI_CLK_96MHZ:
            return "96";
        case AM_HAL_MSPI_CLK_48MHZ:
            return "48";
        case AM_HAL_MSPI_CLK_32MHZ:
            return "32";
        case AM_HAL_MSPI_CLK_24MHZ:
            return "24";
        case AM_HAL_MSPI_CLK_16MHZ:
            return "16";
        case AM_HAL_MSPI_CLK_12MHZ:
            return "12";
        case AM_HAL_MSPI_CLK_8MHZ:
            return "8";
        case AM_HAL_MSPI_CLK_6MHZ:
            return "6";
        case AM_HAL_MSPI_CLK_4MHZ:
            return "4";
        case AM_HAL_MSPI_CLK_3MHZ:
            return "3";
    }
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void)
{
#ifdef EXEC_TIME_DEBUG
    am_hal_gpio_pinconfig(19, g_AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_output_clear(19);
#endif

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
    // Initialize the printf interface for ITM output
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("MSPI NAND Flash Driver example @ %s MHz,  Mode: %s !\n\n",
        clkfreq_string(MSPI_Flash_Configs[NAND_TEST_FLASH_CFG_MODE]->eClockFreq),
        modeinfo[NAND_TEST_FLASH_CFG_MODE]);

    //
    // Print the device info.
    //
    am_util_id_t sIdDevice;
    am_util_id_device(&sIdDevice);
    am_util_stdio_printf("Vendor Name: %s\n", sIdDevice.pui8VendorName);
    am_util_stdio_printf("Device type: %s\n", sIdDevice.pui8DeviceName);

    uint32_t ui32Status;
    uint8_t ui8EccStatus;

    //
    // fill the test page with some data
    //
    for (uint32_t i = 0; i < AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE; i++)
    {
        write_data[i] = i;
    }

    //
    // fill the oob area with zero
    //
    memset(write_oob, 0x0, sizeof(write_oob));

    ui32Status = am_devices_mspi_ds35x1ga_init(MSPI_TEST_MODULE, MSPI_Flash_Configs[NAND_TEST_FLASH_CFG_MODE], &g_FlashHdl, &g_MSPIHdl);
    if (AM_DEVICES_MSPI_DS35X1GA_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and Flash Device correctly!\n");
    }
    else
    {
        am_util_stdio_printf("Configure the MSPI and Flash Device correctly!\n");
    }

    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);
    am_hal_interrupt_master_enable();

    uint32_t ui32DeviceID;
    am_devices_mspi_ds35x1ga_id(g_FlashHdl, &ui32DeviceID);
    am_util_stdio_printf("NAND flash ID is 0x%x!\n", ui32DeviceID);

#if NAND_TEST_FLASH_CFG_MODE == 1
    ui32Status = am_devices_mspi_ds35x1ga_block_erase(g_FlashHdl, NAND_TEST_PAGE >> 6);
    am_util_stdio_printf("block %d erase status 0x%x!\n", NAND_TEST_PAGE >> 6, ui32Status);

    ui32Status = am_devices_mspi_ds35x1ga_write(g_FlashHdl, NAND_TEST_PAGE, &write_data[0], AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE,
                                   &write_oob[0], AM_DEVICES_MSPI_DS35X1GA_PAGE_OOB_SIZE);
    am_util_stdio_printf("page %d write status 0x%x!\n", NAND_TEST_PAGE, ui32Status);
#endif

    memset(read_data, 0x0, AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE);
    ui32Status = am_devices_mspi_ds35x1ga_read(g_FlashHdl, NAND_TEST_PAGE, &read_data[0], AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE,
                                               &read_oob[0], 16, &ui8EccStatus);
    am_util_stdio_printf("page %d read status 0x%x!\n", NAND_TEST_PAGE, ui32Status);
    am_util_stdio_printf("page %d read ECC status 0x%x!\n", NAND_TEST_PAGE, ui8EccStatus);

    uint32_t i;
    for (i = 0; i < AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE; i++)
    {
        if (write_data[i] != read_data[i])
        {
            am_util_stdio_printf("%d : data_check wrong, expected: 0x%x, readback: 0x%x!\n", i, write_data[i], read_data[i]);
            break;
        }
    }

    if (i == AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE)
    {
        am_util_stdio_printf("succeed and read data equals to the write data\n");
    }
    else
    {
        am_util_stdio_printf("first data wrong is at index %d\n", i);
        for (i = 0;  i < AM_DEVICES_MSPI_DS35X1GA_PAGE_DATA_SIZE; i++)
        {
            am_util_stdio_printf("%d : write data: 0x%x, readback: 0x%x!\n", i, write_data[i], read_data[i]);
        }
    }

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
