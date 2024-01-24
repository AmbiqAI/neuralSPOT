//*****************************************************************************
//
//! @file ios_fifo.c
//!
//! @brief Example slave used for demonstrating the use of the IOS FIFO.
//!
//! Purpose: This slave component runs on one EVB and is used in conjunction with
//! the companion host example, ios_fifo_host, which runs on a second EVB.
//!
//! The ios_fifo example has no print output.
//! The host example does use the ITM SWO to let the user know progress and
//! status of the demonstration.
//!
//! This example implements the slave part of a protocol for data exchange with
//! an Apollo IO Master (IOM).  The host sends one byte commands on SPI/I2C by
//! writing to offset 0x80.
//!
//! The command is issued by the host to Start/Stop Data accumulation, and also
//! to acknowledge read-complete of a block of data.
//!
//! On the IOS side, once it is asked to start accumulating data (using START
//! command), two CTimer based events emulate sensors sending data to IOS.
//! When IOS has some data for host, it implements a state machine,
//! synchronizing with the host.
//!
//! The IOS interrupts the host to indicate data availability. The host then
//! reads the available data (as indicated by FIFOCTR) by READing using IOS FIFO
//! (at address 0x7F).  The IOS keeps accumulating any new data coming in the
//! background.
//!
//! Host sends an acknowledgment to IOS once it has finished reading a block
//! of data initiated by IOS (partially or complete). IOS interrupts the host
//! again if and when it has more data for the host to read, and the cycle
//! repeats - till host indicates that it is no longer interested in receiving
//! data by sending STOP command.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! In order to run this example, a host device (e.g. a second EVB) must be set
//! up to run the host example, ios_fifo_host.  The two boards can be connected
//! using fly leads between the two boards as follows.
//!
//! @verbatim
//! Pin connections for the I/O Master board to the I/O Slave board.
//! SPI:
//!     HOST (ios_fifo_host)                    SLAVE (ios_fifo)
//!     --------------------                    ----------------
//!     GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
//!     GPIO[5]  IOM0 SPI SCK                   GPIO[0]  IOS SPI SCK
//!     GPIO[7]  IOM0 SPI MOSI                  GPIO[1]  IOS SPI MOSI
//!     GPIO[6]  IOM0 SPI MISO                  GPIO[2]  IOS SPI MISO
//!     GPIO[11] IOM0 SPI nCE                   GPIO[3]  IOS SPI nCE
//!     GND                                     GND
//!
//! I2C:
//!     HOST (ios_fifo_host)                    SLAVE (ios_fifo)
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

#define     USE_SPI             1   // 0 = I2C, 1 = SPI
#define     I2C_ADDR            0x10

#define     TEST_IOS_XCMP_INT   1
#define     SENSOR0_DATA_SIZE           200
#define     SENSOR1_DATA_SIZE           500

// Sensor Frequencies as factor of 12KHz
// This test silently 'drops' the sensor data if the FIFO can not accomodate it
// Hence the host data validation test would still pass as long as all the data
// written to the FIFO made it to the host intact
// This allows us to configure these values to unrealistically high values for
// testing purpose
#if 1
#define     SENSOR0_FREQ   12 // 12 times a second
#define     SENSOR1_FREQ    7 // 7 times a second
#else // Just to test the throughput
#define     SENSOR0_FREQ   1000 // 1KHz
#define     SENSOR1_FREQ   1700 // 1.7 KHz
#endif

#define     XOR_BYTE            0
#define     AM_HAL_IOS_INT_ERR  (AM_HAL_IOS_INT_FOVFL | AM_HAL_IOS_INT_FUNDFL | AM_HAL_IOS_INT_FRDERR)

#define AM_HAL_IOS_XCMP_INT (AM_HAL_IOS_INT_XCMPWR | AM_HAL_IOS_INT_XCMPWF | AM_HAL_IOS_INT_XCMPRR | AM_HAL_IOS_INT_XCMPRF)


typedef enum
{
    AM_IOSTEST_CMD_START_DATA    = 0,
    AM_IOSTEST_CMD_STOP_DATA     = 1,
    AM_IOSTEST_CMD_ACK_DATA      = 2,
} AM_IOSTEST_CMD_E;

#define AM_IOSTEST_IOSTOHOST_DATAAVAIL_INTMASK  1

typedef enum
{
    AM_IOSTEST_SLAVE_STATE_NODATA   = 0,
    AM_IOSTEST_SLAVE_STATE_DATA     = 1,
} AM_IOSTEST_SLAVE_STATE_E;

volatile AM_IOSTEST_SLAVE_STATE_E g_iosState;
volatile uint32_t g_sendIdx = 0;
volatile bool g_bSensor0Data, g_bSensor1Data;
static void *g_pIOSHandle;

//*****************************************************************************
//
// Message buffers.
//
// data from the IOS interface, which is only 8 bits wide.
//
//*****************************************************************************
#define AM_TEST_REF_BUF_SIZE    512
uint8_t g_pui8TestBuf[AM_TEST_REF_BUF_SIZE];

#define AM_IOS_TX_BUFSIZE_MAX   1023
uint8_t g_pui8TxFifoBuffer[AM_IOS_TX_BUFSIZE_MAX];

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
const am_hal_gpio_pincfg_t g_AM_BSP_GPIO_DISABLE =
{
    .uFuncSel            = AM_HAL_PIN_4_GPIO,
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
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

    .pui8SRAMBuffer = g_pui8TxFifoBuffer,
    .ui32SRAMBufferCap = AM_IOS_TX_BUFSIZE_MAX,
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

    .pui8SRAMBuffer = g_pui8TxFifoBuffer,
    .ui32SRAMBufferCap = AM_IOS_TX_BUFSIZE_MAX,
};

//*****************************************************************************
//
// Timer handling to emulate sensor data
//
//*****************************************************************************
static am_hal_ctimer_config_t g_sTimer =
{
    // Don't link timers.
    0,

    // Set up TimerA.
    (AM_HAL_CTIMER_FN_REPEAT |
     AM_HAL_CTIMER_INT_ENABLE    |
     AM_HAL_CTIMER_HFRC_12KHZ),

    // No configuration for TimerB.
    0,
};

// Timer Interrupt Service Routine (ISR)
void am_ctimer_isr(void)
{
    uint32_t ui32Status;

    ui32Status = am_hal_ctimer_int_status_get(false);
    am_hal_ctimer_int_clear(ui32Status);

    am_hal_ctimer_int_service(ui32Status);
}

// Emulate Sensor0 New Data
void timer0_handler(void)
{

    // Inform main loop of sensor 0 Data availability
    g_bSensor0Data = true;
}

// Emulate Sensor1 New Data
void timer1_handler(void)
{
    // Inform main loop of sensor 1 Data availability
    g_bSensor1Data = true;
}

void stop_sensors(void)
{
    //
    // Stop timer A0
    //
    am_hal_ctimer_stop(0, AM_HAL_CTIMER_TIMERA);
    //
    // Stop timer A1
    //
    am_hal_ctimer_stop(1, AM_HAL_CTIMER_TIMERA);
}

void start_sensors(void)
{
    stop_sensors(); // Just in case host died without sending STOP last time
    // Initialize Data Buffer Index
    g_sendIdx = 0;
    //
    // Start timer A0
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);
    //
    // Start timer A1
    //
    am_hal_ctimer_start(1, AM_HAL_CTIMER_TIMERA);
    g_iosState = AM_IOSTEST_SLAVE_STATE_NODATA;
}

void init_sensors(void)
{
    uint32_t ui32Period;

    //
    // Set up timer A0 & A1.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer);
    am_hal_ctimer_clear(1, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(1, &g_sTimer);

    //
    // Set up timerA0 for Sensor 0 Freq
    //
    ui32Period = 12000 / SENSOR0_FREQ ;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));
    //
    // Set up timerA1 for Sensor 1 Freq
    //
    ui32Period = 12000 / SENSOR1_FREQ ;
    am_hal_ctimer_period_set(1, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA1);

    //
    // Enable the timer Interrupt.
    //
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0,
                               timer0_handler);
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA1,
                               timer1_handler);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA1);

    //
    // Enable the timer interrupt in the NVIC.
    //
    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_EnableIRQ(CTIMER_IRQn);
    am_hal_interrupt_master_enable();
}

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
    am_hal_gpio_pinconfig(4, g_AM_BSP_GPIO_ENABLE);

}


// Inform host of new data available to read
void inform_host(void)
{
    uint32_t ui32Arg = AM_IOSTEST_IOSTOHOST_DATAAVAIL_INTMASK;
    // Update FIFOCTR for host to read
    am_hal_ios_control(g_pIOSHandle, AM_HAL_IOS_REQ_FIFO_UPDATE_CTR, NULL);
    // Interrupt the host
    am_hal_ios_control(g_pIOSHandle, AM_HAL_IOS_REQ_HOST_INTSET, &ui32Arg);
}

//*****************************************************************************
//
// IO Slave Main ISR.
//
//*****************************************************************************
void am_ioslave_ios_isr(void)
{
    uint32_t ui32Status;
    uint8_t  *pui8Packet;
    uint32_t ui32UsedSpace = 0;

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
        //
        // Set up a pointer for writing 32-bit aligned packets through the IO slave
        // interface.
        //
        pui8Packet = (uint8_t *) am_hal_ios_pui8LRAM;
        switch(pui8Packet[0])
        {
            case AM_IOSTEST_CMD_START_DATA:
                // Host wants to start data exchange
                // Start the Sensor Emulation
                start_sensors();
                break;

            case AM_IOSTEST_CMD_STOP_DATA:
                // Host no longer interested in data from us
                // Stop the Sensor emulation
                stop_sensors();
                g_iosState = AM_IOSTEST_SLAVE_STATE_NODATA;
                break;

            case AM_IOSTEST_CMD_ACK_DATA:
                // Host done reading the last block signalled
                // Check if any more data available
                am_hal_ios_fifo_space_used(g_pIOSHandle, &ui32UsedSpace);
                if (ui32UsedSpace)
                {
                    g_iosState = AM_IOSTEST_SLAVE_STATE_DATA;
                    inform_host();
                }
                else
                {
                    g_iosState = AM_IOSTEST_SLAVE_STATE_NODATA;
                }
                break;

            default:
                break;
        }
    }
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int main(void)
{
    int i;

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
    am_util_stdio_printf("IOS FIFO Example\n");

#if 0 // For Debug only
    //
    // Set up GPIO on Pin 23, 24 & 48 for timing debug.
    //
    am_hal_gpio_out_bit_clear(23);
    am_hal_gpio_pin_config(23, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(24);
    am_hal_gpio_pin_config(24, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(48);
    am_hal_gpio_pin_config(48, AM_HAL_GPIO_OUTPUT);
    am_hal_gpio_out_bit_clear(43);
    am_hal_gpio_pin_config(43, AM_HAL_GPIO_OUTPUT);
#endif

    // Initialize Test Data
    for (i = 0; i < AM_TEST_REF_BUF_SIZE; i++)
    {
        g_pui8TestBuf[i] = (i & 0xFF) ^ XOR_BYTE;
    }

    init_sensors();
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
        uint32_t numWritten = 0;
        uint32_t numWritten1 = 0;
        uint32_t chunk1;
        uint32_t ui32UsedSpace = 0;
        uint32_t ui32IntStatus = am_hal_interrupt_master_disable();
        if (g_bSensor0Data || g_bSensor1Data)
        {
            // Enable the interrupts
            am_hal_interrupt_master_set(ui32IntStatus);
            if (g_bSensor0Data)
            {
                chunk1 = AM_TEST_REF_BUF_SIZE - g_sendIdx;
                if (chunk1 > SENSOR0_DATA_SIZE)
                {
                    am_hal_ios_fifo_write(g_pIOSHandle, &g_pui8TestBuf[g_sendIdx], SENSOR0_DATA_SIZE, &numWritten);
                }
                else
                {
                    am_hal_ios_fifo_write(g_pIOSHandle, &g_pui8TestBuf[g_sendIdx], chunk1, &numWritten);
                    if (numWritten == chunk1)
                    {
                        am_hal_ios_fifo_write(g_pIOSHandle, &g_pui8TestBuf[0], SENSOR0_DATA_SIZE - chunk1, &numWritten1);
                        numWritten += numWritten1;
                    }
                }

                g_sendIdx += numWritten;
                g_sendIdx %= AM_TEST_REF_BUF_SIZE;
                g_bSensor0Data = false;
            }
            if (g_bSensor1Data)
            {
                chunk1 = AM_TEST_REF_BUF_SIZE - g_sendIdx;
                if (chunk1 > SENSOR1_DATA_SIZE)
                {
                    am_hal_ios_fifo_write(g_pIOSHandle, &g_pui8TestBuf[g_sendIdx], SENSOR1_DATA_SIZE, &numWritten);
                }
                else
                {
                    am_hal_ios_fifo_write(g_pIOSHandle, &g_pui8TestBuf[g_sendIdx], chunk1, &numWritten);
                    if (numWritten == chunk1)
                    {
                        am_hal_ios_fifo_write(g_pIOSHandle, &g_pui8TestBuf[0], SENSOR1_DATA_SIZE - chunk1, &numWritten1);
                        numWritten += numWritten1;
                    }
                }

                g_sendIdx += numWritten;
                g_sendIdx %= AM_TEST_REF_BUF_SIZE;
                g_bSensor1Data = false;
            }
            // If we were Idle - need to inform Host if there is new data
            if (g_iosState == AM_IOSTEST_SLAVE_STATE_NODATA)
            {
                am_hal_ios_fifo_space_used(g_pIOSHandle, &ui32UsedSpace);
                if (ui32UsedSpace)
                {
                    g_iosState = AM_IOSTEST_SLAVE_STATE_DATA;
                    inform_host();
                }
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
}


