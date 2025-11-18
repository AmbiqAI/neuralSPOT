//*****************************************************************************
//
//! @file uart_stream_common.c
//!
//! @brief Common code and definitions for uart_stream example
//!
//! @addtogroup uart_stream_echo_rx_dma
//! @{
//! @defgroup uart_stream_common_rx_dma_c UART Streaming Common Data
//! @{
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
#ifndef UART_STREAM_COMMON_H
#define UART_STREAM_COMMON_H

#include "uart_stream_example_config.h"
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "uart_example_timer_control.h"

#define NUM_UARTS_USED 1

#if defined(AM_BSP_GPIO_UART0_TX) && defined(AM_BSP_GPIO_UART0_RX)
#define UART0_AVAIL 1
#else
#define UART0_AVAIL 0
#endif
#if defined(AM_BSP_GPIO_UART1_TX) && defined(AM_BSP_GPIO_UART1_RX)
#define UART1_AVAIL 1
#else
#define UART1_AVAIL 0
#endif
#if defined(AM_BSP_GPIO_UART2_TX) && defined(AM_BSP_GPIO_UART2_RX)
#define UART2_AVAIL 1
#else
#define UART2_AVAIL 0
#endif
#if defined(AM_BSP_GPIO_UART3_TX) && defined(AM_BSP_GPIO_UART3_RX)
#define UART3_AVAIL 1
#else
#define UART3_AVAIL 0
#endif

//
//! enumerate uart names that are available
//! @note this is not always the same value as the uart hardware instance number
//
typedef enum
{
#if UART0_AVAIL
    eUART0,
#endif
#if UART1_AVAIL
    eUART1,
#endif
#if UART2_AVAIL
    eUART2,
#endif
#if UART3_AVAIL
    eUART3,
#endif
    eMAX_UARTS,
    euidX32 = 0x70000000,  // force this to 32bit value on all compilers
}
uart_id_e;

#if defined(AM_BSP_UART_STREAM_UART)
 #define UART_ID AM_BSP_UART_STREAM_UART  // use the bsp requested uart for this
#else
 #warning "No AM_BSP_UART_STREAM_UART defined in the BSP"
 #if defined(UART2_AVAIL) && (UART2_AVAIL > 0)
  #define UART_ID                2        // want to default to uart2 is it is defined in the BSP
 #else
  #define UART_ID                0
 #endif
#endif


//
//! defines max DMA buffer sizes
//
#define TX_BUFFER_SIZE (1024 * 8)
#define RX_BUFFER_SIZE (1024 * 8)

//
//! This data block can be used for DMA and is destined for SRAM (not DTCM)
//! when it is assigned to SRAM, the MPU will be used to declare this region
//! non-cacheable
//
typedef struct
{
    uint8_t pui8UARTTXDMABuffer[TX_BUFFER_SIZE];
    uint8_t pui8UARTRXDMABuffer[RX_BUFFER_SIZE];
}
uart_dma_rx_tx_DMA_buffers_t;

typedef struct
{
    uint32_t             pinNUmber;
    am_hal_gpio_pincfg_t *pinCfg;
}
uart_pin_descr_t;

//
//! defines the uart hardware config and use
//
typedef struct
{
    am_hal_gpio_pincfg_t       *pinCfg;
    uart_pin_descr_t           rxPin;
    uart_pin_descr_t           txPin;
    uart_id_e                  uartId;
    uint8_t                    uartHwIndex;
}
uart_hw_defs_t;

//
//! define data used per uart instance
//
typedef struct
{
    //
    //! uart timer params, from file: uart_example_timer_control.h
    //
    uart_examp_timer_params_t   sUartTimerParams;

    const am_hal_uart_config_t  *psUartConfig;  //!< points to uart setup parameters (baud, parity,etc.)
    const uart_hw_defs_t        *psUartPinDefs; //!< points to pin defs for this uart

    pfTimerCallback_t           pfTimerCallback;  //!< Timer is unused if this is NULL

    //
    //! reserve memory for the hal, this is required
    //
    void                       *pUartHandle;

    uint32_t                    ui32ISrErrorCount;
    am_hal_uart_stream_status_t e32Status;

    //
    //! These are used when each uart has different sized buffers
    //! Each example/test may use different buffers sizes for testing
    //
    uint32_t ui32TxCount;

    uart_id_e uartId;       //!< uart instance number

    uint8_t   uart_hardware_instance;
}
uart_examp_stream_vars_t;

//
//! Used to pass initialization data to each test's init function
//
typedef struct
{
    uint32_t ui32TestBuffMax;
    uint8_t  *pui8Testbuff;
}
uart_examp_stream_init_args_t;


//
//! define a function that can be called (optional) on receipt of a specific stream error
//
typedef uint32_t (*pfStreamAction_t)(uart_examp_stream_vars_t *psAsyncLVars);

//
//! Used to define configuration of stream error message map (below)
//! This is used to design a table of actions for each bit set in the
//! uart isr status
//
typedef struct
{
    pfStreamAction_t pfStreamAction;        //!< processing function for status (optional)
    am_hal_uart_stream_status_t streamID;   //!< bit pattern matched for this entry
    const char       *stream_message;       //!< printed if function pointer, pfStreamAction, is null
    bool             bEnabled;              //!< if false, the status will be ignored
}
stream_error_message_action_map_t;

//*****************************************************************************
//
//! @brief called to configure MPU for dma data
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t uart_stream_cmn_mpuConfig(void);

//*****************************************************************************
//
//! @brief called to configure MPU (no cache) for passed in parameters
//!
//! @param ui32BaseAddress  base address for MPU regions (should be 32byte aligned)
//! @param ui32DmaSize      size of the mpu region in bytes
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t mpuConfig(uint32_t ui32BaseAddress, uint32_t ui32DmaSize);

//*****************************************************************************
//
//! @brief
//!
//! @param pUartDesc        local uart data and config descriptor(struct)
//! @param pui8TxBuff       if null, will use SRAM DMA buff(required for DMA),
//! @param ui32TxBuffSize   the size of the tx buffer that will be used for this test
//! @param pui8RxBuff       if null, will use SRAM DMA buff(required for DMA),
//! @param ui32RxBuffSize   the size of the rx buffer that will be used for this test
//!
//! @return standard hal status
//
//*****************************************************************************
extern  uint32_t uart_cmn_serial_interface_init(uart_examp_stream_vars_t *pUartDesc,
                                                const am_hal_uart_stream_rx_config_t *rxConfig,
                                                const am_hal_uart_stream_tx_config_t *txConfig );

//*****************************************************************************
//
//! @brief This will format an error string for either standard hal errors or uart
//! specific errors
//!
//! @param eErrx    The error code returned from a uart call
//! @param printError  if true, the string will be printed in this function
//! @return         Pointer to string associated with the error (eErrx) code
//!
//
//*****************************************************************************
const char * printErrorString( am_hal_uart_errors_t eErrx, bool printError );
//*****************************************************************************
//
//! @brief  will check and log run-time errors
//!
//! @param eErrx  return stats from function call
//! @param ui32LoopCount    loop count
//! @param ui32ErrorPos   call position (id of the error function call)
//
//*****************************************************************************
extern void errorCheck(am_hal_uart_errors_t eErrx,
                       uint32_t ui32LoopCount,
                       uint32_t ui32ErrorPos);
//*****************************************************************************
//
//! @brief will enable uart interrupts
//!
//! @param psStreamLVars pointer to local test RAM variable block
//!
//! @return  standard hal status
//
//*****************************************************************************
extern uint32_t serial_interrupt_enable( uart_examp_stream_vars_t *psStreamLVars);

//*****************************************************************************
//
//! @brief standard MCU setup calls
//!
//! @return standard hal status
//
//*****************************************************************************
uint32_t mcu_setup(void);

//*****************************************************************************
//
//! @brief Process isr returned status with the default map
//!
//! @note this should be called in the background, not ISR
//!
//! @param psAsyncLVars  pointer to test-specific variable
//! @param streamStatus
//!
//! @return  standard hal status
//
//*****************************************************************************
uint32_t processStreamErrorDefMap(uart_examp_stream_vars_t *psAsyncLVars,
                                  am_hal_uart_stream_status_t streamStatus);

//*****************************************************************************
//
//! @brief Process isr returned status with a supplied map
//!
//! @note this should be called in the background, not ISR
//!
//! @param pSEM          status action map
//! @param psAsyncLVars  pointer to test-specific variable
//! @param streamStatus  value returned from
//!
//! @return  standard hal status
//
//*****************************************************************************
uint32_t processStreamError(const stream_error_message_action_map_t *pSEM,
                            uart_examp_stream_vars_t *psAsyncLVars,
                            am_hal_uart_stream_status_t streamStatus);


#endif //UART_STREAM_COMMON_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
