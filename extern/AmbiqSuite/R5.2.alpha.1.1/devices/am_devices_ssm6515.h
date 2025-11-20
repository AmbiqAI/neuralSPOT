//*****************************************************************************
//
//! @file am_devices_ssm6515.h
//!
//! @brief Analog Devices SSM6515 Digital Audio Amplifier Driver with I2C Interface.
//!
//! @addtogroup devices_ssm6515 SSM6515 I2C Audio Amplifier Driver
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_DEVICES_SSM6515_H
#define AM_DEVICES_SSM6515_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "am_mcu_apollo.h"

typedef unsigned char  ADI_REG_TYPE;

#define AM_DEVICES_SSM6515_I2C_WR 0x0000
#define AM_DEVICES_SSM6515_I2C_RD (1u << 0)

/* VENDOR_ID  - Registers (IC 1) */
#define REG_VENDOR_ID_IC_1_ADDR                   0x0
#define REG_VENDOR_ID_IC_1_BYTE                   0
#define REG_VENDOR_ID_IC_1_VALUE                  0x41

/* DEVICE_ID1  - Registers (IC 1) */
#define REG_DEVICE_ID1_IC_1_ADDR                  0x1
#define REG_DEVICE_ID1_IC_1_BYTE                  0
#define REG_DEVICE_ID1_IC_1_VALUE                 0x65

/* DEVICE_ID2  - Registers (IC 1) */
#define REG_DEVICE_ID2_IC_1_ADDR                  0x2
#define REG_DEVICE_ID2_IC_1_BYTE                  0
#define REG_DEVICE_ID2_IC_1_VALUE                 0x15

/* REVISION  - Registers (IC 1) */
#define REG_REVISION_IC_1_ADDR                    0x3
#define REG_REVISION_IC_1_BYTE                    0
#define REG_REVISION_IC_1_VALUE                   0x2

/* PWR_CTRL  - Registers (IC 1) */
#define REG_PWR_CTRL_IC_1_ADDR                    0x4
#define REG_PWR_CTRL_IC_1_BYTE                    1
#define REG_PWR_CTRL_IC_1_VALUE                   0x0

/* CLK_CTRL  - Registers (IC 1) */
#define REG_CLK_CTRL_IC_1_ADDR                    0x5
#define REG_CLK_CTRL_IC_1_BYTE                    1
#define REG_CLK_CTRL_IC_1_VALUE                   0x0

/* PDM_CTRL  - Registers (IC 1) */
#define REG_PDM_CTRL_IC_1_ADDR                    0x6
#define REG_PDM_CTRL_IC_1_BYTE                    1
#define REG_PDM_CTRL_IC_1_VALUE                   0x20

/* DAC_CTRL1  - Registers (IC 1) */
#define REG_DAC_CTRL1_IC_1_ADDR                   0x7
#define REG_DAC_CTRL1_IC_1_BYTE                   1
#define REG_DAC_CTRL1_IC_1_VALUE                  0x14

/* DAC_CTRL2  - Registers (IC 1) */
#define REG_DAC_CTRL2_IC_1_ADDR                   0x8
#define REG_DAC_CTRL2_IC_1_BYTE                   1
#define REG_DAC_CTRL2_IC_1_VALUE                  0x2

/* DAC_CTRL3  - Registers (IC 1) */
#define REG_DAC_CTRL3_IC_1_ADDR                   0x9
#define REG_DAC_CTRL3_IC_1_BYTE                   1
#define REG_DAC_CTRL3_IC_1_VALUE                  0xD0

/* DAC_VOL  - Registers (IC 1) */
#define REG_DAC_VOL_IC_1_ADDR                     0xA
#define REG_DAC_VOL_IC_1_BYTE                     1
#define REG_DAC_VOL_IC_1_VALUE                    0x40

/* DAC_CLIP  - Registers (IC 1) */
#define REG_DAC_CLIP_IC_1_ADDR                    0xB
#define REG_DAC_CLIP_IC_1_BYTE                    1
#define REG_DAC_CLIP_IC_1_VALUE                   0xFF

/* SPT_CTRL1  - Registers (IC 1) */
#define REG_SPT_CTRL1_IC_1_ADDR                   0xC
#define REG_SPT_CTRL1_IC_1_BYTE                   1
#define REG_SPT_CTRL1_IC_1_VALUE                  0x0

/* SPT_CTRL2  - Registers (IC 1) */
#define REG_SPT_CTRL2_IC_1_ADDR                   0xD
#define REG_SPT_CTRL2_IC_1_BYTE                   1
#define REG_SPT_CTRL2_IC_1_VALUE                  0x0

/* AMP_CTRL  - Registers (IC 1) */
#define REG_AMP_CTRL_IC_1_ADDR                    0xE
#define REG_AMP_CTRL_IC_1_BYTE                    1
#define REG_AMP_CTRL_IC_1_VALUE                   0x15

/* LIM_CTRL  - Registers (IC 1) */
#define REG_LIM_CTRL_IC_1_ADDR                    0xF
#define REG_LIM_CTRL_IC_1_BYTE                    1
#define REG_LIM_CTRL_IC_1_VALUE                   0x0

/* LIM_CTRL2  - Registers (IC 1) */
#define REG_LIM_CTRL2_IC_1_ADDR                   0x10
#define REG_LIM_CTRL2_IC_1_BYTE                   1
#define REG_LIM_CTRL2_IC_1_VALUE                  0xA

/* FAULT_CTRL  - Registers (IC 1) */
#define REG_FAULT_CTRL_IC_1_ADDR                  0x11
#define REG_FAULT_CTRL_IC_1_BYTE                  1
#define REG_FAULT_CTRL_IC_1_VALUE                 0x0

/* STATUS_CLR  - Registers (IC 1) */
#define REG_STATUS_CLR_IC_1_ADDR                  0x12
#define REG_STATUS_CLR_IC_1_BYTE                  1
#define REG_STATUS_CLR_IC_1_VALUE                 0x0

/* STATUS  - Registers (IC 1) */
#define REG_STATUS_IC_1_ADDR                      0x13
#define REG_STATUS_IC_1_BYTE                      0
#define REG_STATUS_IC_1_VALUE                     0x0

/* RESET  - Registers (IC 1) */
#define REG_RESET_IC_1_ADDR                       0x14
#define REG_RESET_IC_1_BYTE                       1
#define REG_RESET_IC_1_VALUE                      0x0


/*
 *
 * Control register's field descriptions
 *
 */

/* VENDOR_ID (IC 1) */
#define R0_VENDOR_IC_1                            0x41   /* 01000001b [7:0] */
#define R0_VENDOR_IC_1_MASK                       0xFF
#define R0_VENDOR_IC_1_SHIFT                      0

/* DEVICE_ID1 (IC 1) */
#define R1_DEVICE1_IC_1                           0x65   /* 01100101b [7:0] */
#define R1_DEVICE1_IC_1_MASK                      0xFF
#define R1_DEVICE1_IC_1_SHIFT                     0

/* DEVICE_ID2 (IC 1) */
#define R2_DEVICE2_IC_1                           0x15   /* 00010101b [7:0] */
#define R2_DEVICE2_IC_1_MASK                      0xFF
#define R2_DEVICE2_IC_1_SHIFT                     0

/* REVISION (IC 1) */
#define R3_REV_IC_1                               0x02   /* 00000010b [7:0] */
#define R3_REV_IC_1_MASK                          0xFF
#define R3_REV_IC_1_SHIFT                         0

/* PWR_CTRL (IC 1) */
#define R4_SPWDN_IC_1                             0x0    /* 0b [0] */
#define R4_APWDN_EN_IC_1                          0x0    /* 0b [1] */
#define R4_LIM_EN_IC_1                            0x0    /* 0b [4] */
#define R4_SPWDN_IC_1_MASK                        0x1
#define R4_SPWDN_IC_1_SHIFT                       0
#define R4_APWDN_EN_IC_1_MASK                     0x2
#define R4_APWDN_EN_IC_1_SHIFT                    1
#define R4_LIM_EN_IC_1_MASK                       0x10
#define R4_LIM_EN_IC_1_SHIFT                      4

/* CLK_CTRL (IC 1) */
#define R5_BCLK_RATE_IC_1                         0x0    /* 00000b [4:0] */
#define R5_BCLK_RATE_IC_1_MASK                    0x1F
#define R5_BCLK_RATE_IC_1_SHIFT                   0

/* PDM_CTRL (IC 1) */
#define R6_PDM_MODE_IC_1                          0x0    /* 0b  [0] */
#define R6_PDM_FS_IC_1                            0x0    /* 00b [2:1] */
#define R6_PDM_CHAN_SEL_IC_1                      0x0    /* 0b  [3] */
#define R6_PDM_FILT_IC_1                          0x2    /* 10b [5:4] */
#define R6_PDM_PHASE_SEL_IC_1                     0x0    /* 0b  [7] */
#define R6_PDM_MODE_IC_1_MASK                     0x1
#define R6_PDM_MODE_IC_1_SHIFT                    0
#define R6_PDM_FS_IC_1_MASK                       0x6
#define R6_PDM_FS_IC_1_SHIFT                      1
#define R6_PDM_CHAN_SEL_IC_1_MASK                 0x8
#define R6_PDM_CHAN_SEL_IC_1_SHIFT                3
#define R6_PDM_FILT_IC_1_MASK                     0x30
#define R6_PDM_FILT_IC_1_SHIFT                    4
#define R6_PDM_PHASE_SEL_IC_1_MASK                0x80
#define R6_PDM_PHASE_SEL_IC_1_SHIFT               7

/* DAC_CTRL1 (IC 1) */
#define R7_DAC_FS_IC_1                            0x5    /* 0101b [3:0] */
#define R7_DAC_PWR_MODE_IC_1                      0x1    /* 01b   [5:4] */
#define R7_DAC_IBIAS_IC_1                         0x0    /* 00b   [7:6] */
#define R7_DAC_FS_IC_1_MASK                       0xF
#define R7_DAC_FS_IC_1_SHIFT                      0
#define R7_DAC_PWR_MODE_IC_1_MASK                 0x30
#define R7_DAC_PWR_MODE_IC_1_SHIFT                4
#define R7_DAC_IBIAS_IC_1_MASK                    0xC0
#define R7_DAC_IBIAS_IC_1_SHIFT                   6

/* DAC_CTRL2 (IC 1) */
#define R8_DAC_MUTE_IC_1                          0x0    /* 0b  [0] */
#define R8_DAC_VOL_MODE_IC_1                      0x1    /* 01b [2:1] */
#define R8_DAC_MORE_FILT_IC_1                     0x0    /* 0b  [3] */
#define R8_DAC_VOL_ZC_IC_1                        0x0    /* 0b  [4] */
#define R8_DAC_HARD_VOL_IC_1                      0x0    /* 0b  [5] */
#define R8_DAC_PERF_MODE_IC_1                     0x0    /* 0b  [6] */
#define R8_DAC_INVERT_IC_1                        0x0    /* 0b  [7] */
#define R8_DAC_MUTE_IC_1_MASK                     0x1
#define R8_DAC_MUTE_IC_1_SHIFT                    0
#define R8_DAC_VOL_MODE_IC_1_MASK                 0x6
#define R8_DAC_VOL_MODE_IC_1_SHIFT                1
#define R8_DAC_MORE_FILT_IC_1_MASK                0x8
#define R8_DAC_MORE_FILT_IC_1_SHIFT               3
#define R8_DAC_VOL_ZC_IC_1_MASK                   0x10
#define R8_DAC_VOL_ZC_IC_1_SHIFT                  4
#define R8_DAC_HARD_VOL_IC_1_MASK                 0x20
#define R8_DAC_HARD_VOL_IC_1_SHIFT                5
#define R8_DAC_PERF_MODE_IC_1_MASK                0x40
#define R8_DAC_PERF_MODE_IC_1_SHIFT               6
#define R8_DAC_INVERT_IC_1_MASK                   0x80
#define R8_DAC_INVERT_IC_1_SHIFT                  7

/* DAC_CTRL3 (IC 1) */
#define R9_DAC_HPF_EN_IC_1                        0x0    /* 0b    [0] */
#define R9_DAC_HPF_FC_IC_1                        0xD    /* 1101b [7:4] */
#define R9_DAC_HPF_EN_IC_1_MASK                   0x1
#define R9_DAC_HPF_EN_IC_1_SHIFT                  0
#define R9_DAC_HPF_FC_IC_1_MASK                   0xF0
#define R9_DAC_HPF_FC_IC_1_SHIFT                  4

/* DAC_VOL (IC 1) */
#define R10_DAC_VOL_IC_1                          0x40   /* 01000000b [7:0] */
#define R10_DAC_VOL_IC_1_MASK                     0xFF
#define R10_DAC_VOL_IC_1_SHIFT                    0

/* DAC_CLIP (IC 1) */
#define R11_DAC_HF_CLIP_IC_1                      0xFF   /* 11111111b [7:0] */
#define R11_DAC_HF_CLIP_IC_1_MASK                 0xFF
#define R11_DAC_HF_CLIP_IC_1_SHIFT                0

/* SPT_CTRL1 (IC 1) */
#define R12_SPT_SAI_MODE_IC_1                     0x0    /* 0b   [0] */
#define R12_SPT_DATA_FORMAT_IC_1                  0x0    /* 000b [3:1] */
#define R12_SPT_SLOT_WIDTH_IC_1                   0x0    /* 00b  [5:4] */
#define R12_SPT_BCLK_POL_IC_1                     0x0    /* 0b   [6] */
#define R12_SPT_LRCLK_POL_IC_1                    0x0    /* 0b   [7] */
#define R12_SPT_SAI_MODE_IC_1_MASK                0x1
#define R12_SPT_SAI_MODE_IC_1_SHIFT               0
#define R12_SPT_DATA_FORMAT_IC_1_MASK             0xE
#define R12_SPT_DATA_FORMAT_IC_1_SHIFT            1
#define R12_SPT_SLOT_WIDTH_IC_1_MASK              0x30
#define R12_SPT_SLOT_WIDTH_IC_1_SHIFT             4
#define R12_SPT_BCLK_POL_IC_1_MASK                0x40
#define R12_SPT_BCLK_POL_IC_1_SHIFT               6
#define R12_SPT_LRCLK_POL_IC_1_MASK               0x80
#define R12_SPT_LRCLK_POL_IC_1_SHIFT              7

/* SPT_CTRL2 (IC 1) */
#define R13_SPT_SLOT_SEL_IC_1                     0x0    /* 00000b [4:0] */
#define R13_SPT_SLOT_SEL_IC_1_MASK                0x1F
#define R13_SPT_SLOT_SEL_IC_1_SHIFT               0

/* AMP_CTRL (IC 1) */
#define R14_AMP_LPM_IC_1                          0x1    /* 1b  [0] */
#define R14_EMI_MODE_IC_1                         0x0    /* 0b  [1] */
#define R14_AMP_RLOAD_IC_1                        0x1    /* 01b [3:2] */
#define R14_OCP_EN_IC_1                           0x1    /* 1b  [4] */
#define R14_AMP_LPM_IC_1_MASK                     0x1
#define R14_AMP_LPM_IC_1_SHIFT                    0
#define R14_EMI_MODE_IC_1_MASK                    0x2
#define R14_EMI_MODE_IC_1_SHIFT                   1
#define R14_AMP_RLOAD_IC_1_MASK                   0xC
#define R14_AMP_RLOAD_IC_1_SHIFT                  2
#define R14_OCP_EN_IC_1_MASK                      0x10
#define R14_OCP_EN_IC_1_SHIFT                     4

/* LIM_CTRL (IC 1) */
#define R15_LIM_ATR_IC_1                          0x0    /* 00b [1:0] */
#define R15_LIM_RRT_IC_1                          0x0    /* 00b [5:4] */
#define R15_LIM_ATR_IC_1_MASK                     0x3
#define R15_LIM_ATR_IC_1_SHIFT                    0
#define R15_LIM_RRT_IC_1_MASK                     0x30
#define R15_LIM_RRT_IC_1_SHIFT                    4

/* LIM_CTRL2 (IC 1) */
#define R16_LIM_THRES_IC_1                        0xA    /* 01010b [4:0] */
#define R16_LIM_THRES_IC_1_MASK                   0x1F
#define R16_LIM_THRES_IC_1_SHIFT                  0

/* FAULT_CTRL (IC 1) */
#define R17_ARCV_OCP_IC_1                         0x0    /* 0b [0] */
#define R17_ARCV_OTF_IC_1                         0x0    /* 0b [1] */
#define R17_ARCV_UVLO_IC_1                        0x0    /* 0b [2] */
#define R17_MRCV_IC_1                             0x0    /* 0b [4] */
#define R17_ARCV_OCP_IC_1_MASK                    0x1
#define R17_ARCV_OCP_IC_1_SHIFT                   0
#define R17_ARCV_OTF_IC_1_MASK                    0x2
#define R17_ARCV_OTF_IC_1_SHIFT                   1
#define R17_ARCV_UVLO_IC_1_MASK                   0x4
#define R17_ARCV_UVLO_IC_1_SHIFT                  2
#define R17_MRCV_IC_1_MASK                        0x10
#define R17_MRCV_IC_1_SHIFT                       4

/* STATUS_CLR (IC 1) */
#define R18_STAT_CLR_IC_1                         0x0    /* 0b  [0] */
#define R18_STAT_CLR_IC_1_MASK                    0x1
#define R18_STAT_CLR_IC_1_SHIFT                   0

/* STATUS (IC 1) */
#define R19_OCP_IC_1                              0x0    /* 0b [0] */
#define R19_OTF_IC_1                              0x0    /* 0b [1] */
#define R19_OTW_IC_1                              0x0    /* 0b [2] */
#define R19_UVLO_IC_1                             0x0    /* 0b [3] */
#define R19_CLK_ERR_IC_1                          0x0    /* 0b [4] */
#define R19_SPT_ERR_IC_1                          0x0    /* 0b [5] */
#define R19_CLIP_IC_1                             0x0    /* 0b [6] */
#define R19_LIM_EG_IC_1                           0x0    /* 0b [7] */
#define R19_OCP_IC_1_MASK                         0x1
#define R19_OCP_IC_1_SHIFT                        0
#define R19_OTF_IC_1_MASK                         0x2
#define R19_OTF_IC_1_SHIFT                        1
#define R19_OTW_IC_1_MASK                         0x4
#define R19_OTW_IC_1_SHIFT                        2
#define R19_UVLO_IC_1_MASK                        0x8
#define R19_UVLO_IC_1_SHIFT                       3
#define R19_CLK_ERR_IC_1_MASK                     0x10
#define R19_CLK_ERR_IC_1_SHIFT                    4
#define R19_SPT_ERR_IC_1_MASK                     0x20
#define R19_SPT_ERR_IC_1_SHIFT                    5
#define R19_CLIP_IC_1_MASK                        0x40
#define R19_CLIP_IC_1_SHIFT                       6
#define R19_LIM_EG_IC_1_MASK                      0x80
#define R19_LIM_EG_IC_1_SHIFT                     7

/* RESET (IC 1) */
#define R20_SOFT_RESET_IC_1                       0x0   /* 0b   [0] */
#define R20_SOFT_FULL_RESET_IC_1                  0x0   /* 0b   [4] */
#define R20_SOFT_RESET_IC_1_MASK                  0x1
#define R20_SOFT_RESET_IC_1_SHIFT                 0
#define R20_SOFT_FULL_RESET_IC_1_MASK             0x10
#define R20_SOFT_FULL_RESET_IC_1_SHIFT            4

typedef enum
{
    AM_DEVICES_SSM6515_GAIN_P240DB  = 0x00, /* 24dB */
    AM_DEVICES_SSM6515_GAIN_P228DB  = 0x03, /* 22.875dB */
    AM_DEVICES_SSM6515_GAIN_P180DB  = 0x10, /* 18dB */
    AM_DEVICES_SSM6515_GAIN_P97DB   = 0x26, /* 9.75dB */
    AM_DEVICES_SSM6515_GAIN_P60DB   = 0x30, /* 6dB */
    AM_DEVICES_SSM6515_GAIN_P30DB   = 0x38, /* 3dB */
    AM_DEVICES_SSM6515_GAIN_0DB     = 0x40, /* 0dB */
    AM_DEVICES_SSM6515_GAIN_M11DB   = 0x43, /* -1.125dB */
    AM_DEVICES_SSM6515_GAIN_M30DB   = 0x48, /* -3dB */
    AM_DEVICES_SSM6515_GAIN_M60DB   = 0x50, /* -6dB */
    AM_DEVICES_SSM6515_GAIN_M90DB   = 0x58, /* -9dB */
    AM_DEVICES_SSM6515_GAIN_M720DB  = 0xFF, /* -72dB, Mute */
}
am_devices_ssm6515_gain_e;

// Generic or interface specific status.
typedef enum
{
    AM_DEVICES_SSM6515_STATUS_SUCCESS,
    AM_DEVICES_SSM6515_STATUS_FAIL,
    AM_DEVICES_SSM6515_STATUS_INVALID_HANDLE,
    AM_DEVICES_SSM6515_STATUS_IN_USE,
    AM_DEVICES_SSM6515_STATUS_TIMEOUT,
    AM_DEVICES_SSM6515_STATUS_OUT_OF_RANGE,
    AM_DEVICES_SSM6515_STATUS_INVALID_ARG,
    AM_DEVICES_SSM6515_STATUS_INVALID_OPERATION,
    AM_DEVICES_SSM6515_STATUS_MEM_ERR,
    AM_DEVICES_SSM6515_STATUS_HW_ERR,
} am_devices_SSM6515_status_t;

// SSM6515 channel mode
typedef enum
{
    SSM6515_LEFT = 0,   // Left channel
    SSM6515_RIGHT,      // Right channel
    SSM6515_LEFT_RIGHT, // (Left + Right) / 2
    SSM6515_MONO,       // Mono mode
} am_devices_SSM6515_ch_t;

//  SSM6515 mute status
typedef enum
{
    SSM6515_MUTE = 0, // Mute SSM6515
    SSM6515_UNMUTE,   // Unmute SSM6515
} am_devices_SSM6515_mute_t;

typedef enum
{
    SSM6515_UP = 0x00, // Power Up SSM6515
    SSM6515_DOWN = 0x01   // Power Down SSM6515
} am_devices_SSM6515_power_t;

typedef enum
{
    SSM6515_STEREO = 0, // Select Stereo(I2S, LJ, RJ) Mode
    SSM6515_TDM = 1     // Select TDM Mode
} am_devices_SSM6515_mode_t;
//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Init IOM for the SSM6515 AMP
//!
//! @param ui32Module - IOM Module Number.
//! @param *psIOMSettings - Pointer to the IOM Settings.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_i2c_init(uint32_t             ui32Module,
                                am_hal_iom_config_t *psIOMSettings);

//*****************************************************************************
//
//! @brief Init the Device SSM6515 Amplifier.
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_init(uint8_t chip_addr);

//*****************************************************************************
//
//! @brief Deinit IOM for the SSM6515 Amplifier.
//!
//! @param ui32Module - IOM Module Number.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_i2c_deinit(uint32_t ui32Module);

//*****************************************************************************
//
//! @brief Deinit the Device SSM6515 Amplifier.
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_deinit(uint8_t chip_addr);

//*****************************************************************************
//
//! @brief Read data from SSM6515.
//!
//! @param dev_addr Chip I2C device address.
//! @param reg_addr Register address.
//! @param p_buf The pointer of buffer.
//! @param size The size of buffer.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_i2c_read(uint32_t dev_addr,
                                            uint8_t reg_addr,
                                            uint32_t *p_buf,
                                            uint32_t size);

//*****************************************************************************
//
//! @brief Write data to SSM6515
//!
//! @param dev_addr Chip I2C device address.
//! @param reg_addr Register address.
//! @param p_buf The pointer of buffer.
//! @param size The size of buffer.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_i2c_write(uint32_t dev_addr,
                                             uint8_t reg_addr,
                                             uint32_t *p_buf,
                                             uint32_t size);

//*****************************************************************************
//
//! @brief Set SSM6515 Gain Value
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @param gain - The point to Gain Value.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_set_gain(uint8_t chip_addr, uint8_t *gain);

//*****************************************************************************
//
//! @brief Set SSM6515 Powerdown State
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @param state - The point to Powerdown State.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_power_down(uint8_t chip_addr, bool state);

//*****************************************************************************
//
//! @brief Set SSM6515 SAI Mode
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @param mode - Stereo(I2S, LJ, RJ) or TDM.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_set_sai_mode(uint8_t chip_addr, uint32_t mode);

//*****************************************************************************
//
//! @brief Set SSM6515 TDM slot width (a.k.a audio sample size)
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @param slot_width - valid value is 32, 16 and 24
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_set_tdm_slot_width(uint8_t chip_addr, uint32_t slot_width);

//*****************************************************************************
//
//! @brief Set SSM6515 Sample Rate
//!
//! @param chip_addr - Chip I2C device address.
//!
//! @param sample_rate - DAC sample rate. Support 8kHz, 12kHz, 16kHz, 24kHz, 32kHz,
//! 44.1kHz, 48kHz, 88.2kHz, 96kHz, 176.4kHz, 192kHz, 384kHz, and 768kHz.
//!
//! @return status - Generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_devices_ssm6515_set_sample_rate(uint8_t chip_addr, uint32_t sample_rate);

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
