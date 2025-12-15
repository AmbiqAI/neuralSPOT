//*****************************************************************************
//
//! @file am_devices_510L_radio.c
//!
//! @brief Apollo510L Radio Subsystem Driver with IPC Communication Interface.
//!
//! @addtogroup devices_510L_radio Apollo510L Radio Device Driver
//! @ingroup devices
//! @{
//!
//! Purpose: This module provides a comprehensive hardware abstraction layer
//!          for the Apollo510L radio subsystem. It enables communication between
//!          the main Apollo MCU and the integrated radio core through Inter-Processor
//!          Communication (IPC). The driver supports Bluetooth HCI operations,
//!          radio subsystem mode management, memory access, and RF calibration
//!          for wireless connectivity applications.
//!
//! @section devices_510L_radio_features Key Features
//!
//! 1. @b IPC @b Communication: Efficient inter-processor communication with the radio core.
//! 2. @b Bluetooth @b HCI @b Support: Complete HCI command, event, and data packet handling.
//! 3. @b RSS @b Mode @b Management: Control radio subsystem operational modes (CP, NP, XO).
//! 4. @b Memory @b Access: Direct read/write access to radio subsystem memory and registers.
//! 5. @b RF @b Trimming: Configure and apply RF calibration parameters for optimal performance.
//! 6. @b FreeRTOS @b Integration: Thread-safe operations with semaphores and event groups.
//! 7. @b Asynchronous @b Operations: Non-blocking write operations with callback support.
//! 8. @b Buffer @b Management: Efficient IPC buffer allocation and release mechanisms.
//!
//! @section devices_510L_radio_functionality Functionality
//!
//! - Initialize and configure IPC communication with radio subsystem
//! - Handle Bluetooth HCI command transmission and response reception
//! - Manage radio subsystem operational modes and power states
//! - Provide direct memory and register access for debugging and configuration
//! - Support RF trimming and calibration parameter management
//! - Implement thread-safe communication with FreeRTOS synchronization
//! - Handle asynchronous data transmission with callback mechanisms
//! - Manage IPC shared memory and buffer allocation
//!
//! @section devices_510L_radio_usage Usage
//!
//! 1. Initialize IPC shared memory with am_devices_510L_radio_ipc_shm_init()
//! 2. Initialize radio driver with am_devices_510L_radio_init()
//! 3. Set desired radio subsystem mode with am_devices_510L_radio_set_rss_opmode()
//! 4. Send HCI commands using am_devices_510L_radio_write() or am_devices_510L_radio_write_sync()
//! 5. Read responses and events with am_devices_510L_radio_read()
//! 6. Configure RF trimming with am_devices_510L_radio_set_rftrim_req()
//! 7. Access memory/registers with read_mem_req/write_mem_req functions
//! 8. Reset radio subsystem with am_devices_510L_radio_reset() if needed
//! 9. Deinitialize with am_devices_510L_radio_deinit() when finished
//!
//! @section devices_510L_radio_configuration Configuration
//!
//! - Set up IPC shared memory parameters and buffer allocation
//! - Configure radio subsystem operational modes (CM4-only, Network Processor, etc.)
//! - Set up FreeRTOS synchronization objects (semaphores, event groups)
//! - Configure RF trimming parameters for optimal radio performance
//! - Set up callback functions for asynchronous data reception
//! - Configure IPC buffer sizes and timeout parameters
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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

#include "libmetal/lib/errno.h"

#include "rpmsg_internal.h"
#include "ipc_service.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "am_devices_510L_radio.h"
#include "ns_ambiqsuite_harness.h"

//*****************************************************************************
//
// Macro Definitions.
//
//*****************************************************************************
#define RPMSG_HDR_SIZE      (sizeof(struct rpmsg_hdr))
#define IPC_BUFFER_MAX_SIZE (RPMSG_BUFFER_SIZE - RPMSG_HDR_SIZE)
#define IPC_MBOX_ISR_KICK   0x01
#define IPC_DATA_RX_PROC    0x02
#define IPC_RX_EVENTS       (IPC_MBOX_ISR_KICK | IPC_DATA_RX_PROC)
#define MAXCNT_RSS_BOOTUP_RETRY 0x05
//*****************************************************************************
//
// Function declaration.
//
//*****************************************************************************
static void ipc_ept_bound(void *priv);
static void ipc_ept_recv(const void *data, size_t size, void *priv);
static void ipc_mbox_send(uint32_t data);
static void ipc_mbox_init(void);
static void ipc_mbox_handler(void *pArg);

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
static am_devices_510L_radio_t g_sAm510LRadio;
static am_hal_queue_t g_sIpcRxQueue;
static SemaphoreHandle_t g_sTxSem;
static SemaphoreHandle_t g_sTxSyncSem;
static uint16_t g_ui8TxPendOpcode;
static am_devices_510L_radio_rss_opmode_e g_eCurrentRssOpMode;
static bool g_bIpcShmCfgCmp = false;
static uint32_t g_ui32WrRadioAddr;

struct ipcRxBuf_t
{
    uint8_t *data;
    uint16_t len;
};

struct ipcCb_t
{
  struct ipc_device *instance;
  struct ipc_ept_cfg *eptCfg;
  struct ipc_ept ept;
  struct ipcRxBuf_t rxBuf[10];
  SemaphoreHandle_t boundSem;
  EventGroupHandle_t rxEvt;
  EventBits_t evtMsk;
  TaskHandle_t rxTaskHandle;
};

static struct backend_config g_sIpcBackend =
{
    .role = ROLE_HOST,
    .mbox_data_send = AM_HAL_IPC_MBOX_SIGNAL_MSG_M2D,
    .mbox_data_rcv = AM_HAL_IPC_MBOX_SIGNAL_MSG_D2M,
    .mbox_send = &ipc_mbox_send,
    .mbox_init = &ipc_mbox_init,
    .buffer_size = RPMSG_BUFFER_SIZE
};

static struct ipc_device g_sIpcInstance =
{
    .name = "ipc0",
    .config = &g_sIpcBackend,
    .api = NULL,
    .data = NULL,
};

static struct ipc_ept_cfg g_sIpcEptCfg =
{
    .name = "am_ipc",
    .cb =
    {
        .bound = ipc_ept_bound,
        .received = ipc_ept_recv,
    },
};

static struct ipcCb_t ipcCb =
{
    .instance = &g_sIpcInstance,
    .eptCfg = &g_sIpcEptCfg
};

//*****************************************************************************
//
// IPC endpoint bound callback
//
//*****************************************************************************
static void ipc_ept_bound(void *priv)
{
    xSemaphoreGive(ipcCb.boundSem);
}

//*****************************************************************************
//
// IPC endpoint data received callback
//
//*****************************************************************************
static void ipc_ept_recv(const void *data, size_t size, void *priv)
{
    int ret = 0;
    struct ipcRxBuf_t *buf;

    if (size > IPC_BUFFER_MAX_SIZE)
    {
        am_util_stdio_printf("Received data is too long\n");
        return;
    }

    if (data == NULL)
    {
        am_util_stdio_printf("Received data is NULL\n");
        return;
    }

    //
    // Hold the buffer and queue it for further handling
    //
    ret = ipc_service_hold_rx_buffer(&ipcCb.ept, (void *)data);
    if (ret < 0)
    {
        am_util_stdio_printf("ipc_service_hold_rx_buffer failed with ret %d\n", ret);
        return;
    }

    if (am_hal_queue_full(&g_sIpcRxQueue))
    {
        am_util_stdio_printf("No available RX queue\n");
        return;
    }

    //
    // Get a pointer to the next item in the queue
    //
    buf = am_hal_queue_next_slot(&g_sIpcRxQueue);

    //
    // Queue the IPC RX buffer address in shared memory and buffer length
    //
    buf->data = (uint8_t *)data;
    buf->len = size;

    //
    // Add the item to the queue
    //
    am_hal_queue_item_add(&g_sIpcRxQueue, NULL, 1);

    xEventGroupSetBits(ipcCb.rxEvt, IPC_DATA_RX_PROC);
}

//*****************************************************************************
//
// IPC RX processing task
//
//*****************************************************************************
static void ipc_rx_task(void *pArg)
{
    while (1)
    {
        ipcCb.evtMsk = xEventGroupWaitBits(ipcCb.rxEvt, IPC_RX_EVENTS, pdTRUE,
                                           pdFALSE, portMAX_DELAY);

        if (ipcCb.evtMsk & IPC_MBOX_ISR_KICK)
        {
            //
            // Received the mailbox IRQ, executes the IPC receiving. In normal, the
            // ipc_ept_bound() or ipc_ept_recv() should be executed.
            //
            ipc_receive(ipcCb.instance);
        }

        if (ipcCb.evtMsk & IPC_DATA_RX_PROC)
        {
            //
            // Get the IPC queued RX data
            //
            struct ipcRxBuf_t *buf = am_hal_queue_peek(&g_sIpcRxQueue);
            uint8_t packet_type;
            bool release = true;

            packet_type = *buf->data;
            if (packet_type <= AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_RADIO_END)
            {
                if (g_sAm510LRadio.rxCb[AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_RADIO])
                {
                    release = g_sAm510LRadio.rxCb[AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_RADIO](buf->data, buf->len);
                }
            }
            else if (packet_type >= AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_START)
            {
                struct am_devices_510L_radio_ipc_sys_hdr *hdr =
                    (struct am_devices_510L_radio_ipc_sys_hdr *)buf->data;

                if (hdr->opcode == g_ui8TxPendOpcode)
                {
                    xSemaphoreGive(g_sTxSyncSem);
                }
                else
                {
                    am_util_stdio_printf("Invalid system response, expected 0x%02x "
                        "but received 0x%02x\n", g_ui8TxPendOpcode, hdr->opcode);
                }

                if (g_sAm510LRadio.rxCb[AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_SYS])
                {
                    release = g_sAm510LRadio.rxCb[AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_SYS](buf->data, buf->len);
                }
            }
            else
            {
                am_util_stdio_printf("Unknown IPC packet type!\n");
            }

            //
            // Free the IPC buffer pointer and length in the queue
            //
            am_hal_queue_item_get(&g_sIpcRxQueue, 0, 1);
            if (!am_hal_queue_empty(&g_sIpcRxQueue))
            {
                //
                // If there is more data, set the event to process further
                //
                xEventGroupSetBits(ipcCb.rxEvt, IPC_DATA_RX_PROC);
            }

            if (release)
            {
                //
                // Release the IPC buffer immediately
                //
                ipc_service_release_rx_buffer(&ipcCb.ept, buf->data);
            }
        }
    }
}

//*****************************************************************************
//
// IPC Mailbox Data Interrupt handler
//
//*****************************************************************************
static void ipc_mbox_handler(void *pArg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult;

    if ( xPortIsInsideInterrupt() == pdTRUE )
    {
        xResult = xEventGroupSetBitsFromISR(ipcCb.rxEvt, IPC_MBOX_ISR_KICK,
                                            &xHigherPriorityTaskWoken);
        if (xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else
    {
        xResult = xEventGroupSetBits(ipcCb.rxEvt, IPC_MBOX_ISR_KICK);
        if (xResult != pdFAIL)
        {
            portYIELD();
        }
    }
}

//*****************************************************************************
//
// Send magic number to remote device via IPC mailbox
//
//*****************************************************************************
static void ipc_mbox_send(uint32_t data)
{
    uint32_t wr_len = 1;
    uint32_t data_buf = data;
    am_hal_ipc_mbox_data_write(&data_buf, wr_len);
}

//*****************************************************************************
//
// Initialize the IPC mailbox
//
//*****************************************************************************
static void ipc_mbox_init(void)
{
    am_hal_ipc_mbox_handler_register(AM_HAL_IPC_MBOX_SIGNAL_MSG_D2M, ipc_mbox_handler, NULL);
}

//*****************************************************************************
//
// Handler of IPC share memory configuration response
//
//*****************************************************************************
static void ipc_shm_config_rsp_handler(void *pArg)
{
    g_bIpcShmCfgCmp = true;
}

//*****************************************************************************
//
// Send the IPC share memory configuration request
//
//*****************************************************************************
static uint32_t ipc_shm_config_req(void)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    uint32_t ipcShmCfg[] =
    {
        AM_HAL_IPC_MBOX_SIGNAL_MSG_IPC_SHM_CONFIG_REQ, 2,
        g_sIpcBackend.shm_addr, g_sIpcBackend.shm_size
    };

    uint32_t ui32Timeout10Us = 50000;

    //
    // Register IPC share mmeory configuration response handler
    //
    ui32Status = am_hal_ipc_mbox_handler_register(AM_HAL_IPC_MBOX_SIGNAL_MSG_IPC_SHM_CONFIG_RSP,
                                                  ipc_shm_config_rsp_handler,
                                                  NULL);
    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {   
        ns_lp_printf("am_hal_ipc_mbox_handler_register failed\n");
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    g_bIpcShmCfgCmp = false;

    //
    // Configure the IPC share memory configuration to peer via mailbox
    //
    ui32Status = am_hal_ipc_mbox_data_write(&ipcShmCfg[0], sizeof(ipcShmCfg) / sizeof(uint32_t));
    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        ns_lp_printf("am_hal_ipc_mbox_data_write failed\n");
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    //
    // Wait for the response of IPC share memory configuration request
    //
    while (--ui32Timeout10Us)
    {
        if (g_bIpcShmCfgCmp)
        {
            break;
        }
        am_hal_delay_us(10);
    }

    if (!g_bIpcShmCfgCmp)
    {
        ns_lp_printf("IPC Share memory configuration timeout\n");
        return AM_DEVICES_510L_RADIO_STATUS_TIMEOUT;
    }

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Initialize the IPC service, including create the instance and register the
// endpoints. Before IPC instance creation we need to handshake via mailbox
// to align the IPC share memory address/size between cores.
//
// The handshake protocol is:
//
// application core -> network processing core:
// -------------------------------------------------------------------------------
// |         word0          |     word1    |       word2       |       word3     |
// -------------------------------------------------------------------------------
// IPC_SHM_CONFIG_REQ magic | command size | Share memory addr | Share memory size
// ------------------------------------------------------------------------------
//
// application core <- network processing core:
// --------------------------
// |         word0          |
// --------------------------
// IPC_SHM_CONFIG_RSP magic |
// --------------------------
//
//*****************************************************************************
static uint32_t ipc_init(void)
{
    int ret = 0;
    uint32_t ui32Status = AM_DEVICES_510L_RADIO_STATUS_SUCCESS;

    am_hal_queue_from_array(&g_sIpcRxQueue, ipcCb.rxBuf);

    ipcCb.boundSem = xSemaphoreCreateBinary();
    if (ipcCb.boundSem == NULL)
    {
        am_util_stdio_printf("ipcCb.boundSem create failure\n");
        return AM_DEVICES_510L_RADIO_STATUS_MEM_ERR;
    }

    ipcCb.rxEvt = xEventGroupCreate();
    if (ipcCb.rxEvt == NULL)
    {
        am_util_stdio_printf("ipcCb.rxEvt create failure\n");
        return AM_DEVICES_510L_RADIO_STATUS_MEM_ERR;
    }

    xTaskCreate(ipc_rx_task, "ipc_rx_task", 1024, 0, 4, &ipcCb.rxTaskHandle);

    ui32Status = ipc_shm_config_req();
    if (ui32Status != AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
    {
        am_util_stdio_printf("IPC Share memory configuration failed, status %d\n", ui32Status);
        return ui32Status;
    }

    ret = ipc_service_open_instance(ipcCb.instance);
    if ((ret < 0) && (ret != -EALREADY))
    {
        am_util_stdio_printf("ipc_service_open_instance() failed\n");
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    ret = ipc_service_register_endpoint(ipcCb.instance, &ipcCb.ept,
                                        ipcCb.eptCfg);
    if (ret < 0)
    {
        am_util_stdio_printf("IPC endpoint register fails %d", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    xSemaphoreTake(ipcCb.boundSem, portMAX_DELAY);

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Re-initialize the IPC service, including deregister the endpoints and close
// the instance, then re-create the instance and register the endpoints.
//
//*****************************************************************************
static uint32_t ipc_reinit(void)
{
    int ret = 0;
    struct ipcRxBuf_t *buf = NULL;
    uint32_t ui32Status = AM_DEVICES_510L_RADIO_STATUS_SUCCESS;

    //
    // Free the RX queue and IPC buffers
    //
    while (!am_hal_queue_empty(&g_sIpcRxQueue))
    {
        buf = am_hal_queue_peek(&g_sIpcRxQueue);
        if (buf)
        {
            ipc_service_release_rx_buffer(&ipcCb.ept, buf->data);
        }

        am_hal_queue_item_get(&g_sIpcRxQueue, 0, 1);
    }

    ret = ipc_service_deregister_endpoint(&ipcCb.ept);
    if (ret)
    {
        am_util_stdio_printf("IPC endpoint deregister fails %d", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    ret = ipc_service_close_instance(ipcCb.instance);
    if (ret)
    {
        am_util_stdio_printf("ipc_service_open_instance() failed %d\n", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    ui32Status = ipc_shm_config_req();
    if (ui32Status != AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
    {
        am_util_stdio_printf("IPC Share memory configuration failed, status %d\n", ui32Status);
        return ui32Status;
    }

    ret = ipc_service_open_instance(ipcCb.instance);
    if ((ret < 0) && (ret != -EALREADY))
    {
        am_util_stdio_printf("ipc_service_open_instance() failed %d\n", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    ret = ipc_service_register_endpoint(ipcCb.instance, &ipcCb.ept,
                                        ipcCb.eptCfg);
    if (ret < 0)
    {
        am_util_stdio_printf("IPC endpoint register fails %d", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    xSemaphoreTake(ipcCb.boundSem, portMAX_DELAY);

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
// RX callback of IPC system control block
//
//*****************************************************************************
static bool ipc_sys_rx_callback(uint8_t *data, uint16_t size)
{
    struct am_devices_510L_radio_ipc_sys_hdr *hdr =
        (struct am_devices_510L_radio_ipc_sys_hdr *)data;

    switch (hdr->type)
    {
        case AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_RSP:
        {
            switch (hdr->opcode)
            {
                case AM_DEVICES_510L_RADIO_SYS_OP_READ_REG:
                {
                    struct am_devices_510L_radio_read_reg_rsp_t *rsp =
                        (struct am_devices_510L_radio_read_reg_rsp_t *)data;
                    if (rsp->status == 0)
                    {
                        am_util_stdio_printf("Read remote reg 0x%08x, value: 0x%08x\n",
                            rsp->addr, rsp->data);
                    }
                    else
                    {
                        am_util_stdio_printf("Read remote reg 0x%08x fails, status %d\n",
                            rsp->addr, rsp->status);
                    }
                }
                    break;

                case AM_DEVICES_510L_RADIO_SYS_OP_WRITE_REG:
                {
                    struct am_devices_510L_radio_write_reg_rsp_t *rsp =
                        (struct am_devices_510L_radio_write_reg_rsp_t *)data;
                    am_util_stdio_printf("Write remote reg 0x%08x status %d\n",
                        g_ui32WrRadioAddr, rsp->status);
                }
                    break;

                case AM_DEVICES_510L_RADIO_SYS_OP_READ_MEM:
                {
                    struct am_devices_510L_radio_read_mem_rsp_t *rsp =
                        (struct am_devices_510L_radio_read_mem_rsp_t *)data;
                    if (rsp->status == 0)
                    {
                        am_util_stdio_printf("Read remote mem 0x%08x, size %d, data: \n",
                            rsp->addr, rsp->size);
                        for (uint8_t i = 0; i < rsp->size; i++)
                        {
                            am_util_stdio_printf("%02X ", rsp->data[i]);
                            if ((i % 0x10 == 0) && (i > 0))
                            {
                                am_util_stdio_printf("\n");
                            }
                        }
                        am_util_stdio_printf("\n");
                    }
                    else
                    {
                        am_util_stdio_printf("Read remote mem 0x%08x fails, status %d\n",
                            rsp->addr, rsp->status);
                    }
                }
                    break;

                case AM_DEVICES_510L_RADIO_SYS_OP_WRITE_MEM:
                {
                    struct am_devices_510L_radio_write_mem_rsp_t *rsp =
                        (struct am_devices_510L_radio_write_mem_rsp_t *)data;
                    am_util_stdio_printf("Write remote mem 0x%08x status %d\n",
                        g_ui32WrRadioAddr, rsp->status);
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }

    return true;
}

//*****************************************************************************
//
//  Initialize the radio controller driver.
//
//*****************************************************************************
uint32_t am_devices_510L_radio_init(void)
{
    uint32_t ui32Status;
    uint32_t retry_cnt = 0;

    //
    // Enable the subsystem SWD pins if enabled.
    //
    #ifdef CM4_DEBUG
    am_bsp_cm4_swd_pins_enable();
    #endif
    //
    // Power on subsystem.
    //
    for ( retry_cnt = 0; retry_cnt<MAXCNT_RSS_BOOTUP_RETRY; retry_cnt++ )
    {
        ui32Status = am_hal_pwrctrl_rss_bootup();
        if (( ui32Status != AM_HAL_STATUS_SUCCESS ) && ( ui32Status != AM_HAL_STATUS_IN_USE))
        {
            //Retry to power on RSS while RSS fails to boot up
            am_hal_pwrctrl_rss_pwroff();
        }
        else
        {
            break;
        }
    }

    if (( ui32Status != AM_HAL_STATUS_SUCCESS ) && ( ui32Status != AM_HAL_STATUS_IN_USE))
    {
        am_util_stdio_printf("RSS bootup failure ui32Status 0x=%02x \n", ui32Status);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    am_hal_interrupt_master_enable();

    g_sTxSem = xSemaphoreCreateBinary();
    if (g_sTxSem == NULL)
    {
        am_util_stdio_printf("g_sTxSem create failure\n");
        return AM_DEVICES_510L_RADIO_STATUS_MEM_ERR;
    }
    xSemaphoreGive(g_sTxSem);

    g_sTxSyncSem = xSemaphoreCreateBinary();
    if (g_sTxSyncSem == NULL)
    {
        am_util_stdio_printf("g_sTxSyncSem create failure\n");
        return AM_DEVICES_510L_RADIO_STATUS_MEM_ERR;
    }

    //
    // Register the IPC system control RX callback
    //
    am_devices_510_radio_ipc_register_rx_cback(ipc_sys_rx_callback, AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_SYS);

    //
    // Set the current Radio subsystem mode
    //
    g_eCurrentRssOpMode = AM_DEVICES_510L_RADIO_RSS_OPMODE_CP;

    //
    // Initialize the IPC service, wait for bounding with remote device
    //
    ui32Status = ipc_init();
    if (ui32Status != AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
    {
        return ui32Status;
    }

    am_util_stdio_printf("Radio core boots successfully and IPC is bound\n");

    //
    // Add some delay for radio initialization
    //
    am_hal_delay_us(200);


    //
    // Request the radio core to enter the network processing mode
    //
    uint32_t ui32UsrCnt_XTAL_HS;
    ui32Status = am_hal_clkmgr_clock_status_get(AM_HAL_CLKMGR_CLK_ID_XTAL_HS, &ui32UsrCnt_XTAL_HS);

    if ( (ui32UsrCnt_XTAL_HS) && (ui32Status == AM_HAL_STATUS_SUCCESS) )
    {
        ui32Status = am_devices_510L_radio_set_rss_opmode(AM_DEVICES_510L_RADIO_RSS_OPMODE_NP_XO);
    }
    else
    {
        ui32Status = am_devices_510L_radio_set_rss_opmode(AM_DEVICES_510L_RADIO_RSS_OPMODE_NP);
    }

    if ( ui32Status != AM_DEVICES_510L_RADIO_STATUS_SUCCESS )
    {
        return ui32Status;
    }
    //
    // Enable the TXEN & RXEN pins
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_TXEN, g_AM_BSP_GPIO_TXEN);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_RXEN, g_AM_BSP_GPIO_RXEN);
    return ui32Status;
}

//*****************************************************************************
//
//  De-Initialize the radio controller driver.
//
//*****************************************************************************
uint32_t am_devices_510L_radio_deinit(void)
{
    int ret = 0;
    struct ipcRxBuf_t *buf = NULL;

    //
    // Power off subsystem.
    //
    am_hal_pwrctrl_rss_pwroff();

    //
    // Free the RX queue and IPC buffers
    //
    while (!am_hal_queue_empty(&g_sIpcRxQueue))
    {
        buf = am_hal_queue_peek(&g_sIpcRxQueue);
        if (buf)
        {
            ipc_service_release_rx_buffer(&ipcCb.ept, buf->data);
        }

        am_hal_queue_item_get(&g_sIpcRxQueue, 0, 1);
    }

    ret = ipc_service_deregister_endpoint(&ipcCb.ept);
    if (ret)
    {
        am_util_stdio_printf("IPC endpoint deregister fails %d", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    ret = ipc_service_close_instance(ipcCb.instance);
    if (ret)
    {
        am_util_stdio_printf("ipc_service_close_instance() failed %d\n", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    vSemaphoreDelete(g_sTxSem);
    vSemaphoreDelete(g_sTxSyncSem);
    vSemaphoreDelete(ipcCb.boundSem);
    vEventGroupDelete(ipcCb.rxEvt);
    vTaskDelete(ipcCb.rxTaskHandle);

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
//  Reset the radio Controller.
//
//*****************************************************************************
void am_devices_510L_radio_reset(void)
{
}

//*****************************************************************************
//
//  Execute HCI write to the radio controller
//
//*****************************************************************************
uint32_t am_devices_510L_radio_read(uint8_t* pui8Data, uint32_t* pui32BytesReceived)
{
    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Execute HCI write to the radio controller
//
//*****************************************************************************
uint32_t am_devices_510L_radio_write(uint8_t* pui8Data, uint32_t ui32NumBytes)
{
    int ret;
    void *tx_data;
    uint32_t tx_len = ui32NumBytes;

    if ((ui32NumBytes > IPC_BUFFER_MAX_SIZE) || (pui8Data == NULL))
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    ret = ipc_service_get_tx_buffer(&ipcCb.ept, &tx_data, &tx_len, portMAX_DELAY);
    if (ret < 0)
    {
        am_util_stdio_printf("ipc_service_get_tx_buffer failed with ret %d", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    //
    // Write the data to the available IPC TX buffer in shared memory
    //
    memcpy(tx_data, pui8Data, ui32NumBytes);

    ret = ipc_service_send_nocopy(&ipcCb.ept, tx_data, ui32NumBytes);
    if (ret < 0)
    {
        am_util_stdio_printf("ipc_service_send_nocopy failed with ret %d", ret);
        return AM_DEVICES_510L_RADIO_STATUS_ERROR;
    }

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Execute HCI synchronous write to the radio controller
//
//*****************************************************************************
uint32_t am_devices_510L_radio_write_sync(uint8_t* pui8Data, uint32_t ui32NumBytes)
{
    uint32_t ui32Status = AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
    struct am_devices_510L_radio_ipc_sys_hdr *hdr =
        (struct am_devices_510L_radio_ipc_sys_hdr *)pui8Data;

    //
    // Only use for the System Request packet for now
    //
    if (hdr->type != AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_OPERATION;
    }

    if (xSemaphoreTake(g_sTxSem, (TickType_t)0) != pdPASS)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_OPERATION;
    }

    ui32Status = am_devices_510L_radio_write(pui8Data, ui32NumBytes);
    if (ui32Status == AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
    {
        //
        // Save the Opcode and suspend to wait for the response
        //
        g_ui8TxPendOpcode = hdr->opcode;
        if (xSemaphoreTake(g_sTxSyncSem, (TickType_t)(1000 / portTICK_PERIOD_MS)) != pdPASS)
        {
            am_util_stdio_printf("The peer device is unresponsive, opcode 0x%02x\n", hdr->opcode);
            ui32Status = AM_DEVICES_510L_RADIO_STATUS_TIMEOUT;
        }
    }

    xSemaphoreGive(g_sTxSem);

    return ui32Status;
}

//*****************************************************************************
//
//! Register the IPC RX callback
//
//*****************************************************************************
uint32_t am_devices_510_radio_ipc_register_rx_cback(am_devices_510L_radio_ipc_rx_cback cb,
                                                    uint8_t module)
{
    if (module >= AM_DEVICES_510L_RADIO_IPC_PACKET_MODULE_NUM)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    if (cb)
    {
        g_sAm510LRadio.rxCb[module] = cb;
    }

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! This function can be called to manually release the IPC RX Buffer
//
//*****************************************************************************
uint32_t am_devices_510L_radio_release_ipc_rx_buf(uint8_t *buf)
{
    int ret;

    if ((!buf) || (((uint32_t)buf) < (uint32_t)g_sIpcBackend.shm_addr) ||
        (((uint32_t)buf) > ((uint32_t)g_sIpcBackend.shm_addr + g_sIpcBackend.shm_size)))
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    ret = ipc_service_release_rx_buffer(&ipcCb.ept, buf);
    if (ret)
    {
        am_util_stdio_printf("IPC RX buffer release failed %d\n", ret);
        return AM_DEVICES_510L_RADIO_STATUS_MEM_ERR;
    }

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//!
//! Set the operational mode of radio subsystem via IPC
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_set_rss_opmode(am_devices_510L_radio_rss_opmode_e mode)
{
    uint32_t ui32Status;
    struct am_devices_510L_radio_set_rss_opmode_req_t req;

    if (mode >= AM_DEVICES_510L_RADIO_RSS_OPMODE_INVALID)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    if (g_eCurrentRssOpMode == mode)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_OPERATION;
    }

    if (g_eCurrentRssOpMode == AM_DEVICES_510L_RADIO_RSS_OPMODE_OFF)
    {
        //
        // Turn on RSS power
        //
        ui32Status = am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_NETAOL);
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return AM_DEVICES_510L_RADIO_STATUS_ERROR;
        }
    }

    if ((g_eCurrentRssOpMode == AM_DEVICES_510L_RADIO_RSS_OPMODE_OFF) ||
        (g_eCurrentRssOpMode == AM_DEVICES_510L_RADIO_RSS_OPMODE_XO_CP_OFF))
    {
        //
        // CM55 asserts CM4_wakeup request to the CM4_PMU
        //
        ui32Status = am_hal_pwrctrl_cm4_wakeup_req();
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return AM_DEVICES_510L_RADIO_STATUS_ERROR;
        }

        //
        // IPC rebound
        //
        ui32Status = ipc_reinit();
        if (ui32Status != AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
        {
            return ui32Status;
        }
    }

    req.hdr.type = AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ;
    req.hdr.opcode = AM_DEVICES_510L_RADIO_SYS_OP_SET_RSS_OPMODE;
    req.hdr.size = sizeof(uint8_t);
    req.mode = (uint8_t)mode;

    ui32Status = am_devices_510L_radio_write_sync((uint8_t *)&req, sizeof(req));

    if (ui32Status == AM_DEVICES_510L_RADIO_STATUS_SUCCESS)
    {
        g_eCurrentRssOpMode = mode;
        if (mode == AM_DEVICES_510L_RADIO_RSS_OPMODE_OFF)
        {
            //
            // Turn off RSS power
            //
            ui32Status = am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_NETAOL);
        }
    }

    return ui32Status;
}

//*****************************************************************************
//!
//! Read register of another core via IPC
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_read_reg_req(uint32_t ui32Address)
{
    struct am_devices_510L_radio_read_reg_req_t req;

    req.hdr.type = AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ;
    req.hdr.opcode = AM_DEVICES_510L_RADIO_SYS_OP_READ_REG;
    req.hdr.size = sizeof(uint32_t);
    req.magic   = 0; // TBD
    req.addr    = ui32Address;

    return am_devices_510L_radio_write_sync((uint8_t *)&req, sizeof(req));
}

//*****************************************************************************
//!
//! Write register of another core via IPC
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_write_reg_req(uint32_t ui32Address, uint32_t ui32Data)
{
    struct am_devices_510L_radio_write_reg_req_t req;

    req.hdr.type = AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ;
    req.hdr.opcode = AM_DEVICES_510L_RADIO_SYS_OP_WRITE_REG;
    req.hdr.size = sizeof(uint32_t) + sizeof(uint32_t);
    req.magic   = 0; // TBD
    req.addr    = ui32Address;
    req.data    = ui32Data;

    g_ui32WrRadioAddr = req.addr;

    return am_devices_510L_radio_write_sync((uint8_t *)&req, sizeof(req));
}

//*****************************************************************************
//!
//! Read memory of another core via IPC
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_read_mem_req(uint32_t ui32Address, uint8_t ui8NumBytes)
{
    struct am_devices_510L_radio_read_mem_req_t req;

    if (ui8NumBytes > AM_DEVICES_510L_RADIO_IPC_MEM_RW_MAX_SIZE)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    req.hdr.type = AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ;
    req.hdr.opcode = AM_DEVICES_510L_RADIO_SYS_OP_READ_MEM;
    req.hdr.size = sizeof(uint32_t) + sizeof(uint8_t);
    req.magic   = 0; // TBD
    req.addr    = ui32Address;
    req.size    = ui8NumBytes;

    return am_devices_510L_radio_write_sync((uint8_t *)&req, sizeof(req));
}

//*****************************************************************************
//!
//! Write register of another core via IPC
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_write_mem_req(uint32_t ui32Address, uint8_t *pui8Data,
                                             uint8_t ui8NumBytes)
{
    struct am_devices_510L_radio_write_mem_req_t req;

    if ((pui8Data == NULL) || (ui8NumBytes > AM_DEVICES_510L_RADIO_IPC_MEM_RW_MAX_SIZE))
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    req.hdr.type = AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ;
    req.hdr.opcode = AM_DEVICES_510L_RADIO_SYS_OP_WRITE_MEM;
    req.hdr.size = sizeof(uint32_t) + sizeof(uint8_t) + AM_DEVICES_510L_RADIO_IPC_MEM_RW_MAX_SIZE;
    req.magic   = 0; // TBD
    req.addr    = ui32Address;
    req.size    = ui8NumBytes;
    memcpy((uint8_t *)&req.data[0], pui8Data, ui8NumBytes);

    g_ui32WrRadioAddr = req.addr;

    return am_devices_510L_radio_write_sync((uint8_t *)&req, sizeof(req));
}

//*****************************************************************************
//!
//! Set RF trim values to the network processing core via IPC
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_set_rftrim_req(uint8_t *pui8Data)
{
    struct am_devices_510L_radio_set_rftrim_req_t req;

    if (pui8Data == NULL)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    req.hdr.type = AM_DEVICES_510L_RADIO_IPC_PACKET_TYPE_SYS_REQ;
    req.hdr.opcode = AM_DEVICES_510L_RADIO_SYS_OP_SET_RFTRIM;
    req.hdr.size = sizeof(req.trim_b.data);

    memcpy(&req.trim_b.data[0], pui8Data, req.hdr.size);

    return am_devices_510L_radio_write_sync((uint8_t *)&req, sizeof(req));
}

//*****************************************************************************
//!
//! Initialize the IPC share memory configuration.
//!
//*****************************************************************************
uint32_t am_devices_510L_radio_ipc_shm_init(am_devices_510L_radio_ipc_shm_t *psIpcShm)
{
    if (psIpcShm == NULL)
    {
        return AM_DEVICES_510L_RADIO_STATUS_INVALID_ARG;
    }

    g_sIpcBackend.shm_addr = psIpcShm->ui32IpcShmAddr;
    g_sIpcBackend.shm_size = psIpcShm->ui32IpcShmSize;

    return AM_DEVICES_510L_RADIO_STATUS_SUCCESS;
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

