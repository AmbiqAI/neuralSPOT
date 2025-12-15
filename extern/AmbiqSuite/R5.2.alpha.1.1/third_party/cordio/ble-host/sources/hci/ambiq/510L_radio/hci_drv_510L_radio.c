//*****************************************************************************
//
//! @file hci_drv_510L_radio.c
//!
//! @brief HCI driver interface.
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "wsf_types.h"
#include "wsf_timer.h"
#include "bstream.h"
#include "wsf_msg.h"
#include "wsf_cs.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_tr_apollo.h"
#include "hci_core.h"
#include "dm_api.h"

#include "am_mcu_apollo.h"
#include "am_util.h"
#include "hci_drv_510L_radio.h"

//*****************************************************************************
//
// Macro Definitions
//
//*****************************************************************************
#define NUM_HCI_READ_BUFFERS           8
#define HCI_DRV_MAX_TX_PACKET          512
#define BLE_TRANSFER_NEEDED_EVENT      0x01
#define LE_FEATS_LEN        0x08
//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
//*****************************************************************************
//
// Structure for holding received HCI packets data pointer (in IPC shared memory)
// and packet length.
//
//*****************************************************************************
typedef struct
{
    uint8_t *pui8Data;
    uint16_t ui16Length;
}
hci_drv_read_t;

uint8_t g_BLEMacAddress[6] = {0};
wsfHandlerId_t g_HciDrvHandleID = 0;
static hci_drv_read_t g_psReadBuffers[NUM_HCI_READ_BUFFERS];
static am_hal_queue_t g_sReadQueue;
static uint8_t g_ui8HciTxBuf[HCI_DRV_MAX_TX_PACKET];

//*****************************************************************************
//
// HCI driver RX processing callback.
//
//*****************************************************************************
static bool
hciDrvRxCallback(uint8_t *data, uint16_t size)
{
    hci_drv_read_t *psReadBuffer;

    //
    // Check to see if we still have buffer space.
    //
    if (am_hal_queue_full(&g_sReadQueue))
    {
        am_util_stdio_printf("ERROR: Ran out of HCI receive queue slots.\n");

        //
        // Notify the high level to release the IPC RX buffer in shared memory
        //
        return TRUE;
    }

    //
    // Get a pointer to the next item in the queue.
    //
    psReadBuffer = am_hal_queue_next_slot(&g_sReadQueue);

    //
    // Set all of the fields in the HCI read structure.
    //
    psReadBuffer->pui8Data = data;
    psReadBuffer->ui16Length = size;

    //
    // Add the item to the queue.
    //
    am_hal_queue_item_add(&g_sReadQueue, 0, 1);

    WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);

    //
    // The IPC RX data will be handled further, do not release now.
    //
    return FALSE;
}

//*****************************************************************************
//
// Boot the radio core.
//
//*****************************************************************************
uint32_t
HciDrvRadioBoot(bool bColdBoot)
{
    uint32_t ui32Status;

    //
    // Initialize a queue to help us keep track of HCI read buffers.
    //
    am_hal_queue_from_array(&g_sReadQueue, g_psReadBuffers);

    //
    // Register the HCI RX callback to the corresponding IPC RX radio module
    //
    am_devices_510_radio_ipc_register_rx_cback(hciDrvRxCallback, AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_RADIO);

    ui32Status = am_devices_510L_radio_init();

    // When it's bColdBoot, it will use Apollo's Device ID to form Bluetooth address.
    if (bColdBoot)
    {
        am_hal_mcuctrl_device_t sDevice;
        am_hal_mcuctrl_info_get(AM_HAL_MCUCTRL_INFO_DEVICEID, &sDevice);
       
        if(memcmp(g_BLEMacAddress, &sDevice.ui32ChipID1, sizeof(sDevice.ui32ChipID1)))
        {
            // Bluetooth address formed by ChipID1 (32 bits) and ChipID0 (8-23 bits).
            memcpy(g_BLEMacAddress, &sDevice.ui32ChipID1, sizeof(sDevice.ui32ChipID1));
            // ui32ChipID0 bit 8-31 is test time during chip manufacturing
            g_BLEMacAddress[4] = (sDevice.ui32ChipID0 >> 8) & 0xFF;
            g_BLEMacAddress[5] = (sDevice.ui32ChipID0 >> 16) & 0xFF;

            // To address compatibility issues with certain Android phones where service discovery was not initiated after connection.
            // In accordance with the IEEE 802-2014 agreement, clear universal/local (U/L) address bit, 0 means universal addresses.
            // Clear individual/group (I/G) address bit, 0 indicates that the MAC address field is an individual MAC.
            g_BLEMacAddress[5] &= 0xFC;
        }
        else
        {
            g_BLEMacAddress[0] = rand() % 0xff;
            g_BLEMacAddress[1] = rand() % 0xff;
            g_BLEMacAddress[2] = 0xda;
            g_BLEMacAddress[3] = 0x7d;
            g_BLEMacAddress[4] = 0x1a;
            g_BLEMacAddress[5] = 0x00;
        }
    }

    return ui32Status;
}

//*****************************************************************************
//
// Shut down the radio core.
//
//*****************************************************************************
void
HciDrvRadioShutdown(void)
{
    am_devices_510L_radio_deinit();
}

//*****************************************************************************
//
// Function used by the BLE stack to send HCI messages to the BLE controller.
//
// Internally, the Cordio BLE stack will allocate memory for an HCI message,
//
//*****************************************************************************
uint16_t
hciDrvWrite(uint8_t type, uint16_t len, uint8_t *pData)
{
    uint32_t ui32Status;

    g_ui8HciTxBuf[0] = type;
    memcpy(&g_ui8HciTxBuf[1], pData, len);

    ui32Status = am_devices_510L_radio_write(g_ui8HciTxBuf, len + 1);
    if (ui32Status == AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
    {
        return len;
    }
    else
    {
        return 0;
    }
}

//*****************************************************************************
//
// Save the handler ID of the HciDrvHandler so we can send it events through
// the WSF task system.
//
// Note: These two lines need to be added to the exactle initialization
// function at the beginning of all Cordio applications:
//
//     handlerId = WsfOsSetNextHandler(HciDrvHandler);
//     HciDrvHandler(handlerId);
//
//*****************************************************************************
void
HciDrvHandlerInit(wsfHandlerId_t handlerId)
{
    g_HciDrvHandleID = handlerId;
}

//*****************************************************************************
//
// Event handler for HCI-related events.
//
// This handler can perform HCI reads or writes, and keeps the actions in the
// correct order.
//
//*****************************************************************************
void
HciDrvHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    if (event & BLE_TRANSFER_NEEDED_EVENT)
    {
        uint16_t consumed_bytes;
        hci_drv_read_t *psReadBuffer = am_hal_queue_peek(&g_sReadQueue);

        consumed_bytes = hciTrSerialRxIncoming(psReadBuffer->pui8Data, psReadBuffer->ui16Length);
        if (consumed_bytes != psReadBuffer->ui16Length)
        {
            //
            // We should assume each IPC received packet is complete
            //
            am_util_stdio_printf("ERROR: The received packet is not complete!!\n");
        }

        am_hal_queue_item_get(&g_sReadQueue, 0, 1);
        am_devices_510L_radio_release_ipc_rx_buf(psReadBuffer->pui8Data);
        
        if (!am_hal_queue_empty(&g_sReadQueue))
        {
            //
            // If there is more data in the queue, handle it further
            //
            WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
        }
    }
}

uint8_t nvds_data[HCI_VSC_UPDATE_NVDS_CFG_CMD_LENGTH]=
{
    NVDS_PARAMETER_MAGIC_NUMBER,
    NVDS_PARAMETER_SLEEP_ENABLE
   //NVDS_PARAMETER_SLEEP_DISABLE
};

void HciVscUpdateNvdsParam(void)
{
    HciVendorSpecificCmd(HCI_VSC_UPDATE_NVDS_CFG_CMD_OPCODE, HCI_VSC_UPDATE_NVDS_CFG_CMD_LENGTH, nvds_data);
}

void HciVscUpdateBDAddress(void)
{
    HciVendorSpecificCmd(HCI_VSC_SET_BD_ADDR_CFG_CMD_OPCODE, HCI_VSC_SET_BD_ADDR_CFG_CMD_LENGTH, g_BLEMacAddress);
}

uint8_t ll_local_feats[LE_FEATS_LEN] = {0};

void HciVscUpdateLinklayerFeature(void)
{
    ll_local_feats[0] = (uint8_t)LL_FEATURES_BYTE0;
    ll_local_feats[1] = (uint8_t)(LL_FEATURES_BYTE1>>8);
    ll_local_feats[2] = (uint8_t)(LL_FEATURES_BYTE2>>16);
    ll_local_feats[3] = (uint8_t)(LL_FEATURES_BYTE3>>24);
    ll_local_feats[4] = (uint8_t)(LL_FEATURES_BYTE4>>32);
    ll_local_feats[5] = (uint8_t)(LL_FEATURES_BYTE5>>40);
    ll_local_feats[6] = (uint8_t)(LL_FEATURES_BYTE6>>48);
    ll_local_feats[7] = (uint8_t)(LL_FEATURES_BYTE7>>56);

    HciVendorSpecificCmd(HCI_VSC_UPDATE_LL_FEATURE_CFG_CMD_OPCODE, LE_FEATS_LEN, ll_local_feats);
}

void HciVscUpdateTxpwrLevel(txPowerLevel_t txPowerlevel)
{
    // make sure it's 8 bit
    uint8_t tx_power_level = (uint8_t)txPowerlevel;

    HciVendorSpecificCmd(HCI_VSC_SET_TX_POWER_LEVEL_CFG_CMD_OPCODE, HCI_VSC_SET_TX_POWER_LEVEL_CFG_CMD_LENGTH, &tx_power_level);
}