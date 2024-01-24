//*****************************************************************************
//
//! @file ios_lram.c
//!
//! @brief Example slave used for demonstrating the use of the IOS lram.
//!
//! Purpose: This slave component runs on one EVB and is used in conjunction with
//! the companion host example, ios_lram_host, which runs on a second EVB.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! In order to run this example, a host device (e.g. a second EVB) must be set
//! up to run the host example, ios_lram_host.  The two boards can be connected
//! using fly leads between the two boards as follows.
//!
//! @verbatim
//! Pin connections for the I/O Master board to the I/O Slave board.
//! SPI:
//!     HOST (ios_lram_host)                    SLAVE (ios_lram)
//!     --------------------                    ----------------
//!     GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
//!     GPIO[5]  IOM0 SPI SCK                   GPIO[0]  IOS SPI SCK
//!     GPIO[7]  IOM0 SPI MOSI                  GPIO[1]  IOS SPI MOSI
//!     GPIO[6]  IOM0 SPI MISO                  GPIO[2]  IOS SPI MISO
//!     GPIO[11] IOM0 SPI nCE                   GPIO[3]  IOS SPI nCE
//!     GND                                     GND
//!
//! I2C:
//!     HOST (ios_lram_host)                    SLAVE (ios_lram)
//!     --------------------                    ----------------
//!     GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
//!     GPIO[5]  IOM0 I2C SCL                   GPIO[0]  IOS I2C SCL
//!     GPIO[6]  IOM0 I2C SDA                   GPIO[1]  IOS I2C SDA
//!     GND                                     GND
//! @endverbatim
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

#define USE_SPI             1   // 0 = I2C, 1 = SPI
#define I2C_ADDR            0x10

#define HANDSHAKE_IOS_PIN   4

#define TEST_IOS_XCMP_INT   1

#define AM_HAL_IOS_INT_ERR  (AM_HAL_IOS_INT_FOVFL | AM_HAL_IOS_INT_FUNDFL | AM_HAL_IOS_INT_FRDERR)

#define AM_HAL_IOS_XCMP_INT (AM_HAL_IOS_INT_XCMPWR | AM_HAL_IOS_INT_XCMPWF | AM_HAL_IOS_INT_XCMPRR | AM_HAL_IOS_INT_XCMPRF)

#define AM_IOSTEST_CMD_SEND_DATA    0xFC
#define AM_IOSTEST_CMD_START_DATA   0xFD
#define AM_IOSTEST_CMD_ACK_DATA     0xFE
#define AM_IOSTEST_CMD_STOP_DATA    0xFF

// IOINT bits for handshake
#define HANDSHAKE_IOS_TO_IOM        2

#define INBUFFER_EMPTY              (0xEE)
#define TEST_XOR_BYTE               0xFF // 0 Will not change, 0xFF will invert
#define ROBUFFER_INIT               (0x55)

typedef struct
{
    uint8_t ui8Tag;
    uint8_t ui8Length;
}sHeader;

typedef enum
{
    IOM_DATA_TAG,
    IOM_DATA_LENGTH,
    IOM_DATA_VALUE
}eIOM_Data;

static void *g_pIOSHandle;
volatile bool bIomSendComplete = false;
//*****************************************************************************
//
// Message buffers.
//
// data from the IOS interface, which is only 8 bits wide.
//
//*****************************************************************************
#define AM_IOS_LRAM_SIZE_MAX    0x78
uint8_t g_pIosSendBuf[AM_IOS_LRAM_SIZE_MAX];
#define AM_IOS_HEADER_SIZE          sizeof(sHeader)
#define AM_IOS_MAX_DATA_SIZE        (AM_IOS_LRAM_SIZE_MAX - AM_IOS_HEADER_SIZE)

//*****************************************************************************
//
// GPIO Configuration
//
//*****************************************************************************
const am_hal_gpio_pincfg_t g_AM_BSP_GPIO_ENABLE =
{
    .uFuncSel            = AM_HAL_PIN_4_SLINT,
    .eDriveStrength      = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eCEpol              = AM_HAL_GPIO_PIN_CEPOL_ACTIVEHIGH,
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN,
};

//*****************************************************************************
//
// SPI Slave Configuration
//
//*****************************************************************************
static am_hal_ios_config_t g_sIOSSpiConfig =
{
    // Configure the IOS in SPI mode.
    .ui32InterfaceSelect = AM_HAL_IOS_USE_SPI,

    // Eliminate the "read-only" section, so an external host can use the
    // entire "direct write" section.
    .ui32ROBase = 0x78,

    // Making the "FIFO" section as big as possible.
    .ui32FIFOBase = 0x80,

    // We don't need any RAM space, so extend the FIFO all the way to the end
    // of the LRAM.
    .ui32RAMBase = 0x100,

    // FIFO Threshold - set to half the size
    .ui32FIFOThreshold = 0x20,
};

//*****************************************************************************
//
// I2C Slave Configuration
//
//*****************************************************************************
am_hal_ios_config_t g_sIOSI2cConfig =
{
    // Configure the IOS in I2C mode.
    .ui32InterfaceSelect = AM_HAL_IOS_USE_I2C | AM_HAL_IOS_I2C_ADDRESS(I2C_ADDR << 1),

    // Eliminate the "read-only" section, so an external host can use the
    // entire "direct write" section.
    .ui32ROBase = 0x78,

    // Set the FIFO base to the maximum value, making the "direct write"
    // section as big as possible.
    .ui32FIFOBase = 0x80,

    // We don't need any RAM space, so extend the FIFO all the way to the end
    // of the LRAM.
    .ui32RAMBase = 0x100,
    // FIFO Threshold - set to half the size
    .ui32FIFOThreshold = 0x40,
};

//*****************************************************************************
//
// Configure the SPI slave.
//
//*****************************************************************************
static void ios_set_up(bool bSpi)
{
    if (bSpi)
    {
        // Configure SPI interface
        am_bsp_ios_pins_enable(0, AM_HAL_IOS_USE_SPI);
        //
        // Configure the IOS interface and LRAM structure.
        //
        am_hal_ios_initialize(0, &g_pIOSHandle);
        am_hal_ios_power_ctrl(g_pIOSHandle, AM_HAL_SYSCTRL_WAKE, false);
        am_hal_ios_configure(g_pIOSHandle, &g_sIOSSpiConfig);
    }
    else
    {
        // Configure I2C interface
        am_bsp_ios_pins_enable(0, AM_HAL_IOS_USE_I2C);
        //
        // Configure the IOS interface and LRAM structure.
        //
        am_hal_ios_initialize(0, &g_pIOSHandle);
        am_hal_ios_power_ctrl(g_pIOSHandle, AM_HAL_SYSCTRL_WAKE, false);
        am_hal_ios_configure(g_pIOSHandle, &g_sIOSI2cConfig);
    }

    //
    // Clear out any IOS register-access interrupts that may be active, and
    // enable interrupts for the registers we're interested in.
    //
    am_hal_ios_interrupt_clear(g_pIOSHandle, AM_HAL_IOS_INT_ALL);
    am_hal_ios_interrupt_enable(g_pIOSHandle, AM_HAL_IOS_INT_ERR | AM_HAL_IOS_INT_FSIZE);
#ifdef TEST_IOINTCTL
    am_hal_ios_interrupt_enable(g_pIOSHandle, AM_HAL_IOS_INT_IOINTW);
#endif
#ifdef TEST_IOS_XCMP_INT
    am_hal_ios_interrupt_enable(g_pIOSHandle, AM_HAL_IOS_XCMP_INT);
#endif

    //
    // Set the bit in the NVIC to accept access interrupts from the IO Slave.
    //
    NVIC_EnableIRQ(IOSLAVE_IRQn);

    // Set up the IOSINT interrupt pin
    am_hal_gpio_pinconfig(HANDSHAKE_IOS_PIN, g_AM_BSP_GPIO_ENABLE);

    // Initialize RO & AHB-RAM data with pattern
    for (uint8_t i = 0; i < 8; i++)
    {
        am_hal_ios_pui8LRAM[0x78 + i] = ROBUFFER_INIT;
    }
}


// Inform host of new data available to read
void inform_host(void)
{
    uint32_t ui32Arg = HANDSHAKE_IOS_TO_IOM;
    // Update FIFOCTR for host to read
    am_hal_ios_control(g_pIOSHandle, AM_HAL_IOS_REQ_FIFO_UPDATE_CTR, NULL);
    // Interrupt the host
    am_hal_ios_control(g_pIOSHandle, AM_HAL_IOS_REQ_HOST_INTSET, &ui32Arg);
}

// Receive data from host and prepare the loop back data
static void ios_read(uint32_t offset, uint32_t size)
{
    uint32_t i;
    uint8_t readByte;
    // Read only supported from LRAM
    for (i = 0; i + IOM_DATA_VALUE < size; i++)
    {
        readByte = am_hal_ios_pui8LRAM[offset + IOM_DATA_VALUE + i];
        // Read data and prepare to be sent back after processing
        g_pIosSendBuf[IOM_DATA_VALUE + i] = readByte ^ TEST_XOR_BYTE;
    }
}

// Send data to host (IOM)
static void ios_send(uint32_t address, uint32_t size)
{
    // Send data using the LRAM
    while (size--)
    {
        am_hal_ios_pui8LRAM[address + size] = g_pIosSendBuf[size];
    }
}

//*****************************************************************************
//
// IO Slave Main ISR.
//
//*****************************************************************************
void am_ioslave_ios_isr(void)
{
    uint32_t ui32Status;

    //
    // Check to see what caused this interrupt, then clear the bit from the
    // interrupt register.
    //

    am_hal_ios_interrupt_status_get(g_pIOSHandle, false, &ui32Status);

    am_hal_ios_interrupt_clear(g_pIOSHandle, ui32Status);

    if (ui32Status & AM_HAL_IOS_INT_FUNDFL)
    {
        am_util_stdio_printf("Hitting underflow for the requested IOS FIFO transfer\n");
        // We should never hit this case unless the threshold has beeen set
        // incorrect, or we are unable to handle the data rate
        // ERROR!
        am_hal_debug_assert_msg(0,
            "Hitting underflow for the requested IOS FIFO transfer.");
    }

    if (ui32Status & AM_HAL_IOS_INT_ERR)
    {
        // We should never hit this case
        // ERROR!
        am_hal_debug_assert_msg(0,
            "Hitting ERROR case.");
    }

    if (ui32Status & AM_HAL_IOS_INT_FSIZE)
    {
        //
        // Service the I2C slave FIFO if necessary.
        //
        am_hal_ios_interrupt_service(g_pIOSHandle, ui32Status);
    }

    if (ui32Status & AM_HAL_IOS_INT_XCMPWR)
    {
        bIomSendComplete = true;
    }
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int main(void)
{
    uint8_t  *pui8Packet;
    uint8_t ui8Tag = 1;
    uint8_t ui8Length = 0;

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
    // Enable the ITM print interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("IOS LRAM Example\n");

    //
    // Enable the IOS. Choose the correct protocol based on USE_SPI
    //
    ios_set_up(USE_SPI);

    //
    // Enable interrupts so we can receive messages from the boot host.
    //
    am_hal_interrupt_master_enable();

    //
    // Loop forever.
    //
    while(1)
    {
        uint32_t ui32IntStatus = am_hal_interrupt_master_disable();
        if (bIomSendComplete)
        {
            // Enable the interrupts
            am_hal_interrupt_master_set(ui32IntStatus);
            bIomSendComplete = false;
            pui8Packet = (uint8_t *) am_hal_ios_pui8LRAM;
            if (AM_IOSTEST_CMD_START_DATA == pui8Packet[IOM_DATA_TAG])
            {
                // Host wants to start data transmit
                // Respond the ACK
                *((uint8_t *) am_hal_ios_pui8LRAM) = AM_IOSTEST_CMD_ACK_DATA;
                inform_host();
            }
            else if (AM_IOSTEST_CMD_STOP_DATA == pui8Packet[IOM_DATA_TAG])
            {
                break;
            }
            else if (ui8Tag == pui8Packet[IOM_DATA_TAG])
            {
                ui8Length = pui8Packet[IOM_DATA_LENGTH];
                // Init buffer
                g_pIosSendBuf[IOM_DATA_TAG] = ui8Tag;
                g_pIosSendBuf[IOM_DATA_LENGTH] = ui8Length;
                for (uint8_t i = IOM_DATA_VALUE; i < AM_IOS_LRAM_SIZE_MAX; i++)
                {
                    g_pIosSendBuf[i] = INBUFFER_EMPTY;
                }
                // Read data sent from IOM
                ios_read(0, ui8Length);
                // Reply data to IOM
                ios_send(0, ui8Length);
                ui8Tag++;
                // Notify the host
                inform_host();
            }
        }
        else
        {
            //
            // Go to Deep Sleep.
            //
            am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_NORMAL);
            // Enable the interrupts
            am_hal_interrupt_master_set(ui32IntStatus);
        }
    }

    while(1);
}


