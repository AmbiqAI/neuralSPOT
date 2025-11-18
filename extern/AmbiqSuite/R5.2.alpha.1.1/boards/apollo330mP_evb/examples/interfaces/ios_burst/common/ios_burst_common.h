//*****************************************************************************
//
//! @file ios_burst_common.h
//!
//! @brief ios burst common definitions.
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define INBUFFER_EMPTY              (0xEE)
#define XOR_BYTE                    0// used to initilize the test data, 0 Will not change, 0xFF will invert
#define ROBUFFER_INIT               (0x55)

#define HANDSHAKE_PIN               4

#define IOS_TEST_ITERATION          5
#define AM_IOM_TX_UNIT_SIZE         2048
#define MAX_FIFO_READ_SIZE          1023

#define AM_IOS_LRAM_TEST_LENGTH     ((AM_IOM_TX_UNIT_SIZE - 3) * IOS_TEST_ITERATION)       // 5 packets

//*****************************************************************************
//
// Configure GPIOs for communicating with a SPI device
// This should ideally come from BSP to keep the code portable
//
//*****************************************************************************

#define HANDSHAKE_CMD_STOP         0xFC
#define HANDSHAKE_CMD_WRITE_DATA   0xFD
#define HANDSHAKE_CMD_READ_DATA    0xFE
#define HANDSHAKE_CMD_ACK          0xFF

#define AM_IOSTEST_IOSTOIOM_DATAAVAIL_INTMASK  1

#define IOM_SPEED_SPI              8  // 1 MHz

typedef enum
{
    IOS_STATE_CNTLR_WRITE_START,
    IOS_STATE_CNTLR_WRITE_STREAM,
    IOS_STATE_CNTLR_READ_NODATA,
    IOS_STATE_CNTLR_READ_DATA,
    IOS_STATE_IDLE
} AM_IOS_STATE_E;

typedef struct
{
    uint32_t MHz;
    char *MHzString;
} AM_IOM_SPEED_S;

extern AM_IOM_SPEED_S spiSpeedString[];

void common_setup(void);

