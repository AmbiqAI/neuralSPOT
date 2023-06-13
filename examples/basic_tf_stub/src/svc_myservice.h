//*****************************************************************************
//
//! @file svc_myservice.h
//!
//! @brief AmbiqMicro Data Transfer Protocol service definition
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
// This is part of revision ambiqvos-754956a890 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef NS_SVC_MYSERVICE_H
#define NS_SVC_MYSERVICE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "ns_ble.h"

// Macro definitions

/*! Partial amdtp service UUIDs */
#define ATT_UUID_MYSERVICE_SERVICE_PART 0x1011

/*! Partial amdtp rx characteristic UUIDs */
#define ATT_UUID_MYSERVICE_RX_PART 0x0011

/*! Partial amdtp tx characteristic UUIDs */
#define ATT_UUID_MYSERVICE_TX_PART 0x0012

/*! Partial amdtp ack characteristic UUIDs */
#define ATT_UUID_MYSERVICE_ACK_PART 0x0013

/* Amdtp services */
#define ATT_UUID_MYSERVICE_SERVICE NS_ATT_UUID_BUILD(ATT_UUID_MYSERVICE_SERVICE_PART)

/* Amdtp characteristics */
#define ATT_UUID_MYSERVICE_RX NS_ATT_UUID_BUILD(ATT_UUID_MYSERVICE_RX_PART)
#define ATT_UUID_MYSERVICE_TX NS_ATT_UUID_BUILD(ATT_UUID_MYSERVICE_TX_PART)
#define ATT_UUID_MYSERVICE_ACK NS_ATT_UUID_BUILD(ATT_UUID_MYSERVICE_ACK_PART)

// AM DTP Service
#define MYSERVICE_START_HDL 0x0800
#define MYSERVICE_END_HDL (MYSERVICE_MAX_HDL - 1)

/* AMDTP Service Handles */
enum {
    MYSERVICE_SVC_HDL = MYSERVICE_START_HDL, /* AMDTP service declaration */
    MYSERVICE_RX_CH_HDL,                     /* AMDTP write command characteristic */
    MYSERVICE_RX_HDL,                        /* AMDTP write command data */
    MYSERVICE_TX_CH_HDL,                     /* AMDTP notify characteristic */
    MYSERVICE_TX_HDL,                        /* AMDTP notify data */
    MYSERVICE_TX_CH_CCC_HDL,                 /* AMDTP notify client characteristic configuration */
    MYSERVICE_ACK_CH_HDL,                    /* AMDTP rx ack characteristic */
    MYSERVICE_ACK_HDL,                       /* AMDTP rx ack data */
    MYSERVICE_ACK_CH_CCC_HDL,                /* AMDTP rx ack client characteristic configuration */
    MYSERVICE_MAX_HDL
};

//*****************************************************************************
//
// Function definitions.
//
//*****************************************************************************
void SvcMyServiceAddGroup(void);
void SvcMyServiceRemoveGroup(void);
void SvcMyServiceCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

#ifdef __cplusplus
}
#endif

#endif // SVC_AMDTPS_H
