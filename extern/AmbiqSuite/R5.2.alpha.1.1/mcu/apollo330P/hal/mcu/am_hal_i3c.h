//*****************************************************************************
//
//! @file am_hal_i3c.h
//!
//! @brief Hardware abstraction for the Improved Inter-Integrated Circuit (I3C).
//!
//! @addtogroup i3c_ap510L I3C - Improved Inter-Integrated Circuit
//! @ingroup apollo330P_hal
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
#ifndef AM_HAL_I3C_H
#define AM_HAL_I3C_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! CMSIS-Style macro for handling a variable I3C module number.
//
//*****************************************************************************
#define I3Cn(n)                     ((I3C_Type*)(I3C_BASE + (n * (I3C_BASE - I3C_BASE))))

//*****************************************************************************
//
//! @name I3C Interrupts
//! @brief Macro definitions for I3C interrupt status bits.
//!
//! These macros correspond to the bits in the I3C interrupt status register.
//
//*****************************************************************************
#define AM_HAL_I3C_INT_PIO_TXTHLD             I3C_PIOINTRSTATUSENABLE_TXTHLDSTATEN_Msk
#define AM_HAL_I3C_INT_PIO_RXTHLD             I3C_PIOINTRSTATUSENABLE_RXTHLDSTATEN_Msk
#define AM_HAL_I3C_INT_PIO_IBISTATUSTHLD      I3C_PIOINTRSTATUSENABLE_IBISTATUSTHLDSTATEN_Msk
#define AM_HAL_I3C_INT_PIO_CMDQUEUEREADY      I3C_PIOINTRSTATUSENABLE_CMDQUEUEREADYSTATEN_Msk
#define AM_HAL_I3C_INT_PIO_RESPREADY          I3C_PIOINTRSTATUSENABLE_RESPREADYSTATEN_Msk
#define AM_HAL_I3C_INT_PIO_TRANSFERABORT      I3C_PIOINTRSTATUSENABLE_PIOTRANSFERABORTSTATEN_Msk
#define AM_HAL_I3C_INT_PIO_TRANSFERERR        I3C_PIOINTRSTATUSENABLE_PIOTRANSFERERRSTATEN_Msk
#define AM_HAL_I3C_INT_HCINTERNALERR          I3C_INTRSTATUSENABLE_HCINTERNALERRSTATEN_Msk
#define AM_HAL_I3C_INT_HCSEQCANCEL            I3C_INTRSTATUSENABLE_HCSEQCANCELSTATEN_Msk
#define AM_HAL_I3C_INT_HCWARNCMDSEQSTALL      I3C_INTRSTATUSENABLE_HCWARNCMDSEQSTALLSTATEN_Msk
#define AM_HAL_I3C_INT_HCERRCMDSEQTIMEOUT     I3C_INTRSTATUSENABLE_HCERRCMDSEQTIMEOUTSTATEN_Msk
#define AM_HAL_I3C_INT_RHTRANSFERABORT       (I3C_RHINTRSTATUSENABLE_RHTRANSFERABORTSTATEN_Msk << 16)
#define AM_HAL_I3C_INT_IBIRINGFULL           (I3C_RHINTRSTATUSENABLE_IBIRINGFULLSTATEN_Msk << 16)
#define AM_HAL_I3C_INT_RHTRANSFERERR         (I3C_RHINTRSTATUSENABLE_RHTRANSFERERRSTATEN_Msk << 16)
#define AM_HAL_I3C_INT_RINGOP                (I3C_RHINTRSTATUSENABLE_RINGOPSTATEN_Msk << 16)
#define AM_HAL_I3C_INT_RHTRANSFERCOMPLETION  (I3C_RHINTRSTATUSENABLE_RHTRANSFERCOMPLETIONSTATEN_Msk << 16)
#define AM_HAL_I3C_INT_IBIREADY              (I3C_RHINTRSTATUSENABLE_IBIREADYSTATEN_Msk << 16)
#define AM_HAL_I3C_INT_ALL                   0xFFFFFFFF
#define AM_HAL_I3C_INT_ERR                   (AM_HAL_I3C_INT_HCINTERNALERR | AM_HAL_I3C_INT_PIO_TRANSFERERR | AM_HAL_I3C_INT_RHTRANSFERERR)

#define AM_HAL_I3C_CMD0_DESPC_TOC      (1 << 31)
#define AM_HAL_I3C_CMD0_DESPC_ROC      (1 << 30)

#define AM_HAL_I3C_BROADCAST_ADDR      0x7E

// Commands support both broadcast and unicast modes
#define AM_HAL_I3C_CCC_ENEC(bc)            ((bc) ? 0x0 : 0x80)
#define AM_HAL_I3C_CCC_DISEC(bc)           ((bc) ? 0x1 : 0x81)
#define AM_HAL_I3C_CCC_ENTAS(as, bc)       (((bc) ? 0x02 : 0x82) + (as))
#define AM_HAL_I3C_CCC_RSTDAA(bc)          ((bc) ? 0x6 : 0x86)
#define AM_HAL_I3C_CCC_SETMWL(bc)          ((bc) ? 0x9 : 0x89)
#define AM_HAL_I3C_CCC_SETMRL(bc)          ((bc) ? 0xA : 0x8A)
#define AM_HAL_I3C_CCC_SETXTIME(bc)        ((bc) ? 0x28 : 0x98)
#define AM_HAL_I3C_CCC_VENDOR(bc, id)      ((id) + ((bc) ? 0x61 : 0xE0))
#define AM_HAL_I3C_CCC_RSTACT(bc)          ((bc) ? 0x2A : 0x9A)

// Commands only support broadcast modes
#define AM_HAL_I3C_CCC_ENTDAA              0x7
#define AM_HAL_I3C_CCC_DEFTGTS             0x8
#define AM_HAL_I3C_CCC_ENTTM               0xB
#define AM_HAL_I3C_CCC_ENTHDR0             0x20
#define AM_HAL_I3C_CCC_ENTHDR1             0x21
#define AM_HAL_I3C_CCC_ENTHDR2             0x22
#define AM_HAL_I3C_CCC_SETAASA             0x29

// Commands only support unicast modes
#define AM_HAL_I3C_CCC_SETDASA             0x87
#define AM_HAL_I3C_CCC_SETNEWDA            0x88
#define AM_HAL_I3C_CCC_GETMWL              0x8B
#define AM_HAL_I3C_CCC_GETMRL              0x8C
#define AM_HAL_I3C_CCC_GETPID              0x8D
#define AM_HAL_I3C_CCC_GETBCR              0x8E
#define AM_HAL_I3C_CCC_GETDCR              0x8F
#define AM_HAL_I3C_CCC_GETSTATUS           0x90
#define AM_HAL_I3C_CCC_GETACCCR            0x91
#define AM_HAL_I3C_CCC_SETBRGTGT           0x93
#define AM_HAL_I3C_CCC_GETMXDS             0x94
#define AM_HAL_I3C_CCC_GETCAPS             0x95
#define AM_HAL_I3C_CCC_GETXTIME            0x99

// Max ring number supported by I3C host
#define AM_HAL_I3C_MAX_RING_NUM            4

// Max Ring Size
#define AM_HAL_I3C_MAX_RING_SIZE           255

#define AM_HAL_I3C_TIMEOUT_CNT             100000
#define AM_HAL_I3C_DELAY_US                10

//
//! I3C Xfer Mode
//
typedef enum
{
    AM_HAL_I3C_XFER_PIO,
    AM_HAL_I3C_XFER_DMA,
} am_hal_i3c_xfer_mode_e;

//
//! I3C Speed Mode
//
typedef enum
{
    AM_HAL_I3C_SDR0      = 0x0, // 12MHz
    AM_HAL_I3C_SDR1      = 0x1, // 8MHz
    AM_HAL_I3C_SDR2      = 0x2, // 6MHz
    AM_HAL_I3C_SDR3      = 0x3, // 4MHz
    AM_HAL_I3C_SDR4      = 0x4, // 2MHz
    AM_HAL_I3C_HDR_TS    = 0x5,
    AM_HAL_I3C_HDR_DDR   = 0x6,
    AM_HAL_I3C_I2C_MODE0 = 0x0, // 400KHz
    AM_HAL_I3C_I2C_MODE1 = 0x1, // 1MHz
    AM_HAL_I3C_I2C_MODE2 = 0x2, // User defined speed
    AM_HAL_I3C_I2C_MODE3 = 0x3, // User defined speed
    AM_HAL_I3C_I2C_MODE4 = 0x4, // User defined speed
} am_hal_i3c_speed_mode_e;

//
//! Data Direction
//
typedef enum
{
    AM_HAL_I3C_DIR_WRITE = 0x0,
    AM_HAL_I3C_DIR_READ  = 0x1,
} am_hal_i3c_dir_e;

//
//! Device Type
//
typedef enum
{
    AM_HAL_I3C_DEVICE_I3C,
    AM_HAL_I3C_DEVICE_I2C,
} am_hal_i3c_device_type_e;

//
//! I3C Event Type
//
typedef enum
{
    AM_HAL_I3C_EVT_TX_COMPLETE,
    AM_HAL_I3C_EVT_RX_COMPLETE,
    AM_HAL_I3C_EVT_IBI,
    AM_HAL_I3C_EVT_ABORT,
    AM_HAL_I3C_EVT_ERROR,
} am_hal_i3c_event_type_e;

//
//! I3C Event Struct
//
typedef struct
{
    am_hal_i3c_event_type_e eType;
    uint32_t ui32Context;
    void *pCtxt;
} am_hal_i3c_event_t;

typedef void (*am_hal_i3c_event_cb_t)(am_hal_i3c_event_t *pEvent);

//
//! Command 0 Descriptor Struct
//
typedef union
{
    uint32_t ui32CmdDesc0;
    struct
    {
        uint32_t attr   : 3;
        uint32_t tid    : 4;
        uint32_t cmd    : 8;
        uint32_t cp     : 1;
        uint32_t idx    : 5;
        uint32_t rsvd   : 3;
        uint32_t sre    : 1;
        uint32_t dbp    : 1;
        uint32_t mode   : 3;
        uint32_t rnw    : 1;
        uint32_t roc    : 1;
        uint32_t toc    : 1;
    } RegCmdDesc0;
    struct
    {
        uint32_t attr   : 3;
        uint32_t tid    : 4;
        uint32_t cmd    : 8;
        uint32_t cp     : 1;
        uint32_t idx    : 5;
        uint32_t rsvd   : 2;
        uint32_t dtt    : 3;
        uint32_t mode   : 3;
        uint32_t rnw    : 1;
        uint32_t roc    : 1;
        uint32_t toc    : 1;
    } ImmCmdDesc0;
    struct
    {
        uint32_t attr   : 3;
        uint32_t tid    : 4;
        uint32_t cmd    : 8;
        uint32_t rsvd0  : 1;
        uint32_t idx    : 5;
        uint32_t rsvd1  : 5;
        uint32_t devcnt : 4;
        uint32_t roc    : 1;
        uint32_t toc    : 1;
    } DaaCmdDesc0;
    struct
    {
        uint32_t attr      : 3;
        uint32_t tid       : 4;
        uint32_t cmd       : 8;
        uint32_t cp        : 1;
        uint32_t idx       : 5;
        uint32_t rsvd      : 1;
        uint32_t lenthpos  : 2;
        uint32_t firstmode : 1;
        uint32_t ofs16bit  : 1;
        uint32_t mode      : 3;
        uint32_t rnw       : 1;
        uint32_t roc       : 1;
        uint32_t toc       : 1;
    } ComboCmdDesc0;

} am_hal_i3c_cmd_desc0_t;

//
//! Command 1 Descriptor Struct
//
typedef union
{
    uint32_t ui32CmdDesc1;
    struct
    {
        uint32_t def_byte   : 8;
        uint32_t rsvd       : 8;
        uint32_t data_len   : 16;
    } RegCmdDesc1;
    struct
    {
        uint32_t byte1   : 8;
        uint32_t byte2   : 8;
        uint32_t byte3   : 8;
        uint32_t byte4   : 8;
    } ImmCmdDesc1;
    struct
    {
        uint32_t sub_offset : 16;
        uint32_t data_len   : 16;
    } ComboCmdDesc1;
} am_hal_i3c_cmd_desc1_t;

//
//! I3C Data Descriptor Struct
//
typedef struct
{
    bool     bBLP;
    bool     bIOC;
    uint16_t ui16BlkSize;
    uint32_t ui32DatPtrLO;
    uint32_t ui32DatPtrHI;
} am_hal_i3c_data_desc_t;

//
//! IBI Status Struct
//
typedef union
{
    uint32_t ui32IbiStatus;
    struct
    {
        uint32_t dat_len   : 8;
        uint32_t ibi_id    : 8;
        uint32_t chunks    : 8;
        uint32_t last_stat : 1;
        uint32_t ts        : 1;
        uint32_t hw_ctx    : 3;
        uint32_t stat_type : 1;
        uint32_t error     : 1;
        uint32_t ibi_sts   : 1;
    } IbiStatus;
}am_hal_i3c_ibi_status_t;


//
//! I3C Ring Header Struct
//
typedef struct
{
    void    *pXfer;
    void    *pResp;
    void    *pIbiStat;
    void    *pIbiData;
    uint32_t ui32XferAddr;
    uint32_t ui32RespAddr;
    uint32_t ui32IbiStatAddr;
    uint32_t ui32IbiDataAddr;
    uint32_t ui32XferStructSize;
    uint32_t ui32RespStructSize;
    uint32_t ui32IbiStatStructSize;
    uint32_t ui32DeQptr;
    am_hal_i3c_data_desc_t DataDesc;
} am_hal_i3c_rh_t;


//
//! I3C Ring Struct
//
typedef struct
{
    //
    //! Command Ring Buffer
    //
    uint32_t *pui32CmdRingBuf;
    uint32_t ui32CmdRingBufLen;

    //
    //! Response Ring Buffer
    //
    uint32_t *pui32RespRingBuf;
    uint32_t ui32RespRingBufLen;

    //
    //! IBI Status Ring Buffer
    //
    uint32_t *pui32IbiStatRingBuf;
    uint32_t ui32IbiStatBufLen;

    //
    //! IBI Data Ring Buffer
    //
    uint32_t *pui32IbiDataRingBuf;
    uint32_t ui32IbiDataBufLen;

    //
    //! Ring Header Struct
    //
    am_hal_i3c_rh_t RHeader[AM_HAL_I3C_MAX_RING_NUM];
} am_hal_i3c_ring_t;

//
//! Command Data Struct
//
typedef struct
{
    uint8_t  ui8Addr;
    uint8_t  ui8Id;
    uint8_t  ui8Tid;
    uint32_t ui32CmdDesc0;
    uint32_t ui32CmdDesc1;
    uint32_t ui32Resp;
    am_hal_i3c_dir_e eDir;
    am_hal_i3c_speed_mode_e eSpeedMode;
    uint8_t *pui8Buf;
    uint32_t ui32DataLen;
    uint32_t ui32DataLeft;
    bool     bComboCmd;
    bool     bCombo16bitOfs;
    uint16_t ui16ComboOfs;
} am_hal_i3c_cmd_data_t;

//
//! I3C Device Struct
//
typedef struct
{
    uint8_t ui8StaticAddr;
    uint8_t ui8DynamicAddr;
    uint8_t ui8BCR;
    uint8_t ui8DCR;
    uint64_t ui64PID;
    am_hal_i3c_device_type_e eDeviceType;
} am_hal_i3c_device_t;

//
//! I3C Host Struct
//
typedef struct
{
    void *pHandle;
    uint32_t ui32Module;
    bool bInited;
    uint32_t ui32HciVersion;
    uint32_t ui32Capabilities;
    uint32_t ui32DatTableSize;
    uint32_t ui32DatTableOffset;
    uint32_t ui32DctSectionOffset;
    uint32_t ui32RHSectionOffset;
    uint32_t ui32PioSectionOffset;
    uint32_t ui32EXTCAPSectionOffset;
    uint32_t ui32CAPID;
    uint32_t ui32CAPLENGTH;
    uint32_t ui32QueueSize;
    uint32_t ui32RxBufSize;
    uint32_t ui32TxBufSize;
    uint32_t ui32IbiQueueSize;
    am_hal_i3c_xfer_mode_e eXferMode;
    am_hal_i3c_speed_mode_e eSpeedMode;
    am_hal_i3c_cmd_data_t *pCmdData;
    am_hal_i3c_ring_t *pRing;
    am_hal_i3c_event_cb_t pfunCallback;
    am_hal_i3c_event_t Event;
} am_hal_i3c_host_t;

//
//! I3C transfer structure
//
typedef struct
{
    //
    //! Number of bytes to transfer
    //
    uint32_t ui32NumBytes;

    //
    //! CCC Cmd ID
    //
    uint8_t  ui8CCCID;

    //
    //! speed mode
    //
    am_hal_i3c_speed_mode_e eSpeedMode;

    //
    //! Transfer Direction (Transmit/Receive)
    //
    am_hal_i3c_dir_e eDirection;

    //
    //! Device Struct
    //
    am_hal_i3c_device_t Device;

    //
    //! Enable Combo Xfer
    //
    bool bComboXfer;

    //
    //! Enable 16bit Offset. Default 8bit.
    //
    bool bCombo16bitOfs;

    //
    //! Combo Xfer Offset
    //
    uint16_t ui16ComboOfs;

    //
    //! Data Buffer
    //
    uint32_t *pui32TxBuffer;
    uint32_t *pui32RxBuffer;

    //
    //! IBI Status & Data Buffer
    //
    uint32_t *pui32IbiStatBuf;
    uint32_t *pui32IbiDataBuf;

    //
    //! Command Ring Buffer
    //
    uint32_t *pui32CmdRingBuf;
    uint32_t ui32CmdRingBufLen;

    //
    //! Response Ring Buffer
    //
    uint32_t *pui32RespRingBuf;
    uint32_t ui32RespRingBufLen;

} am_hal_i3c_transfer_t;


//*****************************************************************************
//
//! @brief I3C initialization function
//!
//! @param ui32Module   - module instance.
//! @param ppHandle     - returns the handle for the module instance.
//!
//! This function accepts a module instance, allocates the interface and then
//! returns a handle to be used by the remaining interface functions.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_initialize(uint32_t ui32Module,
                                       void **ppHandle);

//*****************************************************************************
//
//! @brief I3C deinitialization function
//!
//! @param pHandle      - the handle for the module instance.
//!
//! This function accepts a handle to an instance and de-initializes the
//! interface.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_deinitialize(void *pHandle);

//*****************************************************************************
//
//! @brief I3C controller power control function
//!
//! @param pHandle      - handle for the interface.
//! @param ePowerState  - the desired power state to move the peripheral to.
//! @param bRetainState - flag (if true) to save/restore peripheral state upon
//!                       power state change.
//!
//! This function updates the I3C controller to a given power state.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_controller_power_control(void *pHandle,
                                          am_hal_sysctrl_power_state_e ePowerState,
                                          bool bRetainState);

//*****************************************************************************
//
//! @brief I3C PHY power control function
//!
//! @param pHandle      - handle for the interface.
//! @param ePowerState  - the desired power state to move the peripheral to.
//! @param bRetainState - flag (if true) to save/restore peripheral state upon
//!                       power state change.
//!
//! This function updates the I3C PHY to a given power state.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_phy_power_control(void *pHandle,
                                          am_hal_sysctrl_power_state_e ePowerState);

//*****************************************************************************
//
//! @brief I3C setup host function
//!
//! @param pHandle      - handle for the interface.
//! @param eXferMode    - Host Xfer Mode (PIO or DMA).
//! @param eSpeedMode   - Host Speed Mode.
//!
//! This function checks the capabilites of I3C host controller and configures
//! host controller's Xfer mode and speed.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_setup_host(void *pHandle,
                                      am_hal_i3c_xfer_mode_e eXferMode,
                                      am_hal_i3c_speed_mode_e eSpeedMode);


//*****************************************************************************
//
//! @brief I3C enable function
//!
//! @param pHandle      - the handle for the module instance.
//!
//! This function accepts a handle to an instance and enables the interface.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_enable(void *pHandle);

//*****************************************************************************
//
//! @brief I3C disable function
//!
//! @param pHandle      - the handle for the module instance.
//!
//! This function accepts a handle to an instance and disables the interface.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_disable(void *pHandle);

//*****************************************************************************
//
//! @brief I3C blocking transfer function
//!
//! @param pHandle      - handle for the interface.
//! @param psTransaction - pointer to the transaction control structure.
//!
//! This function performs a transaction on the I3C in PIO mode.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_blocking_transfer(void *pHandle,
                                             am_hal_i3c_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief I3C Non-Blocking transfer function
//!
//! @param pHandle      - handle for the interface.
//! @param psTransaction - pointer to the transaction control structure.
//!
//! This function performs a transaction on the I3C in DMA mode.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_nonblocking_transfer(void *pHandle,
                                             am_hal_i3c_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief I3C send CCC command function
//!
//! @param pHandle      - handle for the interface.
//! @param psTransaction - pointer to the transaction control structure.
//!
//! This function sends Common Command Code (CCC) to slave devices.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_send_ccc_cmd(void *pHandle, am_hal_i3c_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief I3C Dynamic Address Assignment
//!
//! @param pHandle      - handle for the interface.
//! @param psTransaction - pointer to the transaction control structure.
//!
//! This function performs I3C Dynamic Address Assignment.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_do_daa(void *pHandle, am_hal_i3c_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief Register I3C Event Callback Function
//!
//! @param pHandle      - handle for the interface.
//!
//! @param pfunCallback - function pointer to the callback function.
//!
//! This function registers the I3C event callback function.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_register_evt_callback(void *pHandle, am_hal_i3c_event_cb_t pfunCallback);

//*****************************************************************************
//
//! @brief Set user defined I2C mode speed
//!
//! @param pHandle       - handle for the interface.
//! @param eI2CSpeedMode - user defined I2C speed mode
//! @param ui8LowCnt     - count for SCL Low
//! @param ui8HighCnt    - count for SCL High
//!
//! This function sets the I2C speed via setting duty cycles of both low and
//! high period of SCL.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_set_i2c_speed(void *pHandle, am_hal_i3c_speed_mode_e eI2CSpeedMode, uint8_t ui8LowCnt, uint8_t ui8HighCnt);
//*****************************************************************************
//
//! @brief I3C IBI enable function
//!
//! @param pHandle      - handle for the interface.
//!
//! @param psTransaction - pointer to the transaction control structure.
//!
//! This function enables I3C device IBI.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_ibi_enable(void *pHandle, am_hal_i3c_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief I3C IBI disable function
//!
//! @param pHandle      - handle for the interface.
//!
//! @param psTransaction - pointer to the transaction control structure.
//!
//! This function disables I3C device IBI.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_ibi_disable(void *pHandle, am_hal_i3c_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief I3C enable interrupts function
//!
//! @param pHandle      - handle for the interface.
//! @param ui32IntMask  - I3C interrupt mask.
//!
//! This function enables the normal or error specific indicated interrupts.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_intr_status_enable(void *pHandle,
                                             uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I3C disable interrupts function
//!
//! @param pHandle      - handle for the interface.
//! @param ui32IntMask  - I3C interrupt mask.
//!
//! This function disables the normal or error specific indicated interrupts.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_intr_status_disable(void *pHandle,
                                              uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I3C interrupt status function
//!
//! @param pHandle      - handle for the interface.
//! @param pui32Status  - returns the interrupt status value.
//! @param bEnabledOnly - TRUE: only report interrupt status for enalbed ints.
//!                       FALSE: report all interrupt status values.
//!
//! This function returns the normal or error specific indicated interrupt status.
//!
//! @return status      - interrupt status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_intr_status_get(void *pHandle,
                                                 uint32_t  *pui32Status,
                                                 bool bEnabledOnly);

//*****************************************************************************
//
//! @brief I3C interrupt clear
//!
//! @param pHandle        - handle for the interface.
//! @param ui32IntMask    - uint32_t for interrupts to clear
//!
//! This function clears the normal or error interrupts for the given peripheral.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_intr_status_clear(void *pHandle,
                                            uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I3C interrupt clear
//!
//! @param pHandle        - handle for the interface.
//! @param ui32IntMask    - uint32_t for interrupts to enable
//!
//! This function enables the normal or error interrupts to generate the I3C IRQ.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_intr_signal_enable(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I3C interrupt clear
//!
//! @param pHandle        - handle for the interface.
//! @param ui32IntMask    - uint32_t for interrupts to clear
//!
//! This function disables the normal or error interrupts to generate the I3C IRQ.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************

extern uint32_t am_hal_i3c_intr_signal_disable(void *pHandle, uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief I3C interrupt service routine
//!
//! @param pHandle       - handle for the interface.
//! @param ui32IntStatus - interrupt status.
//!
//! This function is designed to be called from within the user defined ISR
//! in order to service the non-blocking processing for a given
//! module instance.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_interrupt_service(void *pHandle,
                                              uint32_t ui32IntStatus);

//*****************************************************************************
//
//! @brief I3C initialization function
//!
//! @param ui32Module   - module instance.
//! @param ppHandle     - returns the handle for the module instance.
//! @param eXferMode    - Host Xfer Mode (PIO or DMA).
//! @param eSpeedMode   - Host Speed Mode.
//!
//! This function accepts a module instance, allocates the interface and then
//! returns a handle to be used by the remaining interface functions.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_controller_init(uint32_t ui32Module,
                                            void **ppHandle,
                                            am_hal_i3c_xfer_mode_e eXferMode,
                                            am_hal_i3c_speed_mode_e eSpeedMode);

//*****************************************************************************
//
//! @brief I3C host controller deinitialization function
//!
//! @param pHandle      - the handle for the module instance.
//! @param bPullDown    - flag (if true) to pull down I3C PHY after power off.
//!
//! This function accepts a handle to an instance and de-initializes the
//! interface.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_i3c_controller_deinit(void *pHandle, bool bPullDown);


#ifdef __cplusplus
}
#endif

#endif // AM_HAL_I3C_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

