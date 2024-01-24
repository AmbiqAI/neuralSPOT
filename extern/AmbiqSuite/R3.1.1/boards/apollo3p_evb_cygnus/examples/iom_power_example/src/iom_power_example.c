//*****************************************************************************
//
//! @file iom_power_example.c
//!
//! @brief Example that demonstrates IOM power consumption, connecting to a FRAM
//! Purpose: FRAM is initialized with a known pattern data using Blocking IOM Write.
//! This example starts a 1 second timer. At each 1 second period, it initiates
//! reading a fixed size block from the FRAM device using Non-Blocking IOM
//! Read, and comparing against the predefined pattern
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! USE I2C FRAM on Cygnus: MB85RC64TA
//! Recommend to use 1.8V power supply voltage.
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
#include "am_util.h"

//*****************************************************************************
// Customize the following for the test
//*****************************************************************************
// Control the example execution
#define FRAM_IOM_MODULE         3
#define NUM_INERATIONS          5
#define PATTERN_BUF_SIZE        128
//#define FRAM_DEVICE_MB85RS64V     1     // SPI Fram
//#define FRAM_DEVICE_MB85RC256V    1     // I2C Fram

typedef struct
{
    const uint32_t                iom_speed;
    const char                    *string;
}iom_speed_t;

iom_speed_t iom_test_speeds[] =
{
    { AM_HAL_IOM_48MHZ,     " 48MHz" },
    { AM_HAL_IOM_24MHZ,     " 24MHz" },
    { AM_HAL_IOM_16MHZ,     " 16MHz" },
    { AM_HAL_IOM_12MHZ,     " 12MHz" },
    { AM_HAL_IOM_8MHZ,      " 8MHz" },
    { AM_HAL_IOM_6MHZ,      " 6MHz" },
    { AM_HAL_IOM_4MHZ,      " 4MHz" },
    { AM_HAL_IOM_1MHZ,      " 1MHz" },
    { AM_HAL_IOM_400KHZ,    " 400KHz" },
    { AM_HAL_IOM_100KHZ,    " 100KHz" }
};

#define IOM_START_SPEED_INDEX       7
#define IOM_END_SPEED_INDEX         9

#define ENABLE_LOGGING

#ifdef ENABLE_LOGGING
#define DEBUG_PRINT am_util_stdio_printf
#else
#define DEBUG_PRINT(...)
#endif

#define IOM_TRIGGER_GPIO            0

#define TOGGLE_TRIGGER_GPIO()                                           \
{                                                                       \
    am_hal_gpio_state_write(IOM_TRIGGER_GPIO, AM_HAL_GPIO_OUTPUT_SET);  \
    am_hal_flash_delay(FLASH_CYCLES_US(10));                            \
    am_hal_gpio_state_write(IOM_TRIGGER_GPIO, AM_HAL_GPIO_OUTPUT_CLEAR);\
}

//*****************************************************************************
//*****************************************************************************

#include "am_devices_mb85rc256v.h"
#define FRAM_DEVICE_ID          AM_DEVICES_MB85RC64TA_ID
#define am_iom_test_devices_t   am_devices_mb85rc256v_config_t

#define IOM_INTERRUPT1(n)       AM_HAL_INTERRUPT_IOMASTER ## n
#define IOM_INTERRUPT(n)        IOM_INTERRUPT1(n)
#define FRAM_IOM_IRQn           ((IRQn_Type)(IOMSTR0_IRQn + FRAM_IOM_MODULE))

//
// Typedef - to encapsulate device driver functions
//
typedef struct
{
    uint8_t  devName[20];
    uint32_t (*fram_init)(uint32_t ui32Module, am_iom_test_devices_t *pDevConfig, void **ppHandle, void **ppIomHandle);
    uint32_t (*fram_term)(void *pHandle);

    uint32_t (*fram_read_id)(void *pHandle, uint32_t *pDeviceID);

    uint32_t (*fram_blocking_write)(void *pHandle, uint8_t *ui8TxBuffer,
                             uint32_t ui32WriteAddress,
                             uint32_t ui32NumBytes);

    uint32_t (*fram_nonblocking_write)(void *pHandle, uint8_t *ui8TxBuffer,
                                uint32_t ui32WriteAddress,
                                uint32_t ui32NumBytes,
                                am_hal_iom_callback_t pfnCallback,
                                void *pCallbackCtxt);

    uint32_t (*fram_blocking_read)(void *pHandle, uint8_t *pui8RxBuffer,
                            uint32_t ui32ReadAddress,
                            uint32_t ui32NumBytes);

    uint32_t (*fram_nonblocking_read)(void *pHandle, uint8_t *pui8RxBuffer,
                                                      uint32_t ui32ReadAddress,
                                                      uint32_t ui32NumBytes,
                                                      am_hal_iom_callback_t pfnCallback,
                                                      void *pCallbackCtxt);
} fram_device_func_t;

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
volatile bool                           g_bWriteFram = true;
volatile bool                           g_bWriteComplete = false;
volatile bool                           g_bReadFram = false;
volatile bool                           g_bVerifyReadData = false;
void                                    *g_IomDevHdl;
void                                    *g_pIOMHandle;
am_hal_iom_buffer(PATTERN_BUF_SIZE)     gPatternBuf;
am_hal_iom_buffer(PATTERN_BUF_SIZE)     gRxBuf;
uint32_t                                g_ui32ClockFreq;

// Buffer for non-blocking transactions
uint32_t                                DMATCBBuffer[256];
fram_device_func_t device_func =
{
    // Cygnus installed I2C FRAM device
    .devName = "I2C FRAM MB85RC64TA",
    .fram_init = am_devices_mb85rc256v_init,
    .fram_term = am_devices_mb85rc256v_term,
    .fram_read_id = am_devices_mb85rc256v_read_id,
    .fram_blocking_write = am_devices_mb85rc256v_blocking_write,
    .fram_nonblocking_write = am_devices_mb85rc256v_nonblocking_write,
    .fram_blocking_read = am_devices_mb85rc256v_blocking_read,
    .fram_nonblocking_read = am_devices_mb85rc256v_nonblocking_read,
};

//*****************************************************************************
//
// Timer configuration.
//
//*****************************************************************************
am_hal_ctimer_config_t g_sTimer0 =
{
    // Don't link timers.
    0,

    // Set up Timer0A.
    (AM_HAL_CTIMER_FN_REPEAT |
     AM_HAL_CTIMER_INT_ENABLE    |
     AM_HAL_CTIMER_LFRC_32HZ),

    // No configuration for Timer0B.
    0,
};

//*****************************************************************************
//
// Function to initialize Timer A0 to interrupt every 1 second.
//
//*****************************************************************************
void
timerA0_init(void)
{
    uint32_t ui32Period;

    //
    // Enable the LFRC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Set up timer A0.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer0);

    //
    // Set up timerA0 to 32Hz from LFRC divided to 1 second period.
    //
    ui32Period = 32;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
}

//*****************************************************************************
//
// Timer Interrupt Service Routine (ISR)
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    uint32_t ui32Status;

    ui32Status = am_hal_ctimer_int_status_get(true);

    if ( ui32Status )
    {
        am_hal_ctimer_int_clear(ui32Status);
        am_hal_ctimer_int_service(ui32Status);
    }
}

//
// Take over the interrupt handler for whichever IOM we're using.
//
#define fram_iom_isr                                                          \
    am_iom_isr1(FRAM_IOM_MODULE)
#define am_iom_isr1(n)                                                        \
    am_iom_isr(n)
#define am_iom_isr(n)                                                         \
    am_iomaster ## n ## _isr

//*****************************************************************************
//
// IOM ISRs.
//
//*****************************************************************************
//
//! Take over default ISR. (Queue mode service)
//
void fram_iom_isr(void)
{
    uint32_t ui32Status;

    if (!am_hal_iom_interrupt_status_get(g_pIOMHandle, true, &ui32Status))
    {
        if ( ui32Status )
        {
            am_hal_iom_interrupt_clear(g_pIOMHandle, ui32Status);
            am_hal_iom_interrupt_service(g_pIOMHandle, ui32Status);
        }
    }
}

void
init_pattern(void)
{
    uint32_t i;
    for (i = 0; i < PATTERN_BUF_SIZE; i++)
    {
        gPatternBuf.bytes[i] = i & 0xFF;
    }
}

int
fram_init(void)
{
    uint32_t ui32Status;
    uint32_t ui32DeviceId = 0;

    // Set up IOM
    am_iom_test_devices_t stFramConfig;
    stFramConfig.ui32ClockFreq = g_ui32ClockFreq;
    stFramConfig.pNBTxnBuf = DMATCBBuffer;
    stFramConfig.ui32NBTxnBufLength = sizeof(DMATCBBuffer) / 4;
    // Initialize the Device
    ui32Status = device_func.fram_init(FRAM_IOM_MODULE, &stFramConfig, &g_IomDevHdl, &g_pIOMHandle);
    if (0 == ui32Status)
    {
        ui32Status = device_func.fram_read_id(g_IomDevHdl, &ui32DeviceId);

        if ((ui32Status  != 0) || (ui32DeviceId != FRAM_DEVICE_ID))
        {
            return -1;
        }
        DEBUG_PRINT("%s Found\n", device_func.devName);
        return 0;
    }
    else
    {
        return -1;
    }
}

void
write_complete(void *pCallbackCtxt, uint32_t transactionStatus)
{
    if (transactionStatus != AM_HAL_STATUS_SUCCESS)
    {
        DEBUG_PRINT("\nFRAM Write Failed 0x%x\n", transactionStatus);
    }
    else
    {
        g_bWriteComplete = true;
    }
}

void
read_complete(void *pCallbackCtxt, uint32_t transactionStatus)
{
    if (transactionStatus != AM_HAL_STATUS_SUCCESS)
    {
        DEBUG_PRINT("\nFRAM Read Failed 0x%x\n", transactionStatus);
    }
    else
    {
        DEBUG_PRINT(".");
        g_bVerifyReadData = true;
    }
}

void
initiate_fram_read(void)
{
    g_bReadFram = true;
}

int
verify_fram_data(void)
{
    uint32_t i;
    g_bVerifyReadData = false;
    // Verify Read FRAM data
    for (i = 0; i < PATTERN_BUF_SIZE; i++)
    {
        if (gPatternBuf.bytes[i] != gRxBuf.bytes[i])
        {
            DEBUG_PRINT("Receive Data Compare failed at offset %d - Expected = 0x%x, Received = 0x%x\n",
                i, gPatternBuf.bytes[i], gRxBuf.bytes[i]);
            return -1;
        }
    }
    return 0;
}

void
write_fram(void)
{
    uint32_t ui32Status;
    // Initiate write of a block of data to FRAM
    ui32Status = device_func.fram_nonblocking_write(g_IomDevHdl, &gPatternBuf.bytes[0], 0, PATTERN_BUF_SIZE, write_complete, 0);
    if (ui32Status == 0)
    {
        g_bWriteFram = false;
    }
}

void
read_fram(void)
{
    uint32_t ui32Status;
    // Initiate read of a block of data from FRAM
    ui32Status = device_func.fram_nonblocking_read(g_IomDevHdl, &gRxBuf.bytes[0], 0, PATTERN_BUF_SIZE, read_complete, 0);
    if (ui32Status == 0)
    {
        g_bReadFram = false;
    }
}

void
fram_term(void)
{
    device_func.fram_term(g_IomDevHdl);
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    int iRet;
    uint32_t numRead = 0;
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

#ifdef ENABLE_LOGGING
    // Initialize the Debug output to uart.
    am_bsp_itm_printf_enable();
    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();

#endif
    DEBUG_PRINT("IOM Power Example\n\n");

    //
    // TimerA0 init.
    //
    timerA0_init();

    //
    // Enable the timer Interrupt.
    //
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_EnableIRQ(CTIMER_IRQn);

    init_pattern();

    // Set up the periodic FRAM Read
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0, initiate_fram_read);

    //
    // Start timer A0
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);

    DEBUG_PRINT("Periodically Reading data from FRAM using non-blocking read - %d times\n", NUM_INERATIONS);
    for (uint8_t ui8Speed = IOM_START_SPEED_INDEX; ui8Speed <= IOM_END_SPEED_INDEX; ui8Speed++)
    {
        g_ui32ClockFreq = iom_test_speeds[ui8Speed].iom_speed;
        DEBUG_PRINT("\n*****iom speed %s*****\n", iom_test_speeds[ui8Speed].string);
        am_hal_interrupt_master_enable();

        //
        // Initialize the FRAM Device
        //
        iRet = fram_init();
        if (iRet)
        {
            DEBUG_PRINT("Unable to initialize FRAM\n");
            while(1);
        }
        NVIC_EnableIRQ(FRAM_IOM_IRQn);

        g_bWriteFram = true;
        g_bWriteComplete = false;
        g_bReadFram = false;
        g_bVerifyReadData = false;
        numRead = 0;

        //
        // Loop forever.
        //
        while(1)
        {
            //
            // Disable interrupt while we decide whether we're going to sleep.
            //
            uint32_t ui32IntStatus = am_hal_interrupt_master_disable();

            if (g_bWriteFram)
            {
                //
                // Enable interrupts
                //
                am_hal_interrupt_master_set(ui32IntStatus);
                TOGGLE_TRIGGER_GPIO();
                write_fram();
                TOGGLE_TRIGGER_GPIO();
            }
            else if (g_bWriteComplete && g_bReadFram)
            {
                //
                // Enable interrupts
                //
                am_hal_interrupt_master_set(ui32IntStatus);
                TOGGLE_TRIGGER_GPIO();
                read_fram();
                TOGGLE_TRIGGER_GPIO();
            }
            else if (g_bVerifyReadData)
            {
                //
                // Enable interrupts
                //
                am_hal_interrupt_master_set(ui32IntStatus);
                verify_fram_data();
                if (++numRead >= NUM_INERATIONS)
                {
                    DEBUG_PRINT("\n%d iterations done\n", NUM_INERATIONS);
                        break;
                }
            }
            else
            {
                // Wait for Baud rate detection
                am_hal_sysctrl_sleep(true);

                //
                // Enable interrupts
                //
                am_hal_interrupt_master_set(ui32IntStatus);
            }
        }
        NVIC_DisableIRQ(FRAM_IOM_IRQn);
        fram_term();
    }

    //
    // Disable the timer Interrupt.
    //
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA0);

    //
    // disable the interrupts in the NVIC.
    //
    NVIC_DisableIRQ(CTIMER_IRQn);
    // Cleanup
    DEBUG_PRINT("\nEnd of IOM Power Example\n");
    while(1)
    {
        //
        // Go to Deep Sleep.
        //
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    }
}

