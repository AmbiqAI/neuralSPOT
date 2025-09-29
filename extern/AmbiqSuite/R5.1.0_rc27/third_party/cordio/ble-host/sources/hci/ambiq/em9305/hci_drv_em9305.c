//*****************************************************************************
//
//! @file hci_drv_em9305.c
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
// This is part of revision release_sdk5p1p0-acc60980d8 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "wsf_types.h"
#include "wsf_timer.h"
#include "wsf_trace.h"
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
#include "hci_drv_em9305.h"
#include "am_devices_em9305.h"
#include <string.h>

//*****************************************************************************
//
// Use the interrupt-driven HCI driver?
//
//*****************************************************************************
#define USE_NONBLOCKING_HCI             0
#define SKIP_FALLING_EDGES              0

//*****************************************************************************
//
// Enable the heartbeat command?
//
// Setting this to 1 will cause the MCU to send occasional HCI packets to the
// BLE core if there hasn't been any activity for a while. This can help catch
// communication issues that might otherwise go unnoticed.
//
//*****************************************************************************
#define ENABLE_BLE_HEARTBEAT            1

//*****************************************************************************
//
// Configurable buffer sizes.
//
//*****************************************************************************
#define NUM_HCI_WRITE_BUFFERS           8
#define HCI_DRV_MAX_TX_PACKET           259       // Including 255 maximum HCI command + 4 HCI header
#define HCI_DRV_MAX_RX_PACKET           256

//*****************************************************************************
//
// Configurable error-detection thresholds.
//
//*****************************************************************************
#define HEARTBEAT_TIMEOUT_MS            (10000)   //milli-seconds
#define HCI_DRV_MAX_IRQ_TIMEOUT          2000
#define HCI_DRV_MAX_XTAL_RETRIES         10
#define HCI_DRV_MAX_TX_RETRIES           10000
#define HCI_DRV_MAX_HCI_TRANSACTIONS     1000
#define HCI_DRV_MAX_READ_PACKET          4   // max read in a row at a time

//*****************************************************************************
//
// Structure for holding outgoing HCI packets.
//
//*****************************************************************************
typedef struct
{
    uint32_t ui32Length;
    uint32_t pui32Data[HCI_DRV_MAX_TX_PACKET / 4];
}
hci_drv_write_t;

//*****************************************************************************
//
// Heartbeat implementation functions.
//
//*****************************************************************************
#if ENABLE_BLE_HEARTBEAT

#define BLE_HEARTBEAT_START()                                                 \
    do { WsfTimerStartMs(&g_HeartBeatTimer, HEARTBEAT_TIMEOUT_MS); } while (0)

#define BLE_HEARTBEAT_STOP()                                                  \
    do { WsfTimerStop(&g_HeartBeatTimer); } while (0)

#define BLE_HEARTBEAT_RESTART()                                               \
    do                                                                        \
    {                                                                         \
        WsfTimerStop(&g_HeartBeatTimer);                                      \
        WsfTimerStartMs(&g_HeartBeatTimer, HEARTBEAT_TIMEOUT_MS);             \
    } while (0)

#else

#define BLE_HEARTBEAT_START()
#define BLE_HEARTBEAT_STOP()
#define BLE_HEARTBEAT_RESTART()

#endif

#define delay_us(us)        am_hal_flash_delay(FLASH_CYCLES_US(us))
#define WHILE_TIMEOUT_MS_BREAK(expr, timeout, error)                                \
    {                                                                         \
        uint32_t ui32Timeout = 0;                                             \
        while (expr)                                                          \
        {                                                                     \
            if (ui32Timeout >= (timeout * 1000))                              \
            {                                                                 \
                break;                                                 \
            }                                                                 \
                                                                              \
            delay_us(1);                                                      \
            ui32Timeout++;                                                    \
        }                                                                     \
    }


//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
uint32_t DMATCBBuf[HCI_DRV_MAX_TX_PACKET / 4];

// BLE module handle
void *g_IomDevHdl;
void *g_pvHciSpiHandle;

uint8_t g_BLEMacAddress[6] = {0};

// Global handle used to send BLE events about the Hci driver layer.
wsfHandlerId_t g_HciDrvHandleID = 0;
wsfTimer_t g_HeartBeatTimer;
wsfTimer_t g_WakeTimer;

// Buffers for HCI write data.
hci_drv_write_t g_psWriteBuffers[NUM_HCI_WRITE_BUFFERS];
am_hal_queue_t g_sWriteQueue;

// Buffers for HCI read data.
uint32_t g_pui32ReadBuffer[HCI_DRV_MAX_RX_PACKET / 4];
uint8_t *g_pui8ReadBuffer = (uint8_t *) g_pui32ReadBuffer;
volatile bool bReadBufferInUse = false;

uint32_t g_ui32NumBytes   = 0;
uint32_t g_consumed_bytes = 0;

// Counters for tracking read data.
volatile uint32_t g_ui32InterruptsSeen = 0;

void HciDrvEmptyWriteQueue(void);
//*****************************************************************************
//
// Forward declarations for HCI callbacks.
//
//*****************************************************************************
#if USE_NONBLOCKING_HCI
void hciDrvWriteCallback(uint8_t *pui8Data, uint32_t ui32Length, void *pvContext);
void hciDrvReadCallback(uint8_t *pui8Data, uint32_t ui32Length, void *pvContext);
#endif // USE_NONBLOCKING_HCI

//*****************************************************************************
//
// Events for the HCI driver interface.
//
//*****************************************************************************
#define BLE_TRANSFER_NEEDED_EVENT                   0x01
#define BLE_HEARTBEAT_EVENT                         0x02
#define BLE_SET_WAKEUP                              0x03

//*****************************************************************************
//
// Error-handling wrapper macro.
//
//*****************************************************************************
#define ERROR_CHECK(status)                                              \
    {                                                                         \
        uint32_t ui32ErrChkStatus;                                            \
        if (0 != (ui32ErrChkStatus = (status)))                               \
        {                                                                     \
            am_util_debug_printf("ERROR_CHECK "#status "\n");            \
            error_check(ui32ErrChkStatus);                                    \
            return AM_HAL_STATUS_FAIL;                                                           \
        }                                                                     \
    }

#define ERROR_RETURN(status, retval)                                          \
    if ((status))                                                             \
    {                                                                         \
        error_check(status);                                                  \
        return (retval);                                                      \
    }

#define ERROR_RECOVER(status)                                                 \
    if ((status))                                                             \
    {                                                                         \
        error_check(status);                                                  \
        HciDrvRadioShutdown();                                                \
        HciDrvRadioBoot(0);                                                   \
        HciDrvEmptyWriteQueue();                                              \
        DmDevReset();                                                         \
        return;                                                               \
    }

//*****************************************************************************
//
// Debug section.
//
//*****************************************************************************
#if 1
#define CRITICAL_PRINT(...)                                                   \
    do                                                                        \
    {                                                                         \
        AM_CRITICAL_BEGIN;                                                    \
        am_util_debug_printf(__VA_ARGS__);                                    \
        AM_CRITICAL_END;                                                      \
    } while (0)
#else
#define CRITICAL_PRINT(...)
#endif

#define TASK_LEVEL_DELAYS 0


//*****************************************************************************
//
// Function pointer for redirecting errors
//
//*****************************************************************************
hci_drv_error_handler_t g_hciDrvErrorHandler = 0;
static uint32_t g_ui32FailingStatus = 0;

//*****************************************************************************
//
// By default, errors will be printed. If there is an error handler defined,
// they will be sent there intead.
//
//*****************************************************************************
static void
error_check(uint32_t ui32Status)
{
    //
    // Don't do anything unless there's an error.
    //
    if (ui32Status)
    {
        //
        // Set the global error status. If there's an error handler function,
        // call it. Otherwise, just print the error status and wait.
        //
        g_ui32FailingStatus = ui32Status;

        if (g_hciDrvErrorHandler)
        {
            g_hciDrvErrorHandler(g_ui32FailingStatus);
        }
        else
        {
            CRITICAL_PRINT("Error detected: 0x%08x\n", g_ui32FailingStatus);
        }
    }
}

//*****************************************************************************
//
// Other useful macros.
//
//*****************************************************************************


// Ellisys HCI SPI tapping support

// #define ELLISYS_HCI_LOG_SUPPORT 1

//*****************************************************************************
//
// Boot the radio.
//
//*****************************************************************************
uint32_t
HciDrvRadioBoot(bool bColdBoot)
{
    uint32_t ui32Status = AM_HAL_STATUS_FAIL;

    //
    // Setup SPI interface for EM9305
    //
    am_devices_em9305_config_t sSpiConfig;
    sSpiConfig.pNBTxnBuf = DMATCBBuf;
    sSpiConfig.ui32NBTxnBufLength = sizeof(DMATCBBuf) / 4;

    ui32Status = am_devices_em9305_init(SPI_MODULE,&sSpiConfig,&g_IomDevHdl, &g_pvHciSpiHandle);
    am_util_delay_us(100);

    g_ui32NumBytes   = 0;
    g_consumed_bytes = 0;

    //
    // Initialize a queue to help us keep track of HCI write buffers.
    //
    am_hal_queue_from_array(&g_sWriteQueue, g_psWriteBuffers);

    //
    // Reset the RX interrupt counter.
    //
    g_ui32InterruptsSeen = 0;

    // When it's bColdBoot, it will use Apollo's Device ID to form Bluetooth address.
    if (bColdBoot)
    {
        am_hal_mcuctrl_device_t sDevice;
        am_hal_mcuctrl_info_get(AM_HAL_MCUCTRL_INFO_DEVICEID, &sDevice);

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

    uint32_t IntNum = GPIO_INT;
    am_hal_gpio_interrupt_register(GPIO_INT_CHANNEL, IntNum, (am_hal_gpio_handler_t)HciDrvIntService, NULL);
    am_devices_em9305_enable_interrupt();
    #if defined(apollo510_eb_revb_em9305) || defined(APOLLO510_EVB)
    am_util_stdio_printf("Configuring IRQ for discrete EM9305\r\n");
    #endif
#ifdef AM_IRQ_PRIORITY_DEFAULT
    NVIC_SetPriority(GPIO_INT_IRQ, AM_IRQ_PRIORITY_DEFAULT);
#endif // AM_IRQ_PRIORITY_DEFAULT
    NVIC_EnableIRQ(GPIO_INT_IRQ);

    return ui32Status;
}

//*****************************************************************************
//
// Shut down the BLE core.
//
//*****************************************************************************
void
HciDrvRadioShutdown(void)
{
    BLE_HEARTBEAT_STOP();
    
    //
    // decrement counter and check if we should proceed with turning off the radio
    //
    am_devices_em9305_request_counter_set(false);
    if(am_devices_em9305_request_counter_get() != 0)
    {
        // do not power off EM9305
        return;
    }

    am_devices_em9305_disable_interrupt();
    am_devices_em9305_hsclk_req(false);//disable the EM9305 12M clock output
    am_devices_em9305_disable();

    g_ui32NumBytes   = 0;
    g_consumed_bytes = 0;

    am_devices_em9305_term(g_IomDevHdl);
}

#if USE_NONBLOCKING_HCI
//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************
static void
update_wake(void)
{
    AM_CRITICAL_BEGIN;

    //
    // We want to set WAKE if there's something in the write queue, but not if
    // SPISTATUS or IRQ is high.
    //
    if ( !am_hal_queue_empty(&g_sWriteQueue) &&
         (BLEIFn(0)->BSTATUS_b.SPISTATUS == 0) &&
         (BLE_IRQ_CHECK() == false))
    {
        am_hal_ble_wakeup_set(BLE, 1);

        //
        // If we've set wakeup, but IRQ came up at the same time, we should
        // just lower WAKE again.
        //
        if (BLE_IRQ_CHECK() == true)
        {
            am_hal_ble_wakeup_set(BLE, 0);
        }
    }

    AM_CRITICAL_END;
}
#endif

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
    uint8_t *pui8Wptr;
    hci_drv_write_t *psWriteBuffer;


    //
    // Check to see if we still have buffer space.
    //
    if (am_hal_queue_full(&g_sWriteQueue))
    {
        CRITICAL_PRINT("ERROR: Ran out of HCI transmit queue slots.\n");
        ERROR_RETURN(HCI_DRV_TRANSMIT_QUEUE_FULL, len);
    }

    if (len > (HCI_DRV_MAX_TX_PACKET-1))  // comparison compensates for the type byte at index 0.
    {
        CRITICAL_PRINT("ERROR: Trying to send an HCI packet larger than the hci driver buffer size (needs %d bytes of space).\n",
                       len);

        ERROR_RETURN(HCI_DRV_TX_PACKET_TOO_LARGE, len);
    }

    //
    // Get a pointer to the next item in the queue.
    //
    psWriteBuffer = am_hal_queue_next_slot(&g_sWriteQueue);

    //
    // Set all of the fields in the hci write structure.
    //
    psWriteBuffer->ui32Length = len + 1;

    pui8Wptr = (uint8_t *) psWriteBuffer->pui32Data;

    *pui8Wptr++ = type;

    for (uint32_t i = 0; i < len; i++)
    {
        pui8Wptr[i] = pData[i];
    }

    //
    // Advance the queue.
    //
    am_hal_queue_item_add(&g_sWriteQueue, 0, 1);

#if USE_NONBLOCKING_HCI
    //
    // Wake up the BLE controller.
    //
    CRITICAL_PRINT("INFO: HCI write requested.\n");

    update_wake();

#else
    //
    // Send an event to the BLE transfer handler function.
    //
    WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
#endif
    return len;
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

    g_HeartBeatTimer.handlerId = handlerId;
    g_HeartBeatTimer.msg.event = BLE_HEARTBEAT_EVENT;

    g_WakeTimer.handlerId = handlerId;
    g_WakeTimer.msg.event = BLE_SET_WAKEUP;
}

//*****************************************************************************
//
// Simple interrupt handler to call
//
// Note: These two lines need to be added to the exactle initialization
// function at the beginning of all Cordio applications:
//
//     handlerId = WsfOsSetNextHandler(HciDrvHandler);
//     HciDrvHandler(handlerId);
//
//*****************************************************************************
void
HciDrvIntService(void)
{
    //
    // Advance an event counter to make sure we're keeping track of edges
    // correctly.
    //
    g_ui32InterruptsSeen++;
    //
    // Send an event to get processed in the HCI handler.
    //
    WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
}

#if USE_NONBLOCKING_HCI

//*****************************************************************************
//
// This function determines what to do when a write operation completes.
//
//*****************************************************************************
void
hciDrvWriteCallback(uint8_t *pui8Data, uint32_t ui32Length, void *pvContext)
{
    CRITICAL_PRINT("INFO: HCI physical write complete.\n");

    am_hal_queue_item_get(&g_sWriteQueue, 0, 1);

#if TASK_LEVEL_DELAYS

    // Set a WSF timer to update wake later.
    WsfTimerStartMs(&g_WakeTimer, 30);

#else // TASK_LEVEL_DELAYS

    while ( BLEIFn(0)->BSTATUS_b.SPISTATUS )
    {
        am_util_delay_us(5);
    }

    //
    // Check the write queue, and possibly set wake again.
    //
    if ( !am_hal_queue_empty(&g_sWriteQueue) )
    {
        //
        // In this case, we need to delay before setting wake. Instead of
        // delaying here, we'll post an event to do it later.
        //
        WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
    }

#endif // TASK_LEVEL_DELAYS
}

//*****************************************************************************
//
// This function determines what to do when a read operation completes.
//
//*****************************************************************************
void
hciDrvReadCallback(uint8_t *pui8Data, uint32_t ui32Length, void *pvContext)
{
    (void) pui8Data ;
    (void) pvContext ;
    //
    // Set a "transfer needed" event.
    //
    // CRITICAL_PRINT("INFO: HCI physical read complete.\n");
    g_ui32NumBytes = ui32Length;
    WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);

#if TASK_LEVEL_DELAYS

    // Set a WSF timer to update wake later.
    WsfTimerStartMs(&g_WakeTimer, 30);

#else // TASK_LEVEL_DELAYS

    while ( BLE_IRQ_CHECK() )
    {
        am_util_delay_us(5);
    }

    //
    // Check the write queue, and possibly set wake.
    //
    if ( !am_hal_queue_empty(&g_sWriteQueue) )
    {
        am_hal_ble_wakeup_set(BLE, 1);
    }

#endif // TASK_LEVEL_DELAYS
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
    uint32_t ui32ErrorStatus, ui32TxRetries = 0;
    uint32_t ui32NumHciTransactions = 0;
    uint32_t read_hci_packet_count = 0;
    HCI_TRACE_INFO0("HciDrvHandler");
    //
    // If this handler was called in response to a heartbeat event, then it's
    // time to run a benign HCI command. Normally, the BLE controller should
    // handle this command without issue. If it doesn't acknowledge the
    // command, we will eventually get an HCI command timeout error, which will
    // alert us to the fact that the BLE core has become unresponsive in
    // general.
    //
    if (pMsg != NULL)
    {
        if (pMsg->event == BLE_HEARTBEAT_EVENT)
        {
            HciReadLocalVerInfoCmd();
            BLE_HEARTBEAT_START();
            return;
        }

        if (pMsg->event == BLE_SET_WAKEUP)
        {
            //
            // Attempt to set WAKE again.
            //
            update_wake();
            return;
        }
    }

    //
    // Check to see if we read any bytes over the HCI interface that we haven't
    // already sent to the BLE stack.
    //
    if (g_ui32NumBytes > g_consumed_bytes)
    {
        HCI_TRACE_INFO0("INFO: HCI data transferred to stack.");
        //
        // If we have any bytes saved, we should send them to the BLE stack
        // now.
        //
        g_consumed_bytes += hciTrSerialRxIncoming(g_pui8ReadBuffer + g_consumed_bytes,
                                                  g_ui32NumBytes - g_consumed_bytes);

        //
        // If the stack doesn't accept all of the bytes we had, we will need to
        // keep the event set and come back later. Otherwise, we can just reset
        // our variables and exit the loop.
        //
        if (g_consumed_bytes != g_ui32NumBytes)
        {
            HCI_TRACE_INFO0("INFO: HCI data split up.");
            WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
            return;
        }
        else
        {
            HCI_TRACE_INFO0("INFO: HCI RX packet complete.");
            g_ui32NumBytes   = 0;
            g_consumed_bytes = 0;
            bReadBufferInUse = false;
        }
    }

    if ( BLE_IRQ_CHECK() )
    {
        if (bReadBufferInUse == true)
        {
            HCI_TRACE_INFO0("Read buffer already in use.");
            WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
            return;
        }

        //
        // If the stack has used up all of the saved data we've accumulated so
        // far, we should check to see if we need to read any *new* data.
        //
        HCI_TRACE_INFO0("INFO: HCI Read started.");
        bReadBufferInUse = true;
        ui32ErrorStatus = am_hal_ble_nonblocking_hci_read(BLE,
                                                          g_pui32ReadBuffer,
                                                          hciDrvReadCallback,
                                                          0);

        BLE_HEARTBEAT_RESTART();

        if (g_ui32NumBytes > HCI_DRV_MAX_RX_PACKET)
        {
            HCI_TRACE_INFO1("ERROR: Trying to receive an HCI packet "
                           "larger than the hci driver buffer size "
                           "(needs %d bytes of space).",
                           g_ui32NumBytes);

            error_check(HCI_DRV_RX_PACKET_TOO_LARGE);
        }

        if (ui32ErrorStatus != AM_HAL_STATUS_SUCCESS)
        {
            //
            // If the read didn't succeed for some physical reason, we need
            // to know. We shouldn't get failures here. We checked the IRQ
            // signal before calling the read function, and this driver
            // only contains a single call to the blocking read function,
            // so there shouldn't be any physical reason for the read to
            // fail.
            //
            HCI_TRACE_INFO1("HCI READ failed with status %d. "
                           "Try recording with a logic analyzer "
                           "to catch the error.\n",
                           ui32ErrorStatus);

            ERROR_RECOVER(ui32ErrorStatus);
        }
    }
}
#else
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
    uint32_t ui32ErrorStatus, ui32TxRetries = 0;
    uint32_t ui32NumHciTransactions = 0;
    uint32_t read_hci_packet_count = 0;
    HCI_TRACE_INFO0("HciDrvHandler");
    //
    // If this handler was called in response to a heartbeat event, then it's
    // time to run a benign HCI command. Normally, the BLE controller should
    // handle this command without issue. If it doesn't acknowledge the
    // command, we will eventually get an HCI command timeout error, which will
    // alert us to the fact that the BLE core has become unresponsive in
    // general.
    //
    if (pMsg != NULL)
    {
        if (pMsg->event == BLE_HEARTBEAT_EVENT)
        {
            HciReadLocalVerInfoCmd();
            BLE_HEARTBEAT_START();
            return;
        }
    }
    HCI_TRACE_INFO0("HciDrvHandler 2 past null check");
    //
    // Check to see if we read any bytes over the HCI interface that we haven't
    // already sent to the BLE stack.
    //
    if (g_ui32NumBytes > g_consumed_bytes)
    {
        HCI_TRACE_INFO0("HciDrvHandler 2 past g_ui32NumBytes > g_consumed_bytes check");
        //
        // If we have any bytes saved, we should send them to the BLE stack
        // now.
        //
        g_consumed_bytes += hciTrSerialRxIncoming(g_pui8ReadBuffer + g_consumed_bytes,
                                                  g_ui32NumBytes - g_consumed_bytes);

        //
        // If the stack doesn't accept all of the bytes we had,
        //
        HCI_TRACE_INFO2("HciDrvHandler 2 g_consumed_bytes, g_ui32NumBytes", g_consumed_bytes, g_ui32NumBytes);
        if (g_consumed_bytes != g_ui32NumBytes)
        {
            WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
            return;
        }
        else
        {
            g_ui32NumBytes   = 0;
            g_consumed_bytes = 0;
        }
    }

    //
    // Loop indefinitely, checking to see if there are still tranfsers we need
    // to complete.
    //
    while (ui32NumHciTransactions++ < HCI_DRV_MAX_HCI_TRANSACTIONS)
    {
        //
        // Figure out what kind of transfer the BLE core will accept.
        //
        if ( am_hal_gpio_input_read(GPIO_INT) )
        {
            HCI_TRACE_INFO0("HciDrvHandler 2 am_hal_gpio_input_read(GPIO_INT)");
            uint32_t ui32OldInterruptsSeen = g_ui32InterruptsSeen;

            BLE_HEARTBEAT_RESTART();
            HCI_TRACE_INFO0("HciDrvHandler 2 BLE_HEARTBEAT_RESTART");
            //
            // Is the BLE core asking for a read? If so, do that now.
            //
            g_ui32NumBytes = 0;
            HCI_TRACE_INFO0("HciDrvHandler 2 g_ui32NumBytes = 0");
            ui32ErrorStatus = am_devices_em9305_block_read(g_IomDevHdl, (uint32_t*)g_pui32ReadBuffer, &g_ui32NumBytes);
            HCI_TRACE_INFO1("HciDrvHandler 2 ui32ErrorStatus = am_devices_em9305_block_read %d", ui32ErrorStatus);

            if (g_ui32NumBytes > HCI_DRV_MAX_RX_PACKET)
            {
                HCI_TRACE_INFO1("ERROR: Trying to receive an HCI packet larger than the hci driver buffer size (needs %d bytes of space).",
                               g_ui32NumBytes);

                error_check(HCI_DRV_RX_PACKET_TOO_LARGE);
            }

            if ( ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS)
            {
                //
                // If the read succeeded, we need to wait for the IRQ signal to
                // go back down. If we don't we might inadvertently try to read
                // the same packet twice.
                //
                uint32_t ui32IRQRetries;
                for (ui32IRQRetries = 0; ui32IRQRetries < HCI_DRV_MAX_IRQ_TIMEOUT; ui32IRQRetries++)
                {
                    if (am_hal_gpio_input_read(GPIO_INT) == 0 || g_ui32InterruptsSeen != ui32OldInterruptsSeen)
                    {
                        break;
                    }

                    am_util_delay_us(1);
                }
                HCI_TRACE_INFO1("HciDrvHandler 2 ui32IRQRetries = %d", ui32IRQRetries);
                //
                // Pass the data along to the stack. The stack should be able
                // to read as much data as we send it.  If it can't, we need to
                // know that.
                //
                g_consumed_bytes = hciTrSerialRxIncoming(g_pui8ReadBuffer, g_ui32NumBytes);
                HCI_TRACE_INFO1("HciDrvHandler 2 g_consumed_bytes = %d", g_consumed_bytes);
                if (g_consumed_bytes != g_ui32NumBytes)
                {
                    HCI_TRACE_INFO2("HciDrvHandler 2 second g_consumed_bytes, g_ui32NumBytes", g_consumed_bytes, g_ui32NumBytes);
                    // need to come back again
                    WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
                    // take a break now

                    // worst case disable BLE_IRQ
                    break;
                }

                g_ui32NumBytes   = 0;
                g_consumed_bytes = 0;

                read_hci_packet_count++;
            }
            else
            {
                HCI_TRACE_INFO1("HciDrvHandler 2 HCI READ failed with status %d. Try recording with a logic analyzer to catch the error.", ui32ErrorStatus);
                //
                // If the read didn't succeed for some physical reason, we need
                // to know. We shouldn't get failures here. We checked the IRQ
                // signal before calling the read function, and this driver
                // only contains a single call to the blocking read function,
                // so there shouldn't be any physical reason for the read to
                // fail.
                //
                CRITICAL_PRINT("HCI READ failed with status %d. Try recording with a logic analyzer to catch the error.\n",
                               ui32ErrorStatus);

                ERROR_RECOVER(ui32ErrorStatus);
            }

            if (read_hci_packet_count >= HCI_DRV_MAX_READ_PACKET)
            {
                // It looks like there's time that we won't get interrupt even though
                // there's packet waiting for host to grab.
                WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);

                break;
            }

        }
        else
        {
            HCI_TRACE_INFO0("HciDrvHandler 2 am_hal_queue_empty(&g_sWriteQueue)");
            //
            // If we don't have anything to read, we can start checking to see
            // if we have things to write.
            //
            if (am_hal_queue_empty(&g_sWriteQueue))
            {
                //
                // If not, we're done!
                //
                break;
            }
            else
            {
                //
                // If we do have something to write, just pop a single item
                // from the queue and send it.
                //
                hci_drv_write_t *psWriteBuffer = am_hal_queue_peek(&g_sWriteQueue);
                ui32ErrorStatus = am_devices_em9305_block_write(g_IomDevHdl,
                                                                (uint8_t *)psWriteBuffer->pui32Data,
                                                                psWriteBuffer->ui32Length);

                //
                // If we managed to actually send a packet, we can go ahead and
                // advance the queue.
                //
                if (ui32ErrorStatus == AM_DEVICES_EM9305_STATUS_SUCCESS)
                {
                    //
                    // Restart the heartbeat timer.
                    //
                    BLE_HEARTBEAT_RESTART();

                    am_hal_queue_item_get(&g_sWriteQueue, 0, 1);

                    ui32TxRetries = 0;
                    // Resetting the cumulative count
                    ui32NumHciTransactions = 0;
                }
                else
                {
                    //
                    // If we fail too many times in a row, we should throw an
                    // error to avoid a lock-up.
                    //
                    ui32TxRetries++;

                    if (ui32TxRetries > HCI_DRV_MAX_TX_RETRIES)
                    {
                        // we need to come back again later.
                        WsfSetEvent(g_HciDrvHandleID, BLE_TRANSFER_NEEDED_EVENT);
                        break;
                    }
                }

            }
        }
    }

    if (ui32NumHciTransactions == HCI_DRV_MAX_HCI_TRANSACTIONS)
    {
        HCI_TRACE_INFO0("HciDrvHandler 2 ERROR: Maximum number of successive HCI transactions exceeded.");
        ERROR_RECOVER(HCI_DRV_TOO_MANY_PACKETS);
    }
}
#endif

//*****************************************************************************
//
// Register an error handler for the HCI driver.
//
//*****************************************************************************
void
HciDrvErrorHandlerSet(hci_drv_error_handler_t pfnErrorHandler)
{
    g_hciDrvErrorHandler = pfnErrorHandler;
}

/*************************************************************************************************/
/*!
 *  \fn     HciVscUpdateLinklayerFeature
 *
 *  \brief  Vendor specific command for settting LE feature mask for EM9305.
 *   Note: About the The bit positions for each Link Layer feature please refer to
 *         BLUETOOTH CORE SPECIFICATION Version 5.4 | Vol 6, Part B from page 2844.
 *  \return none
 */
/*************************************************************************************************/
uint8_t ll_local_feats[LE_FEATS_LEN] = {0};

void HciVscUpdateLinklayerFeature(void)
{
    ll_local_feats[0] = (uint8_t)LL_FEATURES_BYTE0;
    ll_local_feats[1] = (uint8_t)(LL_FEATURES_BYTE1>>8);
    ll_local_feats[2] = (uint8_t)(LL_FEATURES_BYTE2>>16);
    ll_local_feats[3] = (uint8_t)(LL_FEATURES_BYTE3>>24);
    ll_local_feats[4] = (uint8_t)(LL_FEATURES_BYTE4>>32);
    ll_local_feats[5] = (uint8_t)(LL_FEATURES_BYTE5>>40);

    HciVendorSpecificCmd(HCI_VSC_SET_LOCAL_SUP_FEAT_CMD_OPCODE, LE_FEATS_LEN, ll_local_feats);
}

void HciVscDisablePowerCtrl(void)
{
     // Clear the 33rd, 34th, and 35th bits, which are in the 4th, 5th, and 6th bytes respectively
    hciCoreCb.leStates[4] &= ~(1 << 0); // 33rd bit
    hciCoreCb.leStates[4] &= ~(1 << 1); // 34th bit
    hciCoreCb.leStates[4] &= ~(1 << 2); // 35th bit

    HciVendorSpecificCmd(HCI_VSC_SET_LOCAL_SUP_FEAT_CMD_OPCODE, HCI_LE_STATES_LEN, hciCoreCb.leStates);
}

/*************************************************************************************************/
/*!
 *  \fn     HciVscSetRfPowerLevelEx
 *
 *  \brief  Vendor specific command for settting Radio transmit power level
 *          for EM9305.
 *
 *  \param  txPowerlevel    valid range from 0 to 15 in decimal.
 *  Note: The Tx power will be linear if txPowerlevel is between 4-15, so we strongly recommend not
 *        to set txPowerlevel below 4.
 *
 *  \return true when success, otherwise false
 */
/*************************************************************************************************/
bool_t
HciVscSetRfPowerLevelEx(int8_t txPowerlevel)
{
    if((txPowerlevel > TX_POWER_LEVEL_MIN_DBM) && (txPowerlevel <= TX_POWER_LEVEL_MAXIMUM))
    {
        HciVendorSpecificCmd(HCI_VSC_SET_TX_POWER_LEVEL_CMD_OPCODE, HCI_VSC_SET_TX_POWER_LEVEL_CMD_LENGTH, (uint8_t *)&txPowerlevel);
        return true;
    }
    else
    {
        // set value out of range
        return false;
    }
}

/*************************************************************************************************/
/*!
 *  \fn     HciVscConstantTransmission
 *
 *  \brief  This procedure is to enable/disable BLE Radio into constant transmission mode.
 *
 *  \param  txchannel  BLE TX channel
 *
 *  \return none
 */
/*************************************************************************************************/

void
HciVscConstantTransmission(uint8_t txchannel)
{
    //TODO add vender cmd
}

/*************************************************************************************************/
/*!
 *  \fn     HciVscSetCustom_BDAddr
 *
 *  \brief  This procedure is to set customer-provided Bluetooth address if needed.
 *
 *  \param  bd_addr  pointer to a bluetooth address customer allocates or NULL to use Apollo Device ID.
 *
 *  \return true when success
 */
/*************************************************************************************************/
bool_t HciVscSetCustom_BDAddr(uint8_t *bd_addr)
{
    uint8_t invalid_bd_addr[6] = {0};

    // When bd_addr is null, it will use Apollo's Device ID to form Bluetooth address.
    if ((bd_addr == NULL) || (memcmp(invalid_bd_addr, bd_addr, 6) == 0))
        return false;
    else {
        memcpy(g_BLEMacAddress, bd_addr, 6);
        return true;
    }
}

void HciVscUpdateBDAddress(void)
{
    HciVendorSpecificCmd(HCI_VSC_SET_DEV_PUB_ADDR_CMD_OPCODE, 6, g_BLEMacAddress);
}
/*************************************************************************************************/
/*!
 *  \fn     HciVscSetConstantTransmission
 *
 *  \brief  This procedure is to start/stop carrier wave output in BLE Radio.
 *
 *  \param  txchannel  BLE TX channel
 *
 *  \return none
 */
/*************************************************************************************************/

void
HciVscCarrierWaveMode(uint8_t txchannel)
{
    //TODO add vender cmd
}

/*************************************************************************************************/
/*!
 *  \fn     HciVscTransmitTestStart
 *
 *  \brief  This is to start vendor command to end the transmitter test.
 *
 *  \param transTestParam   Pamameters for transmitter test
 *
 */
/*************************************************************************************************/
void
HciVscTransmitTestStart(hciVscStartTransTestCmd_t * transTestParam)
{
    hciVscStartTransTestCmd_t startTransTestCmd = {0};

    uint8_t *p = (uint8_t *)&startTransTestCmd;

    UINT8_TO_BSTREAM(p, transTestParam->tx_test_mode);
    UINT8_TO_BSTREAM(p, transTestParam->tx_channel);
    UINT8_TO_BSTREAM(p, transTestParam->test_data_len);
    UINT8_TO_BSTREAM(p, transTestParam->pkt_payl);
    UINT8_TO_BSTREAM(p, transTestParam->phy);
    UINT8_TO_BSTREAM(p, transTestParam->tx_power_level);

    HciVendorSpecificCmd(HCI_VSC_START_TRANSMIT_TEST_CMD_OPCODE, HCI_VSC_START_TRANSMIT_TEST_CMD_LENGTH, (uint8_t *)&startTransTestCmd);
}

/*************************************************************************************************/
/*!
 *  \fn     HciVscTransmitTestEnd
 *
 *  \brief  This is to send vendor command to end the transmitter test.
 *
 *
 */
/*************************************************************************************************/

void
HciVscTransmitTestEnd(void)
{
    HciVendorSpecificCmd(HCI_VSC_END_TRANSMIT_TEST_CMD_OPCODE, HCI_VSC_END_TRANSMIT_TEST_CMD_LENGTH, NULL);
}

/*************************************************************************************************/
/*!
 *  \fn     HciDrvBleSleepSet
 *
 *  \brief  Set BLE sleep enable/disable for the BLE core.
 *
 *  \param  enable 'true' set sleep enable, 'false' set sleep disable
 *
 *  \return none
 */
/*************************************************************************************************/
void
HciDrvBleSleepSet(bool enable)
{
    HciVendorSpecificCmd(HCI_VSC_SET_SLEEP_OPTION_CMD_OPCODE, HCI_VSC_SET_SLEEP_OPTION_CMD_LENGTH, (uint8_t *)&enable);
}

//*****************************************************************************
//
// Clear the HCI write queue
//
//*****************************************************************************
void
HciDrvEmptyWriteQueue(void)
{
    am_hal_queue_from_array(&g_sWriteQueue, g_psWriteBuffers);
}
