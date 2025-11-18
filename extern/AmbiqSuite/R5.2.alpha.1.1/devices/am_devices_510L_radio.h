//*****************************************************************************
//
//! @file am_devices_510L_radio.h
//!
//! @brief Apollo510L Radio Subsystem Driver with IPC Communication Interface.
//!
//! @addtogroup devices_510L_radio Apollo510L Radio Device Driver
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_510L_RADIO_H
#define AM_DEVICES_510L_RADIO_H

#include "am_util.h"
#include "ipc_rpmsg_static_vrings.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! MACRO definitions.
//
//*****************************************************************************
#define AM_DEVICES_510L_RADIO_IPC_MEM_RW_MAX_SIZE  (128)
// OTP INFO1 0x42006970 ~ 0x42006A50 is allocated for RF Trim
#define AM_DEVICES_510L_RADIO_RF_TRIM_SIZE         (0xE0)
//
//! @brief Default size of IPC share memory
//
// Note: the IPC share memory includes the IPC TX/RX buffer, vring size, status
// and necessary memory alignment. The IPC TX/RX buffer number can only be the
// power of 2, for example, 4, 8, 16. The IPC backend will calcuate the maximum
// supported buffer number in IPC instance creation based on allocated total IPC
// memory size. We need to define a right memory size to match with the expected
// buffer number, otherwise the actual buffer number maybe less than the expected
// one and cause memory waste.
//
// The macro IPC_SHM_SIZE(num, buf_size) can be used to get the matched total IPC
// memory size.
#define AM_DEVICES_510L_RADIO_IPC_BUF_NUM          (8)
#define AM_DEVICES_510L_RADIO_IPC_SHM_DEFAULT_SIZE                        \
    (IPC_SHM_SIZE(AM_DEVICES_510L_RADIO_IPC_BUF_NUM, RPMSG_BUFFER_SIZE))

//*****************************************************************************
//
//! Type definitions.
//
//*****************************************************************************

//
//! @brief Function execution status
//
typedef enum
{
    AM_DEVICES_510L_RADIO_STATUS_SUCCESS,
    AM_DEVICES_510L_RADIO_STATUS_ERROR,
    AM_DEVICES_510L_RADIO_STATUS_TIMEOUT,
    AM_DEVICES_510L_RADIO_STATUS_OUT_OF_RANGE,
    AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG,
    AM_DEVICES_510L_RADIO_STATUS_INVALID_OPERATION,
    AM_DEVICES_510L_RADIO_STATUS_MEM_ERR,
    AM_DEVICES_510L_RADIO_STATUS_HW_ERR,
} am_devices_510L_radio_status_t;

//
//! @brief Radio Subsystem Operational Modes
//
typedef enum
{
    /* CM4-only mode - controller/processor on */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_CP = 0,
    /* Network processor mode - CM4 and Radio are on */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_NP,
    /* Network processor mode - CM4 Radio and XO are on */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_NP_XO,
    /* Crystal-only mode - CM4 is on (active or retention) */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_XO_CP_ON,
    /* Crystal-only mode - CM4 is off */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_XO_CP_OFF,
    /* Subsystem is off */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_OFF,
    /* Invalid RSS mode */
    AM_DEVICES_510L_RADIO_RSS_OPMODE_INVALID
} am_devices_510L_radio_rss_opmode_e;

//*****************************************************************************
//
//! @brief IPC data packet module enumeration
//!
//! The standard BT HCI and RW-MM specific packet types are included in the
//! AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_RADIO.
//! The Ambiq specific system packet types are included in the
//! AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_SYS.
//!
//! This enumeration will be identified to execute the corresponding callback,
//! not tranmsit over IPC.
//!
//*****************************************************************************
enum
{
    AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_RADIO = 0x00,
    AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_SYS,
    AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_NUM
};

//*****************************************************************************
//!
//! @brief Ambiq IPC data packet type definition
//!
//! This packet type is always put in the first byte of IPC transmission packet.
//!
//! The BT HCI type definitions and data format are referring the Bluetooth Core
//! specification.
//! The MM type definitions and data format are referring the RW-MM radio stack
//! specific definition.
//! The SYS types are Ambiq specific, their general data format is defined as:
//! ---------------------------------------------------------------------------
//! 1-byte Packet Type | 1-byte Opcode | 2-byte Data Size | N-byte Data Payload
//! ---------------------------------------------------------------------------
//!
//*****************************************************************************
typedef enum
{
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_NONE = 0x00,       /* None of the known packet types */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_BT_HCI_CMD = 0x01, /* HCI Command packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_BT_HCI_ACL = 0x02, /* HCI ACL Data packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_BT_HCI_SCO = 0x03, /* HCI Synchronous Data packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_BT_HCI_EVT = 0x04, /* HCI Event packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_BT_HCI_ISO = 0x05, /* HCI ISO Data packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_MM_AHI = 0x10,     /* RW-MM AHI Data packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_RADIO_END =
        AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_MM_AHI, /* End of Radio packet type */

    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_START = 0xe0, /* Start of System specific packet type*/
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ =
        AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_START, /* System Request packet */
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_RSP,       /* System Response packet*/
    AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_NTF,       /* System Data/Status Notification packet */
} am_devices_510L_radio_ipc_packet_type_e;

//*****************************************************************************
//!
//! @brief Ambiq IPC System Packet Opcode definition
//!
//*****************************************************************************
typedef enum
{
    AM_DEVICES_510L_RADIO_SYS_OP_SET_RSS_OPMODE = 0x00,
    AM_DEVICES_510L_RADIO_SYS_OP_READ_REG = 0x01,
    AM_DEVICES_510L_RADIO_SYS_OP_WRITE_REG = 0x02,
    AM_DEVICES_510L_RADIO_SYS_OP_READ_MEM = 0x03,
    AM_DEVICES_510L_RADIO_SYS_OP_WRITE_MEM = 0x04,
    AM_DEVICES_510L_RADIO_SYS_OP_SET_RFTRIM = 0x05,
} am_devices_510L_radio_opcode_e;

//*****************************************************************************
//!
//! @brief Ambiq IPC System Packet Common Header structure
//!
//*****************************************************************************
struct am_devices_510L_radio_ipc_sys_hdr
{
    uint8_t type;      /* System Packet type */
    uint8_t opcode;    /* System Packet Opcode */
    uint16_t size;     /* System Packet data payload length */
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Set Radio Subsystem Opmoode Request structure
//!
//*****************************************************************************
struct am_devices_510L_radio_set_rss_opmode_req_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t mode;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Set Radio Subsystem Opmoode Response structure
//!
//*****************************************************************************

struct am_devices_510L_radio_set_rss_opmode_rsp_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t status;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Read Register Request structure
//!
//*****************************************************************************
struct am_devices_510L_radio_read_reg_req_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint32_t magic;
    uint32_t addr;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Read Register Response structure
//!
//*****************************************************************************
struct am_devices_510L_radio_read_reg_rsp_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t status;
    uint32_t addr;
    uint32_t data;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Write Register Request structure
//!
//*****************************************************************************
struct am_devices_510L_radio_write_reg_req_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint32_t magic;
    uint32_t addr;
    uint32_t data;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Write Register Response structure
//!
//*****************************************************************************
struct am_devices_510L_radio_write_reg_rsp_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t status;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Read Memory Request structure
//!
//*****************************************************************************
struct am_devices_510L_radio_read_mem_req_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint32_t magic;
    uint32_t addr;
    uint8_t size;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Read Memory Response structure
//!
//*****************************************************************************
struct am_devices_510L_radio_read_mem_rsp_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t status;
    uint32_t addr;
    uint8_t size;
    uint8_t data[AM_DEVICES_510L_RADIO_IPC_MEM_RW_MAX_SIZE];
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Write Memory Request structure
//!
//*****************************************************************************
struct am_devices_510L_radio_write_mem_req_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint32_t magic;
    uint32_t addr;
    uint8_t size;
    uint8_t data[AM_DEVICES_510L_RADIO_IPC_MEM_RW_MAX_SIZE];
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Write Memory Response structure
//!
//*****************************************************************************
struct am_devices_510L_radio_write_mem_rsp_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t status;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Set RF Trim Request structure
//!
//*****************************************************************************
struct rf_trim_data_t
{
    /*!< icytrxdm_trim_version */
    uint32_t rf_trim_verion : 4; /* OTP Info1 Offset: 0x970 */
    uint32_t : 28;

    /*!< xoconfig (RF PMU Address: 0x40030004) */
    uint32_t xoconfig; /* OTP Info1 Offset: 0x974 */

    /*!< osctrim (RF PMU Address: 0x40030008) */
    uint32_t osctrim; /* OTP Info1 Offset: 0x978 */

    /*!< radio tune (RF PMU Address: 0x4003001C) */
    uint32_t rtune; /* OTP Info1 Offset: 0x97C */

    /*!< icytrxdm main registers offset: 0x0FC ~ 0x1AB */
    uint8_t rf_main_reg_0x0fc_0x1ab[176]; /* OTP Info1 Offset: 0x980 */

    /*!< icytrxdm main registers offset: 0x1BC ~ 0x1D3 */
    uint8_t rf_main_reg_0x1bc_0x1d3[24]; /* OTP Info1 Offset: 0xA30 */

    /*!< icytrxdm main ana_trx_rf_vref_lna_prepa_bqf, offset: 0x31F */
    uint8_t rf_main_ana_trx_rf_vref_lna_prepa_bqf : 7; /* OTP Info1 Offset: 0xA48 */
    uint8_t : 1;

    /*!< icytrxdm main ana_trx_bb_adcs_ldo_vref_tune, offset: 0x32A */
    uint8_t rf_main_ana_trx_bb_adcs_ldo_vref_tune : 7; /* OTP Info1 Offset: 0xA49 */
    uint8_t : 1;

    /*!< icytrxdm main refe_ldo_msic_vref_tune, offset: 0x340 */
    uint8_t rf_main_refe_ldo_msic_vref_tune : 7; /* OTP Info1 Offset: 0xA4A */
    uint8_t : 1;

    /*!< icytrxdm adpll pmu_adpll_vref_tune, offset: 0xE76 */
    uint8_t rf_adpll_pmu_adpll_vref_tune : 7; /* OTP Info1 Offset: 0xA4B */
    uint8_t : 1;

    uint8_t reserved[180];

} __attribute__((packed));

struct am_devices_510L_radio_set_rftrim_req_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    union
    {
        uint8_t data[AM_DEVICES_510L_RADIO_RF_TRIM_SIZE];
        struct rf_trim_data_t rf_trim;
    } trim_b;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC System Set RF Trim Response structure
//!
//*****************************************************************************
struct am_devices_510L_radio_set_rftrim_rsp_t
{
    struct am_devices_510L_radio_ipc_sys_hdr hdr;
    uint8_t status;
} __attribute__((packed));

//*****************************************************************************
//!
//! @brief Ambiq IPC share memory structure
//!
//*****************************************************************************
typedef struct
{
    uint32_t ui32IpcShmAddr;
    uint32_t ui32IpcShmSize;
} am_devices_510L_radio_ipc_shm_t;

//*****************************************************************************
//
//! @typedef am_devices_510L_radio_ipc_rx_cback
//! @brief Define the Ambiq IPC RX callback
//!
//! @param data Pointer of IPC reception packet.
//! @param size Length of IPC reception packet.
//!
//! @return true indicates the IPC RX buffer can be released immediately, otherwise
//! the corresponding module of this callback needs handle the buffer release by
//! calling am_devices_510L_radio_release_ipc_rx_buf();
//*****************************************************************************
typedef bool (*am_devices_510L_radio_ipc_rx_cback)(uint8_t *data, uint16_t size);

//
//! Device Control Block
//
typedef struct
{
    am_devices_510L_radio_ipc_rx_cback rxCb[AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_NUM];
} am_devices_510L_radio_t;

//*****************************************************************************
//
//! @brief Vendor Specific commands.
//!
//! @note Lengths are reported as "4 + <parameter length>". Each vendor-specific
//! header is 4 bytes long. This definition allows the macro version of the
//! length to be used in all BLE APIs.
//
//*****************************************************************************
typedef enum
{
    //! Ambiq Vendor Specific Command set NVDS parameters
    HCI_VSC_UPDATE_NVDS_CFG_CMD_OPCODE             = 0xFC80,
    //! Ambiq Vendor Specific Command update bd address
    HCI_VSC_SET_BD_ADDR_CFG_CMD_OPCODE             = 0xFC81,
    //! Ambiq Vendor Specific Command set link layer features
    HCI_VSC_UPDATE_LL_FEATURE_CFG_CMD_OPCODE       = 0xFC82,
    //! Ambiq Vendor Specific Command set Tx power level
    HCI_VSC_SET_TX_POWER_LEVEL_CFG_CMD_OPCODE       = 0xFC83,

} vsc_opcode;

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

} eMemAccess_type;

#define HCI_VSC_UPDATE_NVDS_CFG_CMD_LENGTH             240
#define HCI_VSC_SET_BD_ADDR_CFG_CMD_LENGTH             6
#define HCI_VSC_SET_TX_POWER_LEVEL_CFG_CMD_LENGTH      1

// The BTDM host stack has defined the same PARAM_ID enumeration,
// do not define repeatedly here.
#ifndef BTDM_HOST
/// List of parameters identifiers
enum PARAM_ID
{
    /// Definition of the tag associated to each parameters
    /// Local Bd Address
    PARAM_ID_BD_ADDRESS = 0x01,
    /// Device Name
    PARAM_ID_DEVICE_NAME = 0x02,
    /// Low Power Clock Drift
    PARAM_ID_LPCLK_DRIFT = 0x07,
    /// Active Clock Drift
    PARAM_ID_ACTCLK_DRIFT = 0x09,
    /// External wake-up time
    PARAM_ID_EXT_WAKEUP_TIME = 0x0D,
    /// Oscillator wake-up time
    PARAM_ID_OSC_WAKEUP_TIME = 0x0E,
    /// Radio wake-up time
    PARAM_ID_RM_WAKEUP_TIME = 0x0F,
    /// UART baudrate
    PARAM_ID_UART_BAUDRATE = 0x10,
    /// Enable sleep mode
    PARAM_ID_SLEEP_ENABLE = 0x11,
    /// Enable External Wakeup
    PARAM_ID_EXT_WAKEUP_ENABLE = 0x12,

    /// Activity Move Configuration (enables/disables activity move for BLE
    /// connections and BT (e)SCO links)
    PARAM_ID_ACTIVITY_MOVE_CONFIG = 0x15,

    /// Enable/disable scanning for extended advertising PDUs
    PARAM_ID_SCAN_EXT_ADV = 0x16,

    /// Duration of the schedule reservation for long activities such as scan,
    /// inquiry, page, HDC advertising
    PARAM_ID_SCHED_SCAN_DUR = 0x17,

    /// Programming delay, margin for programming the baseband in advance of each
    /// activity (in half-slots)
    PARAM_ID_PROG_DELAY = 0x18,

    /// Enable/disable channel assessment for BT and/or BLE
    PARAM_ID_CH_ASS_EN = 0x19,

    /// Default MD bit used by slave when sending a data packet on a BLE
    /// connection
    PARAM_ID_DFT_SLAVE_MD = 0x20,

    /// Synchronous links configuration
    PARAM_ID_SYNC_CONFIG = 0x2C,
    /// Tracer configuration
    PARAM_ID_TRACER_CONFIG = 0x2F,

    /// Diagport configuration
    PARAM_ID_DIAG_BT_HW = 0x30,
    PARAM_ID_DIAG_BLE_HW = 0x31,
    PARAM_ID_DIAG_SW = 0x32,
    PARAM_ID_DIAG_DM_HW = 0x33,
    PARAM_ID_DIAG_PLF = 0x34,
    PARAM_ID_DIAG_CHSD_HW = 0x35,
    PARAM_ID_DIAG_MAC154_HW = 0x39,

    /// Channel classification reporting interval (in ms) (0 if not used) (default
    /// value: 0ms)
    PARAM_ID_CH_CLASS_REP_INTV = 0x36,

    /// IDC selection (for audio demo)
    PARAM_ID_IDCSEL_PLF = 0x37,

    /// RSSI threshold tags
    PARAM_ID_RSSI_HIGH_THR = 0x3A,
    PARAM_ID_RSSI_LOW_THR = 0x3B,
    PARAM_ID_RSSI_INTERF_THR = 0x3C,

    /// RF identifier
    PARAM_ID_RF_ID = 0x3D,

    /// RF BTIPT
    PARAM_ID_RF_BTIPT_VERSION = 0x3E,
    PARAM_ID_RF_BTIPT_XO_SETTING = 0x3F,
    PARAM_ID_RF_BTIPT_GAIN_SETTING = 0x40,

    /// Link keys
    PARAM_ID_BT_LINK_KEY_NB = 0x60,
    PARAM_ID_BT_LINK_KEY_FIRST = 0x61,
    PARAM_ID_BLE_LINK_KEY_FIRST = 0x70,
    PARAM_ID_BLE_LINK_KEY_LAST = 0x7F,

    /// SC Private Key (Low Energy)
    PARAM_ID_LE_PRIVATE_KEY_P256 = 0x80,
    /// SC Debug: Used Fixed Private Key from NVDS (Low Energy)
    PARAM_ID_LE_DBG_FIXED_P256_KEY = 0x82,

  #if (!BLE_CSSA)
    /// LE Coded PHY 500 Kbps selection
    PARAM_ID_LE_CODED_PHY_500 = 0x85,
  #endif // (!BLE_CSSA)

    /// Application specific
    PARAM_ID_APP_SPECIFIC_FIRST = 0x90,
    PARAM_ID_APP_SPECIFIC_LAST = 0xAF,
};
#endif // BTDM_HOST

//*****************************************************************************
//
//! NVDS parameter definitions
//
//*****************************************************************************
//! NVDS magic number
#define NVDS_PARAMETER_MAGIC_NUMBER     0x4e, 0x56, 0x44, 0x53
//! Local BD address
#define NVDS_PARAMETER_BD_ADDRESS       PARAM_ID_BD_ADDRESS, 0x06, 0x06, 0x22, 0x44, 0x66, 0x88, 0x48, 0x59

#ifndef LPCLK_DRIFT_VALUE
//! Radio Drift
#define LPCLK_DRIFT_VALUE               500 //!<  PPM
#endif
#define NVDS_PARAMETER_LPCLK_DRIFT      PARAM_ID_LPCLK_DRIFT, 0x06, 0x2, UINT16_TO_BYTE0(LPCLK_DRIFT_VALUE), UINT16_TO_BYTE1(LPCLK_DRIFT_VALUE)

#ifndef EXT_WAKEUP_TIME_VALUE
//! External wake-up time
#define EXT_WAKEUP_TIME_VALUE           1000 //!<  microsecond
#endif
#define NVDS_PARAMETER_EXT_WAKEUP_TIME  PARAM_ID_EXT_WAKEUP_TIME, 0x06, 0x02, UINT16_TO_BYTE0(EXT_WAKEUP_TIME_VALUE), UINT16_TO_BYTE1(EXT_WAKEUP_TIME_VALUE)
#ifndef OSC_WAKEUP_TIME_VALUE
//! Oscillator wake-up time
#define OSC_WAKEUP_TIME_VALUE           1000 //!<  microsecond
#endif
#define NVDS_PARAMETER_OSC_WAKEUP_TIME  PARAM_ID_OSC_WAKEUP_TIME, 0x06, 0x02, UINT16_TO_BYTE0(OSC_WAKEUP_TIME_VALUE), UINT16_TO_BYTE1(OSC_WAKEUP_TIME_VALUE)
//! sleep algorithm enabled
#define NVDS_PARAMETER_SLEEP_ENABLE     PARAM_ID_SLEEP_ENABLE, 0x06, 0x01, 0x01
//! sleep algorithm disabled
#define NVDS_PARAMETER_SLEEP_DISABLE    PARAM_ID_SLEEP_ENABLE, 0x06, 0x01, 0x00
//! external wake-up support
#define NVDS_PARAMETER_EXT_WAKEUP_ENABLE PARAM_ID_EXT_WAKEUP_ENABLE, 0x06, 0x01, 0x01

//*****************************************************************************
//
//! @brief Initialize the radio driver.
//!
//! @param None.
//!
//! @note This function should be called before any other am_devices_510L_radio
//! functions. It is used to set tell the other functions how to communicate
//! with the radio controller hardware.
//!
//! @return Status.
//
//*****************************************************************************
uint32_t am_devices_510L_radio_init(void);

//*****************************************************************************
//
//! @brief De-Initialize the radio controller driver.
//!
//! @param None.
//!
//! This function reverses the initialization
//!
//! @return Status.
//
//*****************************************************************************
uint32_t am_devices_510L_radio_deinit(void);

//*****************************************************************************
//
//! @brief Reset radio Controller.
//!
//! This function resets the radio controller
//
//*****************************************************************************
void am_devices_510L_radio_reset(void);

//*****************************************************************************
//
//! @brief Execute HCI write to the radio controller
//!
//! @param pui8Data    - Buffer to write the data from
//! @param ui32NumBytes - Number of bytes to write
//!
//! @return 32-bit status
//
//*****************************************************************************
uint32_t am_devices_510L_radio_write(uint8_t* pui8Data, uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Execute HCI synchronous write to the radio controller
//!
//! This function is used by the core to send a synchronous request packet that
//! need to wait for the corresponding response from the peer. For example, the
//! radio subsystem operational mode change request.
//! The synchronization of BT HCI command should be handled by the BT host stack,
//! so sending HCI packets should all use am_devices_510L_radio_write() API.
//!
//! @param pui8Data    - Buffer to write the data from
//! @param ui32NumBytes - Number of bytes to write
//!
//! @return 32-bit status
//
//*****************************************************************************
uint32_t am_devices_510L_radio_write_sync(uint8_t* pui8Data, uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Register the IPC RX callback
//!
//! @param cb     Pointer to the registered IPC RX callback
//! @param module Module number of IPC packet, should less than
//! AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_NUM
//!
//! @return Status of RX callback registration
//!
//*****************************************************************************
uint32_t am_devices_510_radio_ipc_register_rx_cback(am_devices_510L_radio_ipc_rx_cback cb,
                                                    uint8_t module);

//*****************************************************************************
//
//! @brief This function can be called to manually release the IPC RX Buffer
//!
//! @param buf    Pointer to the IPC RX buffer needs to be released
//!
//! @return Status of RX callback registration
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_release_ipc_rx_buf(uint8_t *buf);

//*****************************************************************************
//!
//! @brief Set the operational mode of radio subsystem via IPC
//!
//! @param mode request operational mode
//!
//! @return Status.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_set_rss_opmode(am_devices_510L_radio_rss_opmode_e mode);

//*****************************************************************************
//!
//! @brief Read register of another core via IPC
//!
//! @param ui32Address address of register
//!
//! @return Status.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_read_reg_req(uint32_t ui32Address);

//*****************************************************************************
//!
//! @brief Write register of another core via IPC
//!
//! @param ui32Address address of register
//! @param ui32Data  32-bit data to be written
//!
//! @return Status.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_write_reg_req(uint32_t ui32Address, uint32_t ui32Data);

//*****************************************************************************
//!
//! @brief Read memory of another core via IPC
//!
//! @param ui32Address start address of memory
//! @param ui8NumBytes number of bytes to be read
//!
//! @return Status.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_read_mem_req(uint32_t ui32Address, uint8_t ui8NumBytes);

//*****************************************************************************
//!
//! @brief Write register of another core via IPC
//!
//! @param ui32Address address of memory
//! @param pui8Data data pointer of buffer to be written
//! @param ui8NumBytes number of bytes to be written
//!
//! @return Status.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_write_mem_req(uint32_t ui32Address, uint8_t *pui8Data,
                                             uint8_t ui8NumBytes);

//*****************************************************************************
//!
//! @brief Set RF trim values to the network processing core via IPC
//!
//! @param pui8Data data pointer of buffer to be written
//!
//! @return Status.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_set_rftrim_req(uint8_t *pui8Data);

//*****************************************************************************
//
//! @brief Initialize the IPC share memory configuration.
//!
//! @param None.
//!
//! @note This function should be called before am_devices_510L_radio_init
//! to configure the ICP share memory information.
//!
//! @param psIpcShm - Pointer of IPC share memory structure
//!
//! @return Status.
//
//*****************************************************************************
uint32_t am_devices_510L_radio_ipc_shm_init(am_devices_510L_radio_ipc_shm_t *psIpcShm);

#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_510L_RADIO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

