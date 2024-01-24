//*****************************************************************************
//
//! @file uart_boot_host.c
//!
//! @brief Converts UART Wired transfer commands to SPI or I2C for use with
//! SBL SPI/I2C testing.
//!
//! Purpose: This example running on an intermediate board, along with the
//! standard uart_wired_update script running on host PC, can be used as a
//! way to communicate with the Apollo SBL (secure boot loader).
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! @verbatim
//! PIN fly lead connections assumed:
//!     HOST (this board)                       SLAVE (Apollo SBL target)
//!     --------                                --------
//! Apollo3 SPI or I2C common connections:
//!     GPIO[2]  GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt (SLINT)
//!     GPIO[16] OVERRIDE pin   (host to slave) GPIO[16] Override pin or n/c
//!     GPIO[17] Slave reset (host to slave)    Reset Pin or n/c
//!     GND                                     GND
//!
//! Apollo3 SPI additional connections:
//!     GPIO[5]  IOM0 SPI CLK                   GPIO[0]  IOS SPI SCK
//!     GPIO[6]  IOM0 SPI MISO                  GPIO[2]  IOS SPI MISO
//!     GPIO[7]  IOM0 SPI MOSI                  GPIO[1]  IOS SPI MOSI
//!     GPIO[11] IOM0 SPI nCE                   GPIO[3]  IOS SPI nCE
//!
//! Apollo3 I2C additional connections:
//!     GPIO[5]  I2C SCL                        GPIO[0]  I2C SCL
//!     GPIO[6]  I2C SDA                        GPIO[1]  I2C SDA
//!
//! Reset and Override pin connections from Host are optional, but using them
//! automates the entire process.
//!
//! SPI or I2C mode can be handled in a couple of ways:
//! - SPI mode is the default (i.e. don't press buttons or tie pins low).
//! - For I2C, press button2 during reset and hold it until the program begins,
//!     i.e. you see the "I2C clock = " msg.
//!   Alternatively the button2 pin can be tied low.
//! - To specifically force either SPI or I2C mode, define the USE_SPI macro
//!   appropriately.
//! - Note that on the Apollo3 EVB, button2 is labelled as 'BTN4', which is
//!   the button located nearest the end of the board.
//!   Also on the Apollo3 EVB, BTN4 uses pin 19.  It happens that the header
//!   pin for pin 19 on the EVB is adjacent to a ground pin, so a jumper can
//!   be used to assert I2C mode.
//!
//! General procedure:
//! - Update any of the pin defines below as needed for your particular setup.
//!   Typical ones that may need to be changed are marked with the phrase
//!   "Platform specific pin".
//! - Make all of the above connections between the host and the target device.
//!   This procedure assumes that the optional SLINT and Reset connections
//!   above are made.
//! - Compile and load this program onto the host device, e.g. an Apollo3 EVB.
//! - Start the UART script (e.g. uart_wired_update.py, which can be used for
//!   loading a binary onto a target device) using the appropriate parameters.
//! - Press the reset button on the host device.
//! - This program will take the UART commands and convert them to SPI or I2C
//!   signals to be used for transmitting data to the target device.
//!
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

//*****************************************************************************
//
// Configuration options
//
//*****************************************************************************
//
// USE_SPI: Define to force configuration to output as SPI or I2C.
// Leave undefined when using a button/pin to determine SPI or I2C mode.
// By default, it is undefined and uses a pull-up input pin configuration
// to designate SPI mode.
//
//#define USE_SPI                         1   // 0 = I2C, 1 = SPI

//
// Define the host pins that will be used for connecting to
// corresponding pins on the target.
//
// Platform specific pin:
// TARGET_BOARD_OVERRIDE_PIN is connected to the corresponding
// 'Override' pin on the slave device. This signal is used to force the
// target SBL to wait for signaling from the host.
//
#ifdef AM_BSP_GPIO_BUTTON0
#define TARGET_BOARD_OVERRIDE_PIN       AM_BSP_GPIO_BUTTON0
#else
#define TARGET_BOARD_OVERRIDE_PIN       16
#endif

//
// Platform specific pin:
// To simplify testing between SPI and I2C, an input pin can be defined
// to determine at run-time whether SPI or I2C is being tested.
// This method can only be used if USE_SPI (above) is not defined.
//
#ifndef USE_SPI
#ifdef AM_BSP_GPIO_BUTTON2
#define USE_SPI_PIN                     AM_BSP_GPIO_BUTTON2 // Labelled BTN4 on the Apollo3 EVB
#else
#define USE_SPI_PIN                     19
#endif
#endif

//
// Platform specific pin:
// Slave interrupt (SLINT) pin is connected to this host pin.
// This interrupt is an input from the slave device.
//
#define BOOTLOADER_HANDSHAKE_PIN        2

//
// Platform specific pin:
// DRIVE_SLAVE_RESET_PIN is the pin connected to the nRst pin of the slave.
//
#define DRIVE_SLAVE_RESET_PIN           17

//
// Define the host UART module (0 or 1) to be used.
// Also define the max packet size
//
#define UART_HOST                       0
#define MAX_UART_PACKET_SIZE            2048
#define IOM_MODULE                      0
#define SLAVE_ADDRESS                   0x20
#define MAX_SPI_SIZE                    1023
#define MAX_I2C_SIZE                    255
#define MAX_IOS_LRAM_SIZE               120     // LRAM can only accept 120 bytes at a time.

//
// Host IOS parameters.
//
#define IOSOFFSET_WRITE_CMD             0x80
#define IOSOFFSET_READ_FIFO             0x7F
#define IOSOFFSET_READ_FIFOCTR          0x7C

#define PRT_INFO        am_util_stdio_printf
//#define PRT_INFO        no_print

//
// Define PRT_DATA if additional pkt data and other information is desired.
// PRT_INFO must also be defined.
//
#if PRT_INFO == am_util_stdio_printf
#define PRT_DATA        no_print
//#define PRT_DATA        am_util_stdio_printf
#endif

//*****************************************************************************
//
// Custom data type.
// Note - am_uart_buffer was simply derived from the am_hal_iom_buffer macro.
//
//*****************************************************************************
#define am_uart_buffer(A)                                           \
union                                                               \
{                                                                   \
    uint32_t words[(A + 3) >> 2];                                   \
    uint8_t bytes[A];                                               \
}

typedef struct
{
    uint32_t                     crc32;     // First word
    uint16_t                     msgType;   // am_secboot_wired_msgtype_e
    uint16_t                     length;
} am_secboot_wired_msghdr_t;

typedef struct
{
    uint32_t                      length  : 16;
    uint32_t                      resv    : 14;
    uint32_t                      bEnd    : 1;
    uint32_t                      bStart  : 1;
} am_secboot_ios_pkthdr_t;

typedef enum
{
    AM_SECBOOT_WIRED_MSGTYPE_HELLO,
    AM_SECBOOT_WIRED_MSGTYPE_STATUS,
    AM_SECBOOT_WIRED_MSGTYPE_OTADESC,
    AM_SECBOOT_WIRED_MSGTYPE_UPDATE,
    AM_SECBOOT_WIRED_MSGTYPE_ABORT,
    AM_SECBOOT_WIRED_MSGTYPE_RECOVER,
    AM_SECBOOT_WIRED_MSGTYPE_RESET,
    AM_SECBOOT_WIRED_MSGTYPE_ACK,
    AM_SECBOOT_WIRED_MSGTYPE_DATA,
} am_secboot_wired_msgtype_e;

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
uint8_t g_pui8UARTTXBuffer[MAX_UART_PACKET_SIZE];
am_uart_buffer(12 * 1024) g_psWriteData;
am_uart_buffer(12 * 1024) g_psReadData;

struct
{
    am_secboot_ios_pkthdr_t       header;
    uint8_t                       data[MAX_IOS_LRAM_SIZE - sizeof(am_secboot_ios_pkthdr_t)];
} g_IosPktData;

volatile uint32_t g_ui32UARTRxIndex = 0;
volatile bool g_bRxTimeoutFlag = false;
volatile bool bIosInt = false;

void *g_IOMHandle;
void *g_pvUART;

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
void iom_slave_write(bool bSpi, uint32_t offset, uint32_t *pBuf, uint32_t size);

//*****************************************************************************
//
// Configuration structure for the IO Master.
//
//*****************************************************************************
static am_hal_gpio_pincfg_t g_AM_BSP_GPIO_BOOT_HANDSHAKE =
{
    .uFuncSel       = AM_HAL_PIN_2_GPIO,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eIntDir        = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
};

static am_hal_iom_config_t g_sIOMI2cConfig =
{
    .eInterfaceMode = AM_HAL_IOM_I2C_MODE,
    .ui32ClockFreq  = AM_HAL_IOM_1MHZ,
};

static am_hal_iom_config_t g_sIOMSpiConfig =
{
    .eInterfaceMode = AM_HAL_IOM_SPI_MODE,
    .ui32ClockFreq = AM_HAL_IOM_8MHZ,
    .eSpiMode = AM_HAL_IOM_SPI_MODE_0,    // Default
};

//*****************************************************************************
//
// no_print
//
//*****************************************************************************
int no_print(char*pFmtStr, ...)
{
    return 0;
}

//*****************************************************************************
//
// Interrupt handler for the UART.
//
//*****************************************************************************
#if UART_HOST == 0
void am_uart_isr(void)
#else
void am_uart1_isr(void)
#endif
{
    uint32_t ui32Status;

    //
    // Read the masked interrupt status from the UART.
    //
    am_hal_uart_interrupt_status_get(g_pvUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(g_pvUART, ui32Status);
    am_hal_uart_interrupt_service(g_pvUART, ui32Status, 0);

    //
    // If there's an RX interrupt, handle it in a way that preserves the
    // timeout interrupt on gaps between packets.
    //
    if (ui32Status & (AM_HAL_UART_INT_RX_TMOUT | AM_HAL_UART_INT_RX))
    {
        uint32_t ui32BytesRead;

        am_hal_uart_transfer_t sRead =
        {
            .ui32Direction = AM_HAL_UART_READ,
            .pui8Data = (uint8_t *) &(g_psWriteData.bytes[g_ui32UARTRxIndex]),
            .ui32NumBytes = 23,
            .ui32TimeoutMs = 0,
            .pui32BytesTransferred = &ui32BytesRead,
        };

        am_hal_uart_transfer(g_pvUART, &sRead);

        g_ui32UARTRxIndex += ui32BytesRead;

        //
        // If there is a TMOUT interrupt, assume we have a compete packet, and
        // send it over SPI.
        //
        if (ui32Status & (AM_HAL_UART_INT_RX_TMOUT))
        {
            NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn + UART_HOST));
            g_bRxTimeoutFlag = true;
        }
    }
}

//*****************************************************************************
//
// Interrupt handler for the GPIO pins.
//
//*****************************************************************************
void am_gpio_isr(void)
{
    //
    // Read and clear the GPIO interrupt status.
    //
#if defined(AM_PART_APOLLO3P)
    AM_HAL_GPIO_MASKCREATE(GpioIntStatusMask);

    am_hal_gpio_interrupt_status_get(false, pGpioIntStatusMask);
    am_hal_gpio_interrupt_clear(pGpioIntStatusMask);
    am_hal_gpio_interrupt_service(pGpioIntStatusMask);
#elif defined(AM_PART_APOLLO3)
    uint64_t ui64Status;

    am_hal_gpio_interrupt_status_get(false, &ui64Status);
    am_hal_gpio_interrupt_clear(ui64Status);
    am_hal_gpio_interrupt_service(ui64Status);
#else
    #error Unknown device.
#endif
}

// ISR callback for the host IOINT
static void hostint_handler(void)
{
    bIosInt = true;
}

//*****************************************************************************
//
// Initialize the IOM.
//
//*****************************************************************************
static void iom_set_up(uint32_t iomModule, bool bSpi)
{
    //
    // Initialize the IOM.
    //
    am_hal_iom_initialize(iomModule, &g_IOMHandle);

    am_hal_iom_power_ctrl(g_IOMHandle, AM_HAL_SYSCTRL_WAKE, false);

    if ( bSpi )
    {
        //
        // Configure the IOM for SPI.
        //
        am_hal_iom_configure(g_IOMHandle, &g_sIOMSpiConfig);

        //
        // Configure the IOM pins.
        //
        am_bsp_iom_pins_enable(iomModule, AM_HAL_IOM_SPI_MODE);
    }
    else
    {
        //
        // Configure the IOM for I2C.
        //
        am_hal_iom_configure(g_IOMHandle, &g_sIOMI2cConfig);

        //
        // Configure the IOM pins.
        //
        am_bsp_iom_pins_enable(iomModule, AM_HAL_IOM_I2C_MODE);
    }

    //
    // Enable the IOM.
    //
    am_hal_iom_enable(g_IOMHandle);

    //
    // Set up the host IO interrupt
    //
    am_hal_gpio_pinconfig(BOOTLOADER_HANDSHAKE_PIN, g_AM_BSP_GPIO_BOOT_HANDSHAKE);

    AM_HAL_GPIO_MASKCREATE(GpioIntMask);
    am_hal_gpio_interrupt_clear(AM_HAL_GPIO_MASKBIT(pGpioIntMask, BOOTLOADER_HANDSHAKE_PIN));

    //
    // Register handler for IOS => IOM interrupt
    //
    am_hal_gpio_interrupt_register(BOOTLOADER_HANDSHAKE_PIN, hostint_handler);
    am_hal_gpio_interrupt_enable(AM_HAL_GPIO_MASKBIT(pGpioIntMask, BOOTLOADER_HANDSHAKE_PIN));
    NVIC_EnableIRQ(GPIO_IRQn);
}

//*****************************************************************************
//
// Initialize the UART.
//
//*****************************************************************************
static void uart_set_up(uint32_t UARTNum)
{
    //
    // Start the UART.
    //
    am_hal_uart_config_t sUartConfig =
    {
        //
        // Standard UART settings: 115200-8-N-1
        //
        .ui32BaudRate    = 115200,
        .ui32DataBits    = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity      = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits    = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

        //
        // Set TX and RX FIFOs to interrupt at three-quarters full.
        //
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_3_4 |
                           AM_HAL_UART_RX_FIFO_3_4),

        //
        // This code will use the standard interrupt handling for UART TX, but
        // we will have a custom routine for UART RX.
        //
        .pui8TxBuffer = g_pui8UARTTXBuffer,
        .ui32TxBufferSize = sizeof(g_pui8UARTTXBuffer),
        .pui8RxBuffer = 0,
        .ui32RxBufferSize = 0,
    };

    am_hal_uart_initialize(UARTNum, &g_pvUART);
    am_hal_uart_power_control(g_pvUART, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(g_pvUART, &sUartConfig);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    //
    // Make sure to enable the interrupts for RX, since the HAL doesn't already
    // know we intend to use them.
    //
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UARTNum));
    am_hal_uart_interrupt_enable(g_pvUART, (AM_HAL_UART_INT_RX |
                                 AM_HAL_UART_INT_RX_TMOUT));

    am_hal_interrupt_master_enable();
}

//*****************************************************************************
//
// Initialize the UART.
//
//*****************************************************************************
static bool USE_SPI_get(void)
{
#ifndef USE_SPI
    uint32_t ui32Pinval;

    //
    // Configure the pin as input with a pullup.
    //
    am_hal_gpio_pinconfig(USE_SPI_PIN, g_AM_HAL_GPIO_INPUT_PULLUP);

    //
    // Short delay for the pin configuration.
    //
    am_util_delay_us(5);

    //
    // If the pin is not pulled down, assume SPI.
    //
    am_hal_gpio_state_read(USE_SPI_PIN, AM_HAL_GPIO_INPUT_READ, &ui32Pinval);

    //
    // Done with the pin.
    //
    am_hal_gpio_pinconfig(USE_SPI_PIN, g_AM_HAL_GPIO_DISABLE);

    return ui32Pinval ? true : false;
#else
    //
    // If USE_SPI is defined, return true for SPI or false for I2C.
    //
    return (USE_SPI >= 1);
#endif
} // USE_SPI_get()

//*****************************************************************************
//
// Reset the slave device and force it into boot mode.
//
//*****************************************************************************
void start_boot_mode(bool bReset)
{
    if ( !bReset )
    {
        //
        // Drive RESET high and configure the pin.
        //
        am_hal_gpio_state_write(DRIVE_SLAVE_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);
        am_hal_gpio_pinconfig(DRIVE_SLAVE_RESET_PIN, g_AM_HAL_GPIO_OUTPUT);

        //
        // Drive the override pin high and configure the pin.
        //
        am_hal_gpio_state_write(TARGET_BOARD_OVERRIDE_PIN, AM_HAL_GPIO_OUTPUT_SET);
        am_hal_gpio_pinconfig(TARGET_BOARD_OVERRIDE_PIN, g_AM_HAL_GPIO_OUTPUT);
    }
    else
    {
        //
        // Drive RESET low.
        //
        am_hal_gpio_state_write(DRIVE_SLAVE_RESET_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);

        //
        // Drive the override pin low to force the slave into boot mode.
        //
        am_hal_gpio_state_write(TARGET_BOARD_OVERRIDE_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);

        //
        // Short delay.
        //
        am_util_delay_us(5);

        //
        // Release RESET.
        //
        am_hal_gpio_state_write(DRIVE_SLAVE_RESET_PIN, AM_HAL_GPIO_OUTPUT_SET);

        //
        // Short delay.
        //
        am_util_delay_us(5);
    }
}

//*****************************************************************************
//
// Read a packet from the SBL IOS.
//
//*****************************************************************************
void iom_slave_read(bool bSpi, uint32_t offset, uint32_t *pBuf, uint32_t size)
{
    am_hal_iom_transfer_t       Transaction;

    Transaction.ui32InstrLen    = 1;
    Transaction.ui32Instr       = offset;
    Transaction.eDirection      = AM_HAL_IOM_RX;
    Transaction.ui32NumBytes    = size;
    Transaction.pui32RxBuffer   = pBuf;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if ( bSpi )
    {
        Transaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_IOM0_CS_CHNL;
    }
    else
    {
        Transaction.uPeerInfo.ui32I2CDevAddr = (SLAVE_ADDRESS);
    }

    am_hal_iom_blocking_transfer(g_IOMHandle, &Transaction);
}

//*****************************************************************************
//
// Write a packet to the SBL IOS.
//
//*****************************************************************************
void iom_slave_write(bool bSpi, uint32_t offset, uint32_t *pBuf, uint32_t size)
{
    am_hal_iom_transfer_t       Transaction;

    Transaction.ui32InstrLen    = 1;
    Transaction.ui32Instr       = offset;
    Transaction.eDirection      = AM_HAL_IOM_TX;
    Transaction.ui32NumBytes    = size;
    Transaction.pui32TxBuffer   = pBuf;
    Transaction.bContinue       = false;
    Transaction.ui8RepeatCount  = 0;
    Transaction.ui32PauseCondition = 0;
    Transaction.ui32StatusSetClr = 0;

    if ( bSpi )
    {
        Transaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_IOM0_CS_CHNL;
    }
    else
    {
        Transaction.uPeerInfo.ui32I2CDevAddr = (SLAVE_ADDRESS);
    }

    am_hal_iom_blocking_transfer(g_IOMHandle, &Transaction);
}

//*****************************************************************************
//
// Send a "HELLO" packet.
//
//*****************************************************************************
void send_hello(bool bSpi)
{
    struct
    {
        am_secboot_ios_pkthdr_t   hdr;
        am_secboot_wired_msghdr_t msg;
    } pkt;

    pkt.hdr.bStart = 1;
    pkt.hdr.bEnd = 1;
    pkt.hdr.length = 12;
    pkt.msg.msgType = AM_SECBOOT_WIRED_MSGTYPE_HELLO;
    pkt.msg.length = sizeof(am_secboot_wired_msghdr_t);

    //
    // Compute CRC
    //
    PRT_INFO("send_hello: sending bytes: %d.\n", pkt.msg.length );
    am_hal_crc32((uint32_t)&pkt.msg.msgType, pkt.msg.length - sizeof(uint32_t), &pkt.msg.crc32);
    iom_slave_write(bSpi, IOSOFFSET_WRITE_CMD, (uint32_t*)&pkt, sizeof(pkt));
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    bool     bSpi;
    uint32_t ui32MaxSize, ui32XferSize, ui32HdrSz;
    uint32_t ui32ByteCnt;
    bool     bIOShdr;
    uint32_t ui32DotCnt = 0;

    //
    // Default setup.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_bsp_low_power_init();

    //
    // Check the SPI pin
    //
    bSpi = USE_SPI_get();
    ui32MaxSize = bSpi ? MAX_SPI_SIZE: MAX_I2C_SIZE;
    //
    // Enable the ITM
    //
    am_bsp_itm_printf_enable();
    am_util_stdio_printf("\nApollo3 UART to IOS Host Bridge\n");

    if ( bSpi )
    {
        am_util_stdio_printf("SPI clock = %d.%d MHz\n",
                             g_sIOMSpiConfig.ui32ClockFreq / 1000000,
                             g_sIOMSpiConfig.ui32ClockFreq % 1000000);
    }
    else
    {
        am_util_stdio_printf("I2C clock = %d.%d MHz\n",
                             g_sIOMI2cConfig.ui32ClockFreq / 1000000,
                             g_sIOMI2cConfig.ui32ClockFreq % 1000000);
    }

    //
    // Set and configure the reset/bootmode pins high, but don't reset slave.
    //
    start_boot_mode(false);

    //
    // Start the IOM interface.
    //
    iom_set_up(IOM_MODULE, bSpi);

    //
    // Start up the UART interface.
    //
    uart_set_up(UART_HOST);

    //
    // Force the slave into boot mode.
    //
    start_boot_mode(true);

    //
    // Wait for initial handshake signal to know that IOS interface is alive
    //
    while( !bIosInt );
    bIosInt = false;

    //
    // Short delay.
    //
    am_util_delay_ms(1);

    //
    // Send the "HELLO" message to connect to the interface.
    //
    send_hello(bSpi);

    while( !bIosInt );
    bIosInt = false;

    //
    // Read the "STATUS" response from the slave device and
    // also determine the size of the transfer.
    //
    ui32HdrSz = sizeof(am_secboot_wired_msghdr_t);      // Size is 8 bytes
    iom_slave_read(bSpi, IOSOFFSET_READ_FIFO, (uint32_t*)&g_psReadData, ui32HdrSz);
    ui32XferSize = ((am_secboot_wired_msghdr_t*)&g_psReadData)->length;

    //
    // Now that we know the size, get the rest of the data
    //
    iom_slave_read(bSpi, IOSOFFSET_READ_FIFO,
                   (uint32_t*)&g_psReadData.bytes[ui32HdrSz],
                   ui32XferSize - ui32HdrSz);

    //
    // Loop forever.
    //
    while (1)
    {
        if ( bIosInt == true )
        {
            bIosInt = false;

            //
            // Read the Data Size from the slave device.
            //
            iom_slave_read(bSpi, IOSOFFSET_READ_FIFOCTR, &ui32XferSize, 2);
            ui32XferSize = (ui32XferSize > ui32MaxSize) ? ui32MaxSize : ui32XferSize;

            if ( ui32XferSize > 0 )
            {
                //
                // Read the Data from the IOS.
                //
                iom_slave_read(bSpi, IOSOFFSET_READ_FIFO, (uint32_t*)&g_psReadData, ui32XferSize);

                //
                // Write the Data to the UART.
                //
                {
                    am_hal_uart_transfer_t sWrite =
                    {
                        .ui32Direction = AM_HAL_UART_WRITE,
                        .pui8Data = g_psReadData.bytes,
                        .ui32NumBytes = ui32XferSize,
                        .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,
                        .pui32BytesTransferred = 0,
                    };

                    //PRT_INFO("\nUART%d: %3d bytes.\n", UART_HOST, ui32XferSize);
//                  bIOShdr = false;
                    am_hal_uart_transfer(g_pvUART, &sWrite);
                    am_util_delay_ms(1);
                }
            }
        }
        else if (g_bRxTimeoutFlag)
        {
            //
            // UART Buffer can contain more than one packet.
            // This outer loop parses
            //
            PRT_DATA("UART Data Size    : %d\n", g_ui32UARTRxIndex);

            uint32_t ui32UARTBufOffset = 0;
            while ((ui32UARTBufOffset < g_ui32UARTRxIndex) && (g_ui32UARTRxIndex > 0))
            {
                am_secboot_wired_msghdr_t *pHdr = (am_secboot_wired_msghdr_t *)&g_psWriteData.bytes[ui32UARTBufOffset];
                uint32_t ui32PktLength = pHdr->length;

                PRT_DATA("Packet Length     : %d\n", ui32PktLength);

                if ( 0 == ui32PktLength )
                {
                    break;
                }

                for (uint32_t index = 0; index < ui32PktLength; index += sizeof(g_IosPktData.data))
                {
                    PRT_DATA("index             : %d\n", index);

                    g_IosPktData.header.bStart = 0;
                    g_IosPktData.header.bEnd = 0;
                    g_IosPktData.header.length = 0;

                    //
                    // If this is the first packet, then set the Start flag.
                    //
                    if ( 0 == index )
                    {
                        g_IosPktData.header.bStart = 1;
                    }

                    //
                    // If this this the last packet, then set the End flag.
                    //
                    if ((index + sizeof(g_IosPktData.data)) >= ui32PktLength)
                    {
                        g_IosPktData.header.bEnd = 1;
                    }

                    //
                    // Build and Send the next packet.
                    //
                    g_IosPktData.header.length = ((ui32PktLength - index) < sizeof(g_IosPktData.data)) ? (ui32PktLength - index) : sizeof(g_IosPktData.data);
                    memcpy(&g_IosPktData.data[0], &g_psWriteData.bytes[index + ui32UARTBufOffset], g_IosPktData.header.length);
                    g_IosPktData.header.length += sizeof(am_secboot_ios_pkthdr_t);
                    bIosInt = false;

                    PRT_DATA("IOS Length        : %d\n", g_IosPktData.header.length);
                    PRT_DATA("IOS Start Bit     : %d\n", g_IosPktData.header.bStart);
                    PRT_DATA("IOS End Bit       : %d\n", g_IosPktData.header.bEnd);

                    ui32ByteCnt += g_IosPktData.header.length;
                    if ( bIOShdr )
                    {
                        PRT_DATA(", %d", g_IosPktData.header.length );
                    }
                    else
                    {
                        PRT_DATA("IOM  : sent bytes: %d (count=%d)", g_IosPktData.header.length, ui32ByteCnt );
                        bIOShdr = true;
                    }

                    if ( ui32DotCnt >= 20 )
                    {
                        PRT_INFO("\n");
                        ui32DotCnt = 0;
                    }
                    PRT_INFO("*");
                    ui32DotCnt++;

                    iom_slave_write(bSpi, IOSOFFSET_WRITE_CMD, (uint32_t*)&g_IosPktData, g_IosPktData.header.length);

                    //
                    // Wait for the GPIO Interrupt before sending the next packet.
                    //
                    for (uint32_t timeout = 0; timeout < 10000; timeout++)
                    {
                        if ( bIosInt )
                        {
                            PRT_DATA("Received Handshake for next packet\n");
                            break;
                        }
                        else
                        {
                            am_util_delay_us(1);
                        }
                    }

                    if ( !bIosInt )
                    {
                        PRT_DATA("Timed out waiting for Handshake signal\n");
                    }
                }
                ui32UARTBufOffset += ui32PktLength;
            }

            g_ui32UARTRxIndex = 0;
            g_bRxTimeoutFlag = false;
            NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + UART_HOST));
        }
    }
}

