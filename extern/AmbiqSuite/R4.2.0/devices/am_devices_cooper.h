//*****************************************************************************
//
//! @file am_devices_cooper.h
//!
//! @brief A re-implementation of the Apollo BLE interface using the IOM.
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
// This is part of revision release_sdk_4_2_0-0a31245a64 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_COOPER_H
#define AM_DEVICES_COOPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_util.h"
#include <stdlib.h>
//*****************************************************************************
//
// Type definitions.
//
//*****************************************************************************
#define am_devices_cooper_buffer(A)                                                  \
    union                                                                     \
    {                                                                         \
        uint32_t words[(A + 3) >> 2];                                         \
        uint8_t bytes[A];                                                     \
    }

//*****************************************************************************
//
// Print Errors.
//
//*****************************************************************************
#define PRINT_ERRORS(x)                                                       \
    if (x)                                                                    \
    {                                                                         \
        am_util_debug_printf("%s. Line %d ERROR: 0x%08x\n",                   \
                             __FILE__, __LINE__, (x));                        \
        while (1);                                                            \
    }

#define WHILE_TIMEOUT_MS(expr, limit, timeout)                                    \
        {                                                                         \
            uint32_t ui32Timeout = 0;                                             \
            while (expr)                                                          \
            {                                                                     \
                if (ui32Timeout == (limit * 1000))                                \
                {                                                                 \
                    timeout = 1;                                                  \
                    break;                                                        \
                }                                                                 \
                am_util_delay_us(1);                                              \
                ui32Timeout++;                                                    \
            }                                                                     \
        }

#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
#define UINT32_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT32_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
#define UINT32_TO_BYTE2(n)        ((uint8_t) ((n) >> 16))
#define UINT32_TO_BYTE3(n)        ((uint8_t) ((n) >> 24))

typedef enum
{
    AM_DEVICES_COOPER_STATUS_SUCCESS,
    AM_DEVICES_COOPER_STATUS_ERROR,
    //
    // This error occurs when an HCI read or write function is called while
    // another HCI communication function is already in progress.
    //
    AM_DEVICES_COOPER_STATUS_BUS_BUSY,

    //
    // This error means that the MCU tried to execute an HCI write, but can not
    // get the space available (0xA868).
    // This might mean that the controller has not finished the wakeup process,
    // the MCU need to wait until the IRQ is asserted.
    //
    AM_DEVICES_COOPER_STATUS_CONTROLLER_NOT_READY,

    // Wrong length of data been read back (==0 or > 260)
    AM_DEVICES_COOPER_STATUS_WRONG_DATA_LENGTH,

    //
    // We are expecting an HCI response to a packet we just sent, but the BLE
    // core isn't asserting BLEIRQ. Its software may have crashed, and it may
    // need to restart.
    //
    AM_DEVICES_COOPER_STATUS_NO_RESPONSE,

    // The transaction ends up in error and cannot finish the HCI packet R/W
    AM_DEVICES_COOPER_STATUS_PACKET_INCOMPLETE,

    // The CQ does not complete transaction before times out
    AM_DEVICES_COOPER_STATUS_TIMEOUT,
    AM_DEVICES_COOPER_STATUS_INVALID_OPERATION,
} am_devices_cooper_status_t;

#define AM_DEVICES_COOPER_QFN_PART            0
#if (AM_DEVICES_COOPER_QFN_PART)
#define AM_DEVICES_COOPER_IRQ_PIN             73
#define AM_DEVICES_COOPER_RESET_PIN           57
#define AM_DEVICES_COOPER_CLKREQ_PIN          64
#define AM_DEVICES_COOPER_STATUS_PIN          66
#define AM_DEVICES_COOPER_32M_OSCEN_PIN       67
#define AM_DEVICES_COOPER_SPI_CS              72 // BGA&SIP share the same CS pin(NCE72) on the QFN shiled board
#define g_AM_DEVICES_COOPER_SPI_CS            g_AM_BSP_GPIO_IOM2_CS
#else

#ifdef AM_PART_APOLLO4L
#define AM_DEVICES_COOPER_IRQ_PIN             53
#define AM_DEVICES_COOPER_CLKREQ_PIN          52
// #define AM_DEVICES_COOPER_CLKACK_PIN          41
#define AM_DEVICES_COOPER_RESET_PIN           55
//#define AM_DEVICES_COOPER_STATUS_PIN          44
#define AM_DEVICES_COOPER_SWDIO               97
#define AM_DEVICES_COOPER_SWCLK               98
#define AM_DEVICES_COOPER_32M_CLK             46
#define AM_DEVICES_COOPER_32K_CLK             4
#define AM_DEVICES_COOPER_SPI_CS              AM_BSP_GPIO_IOM4_CS
#define g_AM_DEVICES_COOPER_SPI_CS            g_AM_BSP_GPIO_IOM4_CS
#else
#define AM_DEVICES_COOPER_IRQ_PIN             39
#define AM_DEVICES_COOPER_CLKREQ_PIN          40
#define AM_DEVICES_COOPER_CLKACK_PIN          41
#define AM_DEVICES_COOPER_RESET_PIN           42
#define AM_DEVICES_COOPER_STATUS_PIN          44
#define AM_DEVICES_COOPER_SWDIO               97
#define AM_DEVICES_COOPER_SWCLK               98
#define AM_DEVICES_COOPER_32M_CLK             46
#define AM_DEVICES_COOPER_32K_CLK             45
#define AM_DEVICES_COOPER_SPI_CS              AM_BSP_GPIO_IOM4_CS
#define g_AM_DEVICES_COOPER_SPI_CS            g_AM_BSP_GPIO_IOM4_CS
#endif

#endif


//*****************************************************************************
//
// Configurable buffer sizes.
//
//*****************************************************************************
#define AM_DEVICES_COOPER_MAX_TX_PACKET       524 // the max packet of SBL to controller is 512 plus 12 bytes header
#define AM_DEVICES_COOPER_MAX_RX_PACKET       258 // 255 data + 3 header

//*****************************************************************************
//
// SPI configuration.
//
//*****************************************************************************
#if (AM_DEVICES_COOPER_QFN_PART)
#define SPI_MODULE           2
#define AM_COOPER_IRQn       GPIO0_405F_IRQn
#define am_cooper_irq_isr    am_gpio0_405f_isr
//
// we need to slow down SPI clock for fly-wire case in between
// Apollo3/3p/4 EB/EVBB and Cooper QFN part. 8MHz is chosen conservatively.
//
#define COOPER_IOM_FREQ         AM_HAL_IOM_8MHZ
#else
#define SPI_MODULE           4
#define AM_COOPER_IRQn       GPIO0_203F_IRQn
#define am_cooper_irq_isr    am_gpio0_203f_isr
#define COOPER_IOM_FREQ         AM_HAL_IOM_24MHZ
#endif

//
// Take over the interrupt handler for whichever IOM we're using.
//
#define cooper_iom_isr                                                          \
    am_iom_isr1(SPI_MODULE)
#define am_iom_isr1(n)                                                        \
    am_iom_isr(n)
#define am_iom_isr(n)                                                         \
    am_iomaster ## n ## _isr

#define IOM_INTERRUPT1(n)       AM_HAL_INTERRUPT_IOMASTER ## n
#define IOM_INTERRUPT(n)        IOM_INTERRUPT1(n)
#define COOPER_IOM_IRQn         ((IRQn_Type)(IOMSTR0_IRQn + SPI_MODULE))

enum PARAM_ID
{
    /// Definition of the tag associated to each parameters
    /// Local Bd Address
    PARAM_ID_BD_ADDRESS                 = 0x01,
    /// Device Name
    PARAM_ID_DEVICE_NAME                = 0x02,
    /// 32K source
    PARAM_ID_32K_CLK_SOURCE             = 0x03,
    /// Radio Drift
    PARAM_ID_LPCLK_DRIFT                = 0x07,
    /// Radio Jitter
    PARAM_ID_LPCLK_JITTER               = 0x08,
    /// External wake-up time
    PARAM_ID_EXT_WAKEUP_TIME            = 0x0D,
    /// Oscillator wake-up time
    PARAM_ID_OSC_WAKEUP_TIME            = 0x0E,
    /// Radio wake-up time
    PARAM_ID_RM_WAKEUP_TIME             = 0x0F,
    /// UART baudrate
    PARAM_ID_UART_BAUDRATE              = 0x10,
    /// Enable sleep mode
    PARAM_ID_SLEEP_ENABLE               = 0x11,
    /// Enable External Wakeup
    PARAM_ID_EXT_WAKEUP_ENABLE          = 0x12,
    /// SP Private Key 192
    PARAM_ID_SP_PRIVATE_KEY_P192        = 0x13,
    /// SP Public Key 192
    PARAM_ID_SP_PUBLIC_KEY_P192         = 0x14,

    /// Activity Move Configuration (enables/disables activity move for BLE connections and BT (e)SCO links)
    PARAM_ID_ACTIVITY_MOVE_CONFIG       = 0x15,

    /// Enable/disable scanning for extended advertising PDUs
    PARAM_ID_SCAN_EXT_ADV               = 0x16,

    /// Duration of the schedule reservation for long activities such as scan, inquiry, page, HDC advertising
    PARAM_ID_SCHED_SCAN_DUR             = 0x17,

    /// Programming delay, margin for programming the baseband in advance of each activity (in half-slots)
    PARAM_ID_PROG_DELAY                 = 0x18,

    /// Enable/disable channel assessment for BT and/or BLE
    PARAM_ID_CH_ASS_EN                  = 0x19,

    /// Synchronous links configuration
    PARAM_ID_SYNC_CONFIG                = 0x2C,
    /// PCM Settings
    PARAM_ID_PCM_SETTINGS               = 0x2D,
    /// Sleep algorithm duration
    PARAM_ID_SLEEP_ALGO_DUR             = 0x2E,
    /// Tracer configuration
    PARAM_ID_TRACER_CONFIG              = 0x2F,

    /// Diagport configuration
    PARAM_ID_DIAG_BT_HW                 = 0x30,
    PARAM_ID_DIAG_BLE_HW                = 0x31,
    PARAM_ID_DIAG_SW                    = 0x32,
    PARAM_ID_DIAG_DM_HW                 = 0x33,
    PARAM_ID_DIAG_PLF                   = 0x34,

    /// IDC selection (for audio demo)
    PARAM_ID_IDCSEL_PLF                 = 0x37,

    /// RSSI threshold tags
    PARAM_ID_RSSI_HIGH_THR              = 0x3A,
    PARAM_ID_RSSI_LOW_THR               = 0x3B,
    PARAM_ID_RSSI_INTERF_THR            = 0x3C,

    /// RF BTIPT
    PARAM_ID_RF_BTIPT_VERSION          = 0x3E,
    PARAM_ID_RF_BTIPT_XO_SETTING       = 0x3F,
    PARAM_ID_RF_BTIPT_GAIN_SETTING     = 0x40,


    PARAM_ID_BT_LINK_KEY_FIRST          = 0x60,
    PARAM_ID_BT_LINK_KEY_LAST           = 0x67,

    PARAM_ID_BLE_LINK_KEY_FIRST         = 0x70,
    PARAM_ID_BLE_LINK_KEY_LAST          = 0x7F,
    /// SC Private Key (Low Energy)
    PARAM_ID_LE_PRIVATE_KEY_P256        = 0x80,
    /// SC Public Key (Low Energy)
    PARAM_ID_LE_PUBLIC_KEY_P256         = 0x81,
    /// SC Debug: Used Fixed Private Key from NVDS (Low Energy)
    PARAM_ID_LE_DBG_FIXED_P256_KEY      = 0x82,
    /// SP Private Key (classic BT)
    PARAM_ID_SP_PRIVATE_KEY_P256        = 0x83,
    /// SP Public Key (classic BT)
    PARAM_ID_SP_PUBLIC_KEY_P256         = 0x84,

    /// LE Coded PHY 500 Kbps selection
    PARAM_ID_LE_CODED_PHY_500           = 0x85,

    /// Application specific
    PARAM_ID_APP_SPECIFIC_FIRST         = 0x90,
    PARAM_ID_APP_SPECIFIC_LAST          = 0xAF,

    /// Mesh NVDS values
    PARAM_ID_MESH_SPECIFIC_FIRST        = 0xB0,
    PARAM_ID_MESH_SPECIFIC_LAST         = 0xF0,
};

//NVDS_MAGIC_NUMBER
#define NVDS_PARAMETER_MAGIC_NUMBER     0x4e, 0x56, 0x44, 0x53
/// Local Bd Address
#define NVDS_PARAMETER_BD_ADDRESS       PARAM_ID_BD_ADDRESS, 0x06, 0x06, 0x22, 0x44, 0x66, 0x88, 0x48, 0x59
//device name
#define NVDS_PARAMETER_DEVICE_NAME      PARAM_ID_DEVICE_NAME, 0x06, 0x06, 0x43, 0x6F, 0x6F, 0x70, 0x65, 0x72
#define NVDS_PARAMETER_EXT_32K_CLK_SOURCE PARAM_ID_32K_CLK_SOURCE, 0x06, 0x01, 0x01

//*****************************************************************************
//
// HCI packet types.
//
//*****************************************************************************
#define AM_DEVICES_COOPER_RAW                      0x0
#define AM_DEVICES_COOPER_CMD                      0x1
#define AM_DEVICES_COOPER_ACL                      0x2
#define AM_DEVICES_COOPER_EVT                      0x4

typedef enum
{
    RD_8_Bit                   = 8,
    /// 16 bit access types
    RD_16_Bit                  = 16,
    /// 32 bit access types
    RD_32_Bit                  = 32

}eMemAccess_type;

//*****************************************************************************
//
// Vendor Specific commands.
//
// Note: Lengths are reported as "4 + <parameter length>". Each vendor-specific
// header is 4 bytes long. This definition allows the macro version of the
// length to be used in all BLE APIs.
//
//*****************************************************************************
#define HCI_VSC_CMD_HEADER_LENGTH                    4
typedef enum
{
    HCI_VSC_RD_MEM_CMD_OPCODE                      = 0xFC01,
    HCI_VSC_WR_MEM_CMD_OPCODE                      = 0xFC02,

    HCI_VSC_ID_FLASH_CMD_OPCODE                    = 0xFC05,
    HCI_VSC_ER_FLASH_CMD_OPCODE                    = 0xFC06,
    HCI_VSC_WR_FLASH_CMD_OPCODE                    = 0xFC07,
    HCI_VSC_RD_FLASH_CMD_OPCODE                    = 0xFC08,

    HCI_VSC_PLF_RESET_CMD_OPCODE                   = 0xFC11,

    HCI_VSC_REG_RD_CMD_OPCODE                      = 0xFC39,
    HCI_VSC_REG_WR_CMD_OPCODE                      = 0xFC3A,

    // Ambiq Vendor Specific Command set Tx power level
    HCI_VSC_SET_TX_POWER_LEVEL_CFG_CMD_OPCODE      = 0xFC70,
    // Ambiq Vendor Specific Command  start transmitter test
    HCI_VSC_START_TRANS_TEST_CFG_CMD_OPCODE        = 0xFC71,
    // Ambiq Vendor Specific Command end transmitter test
    HCI_VSC_END_TRANS_TEST_CFG_CMD_OPCODE          = 0xFC72,
    // Ambiq Vendor Specific Command set debug log bitmap
    HCI_VSC_SET_LOG_BITMAP_CFG_CMD_OPCODE          = 0xFC73,
    // Ambiq Vendor Specific Command update bd address
    HCI_VSC_SET_BD_ADDR_CFG_CMD_OPCODE             = 0xFC74,
    // Ambiq Vendor Specific Command update FW
    HCI_VSC_UPDATE_FW_CFG_CMD_OPCODE               = 0xFC75,
    // Ambiq Vendor Specific Command get device ID
    HCI_VSC_GET_DEVICE_ID_CFG_CMD_OPCODE           = 0xFC76,
    // Ambiq Vendor Specific Command set NVDS parameters
    HCI_VSC_UPDATE_NVDS_CFG_CMD_OPCODE             = 0xFC77,
    // Ambiq Vendor Specific Command set link layer features
    HCI_VSC_UPDATE_LL_FEATURE_CFG_CMD_OPCODE       = 0xFC78,

    // SBL use only
    // Ambiq Vendor Specific Command store info0 trim values to RAM
    HCI_VSC_STORE_INFO0_TRIM_CMD_OPCODE            = 0xFCC0,
    // Ambiq Vendor Specific Command flash info0 trim values to cooper
    HCI_VSC_FLASH_INFO0_TRIM_CMD_OPCODE            = 0xFCC1,
    // Ambiq Vendor Specific Command trigger Apollo4B to enter sleep
    HCI_VSC_ENTER_SLEEP_CMD_OPCODE                 = 0xFCC2
}vsc_opcode;

#define HCI_VSC_RD_MEM_CMD_LENGTH                      6
#define HCI_VSC_WR_MEM_CMD_LENGTH                      136
#define HCI_VSC_ID_FLASH_CMD_LENGTH                    0
#define HCI_VSC_ER_FLASH_CMD_LENGTH                    9
#define HCI_VSC_WR_FLASH_CMD_LENGTH                    140
#define HCI_VSC_RD_FLASH_CMD_LENGTH                    6
#define HCI_VSC_PLF_RESET_CMD_LENGTH                   1
#define HCI_VSC_REG_RD_CMD_LENGTH                      4
#define HCI_VSC_REG_WR_CMD_LENGTH                      8
#define HCI_VSC_SET_TX_POWER_LEVEL_CFG_CMD_LENGTH      1
#define HCI_VSC_START_TRANS_TEST_CFG_CMD_LENGTH        4
#define HCI_VSC_END_TRANS_TEST_CFG_CMD_LENGTH          0
#define HCI_VSC_SET_LOG_BITMAP_CFG_CMD_LENGTH          4
#define HCI_VSC_SET_BD_ADDR_CFG_CMD_LENGTH             6
#define HCI_VSC_UPDATE_FW_CFG_CMD_LENGTH               4
#define HCI_VSC_GET_DEVICE_ID_CFG_CMD_LENGTH           0
#define HCI_VSC_UPDATE_NVDS_CFG_CMD_LENGTH             240
#define HCI_VSC_STORE_INFO0_TRIM_CMD_LENGTH            6
#define HCI_VSC_FLASH_INFO0_TRIM_CMD_LENGTH            0
#define HCI_VSC_ENTER_SLEEP_CMD_LENGTH                 0

#define HCI_VSC_CMD_LENGTH(n)                          (HCI_VSC_CMD_HEADER_LENGTH + n)
#define HCI_VSC_UPDATE_NVDS_CFG_CMD_OFFSET             (HCI_VSC_CMD_HEADER_LENGTH + 4) //NVDS_PARAMETER_MAGIC_NUMBER

#define HCI_VSC_CMD(CMD, ...)                          {AM_DEVICES_COOPER_CMD, UINT16_TO_BYTE0(CMD##_CMD_OPCODE), UINT16_TO_BYTE1(CMD##_CMD_OPCODE), CMD##_CMD_LENGTH, ##__VA_ARGS__}
#define HCI_RAW_CMD(OPCODE, LEN, ...)                  {AM_DEVICES_COOPER_CMD, UINT16_TO_BYTE0(OPCODE), UINT16_TO_BYTE1(OPCODE), LEN, ##__VA_ARGS__}

//Radio Drift
#ifndef LPCLK_DRIFT_VALUE
#define LPCLK_DRIFT_VALUE               500 // PPM
#endif
#define NVDS_PARAMETER_LPCLK_DRIFT      PARAM_ID_LPCLK_DRIFT, 0x06, 0x2, UINT16_TO_BYTE0(LPCLK_DRIFT_VALUE), UINT16_TO_BYTE1(LPCLK_DRIFT_VALUE)
//External wake-up time

#ifndef EXT_WAKEUP_TIME_VALUE
#define EXT_WAKEUP_TIME_VALUE           1000 // microsecond
#endif
#define NVDS_PARAMETER_EXT_WAKEUP_TIME  PARAM_ID_EXT_WAKEUP_TIME, 0x06, 0x02, UINT16_TO_BYTE0(EXT_WAKEUP_TIME_VALUE), UINT16_TO_BYTE1(EXT_WAKEUP_TIME_VALUE)
//Oscillator wake-up time
#ifndef OSC_WAKEUP_TIME_VALUE
#define OSC_WAKEUP_TIME_VALUE           1000 // microsecond
#endif
#define NVDS_PARAMETER_OSC_WAKEUP_TIME  PARAM_ID_OSC_WAKEUP_TIME, 0x06, 0x02, UINT16_TO_BYTE0(OSC_WAKEUP_TIME_VALUE), UINT16_TO_BYTE1(OSC_WAKEUP_TIME_VALUE)
//Radio wake-up time
#define NVDS_PARAMETER_RM_WAKEUP_TIME   PARAM_ID_RM_WAKEUP_TIME, 0x06, 0x02, 0x1E, 0x00
//set UART_BAUDRATE
#define NVDS_PARAMETER_UART_BAUDRATE    PARAM_ID_UART_BAUDRATE, 0x06, 0x04, 0x00, 0x10, 0x0E, 0x00
//sleep algorithm enabled
#define NVDS_PARAMETER_SLEEP_ENABLE     PARAM_ID_SLEEP_ENABLE, 0x06, 0x01, 0x01
//sleep algorithm disabled
#define NVDS_PARAMETER_SLEEP_DISABLE    PARAM_ID_SLEEP_ENABLE, 0x06, 0x01, 0x00
//external wake-up support
#define NVDS_PARAMETER_EXT_WAKEUP_ENABLE PARAM_ID_EXT_WAKEUP_ENABLE, 0x06, 0x01, 0x01
//Activity Move Configuration
#define NVDS_PARAMETER_ACTIVITY_MOVE_CONFIG PARAM_ID_ACTIVITY_MOVE_CONFIG, 0x06, 0x01, 0x01
//Enable/disable scanning for extended advertising PDUs
#define NVDS_PARAMETER_SCAN_EXT_ADV      PARAM_ID_SCAN_EXT_ADV, 0x06, 0x01, 0x01
//default scan duration
#define NVDS_PARAMETER_SCHED_SCAN_DUR    PARAM_ID_SCHED_SCAN_DUR, 0x06, 0x02, 0xE4, 0x57
//Programming delay
#define NVDS_PARAMETER_PROG_DELAY        PARAM_ID_PROG_DELAY, 0x06, 0x01, 0x1
//channel assessment for BT and/or BLE
#define NVDS_PARAMETER_CH_ASS_EN         PARAM_ID_CH_ASS_EN, 0x06, 0x01, 0x1
// sleep algorithm duration
#define NVDS_PARAMETER_SLEEP_ALGO_DUR   PARAM_ID_SLEEP_ALGO_DUR, 0x06, 0x02, 0xF4, 0x01
// debug trace config
#define NVDS_PARAMETER_TRACE_CONFIG     PARAM_ID_TRACER_CONFIG, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00
// diagnostic port
#define NVDS_PARAMETER_DIAG_BLE_HW      PARAM_ID_DIAG_BLE_HW, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00
// SW diags configuration
#define NVDS_PARAMETER_DIAG_SW          PARAM_ID_DIAG_SW, 0x06, 0x04, 0xFF, 0xFF, 0xFF, 0xFF
// diagport configuration
#define NVDS_PARAMETER_DIAG_DM_HW       PARAM_ID_DIAG_DM_HW, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00
//SC Private Key (Low Energy)
#define NVDS_PARAMETER_LE_PRIVATE_KEY_P256   PARAM_ID_LE_PRIVATE_KEY_P256, 0x06, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// SC Debug: Used Fixed Private Key from NVDS (Low Energy)
#define NVDS_PARAMETER_LE_DBG_FIXED_P256_KEY  PARAM_ID_LE_DBG_FIXED_P256_KEY, 0x06, 0x01, 0x00
// LE Coded PHY 500 Kbps selection
#define NVDS_PARAMETER_LE_CODED_PHY_500   PARAM_ID_LE_CODED_PHY_500, 0x06, 0x01, 0x00

//*****************************************************************************
//
// SBL Defines
//
//
//*****************************************************************************
#define USE_SPI_PIN                     19

//
// Slave interrupt pin is connected here
//
#define BOOTLOADER_HANDSHAKE_PIN        42

//
// This pin is connected to RESET pin of slave
//
#define DRIVE_SLAVE_RESET_PIN           17

//
// This pin is connected to the 'Override' pin of slave
//
#define DRIVE_SLAVE_OVERRIDE_PIN        4

#define AM_DEVICES_COOPER_SBL_UPDATE_STATE_INIT                     0x00
#define AM_DEVICES_COOPER_SBL_UPDATE_STATE_HELLO                    0x01
#define AM_DEVICES_COOPER_SBL_UPDATE_STATE_UPDATE                   0x02
#define AM_DEVICES_COOPER_SBL_UPDATE_STATE_DATA                     0x03
#define AM_DEVICES_COOPER_SBL_UPDATE_STATE_IMAGE_OK                 0x04

#define AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_FW                  0x00
#define AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_INFO_0              0x01
#define AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_INFO_1              0x02
#define AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_NONE                0x03

#define AM_DEVICES_COOPER_SBL_UPADTE_MAX_SPI_PKT_SIZE               0x200
#define AM_DEVICES_COOPER_SBL_UPADTE_INVALID_PSI_PKT_SIZE           0xFFFFFFFF
#define AM_DEVICES_COOPER_SBL_UPADTE_IMAGE_HDR_SIZE                 0x40

#define AM_DEVICES_COOPER_SBL_MAX_COMM_ERR_COUNT                    0x03

#define AM_DEVICES_COOPER_SBL_ACK_RESP_SUCCESS                      0x00
#define AM_DEVICES_COOPER_SBL_ACK_RESP_FAIL                         0x01
#define AM_DEVICES_COOPER_SBL_ACK_RESP_BAD_HDR                      0x02
#define AM_DEVICES_COOPER_SBL_ACK_RESP_BAD_CRC                      0x03
#define AM_DEVICES_COOPER_SBL_ACK_RESP_VERSION_INVALID              0x04
#define AM_DEVICES_COOPER_SBL_ACK_RESP_MSG_TOO_BIG                  0x05
#define AM_DEVICES_COOPER_SBL_ACK_RESP_UNKNOWN_MSG                  0x06
#define AM_DEVICES_COOPER_SBL_ACK_RESP_INVALID_ADDRESS              0x07
#define AM_DEVICES_COOPER_SBL_ACK_RESP_INVALID_OPERATION            0x08
#define AM_DEVICES_COOPER_SBL_ACK_RESP_INVALID_PARAM                0x09
#define AM_DEVICES_COOPER_SBL_ACK_RESP_INVALID_DATA_LEN             0x0A
#define AM_DEVICES_COOPER_SBL_ACK_RESP_SEQ                          0x0B
#define AM_DEVICES_COOPER_SBL_ACK_RESP_TOO_BIG_DATA                 0x0C
#define AM_DEVICES_COOPER_SBL_ACK_RESP_BAD_IMAGE                    0x0D
#define AM_DEVICES_COOPER_SBL_ACK_RESP_FLASH_WRITE_FAILED           0x0E
#define AM_DEVICES_COOPER_SBL_ACK_RESP_INVALID_DEVICE_ID            0x0F
#define AM_DEVICES_COOPER_SBL_ACK_RESP_BAD_KEY                      0x10

#define AM_DEVICES_COOPER_SBL_STAT_RESP_SUCCESS                     0x00
#define AM_DEVICES_COOPER_SBL_STAT_RESP_FAIL                        0x01
#define AM_DEVICES_COOPER_SBL_STAT_RESP_BAD_HDR                     0x02
#define AM_DEVICES_COOPER_SBL_STAT_RESP_BAD_CRC                     0x03
#define AM_DEVICES_COOPER_SBL_STAT_RESP_STAT_BAD_SBL                0x04
#define AM_DEVICES_COOPER_SBL_STAT_RESP_BAD_IMAGE                   0x05
#define AM_DEVICES_COOPER_SBL_STAT_RESP_UNKNOWN_MSG                 0x06
#define AM_DEVICES_COOPER_SBL_STAT_RESP_FW_UPDATE_REQ               0x07
#define AM_DEVICES_COOPER_SBL_STAT_RESP_INFO0_UPDATE_REQ            0x08
#define AM_DEVICES_COOPER_SBL_STAT_RESP_INFO1_UPDATE_REQ            0x09

#define AM_DEVICES_COOPER_SBL_STATUS_INIT                           0x00000000
#define AM_DEVICES_COOPER_SBL_STATUS_OK                             0xA5A5A5A5
#define AM_DEVICES_COOPER_SBL_STATUS_FAIL                           0xA1A1A1A1
#define AM_DEVICES_COOPER_SBL_STATUS_CRC_FAIL                       0xA2A2A2A2
#define AM_DEVICES_COOPER_SBL_STATUS_UPDATE_FW                      0x4598F231
#define AM_DEVICES_COOPER_SBL_STATUS_UPDATE_INFO_0                  0x8730DA5B
#define AM_DEVICES_COOPER_SBL_STATUS_UPDATE_INFO_1                  0x09FA3725
#define AM_DEVICES_COOPER_SBL_STATUS_IN_PROGRESS                    0xA78BD32C
#define AM_DEVICES_COOPER_SBL_STATUS_UPDATE_IMAGE_FAIL              0xA3A3A3A3

#define AM_DEVICES_COOPER_SBL_DEFAULT_FW_VERSION                    0xFFFFFFFF

#define AM_DEVICES_COOPER_SBL_STAT_VER_ROLL_BACK_EN                 0xFFFFFFFF
#define AM_DEVICES_COOPER_SBL_STAT_VER_ROLL_BACK_DBL                0xFFFFFF00

#define AM_DEVICES_COOPER_SBL_FW_IMAGE_ADDRESS                      0x10030000
#define AM_DEVICES_COOPER_SBL_FW_IMAGE_SIZE                         0x22890
#define AM_DEVICES_COOPER_SBL_FW_IMAGE_SIZE_MAX                     0x30000

#define AM_DEVICES_COOPER_SBL_INFO0_PATCH_ADDRESS                   AM_DEVICES_COOPER_SBL_FW_IMAGE_ADDRESS +  AM_DEVICES_COOPER_SBL_FW_IMAGE_SIZE_MAX
#define AM_DEVICES_COOPER_SBL_INFO0_PATCH_SIZE                      0x180
#define AM_DEVICES_COOPER_SBL_INFO0_PATCH_SIZE_MAX                  0x200

#define AM_DEVICES_COOPER_SBL_INFO1_PATCH_ADDRESS                   AM_DEVICES_COOPER_SBL_INFO0_PATCH_ADDRESS +  AM_DEVICES_COOPER_SBL_INFO0_PATCH_SIZE_MAX
#define AM_DEVICES_COOPER_SBL_INFO1_PATCH_SIZE                      0x80
#define AM_DEVICES_COOPER_SBL_INFO1_PATCH_SIZE_MAX                  0x100

// Signatures for the image downloads
#define COOPER_INFO0_UPDATE_SIGN   0xB35D18C9
#define COOPER_INFO1_UPDATE_SIGN   0x38B75A0D
#define COOPER_FW_UPDATE_SIGN      0xC593876A

typedef struct
{
    uint8_t*    pImageAddress;
    uint32_t    imageSize;
    uint32_t    imageType;
    uint32_t    version;
} am_devices_cooper_sbl_update_data_t;


typedef struct
{
    uint32_t    ui32SblUpdateState;
    uint8_t*    pImageBuf;
    uint32_t    ui32ImageSize;
    uint8_t*    pDataBuf;
    uint32_t    ui32DataSize;
    uint32_t    ui32PacketNumber;
    uint32_t    ui32TotalPackets;
    uint32_t    ui32ImageType;
    uint32_t    ui32ErrorCounter;
    void*       pHandle; // cooper_device handle
    uint32_t*   pWorkBuf;

    uint32_t    ui32CooperFWImageVersion;
    uint32_t    ui32CooperSblStatus;
    uint32_t    ui32CooperVerRollBackConfig; //Version 2
    uint32_t    ui32copperChipIdWord0;  //Version 2
    uint32_t    ui32copperChipIdWord1;  //Version 2
} am_devices_cooper_sbl_update_state_t;


typedef struct
{
    uint32_t                     crc32;   // First word
    uint16_t                     msgType; // am_secboot_wired_msgtype_e
    uint16_t                     length;
} am_secboot_wired_msghdr_t;

typedef struct
{
    uint32_t                      length  : 16;
    uint32_t                      resv    : 14;
    uint32_t                      bEnd    : 1;
    uint32_t                      bStart  : 1;
} am_secboot_ios_pkthdr_t;


//Message types
typedef enum
{
    AM_SBL_HOST_MSG_HELLO = 0,
    AM_SBL_HOST_MSG_STATUS,
    AM_SBL_HOST_MSG_UPDATE_STATUS,
    AM_SBL_HOST_MSG_UPDATE,
    AM_SBL_HOST_MSG_FW_CONTINUE,
    AM_SBL_HOST_MSG_NACK,
    AM_SBL_HOST_MSG_RESET,
    AM_SBL_HOST_MSG_ACK,
    AM_SBL_HOST_MSG_DATA
} AM_SBL_HOST_MSG_E;

///////////////////SBL Messages To and From Host////////////////////////

// Message header
typedef struct
{
    uint32_t    msgCrc;
    uint16_t    msgType;
    uint16_t    msgLength;
} am_sbl_host_msg_hdr_t;

// Hello Message
typedef struct
{
    am_sbl_host_msg_hdr_t    msgHdr;
} am_sbl_host_msg_hello_t;

// Status message
typedef struct
{
    am_sbl_host_msg_hdr_t   msgHdr;
    uint32_t                versionNumber;
    uint32_t                maxImageSize;
    uint32_t                bootStatus;
    uint32_t                verRollBackStatus;  //Version 2
    uint32_t                copperChipIdWord0;  //Version 2
    uint32_t                copperChipIdWord1;  //Version 2
} am_sbl_host_msg_status_t;

//Update Message
typedef struct
{
    am_sbl_host_msg_hdr_t   msgHdr;
    uint32_t                imageSize;
    uint32_t                maxPacketSize;
    uint32_t                NumPackets;
    uint32_t                versionNumber;
} am_sbl_host_msg_update_t;

//Data Message
typedef struct
{
    am_sbl_host_msg_hdr_t   msgHdr;
    uint32_t                packetNumber;
    uint8_t                 data[];
} am_sbl_host_msg_data_t;

//FW continue Message
typedef struct
{
    am_sbl_host_msg_hdr_t   msgHdr;
    uint32_t                reserved;
} am_sbl_host_msg_fw_continue_t;

//Reset Message
typedef struct
{
    am_sbl_host_msg_hdr_t   msgHdr;
    uint32_t                reserved;
} am_sbl_host_msg_reset_t;

//Ack /Nack Message
typedef struct
{
    am_sbl_host_msg_hdr_t   msgHdr;
    uint32_t                srcMsgType;
    uint32_t                status;
    uint32_t                nextPacketNum;  // only valid for data messages ack
    uint32_t                reserved[3];    //Version 2
} am_sbl_host_msg_ack_nack_t;


#define   AM_DEVICES_COOPER_SBL_MAX_INFO_0_PATCH_VALUES         64
#define   AM_DEVICES_COOPER_SBL_BIT_MASK_PER_WORD               16
#define   AM_DEVICES_COOPER_SBL_INFO_0_REPLACE_TRIM        0x00000003

// INFO 0 patch for Rev A only
typedef struct
{
    uint32_t     magicNumNSize;             // 0x0A500180
    uint32_t    rsaSize;            // 0x00000000
    uint32_t    loadHdrReserved[14];            // All Zeros
    uint32_t    cmacHash[4];            // All Zeros
    uint32_t    authKeyNHashDataSize;            // All Zeros
    uint32_t    xBlock;                // 0x00000000
    uint32_t    keyDervData[4];            // All Zeros

    uint32_t    bitMaskWord[4];
    uint32_t    reserved_0;
    uint32_t    trimDataWords[AM_DEVICES_COOPER_SBL_MAX_INFO_0_PATCH_VALUES];
    uint32_t    reserved_1;

} am_sbl_info0_patch_blob_t;

// INFO 0 patch data
typedef struct
{
    uint32_t    wordOffset;
    uint32_t    value;
} am_sbl_info0_patch_data_t;

uint32_t am_devices_cooper_update_image(void);
uint32_t am_devices_cooper_image_update_init(void* pHandle, uint32_t* pWorkbuf);
bool am_devices_cooper_get_FwImage(am_devices_cooper_sbl_update_data_t *pFwImage );
bool am_devices_cooper_get_info1_patch(am_devices_cooper_sbl_update_data_t *pInfo1Image);
bool am_devices_cooper_get_info0_patch(am_devices_cooper_sbl_update_data_t *pInfo0Image);

typedef struct
{
    uint32_t* pNBTxnBuf;
    uint32_t ui32NBTxnBufLength;
} am_devices_cooper_config_t;

typedef void (*am_devices_cooper_callback_t)(void* pCallbackCtxt);

typedef struct
{
    uint32_t                     ui32Module;
    uint32_t                     ui32CS;
    uint32_t                     ui32CSDuration;
    uint32_t                     ui32Firmver;
    am_devices_cooper_callback_t pfnCallback;
    void*                        pCallbackCtxt;
    void*                        pBleHandle;
    bool                         bOccupied;
    bool                         bBusy;
    bool                         bNeedCallback;
    volatile bool                bDMAComplete;
    bool                         bWakingUp;
} am_devices_cooper_t;

#define AM_DEVICES_COOPER_MAX_DEVICE_NUM 1

extern void am_devices_cooper_pins_enable(void);
extern void am_devices_cooper_pins_disable(void);
extern uint32_t am_devices_cooper_init(uint32_t ui32Module, am_devices_cooper_config_t* pDevConfig, void** ppHandle, void** ppBleHandle);
extern uint32_t am_devices_cooper_term(void* pHandle);
extern void am_devices_cooper_reset(void);
extern uint32_t am_devices_cooper_bus_enable(void* pHandle);
extern uint32_t am_devices_cooper_bus_disable(void* pHandle);
extern uint32_t am_devices_cooper_blocking_read(void* pHandle, uint32_t* pui32Data, uint32_t* pui32BytesReceived);
extern uint32_t am_devices_cooper_blocking_write(void* pHandle, uint8_t ui8Type, uint32_t* pui32Data, uint32_t ui32NumBytes, bool bWaitReady);
extern uint32_t am_devices_cooper_command_write(void* pHandle, uint32_t* pui32Cmd, uint32_t ui32Length, uint32_t* pui32Response, uint32_t* pui32BytesReceived);
extern uint32_t am_devices_cooper_irq_read(void);
extern uint32_t am_devices_cooper_clkreq_read(void* pHandle);
extern uint32_t am_devices_cooper_crystal_trim_set(void *pHandle, uint32_t ui32TrimValue);
extern uint32_t am_devices_cooper_reset_with_sbl_check(void* pHandle, am_devices_cooper_config_t* pDevConfig);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_COOPER_H
