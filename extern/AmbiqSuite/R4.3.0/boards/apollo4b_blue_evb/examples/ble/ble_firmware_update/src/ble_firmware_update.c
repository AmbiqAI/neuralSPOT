//*****************************************************************************
//
//! @file ble_firmware_update.c
//!
//! @brief This is the application just for updating built-in BLE firmware into Cooper.
//!
//!
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
// This is part of revision release_sdk_4_3_0-0ca7d78a2b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_devices_cooper.h"
//*****************************************************************************
//
// Configuration options
//
//*****************************************************************************
// Choose which kind of image to update
// For info0 and inof1, info0_patch.h and info1_patch.h are just templates, the patch binaries differs
// from different ble controller parts, user should first provide ble controller's device id (printed out via SWO)
// to Ambiq engineer and then replace info0_patch_bin or info1_patch_bin with new generated ones before enable
// UPDATE_BLE_INFO0 or UPDATE_BLE_INFO1

#define UPDATE_BLE_FW
//#define UPDATE_BLE_INFO0
//#define UPDATE_BLE_INFO1
#if defined(UPDATE_BLE_FW)
#include "ble_fw_image.h"
#define image_bin       ble_fw_image_bin
#define image_type      AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_FW
#define ble_get_binary  am_devices_cooper_get_FwImage
#define BLE_UPDATE_SIGN COOPER_FW_UPDATE_SIGN
#elif defined(UPDATE_BLE_INFO0)
#include "info0_patch.h"
#define image_bin       info0_patch_bin
#define image_type      AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_INFO_0
#define ble_get_binary  am_devices_cooper_get_info0_patch
#define BLE_UPDATE_SIGN COOPER_INFO0_UPDATE_SIGN
#elif defined(UPDATE_BLE_INFO1)
#include "info1_patch.h"
#define image_bin       info1_patch_bin
#define image_type      AM_DEVICES_COOPER_SBL_UPDATE_IMAGE_TYPE_INFO_1
#define ble_get_binary  am_devices_cooper_get_info1_patch
#define BLE_UPDATE_SIGN COOPER_INFO1_UPDATE_SIGN
#else
#warning No update image available
#endif

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
uint32_t DMATCBBuffer[2048];

void *g_IomDevHdl;
void *g_pvHciSpiHandle;

static am_devices_cooper_sbl_update_data_t     g_sBLEImage =
{
    (uint8_t*)&image_bin,
    sizeof(image_bin),
    image_type,
    0
};


//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    uint32_t ui32Status = 0;

    //
    // Default setup.
    //
    am_bsp_low_power_init();

    //
    // Enable the ITM
    //
    am_bsp_itm_printf_enable();

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();

    am_util_stdio_printf("BLE Firmware Update Application\n");

    am_devices_cooper_config_t stCooperConfig;
    stCooperConfig.pNBTxnBuf = DMATCBBuffer;
    stCooperConfig.ui32NBTxnBufLength = sizeof(DMATCBBuffer) / 4;

    // Update firmware image information to SBL.
    ble_get_binary(&g_sBLEImage);

    //
    // Initialize the SPI module.
    //
    ui32Status = am_devices_cooper_init(SPI_MODULE, &stCooperConfig, &g_IomDevHdl, &g_pvHciSpiHandle);
    if ( ui32Status )
    {
        am_util_stdio_printf("BLE Controller Init Fails!\n");
        return -1;
    }

#if defined(AM_PART_APOLLO4B) || defined(AM_PART_APOLLO4P)
    // For fw patching
    // write HCI command to trigger Cooper to reboot for SBL to do download.
    do
    {
        ui32Status = am_util_ble_update_sign_set(g_IomDevHdl, BLE_UPDATE_SIGN);
        if ( ui32Status != AM_DEVICES_COOPER_STATUS_SUCCESS )
        {
            am_util_stdio_printf("Write signature to BLE Controller failed\n");
            break;
        }
        else
        {
            // reset Cooper to get SBL to update info1
            am_util_stdio_printf("*****Reset cooper to do a forcing upgrade*****\n");
            ui32Status = am_devices_cooper_reset_with_sbl_check(g_IomDevHdl, &stCooperConfig);
            if ( ui32Status )
            {
                g_IomDevHdl = NULL;
                g_pvHciSpiHandle = NULL;
                am_util_stdio_printf("am_devices_cooper_init fails on update\r\n");
                return -1;
            }
        }
        am_util_stdio_printf("BLE Firmware Update Application Done!\r\n");
    } while (0);
#endif

    while (1);
}

