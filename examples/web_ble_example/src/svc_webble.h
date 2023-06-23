//*****************************************************************************
//
//! @file svc_webble.h
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
#ifndef NS_SVC_WEBBLE_H
#define NS_SVC_WEBBLE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "ns_ble.h"

/*! Partial WebBLE service UUIDs */
#define ATT_UUID_WEBBLE_SERVICE_PART 0xCA11

#define ATT_UUID_TEMPERATURE_PART 0xC020
#define ATT_UUID_HUMIDITY_PART 0xC021
#define ATT_UUID_PRESSURE_PART 0xC022

#define ATT_UUID_ACCELEROMETER_PART 0xC030
#define ATT_UUID_GYROSCOPE_PART 0xC031
#define ATT_UUID_QUATERNION_PART 0xC032

#define ATT_UUID_RGBLED_PART 0xC040

#define ATT_UUID_BSEC_PART 0xC050
#define ATT_UUID_CO2_PART 0xC051
#define ATT_UUID_GAS_PART 0xC052

// Build the UUIDs
// Service
#define ATT_UUID_WEBBLE_SERVICE NS_BLE_ATT_UUID_BUILD(ATT_UUID_WEBBLE_SERVICE_PART)

// Characteristics
#define ATT_UUID_WEBBLE_TEMPERATURE NS_BLE_ATT_UUID_BUILD(ATT_UUID_TEMPERATURE_PART)
#define ATT_UUID_WEBBLE_HUMIDITY NS_BLE_ATT_UUID_BUILD(ATT_UUID_HUMIDITY_PART)
#define ATT_UUID_WEBBLE_PRESSURE NS_BLE_ATT_UUID_BUILD(ATT_UUID_PRESSURE_PART)

#define ATT_UUID_WEBBLE_ACCELEROMETER NS_BLE_ATT_UUID_BUILD(ATT_UUID_ACCELEROMETER_PART)
#define ATT_UUID_WEBBLE_GYROSCOPE NS_BLE_ATT_UUID_BUILD(ATT_UUID_GYROSCOPE_PART)
#define ATT_UUID_WEBBLE_QUATERNION NS_BLE_ATT_UUID_BUILD(ATT_UUID_QUATERNION_PART)

#define ATT_UUID_WEBBLE_RGBLED NS_BLE_ATT_UUID_BUILD(ATT_UUID_RGBLED_PART)

#define ATT_UUID_WEBBLE_BSEC NS_BLE_ATT_UUID_BUILD(ATT_UUID_BSEC_PART)
#define ATT_UUID_WEBBLE_CO2 NS_BLE_ATT_UUID_BUILD(ATT_UUID_CO2_PART)
#define ATT_UUID_WEBBLE_GAS NS_BLE_ATT_UUID_BUILD(ATT_UUID_GAS_PART)

// WebBLE Service Handles
#define WEBBLE_START_HDL 0x0800
#define WEBBLE_END_HDL (WEBBLE_MAX_HDL - 1)

enum {
    WEBBLE_SVC_HDL = WEBBLE_START_HDL,
    WEBBLE_TEMPERATURE_CH_HDL,
    WEBBLE_TEMPERATURE_HDL,
    WEBBLE_HUMIDITY_CH_HDL,
    WEBBLE_HUMIDITY_HDL,
    WEBBLE_PRESSURE_CH_HDL,
    WEBBLE_PRESSURE_HDL,
    WEBBLE_ACCELEROMETER_CH_HDL,
    WEBBLE_ACCELEROMETER_HDL,
    WEBBLE_ACCELEROMETER_CCC_HDL,
    WEBBLE_GYROSCOPE_CH_HDL,
    WEBBLE_GYROSCOPE_HDL,
    WEBBLE_GYROSCOPE_CCC_HDL,
    WEBBLE_QUATERNION_CH_HDL,
    WEBBLE_QUATERNION_HDL,
    WEBBLE_QUATERNION_CCC_HDL,
    WEBBLE_RGBLED_CH_HDL,
    WEBBLE_RGBLED_HDL,
    WEBBLE_BSEC_CH_HDL,
    WEBBLE_BSEC_HDL,
    WEBBLE_CO2_CH_HDL,
    WEBBLE_CO2_HDL,
    WEBBLE_GAS_CH_HDL,
    WEBBLE_GAS_HDL,
    WEBBLE_MAX_HDL,
};

//*****************************************************************************
//
// Function definitions.
//
//*****************************************************************************
void SvcWebbleAddGroup(void);
void SvcWebbleRemoveGroup(void);
void SvcWebbleCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

#ifdef __cplusplus
}
#endif

#endif // SVC_WEBBLE_H
