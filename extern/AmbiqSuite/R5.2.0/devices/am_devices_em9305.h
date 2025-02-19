//*****************************************************************************
//
//! @file am_devices_em9305.h
//!
//! @brief Support functions for the EM Micro EM9305 BLE radio.
//!
//! @addtogroup em9305 EM9305 BLE Radio Device Driver
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_DEVICES_EM9305_H
#define AM_DEVICES_EM9305_H

#include <stdbool.h>
#include "am_bsp.h"

#if !defined(AM_BSP_EM9305_SPI_MODULE)
#define AM_BSP_EM9305_SPI_MODULE            6
#define AM_BSP_GPIO_EM9305_INT              117
#define AM_BSP_EM9305_RADIO_INT_CHNL        0

#endif  // !defined(AM_BSP_EM9305_SPI_MODULE)

#define SPI_MODULE  AM_BSP_EM9305_SPI_MODULE

#if !defined(AM_BSP_EM9305_RADIO_INT_CHNL) || !defined(AM_BSP_EM9305_RADIO_INT_IRQ) || !defined(AM_BSP_EM9305_RADIO_INT_ISR)
    #error "must define AM_BSP_EM9305_RADIO_INT_CHNL and AM_BSP_EM9305_RADIO_INT_IRQ in BSP"
#endif

#define GPIO_INT_CHANNEL    AM_BSP_EM9305_RADIO_INT_CHNL
#define GPIO_INT_IRQ        AM_BSP_EM9305_RADIO_INT_IRQ
#define GPIO_INT_ISR        AM_BSP_EM9305_RADIO_INT_ISR
#define GPIO_INT            AM_BSP_GPIO_EM9305_INT

#define AM_DEVICES_EM9305_MAX_DEVICE_NUM        1

// 255 data + 3 header
#define EM9305_MAX_RX_PACKET                    258

// EM9305 NVM INFO Page1 is 8KB, ddress range is 0x402000 ~ 0x403FFF
#define EM9305_NVM_INFO_PAGE1_START_ADDR        0x402000
// BLE firmware version number length
#define EM9305_FW_VER_NUM_LEN                   4
// Invalid version number, it's the default value from flash
#define EM9305_FW_VER_INVALID                   0xFFFFFFFF
// The data length to read from NVM INFO Page 1
#define EM9305_NVM_INFO_READ_LEN                248
// NVM Main area
#define EM9305_NVM_MAIN_AREA                    0
// NVM Information area
#define EM9305_NVM_INFO_AREA                    1
// NVM Infor area page 1
#define EM9305_NVM_INFO_AREA_PAGE_1             1

#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
#define UINT32_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT32_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
#define UINT32_TO_BYTE2(n)        ((uint8_t) ((n) >> 16))
#define UINT32_TO_BYTE3(n)        ((uint8_t) ((n) >> 24))
#define UINT32_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8)), ((uint8_t)((n) >> 16)), ((uint8_t)((n) >> 24))


#define am_devices_em9305_buffer(A)                                                  \
    union                                                                     \
    {                                                                         \
        uint32_t words[(A + 3) >> 2];                                         \
        uint8_t bytes[A];                                                     \
    }
#define WHILE_TIMEOUT_MS(expr, limit, timeout)     \
        {                                          \
            uint32_t ui32Timeout = 0;              \
            while (expr)                           \
            {                                      \
                if (ui32Timeout == (limit * 1000)) \
                {                                  \
                    timeout = 1;                   \
                    break;                         \
                }                                  \
                am_util_delay_us(1);               \
                ui32Timeout++;                     \
            }                                      \
        }

#define am_devices_em9305_buffer(A)               \
    union                                         \
    {                                             \
        uint32_t words[(A + 3) >> 2];             \
        uint8_t bytes[A];                         \
    }

#define AM_DEVICES_EM9305_RAW                      0x0
#define AM_DEVICES_EM9305_CMD                      0x1

#define HCI_VSC_CMD_HEADER_LENGTH                      4
#define HCI_VSC_CMD_LENGTH(n)                          (HCI_VSC_CMD_HEADER_LENGTH + n)

#define HCI_VSC_CMD(CMD, ...)                          {AM_DEVICES_EM9305_CMD, UINT16_TO_BYTE0(CMD##_CMD_OPCODE), UINT16_TO_BYTE1(CMD##_CMD_OPCODE), CMD##_CMD_LENGTH, ##__VA_ARGS__}
#define HCI_RAW_CMD(OPCODE, LEN, ...)                  {AM_DEVICES_EM9305_CMD, UINT16_TO_BYTE0(OPCODE), UINT16_TO_BYTE1(OPCODE), LEN, ##__VA_ARGS__}

#define HCI_VSC_ENTER_CM_CMD_LENGTH                     0
#define HCI_VSC_CRC_CALCULATE_CMD_LENGTH                8
#define HCI_VSC_NVM_ERASE_PAGE_CMD_LENGTH               2
#define HCI_VSC_SET_TX_POWER_LEVEL_CMD_LENGTH           1
#define HCI_VSC_NVM_ERASE_NVM_MAIN_CMD_LENGTH           0
#define HCI_VSC_START_TRANSMIT_TEST_CMD_LENGTH          6
#define HCI_VSC_END_TRANSMIT_TEST_CMD_LENGTH            0
#define HCI_VSC_SET_SLEEP_OPTION_CMD_LENGTH             1
#define HCI_VSC_SET_DEV_PUB_ADDR_CMD_LENGTH             6
#define HCI_VSC_READ_AT_ADDRESS_CMD_LENGTH              5
#define HCI_VSC_WRITE_AT_ADDRESS_CMD_LENGTH             134
#define HCI_VSC_ENTER_SLEEP_CMD_LENGTH                  0



typedef enum
{
    HCI_VSC_CRC_CALCULATE_CMD_OPCODE                 = 0xFC4E,
    HCI_VSC_ENTER_CM_CMD_OPCODE                      = 0xFC4F,
    HCI_VSC_EXIT_CM_CMD_OPCODE                       = 0xFC50,
    HCI_VSC_READ_AT_ADDRESS_CMD_OPCODE               = 0xFD01,
    HCI_VSC_WRITE_AT_ADDRESS_CMD_OPCODE              = 0xFD03,
    HCI_VSC_NVM_ERASE_NVM_MAIN_CMD_OPCODE            = 0xFD06,
    HCI_VSC_NVM_ERASE_PAGE_CMD_OPCODE                = 0xFD07,
    //Vendor Specific Command set device public address
    HCI_VSC_SET_DEV_PUB_ADDR_CMD_OPCODE              = 0xFC43,
    //Vendor Specific Command set sleep option
    HCI_VSC_SET_SLEEP_OPTION_CMD_OPCODE              = 0xFC49,
    //Vendor Specific Command start transmitter test
    HCI_VSC_START_TRANSMIT_TEST_CMD_OPCODE           = 0xFCC1,
    //Vendor Specific Command end transmitter test
    HCI_VSC_END_TRANSMIT_TEST_CMD_OPCODE             = 0xFCC2,
    //Vendor Specific Command set Tx power level
    HCI_VSC_SET_TX_POWER_LEVEL_CMD_OPCODE            = 0xFCC4,
    //! Ambiq Vendor Specific Command trigger Apollo5B and EM9305 to enter sleep
    HCI_VSC_ENTER_SLEEP_CMD_OPCODE                   = 0xFCF0,
    // Set local supported feature
    HCI_VSC_SET_LOCAL_SUP_FEAT_CMD_OPCODE            = 0xFFF2,
}vsc_opcode;

//
//! access types
//
typedef enum
{
    //! 8 bit access types
    RD_8_Bit                   = 8,
    //! 16 bit access types
    RD_16_Bit                  = 16,
    //! 32 bit access types
    RD_32_Bit                  = 32

}eMemAccess_type;


//*****************************************************************************
//
// EM9305 device structure
//
//*****************************************************************************
typedef struct
{
    // IOM Module
    uint32_t ui32Module;
    // BLE IOM handle
    void     *pIomHandle;
    // Mark if BLE IOM bus is occupied
    bool     bOccupied;
    // Mark if the BLE interface is busy
    bool     bBusy;
}
am_devices_em9305_t;

typedef enum
{
    AM_DEVICES_EM9305_STATUS_SUCCESS = 0,
    AM_DEVICES_EM9305_STATUS_ERROR,
    AM_DEVICES_EM9305_STATUS_BUS_BUSY,
    AM_DEVICES_EM9305_TX_BUSY,
    AM_DEVICES_EM9305_RESET_SPI_RDY_HIGH,
    AM_DEVICES_EM9305_RESET_FAIL,
    AM_DEVICES_EM9305_NOT_READY,
    AM_DEVICES_EM9305_NO_DATA_TX,
    AM_DEVICES_EM9305_RX_FULL,
    AM_DEVICES_EM9305_DATA_LENGTH_ERROR,
    AM_DEVICES_EM9305_CMD_TRANSFER_ERROR,
    AM_DEVICES_EM9305_DATA_TRANSFER_ERROR,
    AM_DEVICES_EM9305_STATUS_INVALID_OPERATION,
    AM_DEVICES_EM9305_INSUFFICIENT_LEN ,
    AM_DEVICES_EM9305_CKECKSUM_ERROR ,
    AM_DEVICES_EM9305_SEND_CMD_FAIL ,
    AM_DEVICES_EM9305_INCORRECT_DATA_TYPE,
    AM_DEVICES_EM9305_GET_MEM_INFO_FAIL,
    AM_DEVICES_EM9305_ALREADY_PROGRAMMED,

} am_devices_em9305_status_t;

typedef struct
{
    uint32_t* pNBTxnBuf;
    uint32_t ui32NBTxnBufLength;
} am_devices_em9305_config_t;

typedef struct
{
    const uint8_t *data;
    uint32_t length;
    uint32_t address;
} ImageRecord ;

typedef struct
{
    uint8_t page;
    uint8_t area;
}NvmPage;

extern void am_devices_em9305_config_pins(void);
extern uint32_t am_devices_em9305_init(uint32_t ui32Module,
                                       am_devices_em9305_config_t *pDevConfig,
                                       void **ppHandle,
                                       void **ppIomHandle);
extern uint32_t am_devices_em9305_term(void* pHandle);
extern uint32_t am_devices_em9305_block_write(void *pHandle,
                                              uint8_t *pui8Values,
                                              uint32_t ui32NumBytes);
extern uint32_t am_devices_em9305_block_read(void *pHandle,
                                             uint32_t *pui32Values,
                                             uint32_t *ui32NumBytes);
extern void am_devices_em9305_enable_interrupt(void);
extern void am_devices_em9305_disable_interrupt(void);
extern uint32_t am_devices_em9305_command_write(void* pHandle, uint32_t* pui32Cmd, uint32_t ui32Length, uint32_t* pui32Response, uint32_t* pui32BytesReceived);
extern uint32_t am_devices_em9305_update_image(void *pHandle, bool force_update);
extern uint32_t am_devices_em9305_reset(void *pHandle);
extern bool am_devices_em9305_get_FwImage(ImageRecord *pFwImage[], uint8_t record_size, uint32_t total_image_len);
extern bool am_devices_em9305_erase_page(NvmPage *pErasePage, uint8_t size);
uint32_t am_devices_em9305_get_fw_verion(void *pHandle, uint32_t *image_ver);
uint32_t am_devices_em9305_upate_fw_version(void *pHandle, uint32_t image_ver);
extern uint32_t enter_cm_mode(void *pHandle);
extern void am_devices_em9305_hsclk_req(bool enable);
void am_devices_em9305_disable(void);
void am_devices_em9305_set_pump_to_1p9v(bool flag);
uint32_t am_devices_em9305_irq_read(void);
bool am_devices_em9305_get_swap_spi_FwImage(ImageRecord *pFwImage[], uint8_t record_size, uint32_t total_image_len);
bool am_devices_em9305_swap_spi_erase_page(NvmPage *pErasePage, uint8_t size);
bool am_devices_em9305_spi_swap_version_enabled(void);
void am_devices_em9305_set_spi_bitbang_mode(bool bitbang_mode);
bool am_devices_em9305_spi_bitbang_enabled(void);
void am_devices_em9305_config_iom6_default(void);

//*****************************************************************************
//
//! @brief Enable the IOM bus
//!
//! This function enables IOM module before any HCI operation take effect
//!
//! @param pHandle  - BLE Controller device handler.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t am_devices_em9305_bus_enable(void* pHandle);

//*****************************************************************************
//
//! @brief Disable the IOM bus
//!
//! This function disables IOM module after HCI operation done to save power
//!
//! @param pHandle  - BLE Controller device handler.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t am_devices_em9305_bus_disable(void* pHandle);


#endif // AM_DEVICES_EM9305_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

