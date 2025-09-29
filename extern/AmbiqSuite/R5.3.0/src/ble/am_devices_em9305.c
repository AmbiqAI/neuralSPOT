//*****************************************************************************
//
//! @file am_devices_em9305.c
//!
//! @brief Support functions for the EM Micro EM9305 BTLE radio.
//!
//! @addtogroup em9305 EM9305 BTLE Radio Device Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
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
// This is part of revision release_sdk5p0p0-5f68a8286b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "am_devices_em9305.h"
#include "am_util_debug.h"
#include "am_util_stdio.h"
#include "am_util_delay.h"

#include "wsf_types.h"
#include "bstream.h"

//*****************************************************************************
//
// Macro definitions
//
//*****************************************************************************
#define USE_IOM_NONBLOCKING

//! SPI header byte
#define EM9305_SPI_HEADER_TX                    0x42
//! SPI header byte
#define EM9305_SPI_HEADER_RX                    0x81
//! SPI header byte
#define EM9305_STS1_READY_VALUE                 0xC0
//! check EM9305 status cnt
#define EM9305_STS_CHK_CNT_MAX                  10

//! EM9305 timeout value.
#define AM_DEVICES_EM9305_TIMEOUT               12000   // Assume worst case cold start counter (1.2 sec)

//! Length of RX buffer
#define EM9305_BUFFER_SIZE                      259     // Including 255 maximum HCI command + 4 HCI header

//! Select the EM9305 -> Set the CSN to low level
#define EM9305_SPISLAVESELECT()                 am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_CS, AM_HAL_GPIO_OUTPUT_CLEAR);

//! Deselect the EM9305 -> Set the CSN to high level
#define EM9305_SPISLAVEDESELECT()               am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_CS, AM_HAL_GPIO_OUTPUT_SET);

//! Indicates the EM9305 RDY pin state
#define EM9305_RDY_INT()                        (am_hal_gpio_input_read(AM_BSP_GPIO_EM9305_INT))

// The maximum length of command to write
#define CMD_LEN_MAX                             (256)
// Maximum packet size sent to EM9305 each time
#define PACKET_SIZE                             (248)

#if !defined(AM_BSP_GPIO_EM9305_32K_CLK)
#define AM_BSP_GPIO_EM9305_CS               149
#define AM_BSP_GPIO_EM9305_INT              117
#define AM_BSP_GPIO_AP5_12M_CLKREQ          136
#define AM_BSP_GPIO_EM9305_32K_CLK          138
#define AM_BSP_GPIO_EM9305_CM               15
#define AM_BSP_GPIO_EM9305_EN               93
//*****************************************************************************
//
// EM9305_32K_CLK (138) - EM9305 32k Clock Export
//
//*****************************************************************************
const am_hal_gpio_pincfg_t g_AM_BSP_GPIO_EM9305_32K_CLK =
{
    .GP.cfg_b.uFuncSel             = 3,
    .GP.cfg_b.eGPOutCfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .GP.cfg_b.eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .GP.cfg_b.eGPRdZero            = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .GP.cfg_b.eIntDir        = AM_HAL_GPIO_PIN_INTDIR_LO2HI
};
#endif

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
// SPI lock when a transmission is in progress
static uint8_t spiTxInProgress;

am_hal_iom_config_t     g_sIomEm9305CfgSPI =
{
    .eInterfaceMode       = AM_HAL_IOM_SPI_MODE,
    .ui32ClockFreq        = AM_HAL_IOM_16MHZ,
    .eSpiMode             = AM_HAL_IOM_SPI_MODE_0,
    .ui32NBTxnBufLength   = 0,
    .pNBTxnBuf = NULL,
};

am_devices_em9305_t gAmEm9305[AM_DEVICES_EM9305_MAX_DEVICE_NUM];

ImageRecord **g_sEM9305FwImage;

uint8_t g_sEM9305_record_size = 0;
uint32_t g_sEM9305_total_image_len = 0;
bool     g_bump_to_v1p9v = false;

NvmPage *g_erasePage;
uint32_t g_erasePage_num = 0;

// EM9305 Swap SPI image definition
ImageRecord **g_sEM9305SwapSpiFwImage;

uint8_t g_sEM9305_swap_spi_record_size = 0;
uint32_t g_sEM9305_swap_spi_total_image_len = 0;
NvmPage *g_erasePages_swap_spi;
uint32_t g_erasePage_num_swap_spi = 0;

//*****************************************************************************
//
//! @brief Configure EM9305 pins.
//!
//! @param None.
//!
//! This function initializes the GPIOs for communication with the EM9305.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_em9305_config_pins(void)
{

    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_CS, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_INT, am_hal_gpio_pincfg_input);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_EN, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_CM, am_hal_gpio_pincfg_input);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_AP5_12M_CLKREQ, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_32K_CLK, g_AM_BSP_GPIO_EM9305_32K_CLK);

#if defined(AM_BSP_EM9305_INT_PULLDOWN)
    // pull down the AM_BSP_GPIO_EM9305_INT before enabling the EM9305
    am_hal_gpio_pincfg_t pull_down_config = am_hal_gpio_pincfg_input;
    pull_down_config.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLDOWN_50K;
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_INT, pull_down_config);
#endif

    am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_CS, AM_HAL_GPIO_OUTPUT_SET);
    am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_EN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_state_write(AM_BSP_GPIO_AP5_12M_CLKREQ, AM_HAL_GPIO_OUTPUT_CLEAR);
}

//*****************************************************************************
//
// SPI bit bang implementation
//
//*****************************************************************************
#define SPI_MISO                                62
#define SPI_MOSI                                63
#define MRAM_INFO1_VAL_DEFAULT                  (0x00)
#define MRAM_INFO1_EM9305_SPI_SWAP_VER          (0x55AA0001)
#define MRAM_INFO1_EM9305_SPI_FIX_VER           (0x55AA0002)
#define MRAM_INFO1_EM9305_SPI_SWAP_VER_ADDR     (0x4200333C)

bool spi_bit_bang_enable = 0;

void SPI_write_byte(uint8_t byte)
{
    uint8_t bit;

    for (bit = 0; bit < 8; bit++)
    {
        //
        // write MOSI on falling edge of previous clock
        //
        if (byte & 0x80)
        {
            am_hal_gpio_output_set(SPI_MOSI);
        }
        else
        {
            am_hal_gpio_output_clear(SPI_MOSI);
        }

        am_hal_gpio_output_set(AM_BSP_GPIO_IOM6_SCK);

        am_hal_gpio_output_clear(AM_BSP_GPIO_IOM6_SCK);

        byte <<= 1;
    }
    am_hal_gpio_output_clear(SPI_MOSI);
}

uint8_t SPI_read_byte(void)
{
    uint8_t bit;
    uint8_t tdata = 0;

    for (bit = 0; bit < 8; bit++)
    {
        tdata = tdata << 1;
        if ( am_hal_gpio_input_read(SPI_MISO) == 1 )
        {
            tdata = tdata | 0x01;
        }
        else
        {
            tdata = tdata & 0xfe;
        }

        am_hal_gpio_output_set(AM_BSP_GPIO_IOM6_SCK);

        am_hal_gpio_output_clear(AM_BSP_GPIO_IOM6_SCK);
    }
    return tdata;
}

//
// This is to check if EM9305 BLE firmware is the swapping SPIS_MOSI and SPIS_MISO version
// For the workaround version (swapping SPIS_MOSI and SPIS_MISO): 0x55AA0001
// For the next version (SIP package version 2): 0x55AA0002
//
bool am_devices_em9305_spi_swap_version_enabled(void)
{
#if !defined(AM_BSP_EM9305_NO_SWAP)
    uint32_t *p_addr = (uint32_t *)MRAM_INFO1_EM9305_SPI_SWAP_VER_ADDR;

    am_util_stdio_printf("EM9305 fw flag:0x%x\r\n", *p_addr);
    if ( (*p_addr == MRAM_INFO1_EM9305_SPI_SWAP_VER)
          || (*p_addr == MRAM_INFO1_VAL_DEFAULT) )
    {
        return true;
    }
    else if ( *p_addr == MRAM_INFO1_EM9305_SPI_FIX_VER )
    {
        return false;
    }
#endif // (AM_BSP_EM9305_NO_SWAP)
    return false;
}

// Set the local flag to indicate if SPI bitbang mode is enabled
void am_devices_em9305_set_spi_bitbang_mode(bool bitbang_mode)
{
    spi_bit_bang_enable = bitbang_mode;
}

// Check if SPI bitbang mode is enabled or not
bool am_devices_em9305_spi_bitbang_enabled(void)
{
    return spi_bit_bang_enable;
}

void am_devices_em9305_reconfig_pins(void)
{
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM6_SCK, am_hal_gpio_pincfg_output);
    am_hal_gpio_pinconfig(SPI_MISO, am_hal_gpio_pincfg_input);
    am_hal_gpio_pinconfig(SPI_MOSI, am_hal_gpio_pincfg_output);

    am_hal_gpio_state_write(AM_BSP_GPIO_IOM6_SCK, AM_HAL_GPIO_OUTPUT_CLEAR);

    am_hal_gpio_state_write(SPI_MOSI, AM_HAL_GPIO_OUTPUT_CLEAR);
}

// The default configure of IOM6 SCK/MISO/MOSI
void am_devices_em9305_config_iom6_default(void)
{
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM6_SCK,  g_AM_BSP_GPIO_IOM6_SCK);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM6_MISO, g_AM_BSP_GPIO_IOM6_MISO);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_IOM6_MOSI, g_AM_BSP_GPIO_IOM6_MOSI);
}

void am_devices_em9305_enable_interrupt(void)
{
    uint32_t IntNum = AM_BSP_GPIO_EM9305_INT;
    am_hal_gpio_interrupt_control(AM_HAL_GPIO_INT_CHANNEL_0,
                                  AM_HAL_GPIO_INT_CTRL_INDV_ENABLE,
                                  (void *)&IntNum);
}

void am_devices_em9305_disable_interrupt(void)
{
    uint32_t IntNum = AM_BSP_GPIO_EM9305_INT;
    am_hal_gpio_interrupt_control(AM_HAL_GPIO_INT_CHANNEL_0,
                                  AM_HAL_GPIO_INT_CTRL_INDV_DISABLE,
                                  (void *)&IntNum);
}

void am_devices_em9305_disable(void)
{
    am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_EN, AM_HAL_GPIO_OUTPUT_CLEAR);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_32K_CLK, am_hal_gpio_pincfg_disabled);
}

//*****************************************************************************
//
//! @brief EM9305 SPI/IOM initialization function.
//!
//! @param ui32Module is the IOM module to be used for EM9305.
//! @param psIomConfig is the configuration information for the IOM.
//!
//! This function initializes the IOM for operation with the EM9305.
//!
//! @return None.
//
//*****************************************************************************
uint32_t
am_devices_em9305_init(uint32_t ui32Module, am_devices_em9305_config_t *pDevConfig, void **ppHandle, void **ppIomHandle)
{
    void *pIomHandle;
    am_hal_iom_config_t     stIOMEM9305Settings;
    uint32_t      ui32Index = 0;

    // Allocate a vacant device handle
    for ( ui32Index = 0; ui32Index < AM_DEVICES_EM9305_MAX_DEVICE_NUM; ui32Index++ )
    {
        if ( gAmEm9305[ui32Index].bOccupied == false )
        {
            break;
        }
    }
    if ( ui32Index == AM_DEVICES_EM9305_MAX_DEVICE_NUM )
    {
        return AM_DEVICES_EM9305_STATUS_ERROR;
    }

    if ( (ui32Module > AM_REG_IOM_NUM_MODULES)  || (pDevConfig == NULL) )
    {
        return AM_DEVICES_EM9305_STATUS_ERROR;
    }

    //
    // Configure the IOM pins.
    //
    am_bsp_iom_pins_enable(ui32Module, AM_HAL_IOM_SPI_MODE);

    //
    // Configure the EM9305 pins
    //
    am_devices_em9305_config_pins();

    stIOMEM9305Settings = g_sIomEm9305CfgSPI;
    stIOMEM9305Settings.ui32NBTxnBufLength = pDevConfig->ui32NBTxnBufLength;
    stIOMEM9305Settings.pNBTxnBuf = pDevConfig->pNBTxnBuf;

    //
    // Initialize the IOM instance.
    //
    if (am_hal_iom_initialize(ui32Module, &pIomHandle) ||
        am_hal_iom_power_ctrl(pIomHandle, AM_HAL_SYSCTRL_WAKE, false) ||
        am_hal_iom_configure(pIomHandle, &stIOMEM9305Settings) ||
        am_hal_iom_enable(pIomHandle))
    {
        return AM_DEVICES_EM9305_STATUS_ERROR;
    }

    gAmEm9305[ui32Index].bOccupied = true;
    gAmEm9305[ui32Index].bBusy = true;
    gAmEm9305[ui32Index].ui32Module = ui32Module;
    *ppIomHandle = gAmEm9305[ui32Index].pIomHandle = pIomHandle;
    *ppHandle = (void *)&gAmEm9305[ui32Index];
    am_devices_em9305_bus_disable(*ppHandle);

    // Reset EM9305 after device initialization.
    uint32_t ui32Status = am_devices_em9305_reset(*ppHandle);
    am_util_debug_printf("EM9305 reset status =%d \r\n", ui32Status);

    uint32_t image_version = 0;
    ui32Status = am_devices_em9305_get_fw_verion(*ppHandle, &image_version);

    if ( (ui32Status == AM_DEVICES_EM9305_STATUS_SUCCESS)
         && (image_version!=EM9305_FW_VER_INVALID) )
    {
        am_util_stdio_printf("BLE FW Ver: %d.%d.%d.%d\r\n", (image_version & 0xFF000000) >> 24, (image_version & 0xFF0000) >> 16,
                                    (image_version & 0xFF00) >> 8, image_version & 0xFF);
    }
    // Default not using 1.9v voltage during programming.
    am_devices_em9305_set_pump_to_1p9v(false);
    //
    // Return the status.
    //
    return AM_DEVICES_EM9305_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  De-Initialize the BLE controller driver.
//
//*****************************************************************************
uint32_t
am_devices_em9305_term(void* pHandle)
{
    am_devices_em9305_t* pIom = (am_devices_em9305_t*)pHandle;

    if ( pIom->ui32Module > AM_REG_IOM_NUM_MODULES )
    {
        return AM_DEVICES_EM9305_STATUS_ERROR;
    }
    // Disable the pins
    am_bsp_iom_pins_disable(pIom->ui32Module, AM_HAL_IOM_SPI_MODE);
    //
    // Disable the IOM.
    //
    am_hal_iom_disable(pIom->pIomHandle);

    //
    // Disable power to and uninitialize the IOM instance.
    //
    if ( pIom->bBusy )
    {
        am_hal_iom_power_ctrl(pIom->pIomHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);
    }
    pIom->bBusy = false;
    am_hal_iom_uninitialize(pIom->pIomHandle);

    // Free this device handle
    pIom->bOccupied = false;

    // Return the status.
    //
    return AM_DEVICES_EM9305_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief Start a transmit transaction to the EM9305.
//!
//! @param psDevice is a pointer to a device structure describing the EM9305.
//!
//! This function handles the initial handshake of asserting the SPI CE and
//! then waiting for the RDY signal from the EM.  It then executes a transmit
//! command to the EM9305 and receives the number of bytes that can be accepted.
//!
//! @return Number of bytes that can be written to EM9305.
//
//*****************************************************************************
uint8_t
am_devices_em9305_tx_starts(void *pHandle)
{
    am_hal_iom_transfer_t Transaction;
    am_devices_em9305_t *pIom = (am_devices_em9305_t *)pHandle;

    // Indicates that a SPI transfer is in progress
    spiTxInProgress = 1;

    am_hal_iom_buffer(2) sCommand;
    am_hal_iom_buffer(2) sStas;
    sCommand.bytes[0] = EM9305_SPI_HEADER_TX ;
    sCommand.bytes[1] = 0x00;

    // Select the EM9305
    EM9305_SPISLAVESELECT();

    // Wait EM9305 RDY signal or timeout
    for (uint32_t i = 0; i < AM_DEVICES_EM9305_TIMEOUT; i++)
    {
        if (EM9305_RDY_INT())
        {
            break;
        }
        am_util_delay_us(100);
    }

    if (!EM9305_RDY_INT())
    {
        return 0;
    }

    for (uint32_t i = 0; i < EM9305_STS_CHK_CNT_MAX; i++)
    {
        //
        // Select the EM9305
        //
        EM9305_SPISLAVESELECT();
#if defined(AM_PART_APOLLO4B)
        am_util_delay_us(200);
#endif

    if ( am_devices_em9305_spi_bitbang_enabled() )
    {
        // Read the data on MISO line at once after writing the data out,
        // and it can only read the second byte, the 0xC0 will be missed.
        SPI_write_byte(sCommand.bytes[0]);

        sStas.bytes[1] = SPI_read_byte();
        if ( sStas.bytes[1] != 0x00 )
        {
            break;
        }
    }
    else
    {
        memset(&Transaction, 0, sizeof(am_hal_iom_transfer_t));
        Transaction.ui8RepeatCount  = 0;
        Transaction.ui32PauseCondition = 0;
        Transaction.ui32StatusSetClr = 0;
        Transaction.eDirection      = AM_HAL_IOM_FULLDUPLEX;
        Transaction.ui32InstrLen    = 0;
        Transaction.ui32NumBytes    = 2;
        Transaction.pui32TxBuffer   = sCommand.words;
        Transaction.pui32RxBuffer   = sStas.words;
        Transaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_IOM0_CS_CHNL;
        Transaction.bContinue       = false;

        uint8_t ret;

        ret = am_hal_iom_spi_blocking_fullduplex(pIom->pIomHandle, &Transaction);

        if (AM_HAL_STATUS_SUCCESS != ret)
        {
            am_util_debug_printf("am_hal_iom_spi_blocking_fullduplex tx_starts ret =%d module =%d \n", ret, pIom->ui32Module);
            return 0;
        }
        //
        // Check if the EM9305 is ready and the rx buffer size is not zero.
        //
        if (( sStas.bytes[0] == EM9305_STS1_READY_VALUE ) && ( sStas.bytes[1] != 0x00 ))
        {
            break;
        }
        else
        {
            EM9305_SPISLAVEDESELECT();
        }
    }

#if defined(AM_PART_APOLLO4B)
        am_util_delay_us(100);
#endif
    }

    return sStas.bytes[1];
}

//*****************************************************************************
//
//! @brief End a transmit transaction to the EM9305.
//!
//! @param None.
//!
//! This function handles the completion of a transmit to the EM9305.  After
//! the IOM has completed the transaction, the CE is de-asserted and the RDY
//! interrupt is reenabled.
//!
//! @return None.
//
//*****************************************************************************
void
am_devices_em9305_tx_ends(void *pHandle)
{
    // Deselect the EM9305
    EM9305_SPISLAVEDESELECT();

    // Indicates that the SPI transfer is finished
    spiTxInProgress = 0;
}

//*****************************************************************************
//
//  Enable the IOM bus
//
//*****************************************************************************
uint32_t
am_devices_em9305_bus_enable(void* pHandle)
{
    am_devices_em9305_t* pBle = (am_devices_em9305_t*)pHandle;
    if ( pBle->bOccupied != true )
    {
        return AM_DEVICES_EM9305_STATUS_INVALID_OPERATION;
    }

    // Mark the BLE interface busy so it doesn't get used by more than one
    // interface.
    if (pBle->bBusy == false)
    {
        pBle->bBusy = true;
    }
    else
    {
        return AM_DEVICES_EM9305_STATUS_BUS_BUSY;
    }

    am_hal_iom_power_ctrl(pBle->pIomHandle, AM_HAL_SYSCTRL_WAKE, true);
    am_hal_iom_enable(pBle->pIomHandle);

    return AM_DEVICES_EM9305_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Disable the IOM bus
//
//*****************************************************************************
uint32_t
am_devices_em9305_bus_disable(void* pHandle)
{
    am_devices_em9305_t* pBle = (am_devices_em9305_t*)pHandle;
    if ( pBle->bOccupied != true )
    {
        return AM_DEVICES_EM9305_STATUS_INVALID_OPERATION;
    }
    if ( pBle->bBusy == false)
    {
      return AM_DEVICES_EM9305_STATUS_SUCCESS;
    }

    //
    // Disable the IOM.
    //
    am_hal_iom_disable(pBle->pIomHandle);
    //
    // Disable power.
    //
    am_hal_iom_power_ctrl(pBle->pIomHandle, AM_HAL_SYSCTRL_DEEPSLEEP, true);
    // Release the bus so someone else can use it.
    pBle->bBusy = false;

    return AM_DEVICES_EM9305_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief EM9305 write function.
//!
//! @param psDevice is a pointer to a device structure describing the EM9305.
//! @param type is the HCI command.
//! @param pui8Values is the HCI packet to send.
//! @param ui32NumBytes is the number of bytes to send (including HCI command).
//!
//! This function perform a write transaction to the EM9305.
//!
//! @return None.
//
//*****************************************************************************
uint32_t am_devices_em9305_block_write(void *pHandle,
                                       uint8_t *pui8Values,
                                       uint32_t ui32NumBytes)
{
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    am_hal_iom_buffer(EM9305_BUFFER_SIZE) sData;
    am_hal_iom_buffer(16) sStas;
    uint8_t em9305BufSize = 0;
    am_hal_iom_transfer_t       Transaction;
    am_devices_em9305_t *pIom = (am_devices_em9305_t *)pHandle;

    // Awake IOM and lock the bus
    ui32ErrorStatus = am_devices_em9305_bus_enable(pHandle);
    if (ui32ErrorStatus != AM_DEVICES_EM9305_STATUS_SUCCESS)
    {
        return ui32ErrorStatus;
    }

    if (ui32NumBytes <= EM9305_BUFFER_SIZE)
    {
        for ( uint32_t i = 0; i < ui32NumBytes; )
        {
            em9305BufSize = am_devices_em9305_tx_starts(pHandle);

            if ( em9305BufSize == 0x00 )
            {
                ui32ErrorStatus = AM_DEVICES_EM9305_RX_FULL;

                am_devices_em9305_tx_ends(pHandle);
                break;
            }
            uint32_t len = (em9305BufSize < (ui32NumBytes - i)) ? em9305BufSize : (ui32NumBytes - i);

            if ((len > 0) && (em9305BufSize))  // check again if there is room to send more data
            {
                memcpy(&(sData.bytes[0]), pui8Values + i, len);
                i += len;

                if ( am_devices_em9305_spi_bitbang_enabled() )
                {
                    for ( uint8_t wr_len = 0; wr_len < len; wr_len++ )
                    {
                        SPI_write_byte(sData.bytes[wr_len]);
                    }
                }
                else
                {
                    //
                    // Write to the IOM.
                    //
                    memset(&Transaction, 0, sizeof(am_hal_iom_transfer_t));
                    Transaction.eDirection      = AM_HAL_IOM_FULLDUPLEX;
                    Transaction.ui32NumBytes    = len;
                    Transaction.pui32TxBuffer   = sData.words;
                    Transaction.pui32RxBuffer   = sStas.words;
                    Transaction.bContinue       = false;
                    Transaction.ui8RepeatCount  = 0;
                    Transaction.ui32PauseCondition = 0;
                    Transaction.ui32StatusSetClr = 0;
                    Transaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_IOM0_CS_CHNL;

                    uint8_t ret;
                    ret = am_hal_iom_spi_blocking_fullduplex(pIom->pIomHandle, &Transaction);

                    if (AM_HAL_STATUS_SUCCESS != ret)
                    {
                        ui32ErrorStatus = AM_DEVICES_EM9305_DATA_TRANSFER_ERROR;
                        am_util_debug_printf("am_hal_iom_blocking_transfer TX ret =%d module =%d \n", ret, pIom->ui32Module);
                    }
                }
            }
            am_devices_em9305_tx_ends(pHandle);
        }
    }
    else
    {
        ui32ErrorStatus = AM_DEVICES_EM9305_DATA_LENGTH_ERROR;
        am_util_debug_printf("HCI TX Error (STATUS ERROR) Packet Too Large\n");
    }

    // Disable IOM to save power
    am_devices_em9305_bus_disable(pHandle);

    return ui32ErrorStatus;
}

//*****************************************************************************
//
//! @brief EM9305 read function.
//!
//! @param psDevice is a pointer to a device structure describing the EM9305.
//! @param pui8Values is the buffer to receive the HCI packet.
//! @param ui32NumBytes is the number of bytes to send (including HCI command).
//!
//! This function a read transaction from the EM9305.
//!
//! @return Number of bytes read.
//
//*****************************************************************************
uint32_t
am_devices_em9305_block_read(void *pHandle,
                             uint32_t *pui32Values,
                             uint32_t *ui32NumBytes)
{
    am_hal_iom_transfer_t Transaction;
    am_devices_em9305_t *pIom = (am_devices_em9305_t *)pHandle;
    am_hal_iom_buffer(2) sCommand;
    am_hal_iom_buffer(2) sStas;
    uint8_t ui8RxBytes;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;

    sCommand.bytes[0] = EM9305_SPI_HEADER_RX;
    sCommand.bytes[1] = 0x0;

    //
    // Check if the SPI is free
    //
    if (spiTxInProgress)
    {
        //
        // TX in progress -> Ignore RDY interrupt
        //
        am_util_debug_printf("EM9305 SPI TX in progress\n");
        return AM_DEVICES_EM9305_TX_BUSY;
    }

    //
    // Check if they are still data to read
    //
    if (!EM9305_RDY_INT())
    {
        // No data
       // am_util_debug_printf("BLE SPI No data\n");
        return AM_DEVICES_EM9305_NO_DATA_TX;
    }

    // Awake IOM and lock the bus
    ui32ErrorStatus = am_devices_em9305_bus_enable(pHandle);
    if (ui32ErrorStatus != AM_DEVICES_EM9305_STATUS_SUCCESS)
    {
        return ui32ErrorStatus;
    }

    do
    {
        if ( am_devices_em9305_spi_bitbang_enabled() )
        {
            EM9305_SPISLAVESELECT();

            SPI_write_byte(0x81);

            sStas.bytes[1] = SPI_read_byte();

            if ( ( sStas.bytes[1] == 0x00) || (sStas.bytes[1] == 0xff) )
            {
                EM9305_SPISLAVEDESELECT();
                am_util_debug_printf("EM9305 Not Ready sStas.byte0 = 0x%02x, sStas.byte1 = 0x%02x\n", sStas.bytes[0], sStas.bytes[1]);
                ui32ErrorStatus = AM_DEVICES_EM9305_NOT_READY;
                break;
            }

            //
            // Set the number of bytes to receive.
            //
            ui8RxBytes = sStas.bytes[1];

            if ( EM9305_RDY_INT() && (ui8RxBytes != 0) )
            {
                if ((*ui32NumBytes + ui8RxBytes) > EM9305_BUFFER_SIZE )
                {
                    //
                    // Error. Packet too large.
                    //
                    EM9305_SPISLAVEDESELECT();
                    am_util_debug_printf("HCI RX Error (STATUS ERROR) Packet Too Large %d, %d\n", sStas.bytes[0], sStas.bytes[1]);
                    ui32ErrorStatus = AM_DEVICES_EM9305_DATA_LENGTH_ERROR;
                    break;
                }

                uint8_t * p_rx_bytes = (uint8_t *)pui32Values + *ui32NumBytes;

                for ( uint8_t rx_bytes = 0; rx_bytes < ui8RxBytes; rx_bytes++ )
                {
                    *p_rx_bytes = SPI_read_byte();
                    p_rx_bytes++;
                }

                *ui32NumBytes += ui8RxBytes;
            }
        }
        else
        {
            for (uint32_t i = 0; i < EM9305_STS_CHK_CNT_MAX; i++)
            {
                //
                // Select the EM9305
                //
                EM9305_SPISLAVESELECT();

                memset(&Transaction, 0, sizeof(am_hal_iom_transfer_t));
                Transaction.ui8RepeatCount  = 0;
                Transaction.ui32PauseCondition = 0;
                Transaction.ui32StatusSetClr = 0;
                Transaction.eDirection      = AM_HAL_IOM_FULLDUPLEX;
                Transaction.ui32InstrLen    = 0;
                Transaction.ui32NumBytes    = 2;
                Transaction.pui32TxBuffer   = sCommand.words;
                Transaction.pui32RxBuffer   = sStas.words;
                Transaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_IOM0_CS_CHNL;
                Transaction.bContinue       = false;

                uint8_t ret;
                ret = am_hal_iom_spi_blocking_fullduplex(pIom->pIomHandle, &Transaction);

                //am_util_debug_printf("am_hal_iom_spi_blocking_fullduplex read ret = %d, byte 0:0x%xbyte 1:0x%x\n",ui32ErrorStatus, sStas.bytes[0],sStas.bytes[1]);
                if (AM_HAL_STATUS_SUCCESS != ret)
                {
                    EM9305_SPISLAVEDESELECT();
                    //am_util_stdio_printf("am_hal_iom_spi_blocking_fullduplex read ui32ErrorStatus =%d\n",ui32ErrorStatus);
                    ui32ErrorStatus = AM_DEVICES_EM9305_CMD_TRANSFER_ERROR;
                    break;
                }

                //
                // Check if the EM9305 is ready and the tx data size.
                //
                if (( sStas.bytes[0] == EM9305_STS1_READY_VALUE ) && ( sStas.bytes[1] != 0x00 ))
                {
                    break;
                }
                else
                {
                    EM9305_SPISLAVEDESELECT();
                }
            }

            //
            // Check that the EM9305 is ready or the receive FIFO is not full.
            //
            if (( sStas.bytes[0] != EM9305_STS1_READY_VALUE ) || ( sStas.bytes[1] == 0x00))
            {
                EM9305_SPISLAVEDESELECT();
                am_util_debug_printf("EM9305 Not Ready sStas.byte0 = 0x%02x, sStas.byte1 = 0x%02x\n", sStas.bytes[0], sStas.bytes[1]);
                ui32ErrorStatus = AM_DEVICES_EM9305_NOT_READY;
                break;
            }

            //
            // Set the number of bytes to receive.
            //
            ui8RxBytes = sStas.bytes[1];

            if ( EM9305_RDY_INT() && (ui8RxBytes != 0) )
            {
                if ((*ui32NumBytes + ui8RxBytes) > EM9305_BUFFER_SIZE )
                {
                    //
                    // Error. Packet too large.
                    //
                    EM9305_SPISLAVEDESELECT();
                    am_util_debug_printf("HCI RX Error (STATUS ERROR) Packet Too Large %d, %d\n", sStas.bytes[0], sStas.bytes[1]);
                    ui32ErrorStatus = AM_DEVICES_EM9305_DATA_LENGTH_ERROR;
                    break;
                }

                //
                // Read to the IOM.
                //
                memset(&Transaction, 0, sizeof(am_hal_iom_transfer_t));
                Transaction.ui32InstrLen    = 0;
                Transaction.eDirection      = AM_HAL_IOM_RX;
                Transaction.ui32NumBytes    = ui8RxBytes;
                Transaction.pui32RxBuffer   = (uint32_t *)((uint8_t *)pui32Values + *ui32NumBytes);
                Transaction.bContinue       = false;
                Transaction.ui8RepeatCount  = 0;
                Transaction.ui32PauseCondition = 0;
                Transaction.ui32StatusSetClr = 0;
                Transaction.uPeerInfo.ui32SpiChipSelect = AM_BSP_IOM0_CS_CHNL;

                uint8_t ret;
                ret = am_hal_iom_blocking_transfer(pIom->pIomHandle, &Transaction);

                if (AM_HAL_STATUS_SUCCESS != ret)
                {
                    EM9305_SPISLAVEDESELECT();
                    am_util_stdio_printf(" am_hal_iom_blocking_transfer AM_HAL_IOM_RX ret =%d\n", ret);
                    ui32ErrorStatus = AM_DEVICES_EM9305_DATA_TRANSFER_ERROR;
                    break;
                }
                else
                {
                    *ui32NumBytes += ui8RxBytes;
                }
            }
        }
        // Deselect the EM9305
        EM9305_SPISLAVEDESELECT();

    } while(EM9305_RDY_INT());

    // Disable IOM to save power
    am_devices_em9305_bus_disable(pHandle);

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Check the state of the IRQ pin.
//
//*****************************************************************************
uint32_t
am_devices_em9305_irq_read(void)
{
    return am_hal_gpio_input_read(AM_BSP_GPIO_EM9305_INT);
}

//*****************************************************************************
//
//! @brief Reset EM9305 BLE controller.
//
//*****************************************************************************
uint32_t am_devices_em9305_reset(void *pHandle)
{
    uint32_t pui32BytesReceived = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    am_devices_em9305_buffer(16) pui32Response = {0};
    uint8_t active_state_entered_evt[] = {0x04, 0xFF, 0x01, 0x01};

    if ( pHandle == NULL )
    {
        return AM_DEVICES_EM9305_STATUS_INVALID_OPERATION;
    }

    do
    {
        // Assert RESET to the EM9305 device.
        am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_EN, AM_HAL_GPIO_OUTPUT_CLEAR);

        am_util_delay_ms(1);

        am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_EN, AM_HAL_GPIO_OUTPUT_SET);

#if defined(AM_BSP_EM9305_READY_DELAY)
        // Wait for the EM9305 ready and run the code.
        am_util_delay_ms(2);
#endif
        // After the reset, the SPI_RDY signal will be pulled high first, which is the behavior of the hardware.
        // Therefore, we wait for the end of the reset process here.
        WHILE_TIMEOUT_MS((am_hal_gpio_input_read(AM_BSP_GPIO_EM9305_INT) == 1), 30, ui32ErrorStatus);
        if (ui32ErrorStatus)
        {
            ui32ErrorStatus = AM_DEVICES_EM9305_RESET_SPI_RDY_HIGH;
            break;
        }

        // Wait reset complete event from EM9305
        WHILE_TIMEOUT_MS((am_hal_gpio_input_read(AM_BSP_GPIO_EM9305_INT) == 0), 30, ui32ErrorStatus);
        if (ui32ErrorStatus)
        {
            ui32ErrorStatus = AM_DEVICES_EM9305_RESET_FAIL;
            break;
        }

        ui32ErrorStatus = am_devices_em9305_block_read(pHandle, pui32Response.words, &pui32BytesReceived);
        if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
        {
            if ( memcmp(pui32Response.bytes, active_state_entered_evt, sizeof(active_state_entered_evt)) == 0 )
            {
                am_util_stdio_printf("Enter active state \r\n");
            }
            else
            {
                ui32ErrorStatus = AM_DEVICES_EM9305_RESET_FAIL;
            }
        }
    }while (0);

    return ui32ErrorStatus;
}

//*****************************************************************************
//
//  Get EM9305 firmware image from local binary
//
//*****************************************************************************
bool am_devices_em9305_get_FwImage(ImageRecord *pFwImage[], uint8_t record_size, uint32_t total_image_len)
{
    if (pFwImage != NULL)
    {
        g_sEM9305FwImage = pFwImage;

        g_sEM9305_record_size = record_size;
        g_sEM9305_total_image_len = total_image_len;
    }

    return (pFwImage != NULL);
}

//*****************************************************************************
//
//  Get EM9305 erase page
//
//*****************************************************************************
bool am_devices_em9305_erase_page(NvmPage *pErasePage, uint8_t size)
{
    if (pErasePage != NULL)
    {
        g_erasePage = pErasePage;
        g_erasePage_num = size;
    }

    return (pErasePage != NULL);
}

//*****************************************************************************
//
//  Get EM9305 swap SPI firmware image from local binary
//
//*****************************************************************************
bool am_devices_em9305_get_swap_spi_FwImage(ImageRecord *pFwImage[], uint8_t record_size, uint32_t total_image_len)
{
    if (pFwImage != NULL)
    {
        g_sEM9305SwapSpiFwImage = pFwImage;

        g_sEM9305_swap_spi_record_size = record_size;
        g_sEM9305_swap_spi_total_image_len = total_image_len;
    }

    return (pFwImage != NULL);
}

//*****************************************************************************
//
//  Get EM9305 Swap SPI image erase page
//
//*****************************************************************************
bool am_devices_em9305_swap_spi_erase_page(NvmPage *pErasePage, uint8_t size)
{
    if (pErasePage != NULL)
    {
        g_erasePages_swap_spi = pErasePage;
        g_erasePage_num_swap_spi = size;
    }

    return (pErasePage != NULL);
}

//*****************************************************************************
//
//  Send HCI raw command to the BLE controller
//
//*****************************************************************************
uint32_t am_devices_em9305_command_write(void* pHandle, uint32_t* pui32Cmd, uint32_t ui32Length, uint32_t* pui32Response, uint32_t* pui32BytesReceived)
{
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;

    if ( !pui32Cmd || !pui32Response || !pui32BytesReceived )
    {
        return AM_DEVICES_EM9305_STATUS_INVALID_OPERATION;
    }

    do
    {
        ui32ErrorStatus = am_devices_em9305_block_write(pHandle,
                          (uint8_t *)pui32Cmd,
                          ui32Length);

        if (ui32ErrorStatus)
        {
            break;
        }

        // Wait for the response, and return it to the caller via our variable.
        WHILE_TIMEOUT_MS ( am_hal_gpio_input_read(AM_BSP_GPIO_EM9305_INT) == 0, 5000, ui32ErrorStatus );
        if (ui32ErrorStatus)
        {
            ui32ErrorStatus = AM_DEVICES_EM9305_CMD_TRANSFER_ERROR;
            break;
        }

        while(1)
        {
            ui32ErrorStatus = am_devices_em9305_block_read(pHandle, pui32Response, pui32BytesReceived);
            // Keep reading until we get the corresponding response
            if ((ui32ErrorStatus) || ((UINT32_TO_BYTE0(pui32Response[1]) == UINT32_TO_BYTE1(pui32Cmd[0])) && (UINT32_TO_BYTE1(pui32Response[1]) == UINT32_TO_BYTE2(pui32Cmd[0]))))
            {
                break;
            }
        }
    } while (0);

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Send vendor command to request the calculation of image data CRC32.
//
//*****************************************************************************
static uint32_t crc32_calculation_request(void *pHandle, uint32_t start_addr, uint32_t end_addr, uint32_t *crc)
{
    uint8_t write_cmd[CMD_LEN_MAX] = {0};
    uint8_t *p_write_cmd = write_cmd;
    uint32_t ui32BytesNum = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    am_devices_em9305_buffer(16) sResponse = {0};

    UINT8_TO_BSTREAM(p_write_cmd, AM_DEVICES_EM9305_CMD);
    UINT16_TO_BSTREAM(p_write_cmd, HCI_VSC_CRC_CALCULATE_CMD_OPCODE);
    UINT8_TO_BSTREAM(p_write_cmd, HCI_VSC_CRC_CALCULATE_CMD_LENGTH);
    UINT32_TO_BSTREAM(p_write_cmd, start_addr);
    UINT32_TO_BSTREAM(p_write_cmd, end_addr);

    ui32ErrorStatus = am_devices_em9305_command_write(pHandle, (uint32_t *)write_cmd,
                            (p_write_cmd - write_cmd), sResponse.words, &ui32BytesNum);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        if ( sResponse.bytes[6] == 0 )
        {
            uint8_t *p_rsp = &sResponse.bytes[7];
            BSTREAM_TO_UINT32(*crc, p_rsp);
        }
        else
        {
            am_util_stdio_printf("send crc32 calc fail, status: 0x%x, start addr:%x\r\n", sResponse.bytes[6], start_addr);

            ui32ErrorStatus = AM_DEVICES_EM9305_SEND_CMD_FAIL;
        }
    }

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Get the record size and fw image pointer.
//
//*****************************************************************************
void get_record_size_and_fw_image(uint32_t *rec_size, ImageRecord ***pFwImage)
{
    if ( am_devices_em9305_spi_bitbang_enabled() )
    {
        *rec_size = g_sEM9305_swap_spi_record_size;
        *pFwImage = g_sEM9305SwapSpiFwImage;
    }
    else
    {
        *rec_size = g_sEM9305_record_size;
        *pFwImage = g_sEM9305FwImage;
    }
}

//*****************************************************************************
//
// Check if EM9305 is already programmed according to the CRC32 of image data.
//
//*****************************************************************************
static bool check_programed(void *pHandle)
{
    uint32_t actual_crc = 0;
    uint32_t expected_crc = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    uint32_t record_size = 0;
    ImageRecord **fw_image = NULL;

    get_record_size_and_fw_image(&record_size, &fw_image);

    for ( uint8_t i = 0; i < record_size; i++ )
    {
        uint8_t* data = (uint8_t *)(fw_image[i]->data);
        uint32_t data_len = fw_image[i]->length;
        uint32_t start_addr = fw_image[i]->address;

        ui32ErrorStatus = crc32_calculation_request(pHandle, start_addr, (start_addr + data_len), &actual_crc);

        if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
        {
            // Get expected CRC32 values
            ui32ErrorStatus = am_hal_crc32((uint32_t)data, (uint32_t)data_len, &expected_crc);

            if ( ui32ErrorStatus == AM_HAL_STATUS_SUCCESS )
            {
                // Compare actual and expected CRC32 values
                if (actual_crc != expected_crc)
                {
                    am_util_stdio_printf("crc mismatch, actual_crc:0x%x,  expected_crc:0x%x\r\n", actual_crc, expected_crc);
                    return false;
                }
            }
            else
            {
                am_util_stdio_printf("calc CRC32 failed, status:%d\r\n", ui32ErrorStatus);
            }
        }
        else
        {
            am_util_stdio_printf("send crc32 calc failed, status:%d\r\n", ui32ErrorStatus);
            break;
        }
    }

    return true;
}

// 30KHz square wave period in microseconds
#define SQUARE_WAVE_PERIOD_US 33   // 1 / 30000 * 1000000

// Function to generate a 30KHz square wave for a specified duration
void generate_square_wave(uint32_t duration_ms)
{
    uint32_t i;

    // Configure GPIO15 as output and default low
    am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_CM, am_hal_gpio_pincfg_output);
    am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_CM, AM_HAL_GPIO_OUTPUT_CLEAR);

    for (i = 0; i < (duration_ms * 1000 / SQUARE_WAVE_PERIOD_US); i++)
    {
        am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_CM, AM_HAL_GPIO_OUTPUT_CLEAR);

        am_util_delay_us(SQUARE_WAVE_PERIOD_US / 2);

        am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_CM, AM_HAL_GPIO_OUTPUT_SET);

        am_util_delay_us(SQUARE_WAVE_PERIOD_US / 2);
    }
}

// Pulse GPIO EN to reset EM9305
static void em9305_pulse_gpio_en(void)
{
    am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_EN, AM_HAL_GPIO_OUTPUT_CLEAR);

    am_util_delay_ms(10);

    am_hal_gpio_state_write(AM_BSP_GPIO_EM9305_EN, AM_HAL_GPIO_OUTPUT_SET);
}

//*****************************************************************************
//
// Send vendor command to enter configuration mode.
// Generate 30KHz signal on EM GPIO5to enter Configuration Mode
//
//*****************************************************************************
uint32_t enter_cm_mode(void *pHandle)
{
    uint32_t pui32BytesReceived = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;

    am_devices_em9305_buffer(16) pui32Response = {0};

    uint8_t entered_cm_evt[] = {0x04, 0xFF, 0x01, 0x03};

    am_util_stdio_printf("Changing to Configuration Mode\r\n");

    em9305_pulse_gpio_en();

    do
    {
        // Output a 30KHz square wave on GPIO15 for 40ms
        generate_square_wave(40);

        // Check if SPI data is ready
        if ( am_hal_gpio_input_read(AM_BSP_GPIO_EM9305_INT) == 0 )
        {
            continue;
        }
        else
        {
            ui32ErrorStatus = am_devices_em9305_block_read(pHandle, pui32Response.words, &pui32BytesReceived);
            if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
            {
                // Check if it's the enter configure mode event
                if ( memcmp(pui32Response.bytes, entered_cm_evt, sizeof(entered_cm_evt)) == 0 )
                {
                    am_util_stdio_printf("Entered Configuration Mode\r\n");
                    break;
                }
            }
            else
            {
                am_util_stdio_printf("Failed to enter Configuration Mode, status:%d\r\n", ui32ErrorStatus);

                em9305_pulse_gpio_en();

                continue;
            }
        }
    }while(1);

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Send vendor command to erase NVM pages.
//
//*****************************************************************************
static uint32_t send_erase_page_cmd(void *pHandle, uint8_t area, uint8_t page)
{
    uint8_t write_cmd[CMD_LEN_MAX] = {0};
    uint8_t *p_write_cmd = write_cmd;
    uint32_t ui32BytesNum = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    am_devices_em9305_buffer(16) sResponse = {0};

    UINT8_TO_BSTREAM(p_write_cmd, AM_DEVICES_EM9305_CMD);
    UINT16_TO_BSTREAM(p_write_cmd, HCI_VSC_NVM_ERASE_PAGE_CMD_OPCODE);
    UINT8_TO_BSTREAM(p_write_cmd, HCI_VSC_NVM_ERASE_PAGE_CMD_LENGTH);
    UINT8_TO_BSTREAM(p_write_cmd, area);
    UINT8_TO_BSTREAM(p_write_cmd, page);

    ui32ErrorStatus = am_devices_em9305_command_write(pHandle, (uint32_t *)write_cmd,
                            (p_write_cmd - write_cmd), sResponse.words, &ui32BytesNum);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        if ( sResponse.bytes[6] != 0 )
        {
            ui32ErrorStatus = AM_DEVICES_EM9305_SEND_CMD_FAIL;
        }
    }

    return ui32ErrorStatus;
}


//*****************************************************************************
//
// Erase entire NVM main area,  It erases only the main area.
//
//*****************************************************************************
static uint32_t erase_entire_nvm_main_area(void *pHandle)
{
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;

    uint8_t write_cmd[CMD_LEN_MAX] = {0};
    uint8_t *p_write_cmd = write_cmd;
    uint32_t ui32BytesNum = 0;
    am_devices_em9305_buffer(16) sResponse = {0};

    am_util_stdio_printf("Erasing NVM main area...\r\n");

    UINT8_TO_BSTREAM(p_write_cmd, AM_DEVICES_EM9305_CMD);
    UINT16_TO_BSTREAM(p_write_cmd, HCI_VSC_NVM_ERASE_NVM_MAIN_CMD_OPCODE);
    UINT8_TO_BSTREAM(p_write_cmd, HCI_VSC_NVM_ERASE_NVM_MAIN_CMD_LENGTH);

    ui32ErrorStatus = am_devices_em9305_command_write(pHandle, (uint32_t *)write_cmd,
                            (p_write_cmd - write_cmd), sResponse.words, &ui32BytesNum);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        if ( sResponse.bytes[6] != 0 )
        {
            ui32ErrorStatus = AM_DEVICES_EM9305_SEND_CMD_FAIL;
        }
    }

    return ui32ErrorStatus;
}

static uint32_t read_data_cmd(void *pHandle, uint32_t address, uint8_t *data_out, uint8_t data_len)
{
    uint8_t read_cmd[CMD_LEN_MAX] = {0};
    uint8_t *p_read_cmd = read_cmd;
    uint32_t ui32BytesNum = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;

    if ( (pHandle == NULL) || (data_out == NULL) )
    {
        return AM_DEVICES_EM9305_STATUS_ERROR;
    }

    UINT8_TO_BSTREAM(p_read_cmd, AM_DEVICES_EM9305_CMD);
    UINT16_TO_BSTREAM(p_read_cmd, HCI_VSC_READ_AT_ADDRESS_CMD_OPCODE);
    UINT8_TO_BSTREAM(p_read_cmd, 4 + sizeof(data_len));
    UINT32_TO_BSTREAM(p_read_cmd, address);
    UINT8_TO_BSTREAM(p_read_cmd, data_len);

    am_devices_em9305_buffer(EM9305_MAX_RX_PACKET) sResponse = {0};

    ui32ErrorStatus = am_devices_em9305_command_write(pHandle, (uint32_t *)read_cmd,
                            (p_read_cmd - read_cmd), sResponse.words, &ui32BytesNum);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        if ( sResponse.bytes[6] != 0 )
        {
            am_util_stdio_printf("write data fail, status: 0x%x\r\n", sResponse.bytes[6]);

            ui32ErrorStatus = AM_DEVICES_EM9305_SEND_CMD_FAIL;
        }
        else
        {
            memcpy(data_out, &sResponse.bytes[7], data_len);
        }
    }

    return ui32ErrorStatus;
}

static uint32_t write_data_cmd(void *pHandle, uint32_t address, uint8_t *data, uint8_t data_len)
{
    uint8_t write_cmd[CMD_LEN_MAX] = {0};
    uint8_t *p_write_cmd = write_cmd;
    uint32_t ui32BytesNum = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;

    UINT8_TO_BSTREAM(p_write_cmd, AM_DEVICES_EM9305_CMD);
    UINT16_TO_BSTREAM(p_write_cmd, HCI_VSC_WRITE_AT_ADDRESS_CMD_OPCODE);
    UINT8_TO_BSTREAM(p_write_cmd, 4 + data_len);
    UINT32_TO_BSTREAM(p_write_cmd, address);
    memcpy(p_write_cmd, data, data_len);
    p_write_cmd += data_len;

    am_devices_em9305_buffer(16) sResponse = {0};

    ui32ErrorStatus = am_devices_em9305_command_write(pHandle, (uint32_t *)write_cmd,
                            (p_write_cmd - write_cmd), sResponse.words, &ui32BytesNum);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        if ( sResponse.bytes[6] != 0 )
        {
            am_util_stdio_printf("write data fail, status: 0x%x\r\n", sResponse.bytes[6]);

            ui32ErrorStatus = AM_DEVICES_EM9305_SEND_CMD_FAIL;
        }
    }

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Display upgrade progress.
//
//*****************************************************************************
static void display_progress(uint32_t total_sent, uint32_t total_size)
{
    uint32_t progress = (100*total_sent) / total_size;

    if ( progress % 10 == 0 )
    {
        am_util_stdio_printf("Sending %d %%\n", progress);
    }
}

//*****************************************************************************
//
// Write data record to NVM.
//
//*****************************************************************************
static uint32_t write_data_to_nvm(void *pHandle, uint32_t total_size)
{
    uint32_t total_sent = 0;
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    uint32_t record_size = 0;
    ImageRecord **fw_image = NULL;

    get_record_size_and_fw_image(&record_size, &fw_image);

    am_util_stdio_printf("Writing data...\r\n");

    for ( uint8_t i = 0; i < record_size; i++ )
    {
        uint32_t addr = fw_image[i]->address;
        uint8_t *data = (uint8_t *)(fw_image[i]->data);
        uint32_t remaining_data = fw_image[i]->length;

        while (remaining_data > 0)
        {
            int size_to_send = (remaining_data > PACKET_SIZE) ? PACKET_SIZE : remaining_data;

            ui32ErrorStatus = write_data_cmd(pHandle, addr, data, size_to_send);

            if (ui32ErrorStatus != AM_DEVICES_EM9305_STATUS_SUCCESS)
            {
                am_util_stdio_printf("Write data error.\n");
                return ui32ErrorStatus;
            }

            addr += size_to_send;
            data += size_to_send;
            remaining_data -= size_to_send;
            total_sent += size_to_send;

            if (total_size > 0)
            {
                display_progress(total_sent, total_size);
            }
        }
    }

    return ui32ErrorStatus;
}

static uint32_t check_data(void *pHandle)
{
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    uint32_t record_size = 0;
    ImageRecord **fw_image = NULL;

    get_record_size_and_fw_image(&record_size, &fw_image);

    am_util_stdio_printf("\r\nChecking data...\r\n");

    for ( uint8_t i = 0; i < record_size; i++ )
    {
        uint32_t actual_crc   = 0;
        uint32_t expected_crc = 0;
        uint32_t addr         = fw_image[i]->address;
        uint32_t data_len     = fw_image[i]->length;
        uint8_t *data         = (uint8_t *)(fw_image[i]->data);

        // Send CRC32 calculation request
        ui32ErrorStatus = crc32_calculation_request(pHandle, addr, addr + data_len, &actual_crc);

        // Check if CRC32 calculation was successful
        if (ui32ErrorStatus != AM_DEVICES_EM9305_STATUS_SUCCESS)
        {
            am_util_stdio_printf("Failed to calculate CRC32\n");
            break;
        }

        // Get expected CRC32 values
        ui32ErrorStatus = am_hal_crc32((uint32_t)data, data_len, &expected_crc);

        if ( ui32ErrorStatus == AM_HAL_STATUS_SUCCESS )
        {
            // Compare actual and expected CRC32 values
            if (actual_crc != expected_crc)
            {
                ui32ErrorStatus = AM_DEVICES_EM9305_CKECKSUM_ERROR;
                am_util_stdio_printf("Invalid CRC32 from 0x%08x to 0x%08x (0x%08x instead of 0x%08x)\n",
                       addr, addr + data_len, actual_crc, expected_crc);
                break;
            }
        }
        else
        {
            am_util_stdio_printf("Calc check CRC32 failed, status:%d, len:%d\r\n", ui32ErrorStatus, data_len);
        }
    }

    return ui32ErrorStatus;
}

// Modifying the settings of the DC-DC logic to maximize the voltage multiplier drive voltage
// and strength and set NVM programming voltage to maximum.
static uint32_t pump_to_1p9v(void *pHandle)
{
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    uint32_t register_value = 0;

    ui32ErrorStatus = read_data_cmd(pHandle, 0xF0040C, (uint8_t *)&register_value, 0x04);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        // Clearing address 0xF0040C, bits 12:8 sets the regulation target voltage to 1.9V.
        // This is setting a comparator monitoring the NVM write voltage to control regulation to 1.9V (available options are 1.7V or 1.9V).
        register_value &= 0xFFFFE0FF;

        ui32ErrorStatus = write_data_cmd(pHandle, 0xF0040C, (uint8_t *)&register_value, sizeof(register_value));
        if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
        {
            ui32ErrorStatus = read_data_cmd(pHandle, 0xF0041C, (uint8_t *)&register_value, 0x4);
            if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
            {
                // Writing decimal 985858 (0x000F0B02) to Addr 0xF0041C does the following:
                // Configures the internal DC-DC to V-Mult mode.
                // Sets doubler PWM output to 2X VBAT2 (available options are 1.5X or 2X).
                // Sets doubler PWM duty cycle to maximum (bits 19:16 = 0xF - strongest drive; available range weakest to strongest is 0x2 to 0xF).
                register_value = 985858;

                ui32ErrorStatus = write_data_cmd(pHandle, 0xF0041C, (uint8_t *)&register_value, sizeof(register_value));

                if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
                {
                    am_util_stdio_printf("Pump TO 1.9V\r\n");
                }
                else
                {
                    am_util_stdio_printf("write register 0xF0041C failed, status:%d\r\n", ui32ErrorStatus);
                }
            }
            else
            {
                am_util_stdio_printf("read register 0xF0041C failed, status:%d\r\n", ui32ErrorStatus);
            }
        }
        else
        {
            am_util_stdio_printf("write register 0xF0040C failed, status:%d\r\n", ui32ErrorStatus);
        }
    }
    else
    {
        am_util_stdio_printf("read register 0xF0040C failed, status:%d\r\n", ui32ErrorStatus);
    }

    return ui32ErrorStatus;
}

// Set a flag to indicate if using 1.9v for NVM programming.
void am_devices_em9305_set_pump_to_1p9v(bool flag)
{
    g_bump_to_v1p9v = flag;
}

uint32_t am_devices_em9305_update_image(void *pHandle, bool force_update)
{
    uint32_t ui32ErrorStatus = AM_DEVICES_EM9305_STATUS_SUCCESS;
    uint32_t image_size = 0;

    do
    {
        if ( am_devices_em9305_spi_bitbang_enabled() )
        {
            am_devices_em9305_reconfig_pins();
            image_size = g_sEM9305_swap_spi_total_image_len;
        }
        else
        {
            image_size = g_sEM9305_total_image_len;
        }

        // Enter configuration mode
        ui32ErrorStatus = enter_cm_mode(pHandle);
        if ( ui32ErrorStatus != AM_DEVICES_EM9305_STATUS_SUCCESS )
        {
            am_util_stdio_printf("Enter CM failed, status:%d\r\n", ui32ErrorStatus);
            break;
        }

        // Check if already programmed
        if ( force_update || !check_programed(pHandle) )
        {
            am_util_stdio_printf("Updating in progress ...\r\n");

            if ( g_bump_to_v1p9v )
            {
                pump_to_1p9v(pHandle);
            }

            // Erase flash
            ui32ErrorStatus = erase_entire_nvm_main_area(pHandle);

            if ( ui32ErrorStatus != AM_DEVICES_EM9305_STATUS_SUCCESS )
            {
                am_util_stdio_printf("Erase NVM main area failed, status:%d\r\n", ui32ErrorStatus);
                break;
            }

            // Write data
            ui32ErrorStatus = write_data_to_nvm(pHandle, image_size);
            if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
            {
                am_util_stdio_printf("Successfully written %d bytes, seconds\r\n", image_size);
            }
            else
            {
                am_util_stdio_printf("Write data failed, status:%d\r\n", ui32ErrorStatus);
                break;
            }

            // Check data.
            ui32ErrorStatus = check_data(pHandle);
            if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
            {
                am_util_stdio_printf("Check data success\r\n");
            }
            else
            {
                am_util_stdio_printf("Check data failed, status:%d\r\n", ui32ErrorStatus);
                break;
            }

        }
        else
        {
            am_util_stdio_printf("Already up to date\r\n");
            break;
        }
    }while(0);

    if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {
        // re-configure GPIO15 as input for 12MHz clock from EM9305
        am_hal_gpio_pinconfig(AM_BSP_GPIO_EM9305_CM, am_hal_gpio_pincfg_input);
    }

    return ui32ErrorStatus;
}

void am_devices_em9305_hsclk_req(bool enable)
{
    if ( enable )
    {
        am_hal_gpio_state_write(AM_BSP_GPIO_AP5_12M_CLKREQ, AM_HAL_GPIO_OUTPUT_SET);
    }
    else
    {
        am_hal_gpio_state_write(AM_BSP_GPIO_AP5_12M_CLKREQ, AM_HAL_GPIO_OUTPUT_CLEAR);
    }
}

// Read BLE firmware version number
uint32_t am_devices_em9305_get_fw_verion(void *pHandle, uint32_t *image_ver)
{
    return read_data_cmd(pHandle, EM9305_NVM_INFO_PAGE1_START_ADDR, (uint8_t*)image_ver, EM9305_FW_VER_NUM_LEN);
}

// Update firmware image version to NVM Info page1 after successfully programed
// The firmware version number is stored in the first 4 bytes in format EM SDK version.xx.xx, such as 3.1.1.0
uint32_t am_devices_em9305_upate_fw_version(void *pHandle, uint32_t image_ver)
{
    uint32_t status = AM_DEVICES_EM9305_STATUS_SUCCESS;

    // Read out data at first
    uint8_t read_out_data[EM9305_NVM_INFO_READ_LEN] = {0};

    status = read_data_cmd(pHandle, EM9305_NVM_INFO_PAGE1_START_ADDR, read_out_data, EM9305_NVM_INFO_READ_LEN);

    if ( status == AM_DEVICES_EM9305_STATUS_SUCCESS )
    {

        // Erase NVM Info Page 1 before writing
        status = send_erase_page_cmd(pHandle, EM9305_NVM_INFO_AREA, EM9305_NVM_INFO_AREA_PAGE_1);

        if ( status == AM_DEVICES_EM9305_STATUS_SUCCESS )
        {
            // Write the expected data after erasing NVM INFO Page 1
            memcpy(read_out_data, (uint8_t *)&image_ver, EM9305_FW_VER_NUM_LEN);

            status = write_data_cmd(pHandle, EM9305_NVM_INFO_PAGE1_START_ADDR, read_out_data, EM9305_NVM_INFO_READ_LEN);

            if ( status != AM_DEVICES_EM9305_STATUS_SUCCESS )
            {
                am_util_stdio_printf("Write NVM INFO Page 1 failed, status:%d\r\n", status);
            }
        }
        else
        {
            am_util_stdio_printf("Erase NVM INFO Page 1 failed, status:%d\r\n", status);
        }
    }
    else
    {
        am_util_stdio_printf("read NVM INFO Page 1 failed, status:%d\r\n", status);
    }

    return status;
}

// End Doxygen group.
//! @}
//
//*****************************************************************************

