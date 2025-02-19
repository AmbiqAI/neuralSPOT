//*****************************************************************************
//
//! @file am_devices_chsc5816_ap5.h
//!
//! @brief General I2C touch driver.
//!
//! @addtogroup chsc5816 CHSC5816 - I2C Touch Driver
//! @ingroup devices
//! @{
//
//**************************************************************************

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

#ifndef AM_DEVICES_CHSC5816_AP5_H
#define AM_DEVICES_CHSC5816_AP5_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Global type definitions.
//
//*****************************************************************************

#define AM_DEVICES_CHSC5816_MAX_DEVICE_NUM      1
#define AM_DEVICES_CHSC5816_REPORT_DATA_LENGTH  12

#define MAX_IO_BUFFER_LEN            32
#define PID_VID_MASK                 (0xffffff00)
#define NVM_W                        0x0
#define NVM_R                        0x3
#define CORE_R                       0x4
#define CORE_W                       0x5

#define TP_CMD_BUFF_ADDR             0x20000000
#define TP_RSP_BUFF_ADDR             0x20000000
#define TP_WR_BUFF_ADDR              0x20002000
#define TP_RD_BUFF_ADDR              0x20002400
#define TP_HOLD_MCU_ADDR             0x40007000
#define TP_AUTO_FEED_ADDR            0x40007010
#define TP_REMAP_MCU_ADDR            0x40007000
#define TP_RELEASE_MCU_ADDR          0x40007000
#define TP_HOLD_MCU_VAL              0x12044000
#define TP_AUTO_FEED_VAL             0x0000925a
#define TP_REMAP_MCU_VAL             0x12044002
#define TP_RELEASE_MCU_VAL           0x12044003
#define VID_PID_BACKUP_ADDR          (40 * 1024 + 0x10)

enum SEMI_DRV_ERR
{
    SEMI_DRV_ERR_OK = 0,
    SEMI_DRV_ERR_HAL_IO,
    SEMI_DRV_ERR_NO_INIT,
    SEMI_DRV_ERR_TIMEOUT,
    SEMI_DRV_ERR_CHECKSUM,
    SEMI_DRV_ERR_RESPONSE,
    SEMI_DRV_INVALID_CMD,
    SEMI_DRV_INVALID_PARAM,
    SEMI_DRV_ERR_NOT_MATCH,
};

//
//! ctp work staus
//
#define CTP_POINTING_WORK            0x00000000
#define CTP_READY_UPGRADE            (1 << 1)
#define CTP_UPGRAD_RUNING            (1 << 2)
#define CTP_SLFTEST_RUNING           (1 << 3)
#define CTP_SUSPEND_GATE             (1 << 16)
#define CTP_GUESTURE_GATE            (1 << 17)
#define CTP_PROXIMITY_GATE           (1 << 18)
#define CTP_GLOVE_GATE               (1 << 19)
#define CTP_ORIENTATION_GATE         (1 << 20)

#define AM_DEVICES_CHSC5816_READ_DATA_BLOCK           (0x2000002C)
#define AM_DEVICES_CHSC5816_I2C_WR                    0x0000
#define AM_DEVICES_CHSC5816_I2C_RD                    (1u << 0)
#define CHSC5816_SLAVE_ADDR                           0x2E

enum CMD_TYPE_ID
{
  CMD_NA              = 0x0f,
  CMD_IDENTITY        = 0x01,
  CMD_CTP_SSCAN       = 0x02,
  CMD_CTP_IOCTL       = 0x03,
  CMD_MEM_WR          = 0x30,
  CMD_MEM_RD          = 0x31,
  CMD_FLASH_ERASE     = 0x32,
  CMD_FW_SUM          = 0x33,
  CMD_WRITE_REGISTER  = 0X35,
  CMD_READ_REGISTER   = 0X36,
  CMD_BSPR_WRITE      = 0x37,
  CMD_BSPR_READ       = 0x38,
};

//
//! cammand struct for mcap
//
struct m_ctp_cmd_std_t
{
  unsigned short chk; // 16 bit checksum
  unsigned short d0;  //data 0
  unsigned short d1;  //data 1
  unsigned short d2;  //data 2
  unsigned short d3;  //data 3
  unsigned short d4;  //data 4
  unsigned short d5;  //data 5

  unsigned char  id;   //offset 15
  unsigned char  tag;  //offset 16
};

//
//! response struct for mcap
//
struct m_ctp_rsp_std_t
{
  unsigned short chk; // 16 bit checksum
  unsigned short d0;  //data 0
  unsigned short d1;  //data 1
  unsigned short d2;  //data 2
  unsigned short d3;  //data 3
  unsigned short d4;  //data 4
  unsigned short d5;  //data 5

  unsigned char  cc;  //offset 15
  unsigned char  id;  //offset 16
};

typedef struct sm_touch_dev
{
    unsigned int ctp_status;
    unsigned int vid_pid;
    unsigned int raw_adr;
    unsigned short fw_ver;
    unsigned short setup_ok;
    unsigned char  needUpd;
    unsigned char  *updPdata;
    unsigned short newBootLen;
}sm_touch_dev, *psm_touch_dev;

typedef struct _img_header_t
{
    uint16_t fw_ver;
    uint16_t checksum;
    uint32_t sig;
    uint32_t vid_pid;
    uint16_t raw_offet;
    uint16_t dif_offet;
}img_header_t;

struct chsc_updfile_header
{
    uint32_t sig;
    uint32_t resv;
    uint32_t n_cfg;
    uint32_t n_match;
    uint32_t len_cfg;
    uint32_t len_boot;
};

union rpt_point_t
{
    struct
    {
        unsigned char x_l8;
        unsigned char y_l8;
        unsigned char z;
        unsigned char x_h4:4;
        unsigned char y_h4:4;
        unsigned char id:4;
        unsigned char event:4;
    }rp;
    unsigned char data[5];
};

union rpt_multi_point_t
{
    struct
    {
        unsigned char x_l8;
        unsigned char y_l8;
        unsigned char z;
        unsigned char x_h4:4;
        unsigned char y_h4:4;
        unsigned char id:4;
        unsigned char event:4;
    }rp[2];
    unsigned char data[10];
};

typedef struct
{
    uint16_t x0;
    uint16_t y0;
    uint16_t x1;
    uint16_t y1;
    uint16_t dx0;
    uint16_t dy0;
    uint16_t dx1;
    uint16_t dy1;
    uint32_t event;
    uint32_t state;
    uint32_t kb_event;
    uint8_t  kb_key;
    uint8_t  finger_number;
    bool     touch_released;
    uint32_t timer_id;
    uint32_t timer_expirations;
    uint8_t  fw_ver;
} am_devices_tc_chsc5816_info_t;

typedef struct
{
    am_devices_tc_chsc5816_info_t *info;
    uint8_t                       *data;
    void                          *pfnCallback;
    void                          *pCallbackCtxt;
}chsc5816_high_level_info_t;

typedef struct
{
    //
    //! Select the interface clock frequency
    //
    uint32_t ui32ClockFreq;

    //
    //! Non-Blocking transaction memory configuration
    //! Set length and pointer to Transfer Control Buffer.
    //! Length is in 4 byte multiples
    //
    uint32_t *pNBTxnBuf;
    uint32_t ui32NBTxnBufLength;
} am_devices_iom_chsc5816_config_t;

typedef struct
{
    uint32_t    ui32IntPinNum;
    uint32_t    ui32RstPinNum;
    uint32_t    ui32Module;
    void        *pIomHandle;
    bool        bOccupied;
} am_devices_iom_gpio_chsc5816_t;

typedef int                             rt_bool_t;
typedef long                            rt_base_t;
typedef unsigned long                   rt_ubase_t;

typedef rt_base_t                       rt_err_t;
typedef rt_base_t                       rt_flag_t;
typedef rt_ubase_t                      rt_size_t;
typedef rt_ubase_t                      rt_dev_t;
typedef rt_base_t                       rt_off_t;

typedef signed   char                   rt_int8_t;
typedef signed   short                  rt_int16_t;
typedef signed   int                    rt_int32_t;
typedef unsigned char                   rt_uint8_t;
typedef unsigned short                  rt_uint16_t;
typedef unsigned int                    rt_uint32_t;

typedef enum
{
    AM_DEVICES_CHSC5816_STATUS_SUCCESS,
    AM_DEVICES_CHSC5816_STATUS_ERROR
} am_devices_chsc5816_status_t;

typedef enum
{
    AM_DEVICES_CHSC5816_STATUS_NO_EVENT,
    AM_DEVICES_CHSC5816_STATUS_TOUCH_DOWN,
    AM_DEVICES_CHSC5816_STATUS_SIGNIFICANT_DISPLACEMENT,
    AM_DEVICES_CHSC5816_STATUS_LIFT_OFF
} am_devices_chsc5816_event_status_t;

//*****************************************************************************
//
//! @brief Transfer callback function prototype
//
//*****************************************************************************
typedef void (*am_device_chsc_callback_t)(void *pCallbackCtxt);

//*****************************************************************************
//
//! @brief Get touch information of CHSC5816 Touch Sensor
//!
//! @param pDevHandle - Device handle.
//! @param touch_info - Pointer to the all touch information.
//!
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t
am_devices_chsc5816_get_point(void *pDevHandle, am_devices_tc_chsc5816_info_t *touch_info);

//*****************************************************************************
//
//! @brief Nonblocking get touch information of CHSC5816 Touch Sensor
//!
//! @param pDevHandle    - Device handle.
//! @param touch_info    - Pointer to the all touch information.
//! @param pfnCallback   - Get complete callback function.
//! @param pCallbackCtxt - The param of callback function.
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
uint32_t
am_devices_chsc5816_nonblocking_get_point(void* pDevHandle, am_devices_tc_chsc5816_info_t *touch_info, am_device_chsc_callback_t pfnCallback, void *pCallbackCtxt);

//*****************************************************************************
//
//! @brief Init the Device Driver and IOM for the CHSC5816 Touch Sensor
//!
//! @param ui32Module - IOM Module Number.
//! @param pDevConfig - Touch device config.
//! @param ppDevHandle - Device handle which needs to be return.
//! @param ppIomHandle - IOM handle which needs to be return.
//! @param ui32IntPin - Touch interrupt pin num.
//! @param ui32RstPin - Touch reset pin num
//! @param touch_handler - Touch interrupt handler.
//! @param pArg - The parameter of touch handler.
//!
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t
am_devices_chsc5816_init(uint32_t ui32Module,
                         am_devices_iom_chsc5816_config_t *pDevConfig,
                         void **ppDevHandle,
                         void **ppIomHandle,
                         uint32_t ui32IntPin,
                         uint32_t ui32RstPin,
                         am_hal_gpio_handler_t touch_handler,
                         void *pArg);

//*****************************************************************************
//
//! @brief deinit the Device Driver and IOM for the CHSC5816 Touch Sensor
//!
//! @param pDveHandle - Device handle.
//!
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t
am_devices_chsc5816_deinit(void *pDveHandle);

//*****************************************************************************
//
//! @brief Update chsc5816 firmware
//!
//! @param pDveHandle - Device handle.
//! @param fw_ver - firmware version.
//!
//!
//! @return status - generic or interface specific status.
//
//*****************************************************************************

#ifdef __cplusplus
}
#endif

#endif

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

