//*****************************************************************************
//
//! @file radio_task.c
//!
//! @brief Task to handle radio operation.
//!
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

//*****************************************************************************
//
// Global includes for this project.
//
//*****************************************************************************
#include "ble_freertos_fcc_test.h"

//*****************************************************************************
//
// WSF standard includes.
//
//*****************************************************************************
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_buf.h"
#include "wsf_timer.h"

//*****************************************************************************
//
// Includes for operating the ExactLE stack.
//
//*****************************************************************************
#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_cooper.h"

#include "am_mcu_apollo.h"
#include "am_util.h"
#include "am_bsp.h"

#include "hci_apollo_config.h"

#include "wsf_msg.h"
#include "app_ui.h"
#include "hci_drv_cooper.h"

//*****************************************************************************
//
// Radio task handle.
//
//*****************************************************************************
TaskHandle_t radio_task_handle;

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************
void exactle_stack_init(void);
void button_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg);

//*****************************************************************************
//
// WSF buffer pools.
//
//*****************************************************************************
#define WSF_BUF_POOLS               4

// Important note: the size of g_pui32BufMem should includes both overhead of internal
// buffer management structure, wsfBufPool_t (up to 16 bytes for each pool), and pool
// description (e.g. g_psPoolDescriptors below).
// The length of buffers in pool MUST be a multiple of sizeof(wsfBufMem_t), which is 8
// by default. Or it may cause WSF buffer pool initialization failure.

// Memory for the buffer pool
static uint32_t g_pui32BufMem[
        (WSF_BUF_POOLS*16
         + 16*8 + 32*4 + 64*6 + 280*14) / sizeof(uint32_t)];

// Default pool descriptor.
static wsfBufPoolDesc_t g_psPoolDescriptors[WSF_BUF_POOLS] =
{
    {  16,  8 },
    {  32,  4 },
    {  64,  6 },
    { 280,  14 }
};

wsfHandlerId_t ButtonHandlerId;
wsfTimer_t ButtonTimer;

#define LE_INVALID_PHY   0x0
#define LE_1M_PHY        0x01
#define LE_2M_PHY        0x02
#define LE_CODED_PHY_S8  0x03
#define LE_CODED_PHY_S2  0x04

// Indicates which test case to start with when button is pressed.
uint8_t current_test_case  = 0;
uint8_t current_phy        = LE_1M_PHY;

char *phy_str[] =
{
    NULL,
    "LE 1M PHY",
    "LE 2M PHY",
    "LE Coded PHY S=8",
    "LE Coded PHY S=2",
};

//*****************************************************************************
//
// Poll the buttons.
//
//*****************************************************************************
void button_handler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    uint32_t ui32ButtonTimer = 10 * 1000;

    hciLeTxTestV3Cmd_t tx_test_v3 =
    {
        .tx_channel =  CHL_2402_INDEX,
        .test_data_len = TEST_LEN_DEFAULT,
        .pkt_payl = PAYL_CONTINUOUS_WAVE,
        .phy = LE_1M_PHY,
        .cte_len = 0,
        .cte_type = 0,
        .switching_pattern_len = MIN_SWITCHING_PATTERN_LEN,
        .antenna_id = {0x0},
    };

    hciLeRxTestV3Cmd_t rx_test_v3 =
    {
        .rx_channel =  CHL_2402_INDEX,
        .phy = LE_1M_PHY,
        .mod_idx = 0,
        .exp_cte_len = 0,
        .exp_cte_type = 0,
        .slot_dur = HCI_CTE_SLOT_DURATION_1_US,
        .switching_pattern_len = MIN_SWITCHING_PATTERN_LEN,
        .antenna_id = {0x0},
    };
#if AM_BSP_NUM_BUTTONS
    ui32ButtonTimer = 10;
#endif
    //
    // Restart the button timer.
    //
    WsfTimerStartMs(&ButtonTimer, ui32ButtonTimer);

#if AM_BSP_NUM_BUTTONS
    //
    // Every time we get a button timer tick, check all of our buttons.
    //
    am_devices_button_array_tick(am_bsp_psButtons, AM_BSP_NUM_BUTTONS);

    //
    // If we got a a press, do something with it.
    //
    if ( am_devices_button_released(am_bsp_psButtons[0]) )
    {
        ++current_phy;

        am_util_debug_printf("Set phy = %s\n", phy_str[current_phy]);

        if ( current_phy == LE_CODED_PHY_S2 )
        {
            current_phy = LE_INVALID_PHY;
        }

        AppUiBtnTest(APP_UI_BTN_1_SHORT);
    }

    if ( am_devices_button_released(am_bsp_psButtons[1]) )
    {
      AppUiBtnTest(APP_UI_BTN_1_SHORT);
#endif
      switch ( current_test_case++ )
      {
        case 0:
            HciLeTestEndCmd();
            tx_test_v3.tx_channel = CHL_2402_INDEX;
            tx_test_v3.pkt_payl = PAYL_CONTINUOUS_WAVE;
            tx_test_v3.phy = current_phy;
            HciLeTransmitterTestCmdV3(&tx_test_v3);
            am_util_debug_printf("Continuous Wave on Channel 2402MHz, %s\n", phy_str[current_phy]);
            break;
        case 1:
            HciLeTestEndCmd();
            tx_test_v3.tx_channel = CHL_2440_INDEX;
            tx_test_v3.pkt_payl = PAYL_CONTINUOUS_WAVE;
            tx_test_v3.phy = current_phy;
            HciLeTransmitterTestCmdV3(&tx_test_v3);
            am_util_debug_printf("Continuous Wave on Channel 2440MHz, %s\n", phy_str[current_phy]);
            break;
        case 2:
            HciLeTestEndCmd();
            tx_test_v3.tx_channel = CHL_2480_INDEX;
            tx_test_v3.pkt_payl = PAYL_CONTINUOUS_WAVE;
            tx_test_v3.phy = current_phy;
            HciLeTransmitterTestCmdV3(&tx_test_v3);
            am_util_debug_printf("Continuous Wave on Channel 2480MHz, %s\n", phy_str[current_phy]);
            break;
        case 3:
            HciLeTestEndCmd();
            tx_test_v3.tx_channel = CHL_2402_INDEX;
            tx_test_v3.pkt_payl = PAYL_CONTINUOUS_MODULATE;
            tx_test_v3.phy = current_phy;
            HciLeTransmitterTestCmdV3(&tx_test_v3);
            am_util_debug_printf("Continuous Modulation on Channel 2402MHz, %s\n", phy_str[current_phy]);
            break;
        case 4:
            HciLeTestEndCmd();
            tx_test_v3.tx_channel = CHL_2440_INDEX;
            tx_test_v3.pkt_payl = PAYL_CONTINUOUS_MODULATE;
            tx_test_v3.phy = current_phy;
            HciLeTransmitterTestCmdV3(&tx_test_v3);
            am_util_debug_printf("Continuous Modulation on Channel 2440MHz, %s\n", phy_str[current_phy]);
            break;

        case 5:
            HciLeTestEndCmd();
            tx_test_v3.tx_channel = CHL_2480_INDEX;
            tx_test_v3.pkt_payl = PAYL_CONTINUOUS_MODULATE;
            tx_test_v3.phy = current_phy;
            HciLeTransmitterTestCmdV3(&tx_test_v3);
            am_util_debug_printf("Continuous Modulation on Channel 2480MHz, %s\n", phy_str[current_phy]);
            break;
        case 6:
            HciLeTestEndCmd();
            rx_test_v3.rx_channel = CHL_2402_INDEX;
            rx_test_v3.phy = current_phy;
            HciLeReceiverTestCmdV3(&rx_test_v3);
            am_util_debug_printf("Receiver Test on Channel 2402MHz, %s\n", phy_str[current_phy]);
            break;
        case 7:
            HciLeTestEndCmd();
            rx_test_v3.rx_channel = CHL_2440_INDEX;
            rx_test_v3.phy = current_phy;
            HciLeReceiverTestCmdV3(&rx_test_v3);
            am_util_debug_printf("Receiver Test on Channel 2440MHz, %s\n", phy_str[current_phy]);
            break;
        case 8:
            HciLeTestEndCmd();
            rx_test_v3.rx_channel = CHL_2480_INDEX;
            rx_test_v3.phy = current_phy;
            HciLeReceiverTestCmdV3(&rx_test_v3);
            am_util_debug_printf("Receiver Test on Channel 2480MHz, %s\n", phy_str[current_phy]);
            break;
        default:
            break;
      }

      if ( current_test_case >= 9 )
      {
        current_test_case = 0;
      }
#if AM_BSP_NUM_BUTTONS
    }
#endif
}

//*****************************************************************************
//
// Sets up a button interface.
//
//*****************************************************************************
void setup_buttons(void)
{
#if AM_BSP_NUM_BUTTONS
    //
    // Enable the buttons for user interaction.
    //
    am_devices_button_array_init(am_bsp_psButtons, AM_BSP_NUM_BUTTONS);
#endif
    ButtonTimer.handlerId = ButtonHandlerId;
    WsfTimerStartSec(&ButtonTimer, 2);
}

//*****************************************************************************
//
// Initialization for the ExactLE stack.
//
//*****************************************************************************
void exactle_stack_init(void)
{
    wsfHandlerId_t handlerId;
    uint16_t       wsfBufMemLen;
    //
    // Set up timers for the WSF scheduler.
    //
    WsfOsInit();
    WsfTimerInit();

    //
    // Initialize a buffer pool for WSF dynamic memory needs.
    //
    wsfBufMemLen = WsfBufInit(sizeof(g_pui32BufMem), (uint8_t *)g_pui32BufMem, WSF_BUF_POOLS,
               g_psPoolDescriptors);

    if (wsfBufMemLen > sizeof(g_pui32BufMem))
    {
        am_util_debug_printf("Memory pool is too small by %d\r\n",
                             wsfBufMemLen - sizeof(g_pui32BufMem));
    }

    //
    // Initialize the WSF security service.
    //
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    //
    // Set up callback functions for the various layers of the ExactLE stack.
    //
    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmPhyInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);

    ButtonHandlerId = WsfOsSetNextHandler(button_handler);
}

//*****************************************************************************
//
// GPIO interrupt handler.
//
//*****************************************************************************
void
am_cooper_irq_isr(void)
{
    uint32_t    ui32IntStatus;

    am_hal_gpio_interrupt_irq_status_get(AM_COOPER_IRQn, false, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(AM_COOPER_IRQn, ui32IntStatus);
    am_hal_gpio_interrupt_service(AM_COOPER_IRQn, ui32IntStatus);
}

//*****************************************************************************
//
// UART interrupt handler.
//
//*****************************************************************************
void am_uart_isr(void)
{
    uint32_t ui32Status;

    //
    // Read and save the interrupt status, but clear out the status register.
    //
    ui32Status = UARTn(0)->MIS;
    UARTn(0)->IEC = ui32Status;

}

//*****************************************************************************
//
// Perform initial setup for the radio task.
//
//*****************************************************************************
void RadioTaskSetup(void)
{
    am_util_debug_printf("RadioTask: setup\r\n");

    NVIC_SetPriority(COOPER_IOM_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(AM_COOPER_IRQn, NVIC_configMAX_SYSCALL_INTERRUPT_PRIORITY);
}

/*************************************************************************************************/
/*!
 *  \fn     TxPowerCtrlDmCback
 *
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void FccTestDmCback(dmEvt_t *pDmEvt)
{
    APP_TRACE_INFO1("recv cmd complete evt for opcode=0x%x", pDmEvt->vendorSpecCmdCmpl.opcode);

    if ( pDmEvt->vendorSpecCmdCmpl.opcode == HCI_VSC_UPDATE_NVDS_CFG_CMD_OPCODE )
    {
        HciResetCmd();
        HciVscSetRfPowerLevelEx(TX_POWER_LEVEL_PLUS_3P0_dBm);
    }
}

//*****************************************************************************
//
// Short Description.
//
//*****************************************************************************
void
RadioTask(void *pvParameters)
{
#if WSF_TRACE_ENABLED == TRUE
    //
    // Enable ITM
    //
    am_util_debug_printf("Starting wicentric trace:\n\n");
#endif

    //
    // Boot the radio.
    //
    HciDrvRadioBoot(1);

    //
    // Initialize the main ExactLE stack.
    //
    exactle_stack_init();

    // Update NVDS parameters in array nvds_data. HCI reset command
    // will be sent unpon command complete event is received.
    HciVscUpdateNvdsParam();

    // uncomment the following to set custom Bluetooth address here
    // {
    //     uint8_t bd_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    //     HciVscSetCustom_BDAddr(&bd_addr[0]);
    // }

    //
    // Prep the buttons for use
    //
    setup_buttons();

    //
    // Need to register DM callback for Vendor specific command's event
    //
    DmRegister(FccTestDmCback);

    am_util_debug_printf("Usage as below:\n");
#if AM_BSP_NUM_BUTTONS
    am_util_debug_printf("Short press Button 0 to set PHY\n");
    am_util_debug_printf("Short press Button 1 to go through different test modes\n");
#else
    am_util_debug_printf("The default PHY is 1M\n");
    am_util_debug_printf("10s timer to switch between test modes\n");
#endif

    // TBD for debug
    //HciVscSetRfPowerLevelEx(17);

    while (1)
    {
        //
        // Calculate the elapsed time from our free-running timer, and update
        // the software timers in the WSF scheduler.
        //
        wsfOsDispatcher();

    }
}
