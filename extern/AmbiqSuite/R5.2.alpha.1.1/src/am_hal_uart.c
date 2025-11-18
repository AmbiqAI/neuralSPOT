//*****************************************************************************
//
//! @file am_hal_uart.c
//!
//! @brief Hardware abstraction for the UART
//!
//! @addtogroup uart_ap510L UART Functionality
//! @ingroup apollo510L_hal
//! @{
//!
//! Purpose: This module provides comprehensive UART (Universal Asynchronous
//!          Receiver/Transmitter) hardware abstraction for Apollo5 devices.
//!          It supports UART configuration, data transfer, interrupt handling,
//!          and DMA operations for serial communication applications.
//!
//! @section hal_uart_features Key Features
//!
//! 1. @b UART @b Configuration: Flexible baud rate and parameter configuration.
//! 2. @b Data @b Transfer: Support for blocking and non-blocking data transfer.
//! 3. @b DMA @b Support: High-speed DMA-based data transfer operations.
//! 4. @b Interrupt @b Handling: Comprehensive interrupt management for UART events.
//! 5. @b FIFO @b Management: Advanced FIFO handling and buffer management.
//!
//! @section hal_uart_functionality Functionality
//!
//! - Initialize and configure UART peripheral
//! - Handle data transfer operations (blocking/non-blocking)
//! - Support DMA-based high-speed transfers
//! - Manage FIFO buffers and data flow
//! - Handle UART interrupts and status monitoring
//!
//! @section hal_uart_usage Usage
//!
//! 1. Initialize UART using am_hal_uart_initialize()
//! 2. Configure UART parameters and baud rate
//! 3. Set up DMA or interrupt handling as needed
//! 4. Perform data transfer operations
//! 5. Handle UART events and status monitoring
//!
//! @section hal_uart_configuration Configuration
//!
//! - @b Baud @b Rate: Configure UART baud rate and timing
//! - @b Data @b Format: Set up data bits, parity, and stop bits
//! - @b DMA @b Settings: Configure DMA transfer parameters
//! - @b Interrupts: Set up interrupt sources and handlers
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
#include "am_mcu_apollo.h"
#include "am_hal_crm_private.h"

// #TODO update when the reg of MCUCTRL D2ASPARE is updated
#define MCUCTRL_D2ASPARE_UART0PLL  (0x01 << 22) // Bit 22 for UART0, Bit 23 for UART1

//*****************************************************************************
//
//! UART magic number for handle verification.
//
//*****************************************************************************
#define AM_HAL_MAGIC_UART               0xEA9E06

#define AM_HAL_UART_CHK_HANDLE(h)                                             \
    ((h) &&                                                                   \
     ((am_hal_handle_prefix_t *)(h))->s.bInit &&                              \
     (((am_hal_handle_prefix_t *)(h))->s.magic == AM_HAL_MAGIC_UART))

//*****************************************************************************
//
//! UART FIFO Size
//
//*****************************************************************************
#define AM_HAL_UART_FIFO_MAX 32

//*****************************************************************************
//
//! Delay constant
//
//*****************************************************************************
#define ONE_BYTE_DELAY(pState)                                              \
    if (1)                                                                  \
    {                                                                       \
        uint32_t delayus;                                                   \
        delayus = 1000000 * 10 / pState->ui32BaudRate;   /* Delay for 10 bit (8 bits,start/stop) */  \
        delayus += 1;                               /* Round up         */  \
        am_hal_delay_us(delayus);                                           \
    }

#define RETURN_ON_ERROR(x)                                                    \
    if ((x) != AM_HAL_STATUS_SUCCESS)                                         \
    {                                                                         \
        return (x);                                                           \
    };
//*****************************************************************************
//
//! Structure for handling UART register state information for power up/down
//
//*****************************************************************************
typedef struct
{
    bool bValid;
    uint32_t regILPR;
    uint32_t regIBRD;
    uint32_t regFBRD;
    uint32_t regLCRH;
    uint32_t regCR;
    uint32_t regIFLS;
    uint32_t regIER;
    uint32_t regDMACFG;
}
am_hal_uart_register_state_t;

//*****************************************************************************
//
//! Structure for handling UART HAL state information.
//
//*****************************************************************************
typedef struct
{
    //
    //! For internal verification purposes
    //
    am_hal_handle_prefix_t prefix;

    //
    //! Register state for power-up/power-down
    //
    am_hal_uart_register_state_t sRegState;

    //
    //! UART module number.
    //
    uint32_t ui32Module;

    //
    //! Pointer to the dma buffer descriptor queue
    //! This is allocated by the calling function in application space
    //! and is attached to this struct via a call to
    //! am_hal_uart_stream_dmaQueueInit
    //! @note Since it is allocated and passed in, this usually shouldn't be on the stack
    //
    am_hal_uart_dma_config_t *psDmaQueue;

    //
    //! Most recently configured baud rate.
    //
    uint32_t ui32BaudRate;


    am_hal_queue_t sTxQueue;
    am_hal_queue_t sRxQueue;


    am_hal_uart_transfer_t sActiveRead;
    volatile uint32_t ui32BytesRead;

    am_hal_uart_transfer_t sActiveWrite;
    volatile uint32_t ui32BytesWritten;

    //
    //! Queued write/read implementation
    //
    bool bEnableTxQueue;

    bool bEnableRxQueue;

    //
    //! Cleared when any transmit transaction is started. Set after TX complete
    //! interrupt is received.
    //
    volatile bool bLastTxComplete;

    //
    // Stores the transaction configurations.
    //
    am_hal_uart_transfer_t  Transaction;

    //
    //! UART clock source.
    //
    am_hal_uart_clock_src_e eClkSrc;

    //
    //! UART clock Divider.
    //
    am_hal_uart_clock_div_e eClkDiv;

    //
    //! UART Dma mode
    //

    //am_hal_uart_streaming_dma_mode_e eStreamingDmaMode;

    bool bCurrentlyWriting;

    //
    //! State information about nonblocking transfers.
    //
    bool bCurrentlyReading;

    //
    //! DMA transaction in progress.
    //
    volatile bool                bDMABusy;
}
am_hal_uart_state_t;

//! Create one state structure for each physical UART.
am_hal_uart_state_t g_am_hal_uart_states[AM_REG_UART_NUM_MODULES];

//*****************************************************************************
//
// Static function prototypes.
//
//*****************************************************************************
static uint32_t config_baudrate(uint32_t ui32Module,
                                uint32_t ui32UartClkFreq,
                                uint32_t ui32DesiredBaudrate,
                                uint32_t *pui32ActualBaud);

static uint32_t blocking_write(void *pHandle,
                               const am_hal_uart_transfer_t *psTransfer);

static uint32_t blocking_read(void *pHandle,
                              const am_hal_uart_transfer_t *psTransfer);

static uint32_t nonblocking_write(void *pHandle,
                                  const am_hal_uart_transfer_t *psTransfer);

static uint32_t nonblocking_read(void *pHandle,
                                 const am_hal_uart_transfer_t *psTransfer);

static void nonblocking_write_sm(void *pHandle);
static void nonblocking_read_sm(void *pHandle);
static uint32_t tx_queue_update(void *pHandle);
static uint32_t rx_queue_update(void *pHandle);
static uint32_t am_hal_uart_crm_apbclk_enable(uint32_t ui32Module, bool bEnable);
static uint32_t am_hal_uart_clock_enable(uint32_t ui32Module, bool bEnable, am_hal_uart_clock_src_e eClkSrc, am_hal_uart_clock_div_e eClkDiv);

//*****************************************************************************
//
// Initialize the UART
//
//*****************************************************************************

uint32_t
am_hal_uart_initialize(uint32_t ui32Module, void **ppHandle)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check that the request module is in range.
    //
    if (ui32Module >= AM_REG_UART_NUM_MODULES )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    //
    // Check for valid arguments.
    //
    if (!ppHandle)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Make sure we're not re-activating an active handle.
    //
    if (AM_HAL_UART_CHK_HANDLE(*ppHandle))
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }
#endif
    //
    // Initialize the handle.
    //
    g_am_hal_uart_states[ui32Module].prefix.s.bInit = true;
    g_am_hal_uart_states[ui32Module].prefix.s.magic = AM_HAL_MAGIC_UART;
    g_am_hal_uart_states[ui32Module].ui32Module = ui32Module;
    g_am_hal_uart_states[ui32Module].sRegState.bValid = false;
    g_am_hal_uart_states[ui32Module].ui32BaudRate = 0;
    g_am_hal_uart_states[ui32Module].bCurrentlyReading = 0;
    g_am_hal_uart_states[ui32Module].bCurrentlyWriting = 0;
    g_am_hal_uart_states[ui32Module].bEnableTxQueue = 0;
    g_am_hal_uart_states[ui32Module].bEnableRxQueue = 0;
    g_am_hal_uart_states[ui32Module].ui32BytesWritten = 0;
    g_am_hal_uart_states[ui32Module].ui32BytesRead = 0;
    g_am_hal_uart_states[ui32Module].bLastTxComplete = true;

    //
    // Return the handle.
    //
    *ppHandle = (void *)&g_am_hal_uart_states[ui32Module];

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Deinitialize the UART interface.
//
//*****************************************************************************
uint32_t
am_hal_uart_deinitialize(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *)pHandle;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check the handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    //
    // Reset the handle.
    //
    pState->prefix.s.bInit = false;
    pState->prefix.s.magic = 0;
    pState->ui32Module = 0;
    pState->sRegState.bValid = false;

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Change the power state of the UART module.
//
//*****************************************************************************
uint32_t
am_hal_uart_power_control(void *pHandle, uint32_t ePowerState,
                          bool bRetainState)
{

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    am_hal_pwrctrl_periph_e eUARTPowerModule = ((am_hal_pwrctrl_periph_e)
                                                (AM_HAL_PWRCTRL_PERIPH_UART0 +
                                                 ui32Module));

    //
    // Decode the requested power state and update UART operation accordingly.
    //
    switch (ePowerState)
    {
        //
        // Turn on the UART.
        //
        case AM_HAL_SYSCTRL_WAKE:
            //
            // Make sure we don't try to restore an invalid state.
            //
            if (bRetainState && !pState->sRegState.bValid)
            {
                return AM_HAL_STATUS_INVALID_OPERATION;
            }
            if ( pState->ui32BaudRate > 1500000 )
            {
                // Resume D2ASPARE, force uart_gate.clken to be 1, making uart.pclk to be always-on
                MCUCTRL->D2ASPARE |= (MCUCTRL_D2ASPARE_UART0PLL << ui32Module);
            }
            //
            // Enable power control.
            //
            am_hal_pwrctrl_periph_enable(eUARTPowerModule);

            //
            // Enable APB UART clock.
            //
            am_hal_uart_crm_apbclk_enable(ui32Module, true);

            if (bRetainState)
            {
                //
                // Enable uart clock.
                //
                am_hal_uart_clock_enable(ui32Module, true, pState->eClkSrc, pState->eClkDiv);

                //
                // Restore UART registers
                //
                // AM_CRITICAL_BEGIN
                UARTn(ui32Module)->ILPR = pState->sRegState.regILPR;
                UARTn(ui32Module)->IBRD = pState->sRegState.regIBRD;
                UARTn(ui32Module)->FBRD = pState->sRegState.regFBRD;
                UARTn(ui32Module)->LCRH = pState->sRegState.regLCRH;
                UARTn(ui32Module)->CR   = pState->sRegState.regCR;
                UARTn(ui32Module)->IFLS = pState->sRegState.regIFLS;
                UARTn(ui32Module)->IER  = pState->sRegState.regIER;
                UARTn(ui32Module)->DCR  = pState->sRegState.regDMACFG;
                pState->sRegState.bValid = false;

                // AM_CRITICAL_END
            }
            break;

        //
        // Turn off the UART.
        //
        case AM_HAL_SYSCTRL_NORMALSLEEP:
        case AM_HAL_SYSCTRL_DEEPSLEEP:
            if (bRetainState)
            {
                // AM_CRITICAL_BEGIN

                pState->sRegState.regILPR = UARTn(ui32Module)->ILPR;
                pState->sRegState.regIBRD = UARTn(ui32Module)->IBRD;
                pState->sRegState.regFBRD = UARTn(ui32Module)->FBRD;
                pState->sRegState.regLCRH = UARTn(ui32Module)->LCRH;
                pState->sRegState.regCR   = UARTn(ui32Module)->CR;
                pState->sRegState.regIFLS = UARTn(ui32Module)->IFLS;
                pState->sRegState.regIER  = UARTn(ui32Module)->IER;
                pState->sRegState.regDMACFG = UARTn(ui32Module)->DCR;
                pState->sRegState.bValid = true;

                // AM_CRITICAL_END
            }
            if ( pState->ui32BaudRate > 1500000 )
            {
                // Clear D2ASPARE to save power
                MCUCTRL->D2ASPARE &= ~(MCUCTRL_D2ASPARE_UART0PLL << ui32Module);
            }
            //
            // Disable uart clock.
            //
            am_hal_uart_clock_enable(ui32Module, false, pState->eClkSrc, pState->eClkDiv);

            //
            // Clear all interrupts before sleeping as having a pending UART
            // interrupt burns power.
            //
            am_hal_uart_interrupt_clear(pState, 0xFFFFFFFF);

            //
            // If the user is going to sleep, certain bits of the CR register
            // need to be 0 to be low power and have the UART shut off.
            // Since the user either wishes to retain state which takes place
            // above or the user does not wish to retain state, it is acceptable
            // to set the entire CR register to 0.
            //
            UARTn(ui32Module)->CR = 0;

            //
            // Disable APB UART clock.
            //
            am_hal_uart_crm_apbclk_enable(ui32Module, false);

            //
            // Disable power control.
            //
            am_hal_pwrctrl_periph_disable(eUARTPowerModule);
            break;

        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;
} // am_hal_uart_power_control()

//*****************************************************************************
//
//! @brief UART DMA configuration function
//!
//! @param pHandle    - handle for the module instance.
//! @param pDMAConfig - pointer to the configuration structure.
//!
//! This function configures the UART DMA for operation.
//!
//! @return status    - generic or interface specific status.
//
//*****************************************************************************
void
am_hal_uart_dma_configure(void *pHandle, am_hal_uart_transfer_t *pTransferCfg)
{
    am_hal_uart_state_t* pUARTState = (am_hal_uart_state_t *)pHandle;
    uint32_t ui32Module = pUARTState->ui32Module;

    //
    // Clear the interrupts
    //
    UARTn(ui32Module)->IEC |= UART0_IEC_DMACPIC_Msk | UART0_IEC_DMAEIC_Msk;

    //
    // Configure the DMA static priority.
    //
    UARTn(ui32Module)->DCR_b.DMAPRI = pTransferCfg->ui8Priority;

    //
    // Set the DMA transfer count.
    //
    UARTn(ui32Module)->COUNT_b.TOTCOUNT = pTransferCfg->ui32NumBytes;

    //
    // Set the DMA target address.
    //
    if ( pTransferCfg->eDirection == AM_HAL_UART_TX )
    {
        UARTn(ui32Module)->TARGADDR = (uint32_t)pTransferCfg->pui32TxBuffer;
    }
    else
    {
        UARTn(ui32Module)->TARGADDR = (uint32_t)pTransferCfg->pui32RxBuffer;
    }
}

//*****************************************************************************
//
// UART DMA nonblocking transfer function
//
//*****************************************************************************
void
am_hal_uart_dma_transfer_start(void *pHandle,   am_hal_uart_transfer_t *pTransferCfg)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    __DMB();

    //
    // Enable the DMA.
    //
    if (pTransferCfg->eDirection == AM_HAL_UART_TX)
    {
        UARTn(ui32Module)->DCR_b.TXDMAE = 1;
    }
    else
    {
        UARTn(ui32Module)->DCR_b.RXDMAE = 1;
    }
}

//*****************************************************************************
//
// am_hal_uart_dma_transfer_complete
//
//*****************************************************************************
void
am_hal_uart_dma_transfer_complete(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // Once completed, software must first write the DCR register to 0,
    // prior to making any update
    //
    UARTn(ui32Module)->DCR = 0x0;
    //
    // Clear dma status.
    //
    UARTn(ui32Module)->RSR_b.DMACPL = 0x0;
    UARTn(ui32Module)->RSR_b.DMAERR = 0x0;
}

//*****************************************************************************
//
//! @brief Validate an UART transaction.
//!
//! @param pUARTState     - pointer to the UART internal state.
//! @param psTransaction  - pointer to UART transaction.
//!
//! This function validates.
//!
//! @return HAL status of the operation.
//
//*****************************************************************************
uint32_t
am_hal_uart_validate_transaction(am_hal_uart_transfer_t *psTransaction)
{
    uint32_t ui32Bytes = psTransaction->ui32NumBytes;
    uint32_t ui32Dir = psTransaction->eDirection;

    if ((ui32Bytes && (ui32Dir != AM_HAL_UART_TX) && (psTransaction->pui32RxBuffer == NULL)) ||
         (ui32Bytes && (ui32Dir != AM_HAL_UART_RX) && (psTransaction->pui32TxBuffer == NULL)))
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// UART non-blocking transfer function
//
//*****************************************************************************
uint32_t
am_hal_uart_dma_transfer(void *pHandle,
                         am_hal_uart_transfer_t *psTransaction)
{
    am_hal_uart_state_t *pUARTState = (am_hal_uart_state_t*)pHandle;
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( !AM_HAL_UART_CHK_HANDLE(pHandle) )
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }

    if ( !psTransaction )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Validate parameters
    //
    ui32Status = am_hal_uart_validate_transaction(psTransaction);

    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        return ui32Status;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    if ( pUARTState->bDMABusy )
    {
        return AM_HAL_STATUS_IN_USE;
    }

    pUARTState->Transaction = *psTransaction;

    am_hal_uart_dma_configure(pHandle, psTransaction);

    am_hal_uart_dma_transfer_start(pHandle, psTransaction);

    pUARTState->bDMABusy = true;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
//! @brief UART DMA full-duplex transfer function
//!
//! @param pTXHandle    - handle for the uart tx instance.
//! @param pRXHandle    - handle for the uart rx instance.
//! @param pTransferCfg - pointer to the configuration structure.
//!
//! This function calls the UART DMA for full-duplex operations.
//!
//! @return status    - generic or interface specific status.
//
//*****************************************************************************
uint32_t
am_hal_uart_dma_fullduplex_transfer(void *pTXHandle, void *pRXHandle,
                                    am_hal_uart_transfer_t *psTransaction)
{
    am_hal_uart_state_t *pUARTTXState = (am_hal_uart_state_t*)pTXHandle;
    am_hal_uart_state_t *pUARTRXState = (am_hal_uart_state_t*)pRXHandle;
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32TxModule = pUARTTXState->ui32Module;
    uint32_t ui32RxModule = pUARTRXState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( (!AM_HAL_UART_CHK_HANDLE(pTXHandle)) || (!AM_HAL_UART_CHK_HANDLE(pRXHandle)) )
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }

    if ( !psTransaction )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    //
    // Validate parameters
    //
    ui32Status = am_hal_uart_validate_transaction(psTransaction);

    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        return ui32Status;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    if ( (pUARTTXState->bDMABusy) || (pUARTRXState->bDMABusy) )
    {
        return AM_HAL_STATUS_IN_USE;
    }

    pUARTTXState->Transaction = *psTransaction;
    pUARTTXState->Transaction.eDirection = AM_HAL_UART_TX;
    am_hal_uart_dma_configure(pTXHandle, &pUARTTXState->Transaction);

    pUARTRXState->Transaction.eDirection = AM_HAL_UART_RX;
    pUARTRXState->Transaction.pui32RxBuffer = psTransaction->pui32RxBuffer;
    pUARTRXState->Transaction.ui8Priority = psTransaction->ui8Priority;
    pUARTRXState->Transaction.pfnCallback = psTransaction->pfnFdRxCallback;
    pUARTRXState->Transaction.pvContext = psTransaction->pvFdRxContext;
    pUARTRXState->Transaction.ui32NumBytes = psTransaction->ui32FdRxNumBytes;
    pUARTRXState->Transaction.pui32BytesTransferred = psTransaction->pui32FdRxBytesTransferred;
    am_hal_uart_dma_configure(pRXHandle, &pUARTRXState->Transaction);
    pUARTTXState->bDMABusy = true;
    pUARTRXState->bDMABusy = true;
    UARTn(ui32TxModule)->DCR_b.TXDMAE = 1;
    UARTn(ui32RxModule)->DCR_b.RXDMAE = 1;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// am_hal_uart_dma_abort
//
//*****************************************************************************
void
am_hal_uart_dma_abort(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    //
    // If an error condition did occur during a DMA operation, the DMA must first be disabled
    //
    UARTn(ui32Module)->DCR = 0x0;
    //
    // DMA status bits cleared.
    //
    UARTn(ui32Module)->RSR_b.DMAERR = 0x0;
    //
    // DMA count cleared.
    //
    UARTn(ui32Module)->COUNT_b.TOTCOUNT = 0x0;
}

//*****************************************************************************
//
// Error handling.
//
//*****************************************************************************
uint32_t
internal_uart_get_int_err(uint32_t ui32Module, uint32_t ui32IntStatus)
{
    //
    // Map the INTSTAT bits for transaction status
    //
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    //
    // Let's accumulate the errors
    //
    ui32IntStatus |= UARTn(ui32Module)->IES;

    if (ui32IntStatus & AM_HAL_UART_INT_RX_TMOUT)
    {
        ui32Status = AM_HAL_UART_STATUS_RX_TIMEOUT;
    }
    else if (ui32IntStatus & AM_HAL_UART_INT_FRAME_ERR)
    {
        ui32Status = AM_HAL_UART_STATUS_FRAME_ERROR;
    }
    else if (ui32IntStatus & AM_HAL_UART_INT_PARITY_ERR)
    {
        ui32Status = AM_HAL_UART_STATUS_PARITY_ERROR;
    }
    else if (ui32IntStatus & AM_HAL_UART_INT_BREAK_ERR)
    {
        ui32Status = AM_HAL_UART_STATUS_BREAK_ERROR;
    }
    else if (ui32IntStatus & AM_HAL_UART_INT_OVER_RUN)
    {
        ui32Status = AM_HAL_UART_STATUS_OVERRUN_ERROR;
    }
    else if (ui32IntStatus & AM_HAL_UART_INT_DMAERIS)
    {
        ui32Status = AM_HAL_UART_STATUS_DMA_ERROR;
    }

    return ui32Status;
} // internal_uart_get_int_err()

//*****************************************************************************
//
// Allows the UART HAL to use extra space to store TX and RX data.
//
//*****************************************************************************
uint32_t
am_hal_uart_buffer_configure(void *pHandle, uint8_t *pui8TxBuffer,
                             uint32_t ui32TxBufferSize,
                             uint8_t *pui8RxBuffer, uint32_t ui32RxBufferSize)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    pState->bEnableTxQueue = false;
    pState->bEnableRxQueue = false;
    //
    // Check to see if we have a TX buffer.
    //
    if (pui8TxBuffer && ui32TxBufferSize)
    {
        //
        // If so, initialize the transmit queue, and enable the TX FIFO
        // interrupt.
        //
        pState->bEnableTxQueue = true;
        am_hal_queue_init(&pState->sTxQueue, pui8TxBuffer, 1, ui32TxBufferSize);
    }

    //
    // Check to see if we have an RX buffer.
    //
    if (pui8RxBuffer && ui32RxBufferSize)
    {
        //
        // If so, initialize the receive queue and the associated interrupts.
        //
        pState->bEnableRxQueue = true;
        am_hal_queue_init(&pState->sRxQueue, pui8RxBuffer, 1, ui32RxBufferSize);
    }

    return AM_HAL_STATUS_SUCCESS;
} // buffer_configure()

//*****************************************************************************
//
// Set Baud Rate based on the UART clock frequency.
//
//*****************************************************************************
#define BAUDCLK     (16) // Number of UART clocks needed per bit.
static uint32_t
config_baudrate(uint32_t ui32Module, uint32_t ui32UartClkFreq, uint32_t ui32DesiredBaudrate, uint32_t *pui32ActualBaud)
{
    uint64_t ui64FractionDivisorLong;
    uint64_t ui64IntermediateLong;
    uint32_t ui32IntegerDivisor;
    uint32_t ui32FractionDivisor;
    uint32_t ui32BaudClk;

    if (ui32UartClkFreq == 0)
    {
        return AM_HAL_UART_STATUS_CLOCK_NOT_CONFIGURED;
    }

    //
    // Calculate register values.
    //
    ui32BaudClk = BAUDCLK * ui32DesiredBaudrate;
    ui32IntegerDivisor = (uint32_t)(ui32UartClkFreq / ui32BaudClk);
    ui64IntermediateLong = (ui32UartClkFreq * 64) / ui32BaudClk; // Q58.6
    ui64FractionDivisorLong = ui64IntermediateLong - (ui32IntegerDivisor * 64); // Q58.6
    ui32FractionDivisor = (uint32_t)ui64FractionDivisorLong;

    //
    // Check the result.
    //
    if (ui32IntegerDivisor == 0)
    {
        *pui32ActualBaud = 0;
        return AM_HAL_UART_STATUS_BAUDRATE_NOT_POSSIBLE;
    }

    //
    // Write the UART regs.
    //
    UARTn(ui32Module)->IBRD = ui32IntegerDivisor;
    UARTn(ui32Module)->FBRD = ui32FractionDivisor;

    //
    // Return the actual baud rate.
    //
    *pui32ActualBaud = (ui32UartClkFreq / ((BAUDCLK * ui32IntegerDivisor) + ui32FractionDivisor / 4));
    return AM_HAL_STATUS_SUCCESS;
} // config_baudrate()

//*****************************************************************************
//
// Save a transaction to the UART state.
//
//*****************************************************************************
uint32_t
write_transaction_save(am_hal_uart_state_t *psState,
                       const am_hal_uart_transfer_t *psTransfer)
{
    uint32_t ui32Error = AM_HAL_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN

    if (psState->bCurrentlyWriting == false)
    {
        psState->bCurrentlyWriting = true;
        psState->sActiveWrite.eType = psTransfer->eType;
        psState->sActiveWrite.pui8Data = psTransfer->pui8Data;
        psState->sActiveWrite.ui32NumBytes = psTransfer->ui32NumBytes;
        psState->sActiveWrite.pui32BytesTransferred = psTransfer->pui32BytesTransferred;
        psState->sActiveWrite.ui32TimeoutMs = psTransfer->ui32TimeoutMs;
        psState->sActiveWrite.pfnCallback = psTransfer->pfnCallback;
        psState->sActiveWrite.pvContext = psTransfer->pvContext;
        psState->sActiveWrite.ui32ErrorStatus = psTransfer->ui32ErrorStatus;
        psState->ui32BytesWritten = 0;
        psState->bLastTxComplete = false;
    }
    else
    {
        ui32Error = AM_HAL_UART_STATUS_TX_CHANNEL_BUSY;
    }

    AM_CRITICAL_END

    return ui32Error;
}

//*****************************************************************************
//
// Abort a transaction to the UART state.
//
//*****************************************************************************
uint32_t
write_transaction_abort(void *pHandle)
{
    am_hal_uart_state_t *pState = pHandle;
    uint32_t ui32Error = AM_HAL_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN

    if (pState->bCurrentlyWriting == true)
    {
        pState->bCurrentlyWriting = false;
        memset(&pState->sActiveWrite, 0, sizeof(am_hal_uart_transfer_t));
        pState->ui32BytesWritten = 0;
        pState->bLastTxComplete = false;
    }
    else
    {
        ui32Error = AM_HAL_STATUS_INVALID_OPERATION;
    }

    AM_CRITICAL_END

    return ui32Error;
}

//*****************************************************************************
//
// Abort active uart tx transaction.
//
//*****************************************************************************
void
am_hal_uart_tx_abort(void *pHandle)
{
    am_hal_uart_state_t *pState = pHandle;
    uint32_t ui32Module = ((am_hal_uart_state_t*)pHandle)->ui32Module;
    bool bCTSEN = UARTn(ui32Module)->CR_b.CTSEN;

    // Disable CTS to allow software to flush FIFO
    if ( bCTSEN )
    {
        UARTn(ui32Module)->CR_b.CTSEN = 0;
    }

    if ( pState->bDMABusy )
    {
        am_hal_uart_dma_abort(pHandle);
    }

    am_hal_uart_tx_flush(pHandle);

    write_transaction_abort(pHandle);

    if ( bCTSEN )
    {
        UARTn(ui32Module)->CR_b.CTSEN = 1;
    }
}

//*****************************************************************************
//
// Save a transaction to the UART state.
//
//*****************************************************************************
uint32_t
read_transaction_save(am_hal_uart_state_t *psState,
                      const am_hal_uart_transfer_t *psTransfer)
{
    uint32_t ui32Error = AM_HAL_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN

    if (psState->bCurrentlyReading == false)
    {
        psState->bCurrentlyReading = true;
        psState->sActiveRead.eType = psTransfer->eType;
        psState->sActiveRead.pui8Data = psTransfer->pui8Data;
        psState->sActiveRead.ui32NumBytes = psTransfer->ui32NumBytes;
        psState->sActiveRead.pui32BytesTransferred = psTransfer->pui32BytesTransferred;
        psState->sActiveRead.ui32TimeoutMs = psTransfer->ui32TimeoutMs;
        psState->sActiveRead.pfnCallback = psTransfer->pfnCallback;
        psState->sActiveRead.pvContext = psTransfer->pvContext;
        psState->sActiveRead.ui32ErrorStatus = psTransfer->ui32ErrorStatus;
        psState->ui32BytesRead = 0;
    }
    else
    {
        ui32Error = AM_HAL_UART_STATUS_RX_CHANNEL_BUSY;
    }

    AM_CRITICAL_END

    return ui32Error;
}

//*****************************************************************************
//
// Abort a transaction to the UART state.
//
//*****************************************************************************
uint32_t
read_transaction_abort(am_hal_uart_state_t *psState)
{
    uint32_t ui32Error = AM_HAL_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN

    if (psState->bCurrentlyReading == true)
    {
        psState->bCurrentlyReading = false;
        memset(&psState->sActiveRead, 0, sizeof(am_hal_uart_transfer_t));
        psState->ui32BytesRead = 0;
    }
    else
    {
        ui32Error = AM_HAL_STATUS_INVALID_OPERATION;
    }

    AM_CRITICAL_END

    return ui32Error;
}
//*****************************************************************************
//
// Abort active uart rx transaction
//
//*****************************************************************************
void
am_hal_uart_rx_abort(void *pHandle)
{
    am_hal_uart_state_t *pState = pHandle;
    uint32_t ui32Module = ((am_hal_uart_state_t*)pHandle)->ui32Module;
    bool bRTSEN = UARTn(ui32Module)->CR_b.RTSEN;

    // Override RTS
    if ( bRTSEN )
    {
        UARTn(ui32Module)->CR_b.RTSEN = 0;
        UARTn(ui32Module)->CR_b.RTS = 0;
    }
    // Disable RX
    UARTn(ui32Module)->CR_b.RXE = 0;
    if ( UARTn(ui32Module)->FR_b.BUSY )
    {
        ONE_BYTE_DELAY(pState);
    }
    // Clear FIFO and DMA count
    if ( pState->bDMABusy )
    {
        am_hal_uart_dma_abort(pHandle);
    }
    am_hal_uart_rx_fifo_drain(pHandle);
    read_transaction_abort(pHandle);
    if ( bRTSEN )
    {
        UARTn(ui32Module)->CR_b.RTSEN = 1;
    }
    UARTn(ui32Module)->CR_b.RXE = 1;
}

//*****************************************************************************
//
// Enable/disable the UART APB Clock
//
//*****************************************************************************
static uint32_t
am_hal_uart_crm_apbclk_enable(uint32_t ui32Module, bool bEnable)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    if ( ui32Module == 0 )
    {
        ui32Status = am_hal_crm_control_UART0_CLOCK_SET(bEnable);
    }
    else
    {
        ui32Status = am_hal_crm_control_UART1_CLOCK_SET(bEnable);
    }
    return ui32Status;
}

//*****************************************************************************
//
// Configure and enable UARTHF clock.
//
//*****************************************************************************
static inline uint32_t
am_hal_uart_crm_hfclk_enable(uint32_t ui32Module, am_hal_uart_clock_src_e eClkSrc, am_hal_uart_clock_div_e eClkDiv)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    if (ui32Module == 0)
    {
        ui32Status = am_hal_crm_clock_config_UART0HF((am_hal_crm_uart0hf_clksel_e)eClkSrc, eClkDiv);
        if ( ui32Status != AM_HAL_STATUS_SUCCESS )
        {
            return ui32Status;
        }
        ui32Status = am_hal_crm_control_UART0HF_CLOCK_SET(true);
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }
    }
    else
    {
        ui32Status = am_hal_crm_clock_config_UART1HF((am_hal_crm_uart1hf_clksel_e)eClkSrc, eClkDiv);
        if ( ui32Status != AM_HAL_STATUS_SUCCESS )
        {
            return ui32Status;
        }
        ui32Status = am_hal_crm_control_UART1HF_CLOCK_SET(true);
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }
    }

     return ui32Status;
}

//*****************************************************************************
//
// Disable UART HF clock
//
//*****************************************************************************
static inline uint32_t
am_hal_uart_crm_hfclk_disable(uint32_t ui32Module)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;

    if (ui32Module == 0)
    {
        ui32Status = am_hal_crm_control_UART0HF_CLOCK_SET(false);
    }
    else
    {
        ui32Status = am_hal_crm_control_UART1HF_CLOCK_SET(false);
    }

     return ui32Status;
}

//*****************************************************************************
//
// USB clock control functions
//
// This function disables the high-frequency clock for the specified UART module.
// It is used to save power when the UART is not in use.
//
//*****************************************************************************
static uint32_t
am_hal_uart_clock_enable(uint32_t ui32Module, bool bEnable, am_hal_uart_clock_src_e eClkSrc, am_hal_uart_clock_div_e eClkDiv)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    am_hal_clkmgr_clock_id_e eUartRequestClk = AM_HAL_CLKMGR_CLK_ID_MAX;

    eUartRequestClk = eClkSrc == AM_HAL_UART_HFCLK_SRC_HFRC_96M ? AM_HAL_CLKMGR_CLK_ID_HFRC : AM_HAL_CLKMGR_CLK_ID_PLLPOSTDIV;

    if ( bEnable )
    {
        if (eUartRequestClk == AM_HAL_CLKMGR_CLK_ID_PLLPOSTDIV)
        {
            uint32_t ui32PLLPOSTDIVFreq = 0;

            //
            // Get the PLLPOSTDIV frequency.
            //
            ui32Status = am_hal_clkmgr_clock_config_get(AM_HAL_CLKMGR_CLK_ID_PLLPOSTDIV, &ui32PLLPOSTDIVFreq, NULL);
            if (ui32Status != AM_HAL_STATUS_SUCCESS)
            {
                return ui32Status;
            }
            //
            // Check if current PLLPOSTDIV frequency matches the expected frequency.
            //
            if (ui32PLLPOSTDIVFreq != AM_HAL_UART_PLLCLK_FREQ)
            {
                return AM_HAL_STATUS_FAIL;
            }
        }

        //
        // Request destination clock from clock manager
        //
        ui32Status = am_hal_clkmgr_clock_request(eUartRequestClk, (am_hal_clkmgr_user_id_e)(AM_HAL_CLKMGR_USER_ID_UART0 + ui32Module));
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }

        ui32Status = am_hal_uart_crm_hfclk_enable(ui32Module, eClkSrc, eClkDiv);
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }
    }
    else
    {
        ui32Status = am_hal_uart_crm_hfclk_disable(ui32Module);
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }
        //
        // Release requested clock.
        //
        ui32Status = am_hal_clkmgr_clock_release(eUartRequestClk, (am_hal_clkmgr_user_id_e)(AM_HAL_CLKMGR_USER_ID_UART0 + ui32Module));
        if (ui32Status != AM_HAL_STATUS_SUCCESS)
        {
            return ui32Status;
        }
    }

    return ui32Status;
}

//*****************************************************************************
//
// Used to configure basic UART settings.
//
//*****************************************************************************
uint32_t
am_hal_uart_configure(void *pHandle, const am_hal_uart_config_t *psConfig)
{

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;
    uint32_t ui32Status;
    uint32_t ui32UartClockSourceFrequency;
    uint32_t ui32UartClockFrequency;
    uint32_t ui32UartActualBaudRate;
    am_hal_uart_clock_div_e clockDivider;
    am_hal_uart_clock_src_e clockSource;

    //
    // Set the clock source.
    //
    clockSource = psConfig->eClockSrc;

    //
    //  Set the clock source and divider
    //
    switch (clockSource)
    {
        case AM_HAL_UART_HFCLK_SRC_HFRC_96M:
            if (psConfig->ui32BaudRate > 1500000)
            {
                //
                // set clock to frequency 48Mhz.
                //
                clockDivider = AM_HAL_UART_HFCLK_DIV2;
            }
            else
            {
                //
                // set clock to frequency 24Mhz.
                //
                clockDivider = AM_HAL_UART_HFCLK_DIV4;
            }
            ui32UartClockSourceFrequency = AM_HAL_UART_HFRCCLK_FREQ;
            break;
        case AM_HAL_UART_HFCLK_SRC_PLLPOSTDIV:
            clockDivider = AM_HAL_UART_HFCLK_DIV1;
            ui32UartClockSourceFrequency = AM_HAL_UART_PLLCLK_FREQ;
            break;
        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    ui32Status = am_hal_uart_clock_enable(ui32Module, true, clockSource, clockDivider);
    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        return ui32Status;
    }
    //
    // calculates the uart clock frequency.
    //
    ui32UartClockFrequency = ui32UartClockSourceFrequency / (clockDivider + 1);

    //
    // Set the baud rate.
    //
    ui32Status = config_baudrate(ui32Module, ui32UartClockFrequency, psConfig->ui32BaudRate,
                                      &ui32UartActualBaudRate);

    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        return ui32Status;
    }
    //
    // Save the configuration to the handle.
    //
    pState->eClkSrc = clockSource;
    pState->eClkDiv = clockDivider;
    pState->ui32BaudRate = ui32UartActualBaudRate;

    if ( pState->ui32BaudRate > 1500000 )
    {
        // Resume D2ASPARE, force uart_gate.clken to be 1, making uart.pclk to be always-on
        MCUCTRL->D2ASPARE |= (MCUCTRL_D2ASPARE_UART0PLL << ui32Module);
    }
    else
    {
        // Disable uart_gate.clken, making uart.pclk to be gated
        MCUCTRL->D2ASPARE &= ~(MCUCTRL_D2ASPARE_UART0PLL << ui32Module);
    }
    //
    // Calculate the parity options.
    //
    uint32_t ui32ParityEnable = 0;
    uint32_t ui32EvenParity = 0;

    switch (psConfig->eParity)
    {
        case AM_HAL_UART_PARITY_ODD:
            ui32ParityEnable = 1;
            ui32EvenParity = 0;
            break;

        case AM_HAL_UART_PARITY_EVEN:
            ui32ParityEnable = 1;
            ui32EvenParity = 1;
            break;

        case AM_HAL_UART_PARITY_NONE:
            ui32ParityEnable = 0;
            ui32EvenParity = 0;
            break;
    }

    //
    // Reset the UART.
    //
    UARTn(ui32Module)->CR = 0;
    UARTn(ui32Module)->CR |= psConfig->eFlowControl;

    //
    // Set the data format.
    //
    UARTn(ui32Module)->LCRH =   _VAL2FLD(UART0_LCRH_BRK, 0)                     |
                                _VAL2FLD(UART0_LCRH_PEN, ui32ParityEnable)      |
                                _VAL2FLD(UART0_LCRH_EPS, ui32EvenParity)        |
                                _VAL2FLD(UART0_LCRH_STP2, psConfig->eStopBits)  |
                                _VAL2FLD(UART0_LCRH_FEN, 1)                     |
                                _VAL2FLD(UART0_LCRH_WLEN, psConfig->eDataBits)  |
                                _VAL2FLD(UART0_LCRH_SPS, 0) ;

    //
    // Set the FIFO levels.
    //
    UARTn(ui32Module)->IFLS =   _VAL2FLD(UART0_IFLS_TXIFLSEL, psConfig->eTXFifoLevel) |
                                _VAL2FLD(UART0_IFLS_RXIFLSEL, psConfig->eRXFifoLevel) ;
    //
    // Enable the UART, RX, and TX.
    //
    UARTn(ui32Module)->CR |= _VAL2FLD(UART0_CR_UARTEN, 1) |
                             _VAL2FLD(UART0_CR_RXE, 1)    |
                             _VAL2FLD(UART0_CR_TXE, 1);

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Read from the UART RX FIFO.
//
//*****************************************************************************
uint32_t
am_hal_uart_fifo_read(void *pHandle, uint8_t *pui8Data, uint32_t ui32NumBytes,
                      uint32_t *pui32NumBytesRead)
{
    uint32_t i = 0;
    uint32_t ui32ReadData;
    uint32_t ui32ErrorStatus = AM_HAL_STATUS_SUCCESS;

    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    //
    // Start a loop where we attempt to read everything requested.
    //
    while (i < ui32NumBytes)
    {
        //
        // If the fifo is empty, return with the number of bytes we read.
        // Otherwise, read the data into the provided buffer.
        //
        if ( UARTn(ui32Module)->FR_b.RXFE )
        {
            break;
        }
        else
        {
            ui32ReadData = UARTn(ui32Module)->DR;

            //
            // If error bits are set, we need to alert the caller.
            //
            if (ui32ReadData & (_VAL2FLD(UART0_DR_OEDATA, UART0_DR_OEDATA_ERR) |
                                _VAL2FLD(UART0_DR_BEDATA, UART0_DR_BEDATA_ERR) |
                                _VAL2FLD(UART0_DR_PEDATA, UART0_DR_PEDATA_ERR) |
                                _VAL2FLD(UART0_DR_FEDATA, UART0_DR_FEDATA_ERR)) )
            {
                ui32ErrorStatus =  AM_HAL_UART_STATUS_BUS_ERROR;
                break;
            }
            else if (pui8Data)
            {
                pui8Data[i++] = ui32ReadData & 0xFF;
            }
        }
    }

    if (pui32NumBytesRead)
    {
        *pui32NumBytesRead = i;
    }

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Write to the UART TX FIFO
//
//*****************************************************************************
uint32_t
am_hal_uart_fifo_write(void *pHandle, uint8_t *pui8Data, uint32_t ui32NumBytes,
                       uint32_t *pui32NumBytesWritten)
{
    uint32_t i = 0;

    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    //
    // Start a loop where we attempt to write everything requested.
    //
    while (i < ui32NumBytes)
    {
        //
        // If the TX FIFO is full, break out of the loop. We've sent everything
        // we can.
        //
        if ( UARTn(ui32Module)->FR_b.TXFF )
        {
            break;
        }
        else
        {
            UARTn(ui32Module)->DR = pui8Data[i++];
        }
    }

    //
    // Let the caller know how much we sent.
    //
    if (pui32NumBytesWritten)
    {
        *pui32NumBytesWritten = i;
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Drain the UART RX FIFO.
//
//*****************************************************************************
uint32_t
am_hal_uart_rx_fifo_drain(void *pHandle)
{
    return am_hal_uart_fifo_read(pHandle, NULL, AM_HAL_UART_FIFO_MAX, NULL);
}

//*****************************************************************************
//
// Empty the UART RX FIFO, and place the data into the RX queue.
//
//*****************************************************************************
static uint32_t
rx_queue_update(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

    uint8_t pui8Data[AM_HAL_UART_FIFO_MAX];
    uint32_t ui32BytesTransferred;
    uint32_t ui32ErrorStatus;

    AM_CRITICAL_BEGIN;

    //
    // Read as much of the FIFO as we can.
    //
    ui32ErrorStatus = am_hal_uart_fifo_read(pHandle, pui8Data, AM_HAL_UART_FIFO_MAX,
                                            &ui32BytesTransferred);
    //
    // If we were successful, go ahead and transfer the data along to the
    // buffer.
    //
    if (ui32ErrorStatus == AM_HAL_STATUS_SUCCESS)
    {
        if (!am_hal_queue_item_add(&pState->sRxQueue, pui8Data,
                                   ui32BytesTransferred))
        {
            ui32ErrorStatus = AM_HAL_UART_STATUS_RX_QUEUE_FULL;
        }
    }

    AM_CRITICAL_END;

    return ui32ErrorStatus;
} // rx_queue_update()

//*****************************************************************************
//
// Transfer as much data as possible from the TX queue to the TX FIFO.
//
//*****************************************************************************
static uint32_t
tx_queue_update(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    uint8_t pui8Data;
    uint32_t ui32BytesTransferred;
    uint32_t ui32ErrorStatus = AM_HAL_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN;

    //
    // Loop as long as the TX fifo isn't full yet.
    //
    while ( !UARTn(ui32Module)->FR_b.TXFF )
    {
        //
        // Attempt to grab an item from the queue, and add it to the fifo.
        //
        if (am_hal_queue_item_get(&pState->sTxQueue, &pui8Data, 1))
        {
            ui32ErrorStatus = am_hal_uart_fifo_write(pHandle, &pui8Data, 1,
                                                     &ui32BytesTransferred);

            if (ui32ErrorStatus != AM_HAL_STATUS_SUCCESS)
            {
                break;
            }
        }
        else
        {
            //
            // If we didn't get anything from the FIFO, we can just return.
            //
            break;
        }
    }

    AM_CRITICAL_END;

    return ui32ErrorStatus;
} // tx_queue_update()

//*****************************************************************************
//
// Run a UART transaction.
//
//*****************************************************************************
uint32_t
am_hal_uart_transfer(void *pHandle,
                     const am_hal_uart_transfer_t *psTransfer)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    switch (psTransfer->eType)
    {
        case AM_HAL_UART_BLOCKING_WRITE:
            return blocking_write(pHandle, psTransfer);

        case AM_HAL_UART_BLOCKING_READ:
            return blocking_read(pHandle, psTransfer);

        case AM_HAL_UART_NONBLOCKING_WRITE:
            return nonblocking_write(pHandle, psTransfer);

        case AM_HAL_UART_NONBLOCKING_READ:
            return nonblocking_read(pHandle, psTransfer);

        default:
            return AM_HAL_STATUS_FAIL;
    }
}

//*****************************************************************************
//
// Blocking UART write.
//
//
//         (start blocking_write)
//        ___________|_____________
//       / Check for timeout?      \     YES
//   ->-/(psTransfer->ui32TimeoutMs)\_________________________________________
//  |   \ and check if we are in    /                                         |
//  |    \ writing state           /                                          |
//  |     \_______________________/                                           |
//  ^                |  No                                                    |
//  |     ___________V____________                                            |
//  |    / Is the software Queue  \     YES                                   |
//  |   / (pHandle->bEnableTxQueue)\______________________                    |
//  |   \  active?                 /                      |                   V
//  |    \________________________/     __________________V________________   |
//  |                |  No             | Write as much as possible to      |  |
//  ^    ____________V___________      | software queue(pHandle->sTxQueue).|  |
//  |   | Write up to 32 samples |     |___________________________________|  |
//  |   | to hardware queue.     |                        |                   |
//  |   |________________________|      __________________V_________________  |
//  |                |                 | Read from software queue and write | V
//  |                |                 | up to 32 samples to hardware queue.| |
//  |                |                 | ('tx_queue_update()' function).    | |
//  |                |                 |____________________________________| |
//  ^                |                                    |                   |
//  |             ___V____________________________________V____               |
//  |            / Keep updating the number of bytes that has  \              |
//  |           / been transferred to the ActiveWrite structure \             V
//  |           \ variable                                      /             |
//  |            \_____________________________________________/              |
//  |                |                                    |                   |
//  |             ___V____________________________________V_____              |
//  |            / Number of bytes transferred till now is equal\    YES      |
//  |           / to psTransfer->ui32NumBytes?                   \_____       V
//  |           \                                                /    |       |
//  |            \______________________________________________/     V       |
//  |________________|  No                                            |       |
//                                ____________________________________|_______|_
//                               | Return number of bytes transferred to queues |
//                               | (psTransfer->pui32BytesTransferred).         |
//                               |______________________________________________|
//                                                      |
//                                             (end blocking_write)
//
// Note: When returning from blocking_write, the Tx data is still in UART queues
//   (software or hardware queues).
//*****************************************************************************
uint32_t
blocking_write(void *pHandle, const am_hal_uart_transfer_t *psTransfer)
{
    uint32_t ui32Status;
    uint32_t ui32TimeSpent = 0;
    am_hal_uart_state_t *psState = pHandle;

    //
    // Blocking write has all of the same requirements as non-blocking write,
    // but we can't guarantee that the interrupt handlers will be called, so we
    // have to call them in a loop instead.
    //
    ui32Status = nonblocking_write(pHandle, psTransfer);
    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        return ui32Status;
    }

    while (psState->bCurrentlyWriting)
    {
        nonblocking_write_sm(pHandle);
        am_hal_delay_us(1000);

        if ( psTransfer->ui32TimeoutMs != AM_HAL_UART_WAIT_FOREVER )
        {
            ++ui32TimeSpent;
            if ( ui32TimeSpent == psTransfer->ui32TimeoutMs )
            {
                psState->bCurrentlyWriting = false;
                return AM_HAL_STATUS_TIMEOUT;
            }
        }
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Blocking UART read.
//
//*****************************************************************************
uint32_t
blocking_read(void *pHandle, const am_hal_uart_transfer_t *psTransfer)
{
    uint32_t ui32Status;
    uint32_t ui32TimeSpent = 0;
    am_hal_uart_state_t *psState = pHandle;

    //
    // Blocking read has all of the same requirements as non-blocking read,
    // but we can't guarantee that the interrupt handlers will be called, so we
    // have to call them in a loop instead.
    //
    ui32Status = nonblocking_read(pHandle, psTransfer);
    if (ui32Status != AM_HAL_STATUS_SUCCESS)
    {
        return ui32Status;
    }

    //
    // Blocking read continue until these two conditions are met:
    // 1) The user defined Timeout occurs, in which case the HAL returns with a timeout error status
    // 2) All the received data in FIFO has been read
    //
    while (psState->bCurrentlyReading)
    {
        nonblocking_read_sm(pHandle);
        am_hal_delay_us(1000);

        if ( psTransfer->ui32TimeoutMs != AM_HAL_UART_WAIT_FOREVER )
        {
            ++ui32TimeSpent;
            if ( ui32TimeSpent == psTransfer->ui32TimeoutMs )
            {
                psState->bCurrentlyReading = false;
                return AM_HAL_STATUS_TIMEOUT;
            }
        }
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Non-blocking UART write.
//
//         (start nonblocking_write)
//                      |
//           ________________________
//          / Is the software Queue  \     YES
//         / (pHandle->bEnableTxQueue)\______________________
//         \  active?                 /                      |
//          \________________________/     __________________V________________
//                      |  No             | Write as much as possible to      |
//          ____________V___________      | software queue(pHandle->sTxQueue).|
//         | Write up to 32 samples |     |___________________________________|
//         | to hardware queue.     |                        |
//         |________________________|      __________________V_________________
//                      |                 | Read from software queue and write |
//                      |                 | up to 32 samples to hardware queue |
//                      |                 | ('tx_queue_update()' function).    |
//                      |                 |____________________________________|
//                      |                                    |
//                   ___V____________________________________V_____
//                  | Return number of bytes transferred to queues |
//                  | (psTransfer->pui32BytesTransferred).         |
//                  |______________________________________________|
//                                          |
//                               (end nonblocking_write)
//
// Note: The transmit data is always written to the software queue first
// because old data may be already in the software queue.
//*****************************************************************************
uint32_t
nonblocking_write(void *pHandle, const am_hal_uart_transfer_t *psTransfer)
{
    uint32_t ui32ErrorStatus;
    uint32_t *pui32NumBytesWritten = psTransfer->pui32BytesTransferred;

    //
    // Initialize the variable tracking how many bytes we've written.
    //
    if (pui32NumBytesWritten)
    {
        *pui32NumBytesWritten = 0;
    }

    //
    // Save the transaction, and run the state machine to transfer the data.
    //
    ui32ErrorStatus = write_transaction_save(pHandle, psTransfer);
    if (ui32ErrorStatus != AM_HAL_STATUS_SUCCESS)
    {
        return ui32ErrorStatus;
    }
    else
    {
        nonblocking_write_sm(pHandle);
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Non-blocking UART read.
//
//*****************************************************************************
uint32_t
nonblocking_read(void *pHandle, const am_hal_uart_transfer_t *psTransfer)
{
    uint32_t ui32ErrorStatus = AM_HAL_STATUS_SUCCESS;
    uint32_t *pui32NumBytesRead = psTransfer->pui32BytesTransferred;

    //
    // Start by setting the number of bytes read to 0.
    //
    if (pui32NumBytesRead)
    {
        *pui32NumBytesRead = 0;
    }

    //
    // Save the read transaction and run the state machine to transfer the data.
    //
    ui32ErrorStatus = read_transaction_save(pHandle, psTransfer);
    if (ui32ErrorStatus != AM_HAL_STATUS_SUCCESS)
    {
        return ui32ErrorStatus;
    }
    else
    {
        nonblocking_read_sm(pHandle);
    }

    return ui32ErrorStatus;
}

//*****************************************************************************
//
// Non-blocking UART write state machine.
//
// This function takes data from the user-supplied buffer and pushes it through
// the UART transmit fifo. If a TX write queue has been enabled, the data will
// go through the queue first. If the transfer is too big to complete all at
// once, this function will save its place in the UART state variable and
// return.
//
// This function will also clean up the UART module state and call the
// appropriate callback if the active transaction is completed.
//
//*****************************************************************************
void
nonblocking_write_sm(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

    //
    // Then, check to see if we currently have an active "write" transaction. If
    // we do, we need to pull data out of the user's buffer and either push them
    // into our internal buffer (if TX buffers were enabled) or push them to the
    // hardware fifo.
    //
    if (pState->bCurrentlyWriting)
    {
        //
        // Start a critical section to avoid problems associated with this
        // function being called from multiple places concurrently.
        //
        //
        // When the callback is called, the function must return immediately after
        // calling the callback. It can't return until the function exits the
        // critical section.
        // This flag tracks the callback usage
        //
        bool bReturnAfterCritical = false;

        AM_CRITICAL_BEGIN

        uint32_t i = pState->ui32BytesWritten;
        uint32_t ui32BytesLeft = pState->sActiveWrite.ui32NumBytes - i;

        uint8_t *pui8Src = &(pState->sActiveWrite.pui8Data[i]);

        uint32_t ui32WriteSize;

        if (pState->bEnableTxQueue)
        {
            //
            // If we are using an internal queue, we'll push as much data as we
            // can to the queue.
            //
            uint32_t ui32QueueSpace =
                am_hal_queue_space_left(&pState->sTxQueue);

            ui32WriteSize = (ui32BytesLeft < ui32QueueSpace ?
                             ui32BytesLeft : ui32QueueSpace);

            bool bSuccess = am_hal_queue_item_add(&pState->sTxQueue,
                                                  pui8Src,
                                                  ui32WriteSize);

            //
            // If something goes wrong with this queue write, even though we've
            // checked the size of the queue, then something isn't right, and we
            // should return with an error. This might indicate thread
            // mis-management.
            //
            if (bSuccess == false)
            {
                pState->bCurrentlyWriting = false;
                if (pState->sActiveWrite.pfnCallback)
                {
                    pState->sActiveWrite.pfnCallback(1, pState->sActiveWrite.pvContext);
                    // After the callback,
                    // this function needs to return once the critical section exits with
                    // no other variables modified.
                    // Set this flag to keep track of that (early return) state.
                    bReturnAfterCritical = true;
                }
            }
        }
        else // if ( !pState->bEnableTxQueue )
        {
            //
            // If we're not using a queue, we'll just write straight to the
            // FIFO. This function will attempt to write everything we have
            // left and return the actual number of bytes written to
            // ui32WriteSize.
            //
            am_hal_uart_fifo_write(pHandle, pui8Src, ui32BytesLeft,
                                   &ui32WriteSize);
        }

        if ( !bReturnAfterCritical)
        {
            //
            // Before returning, we also need to update the saved transaction,
            // closing it if we've consumed all the bytes in the user's
            // original buffer.
            // This variable is not updated after a callback has been called above.
            //
            pState->ui32BytesWritten += ui32WriteSize;
        } // else/if ( !pState->bEnableTxQueue )

        AM_CRITICAL_END

        if (bReturnAfterCritical)
        {
            //
            // the callback was executed in the critical section above,
            // and now this function needs to exit
            //
            return;
        }

        if ( pState->sActiveWrite.pui32BytesTransferred )
        {
            *pState->sActiveWrite.pui32BytesTransferred = pState->ui32BytesWritten;
        }

        if ( (pState->ui32BytesWritten == pState->sActiveWrite.ui32NumBytes) &&
             pState->bCurrentlyWriting )
        {
            //
            // all data has been written and completion notification hasn't been sent yet
            //
            pState->bCurrentlyWriting = false;

            if (pState->sActiveWrite.pfnCallback)
            {
                pState->sActiveWrite.pfnCallback(0, pState->sActiveWrite.pvContext);
            }
        }
    }

    //
    // If we're using a TX queue, send all of the data in the TX buffer to the
    // hardware fifo.
    //
    if (pState->bEnableTxQueue)
    {
        tx_queue_update(pHandle);
    }
}

//*****************************************************************************
//
// Non-blocking UART read state machine.
//
// This function takes data from the UART fifo and writes it to the
// user-supplied buffer. If an RX queue has been enabled, it will transfer data
// from the UART FIFO to the RX queue, and then from the RX queue to the user's
// buffer.
//
// This function will also clean up the UART module state and call the
// appropriate callback if the active transaction is completed.
//
//*****************************************************************************
void
nonblocking_read_sm(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

    //
    // If we're using an RX queue, take all of the data from the RX FIFO and add
    // it to the internal queue.
    //
    if (pState->bEnableRxQueue)
    {
        rx_queue_update(pHandle);
    }

    //
    // If we have an active read, take data from the RX queue and transfer it to
    // the user-supplied destination buffer. If this completes a transaction,
    // this function will also call the user's supplied callback.
    //
    if (pState->bCurrentlyReading)
    {
        //
        // When the callback is called the function must return immediately after
        // calling the callback, but the callback is called in a critical section
        // this flag tracks the callback usage
        //

        bool bReturnAfterCritical = false;
        AM_CRITICAL_BEGIN

        uint32_t i = pState->ui32BytesRead;
        uint32_t ui32BytesLeft = pState->sActiveRead.ui32NumBytes - i;

        uint8_t *pui8Dest = &(pState->sActiveRead.pui8Data[i]);

        uint32_t ui32ReadSize = 0;

        if (pState->bEnableRxQueue)
        {
            //
            // If we're using an internal queue, read from there first.
            //
            uint32_t ui32QueueData =
                am_hal_queue_data_left(&pState->sRxQueue);

            ui32ReadSize = (ui32BytesLeft < ui32QueueData ?
                            ui32BytesLeft : ui32QueueData);

            bool bSuccess = am_hal_queue_item_get(&pState->sRxQueue,
                                             pui8Dest, ui32ReadSize);

            //
            // We checked the amount of data we had available before reading, so
            // the queue read really shouldn't fail. If it does anyway, we need
            // to raise an error and abort the read.
            //
            if (bSuccess == false)
            {
                pState->bCurrentlyReading = false;
                if (pState->sActiveRead.pfnCallback)
                {
                    pState->sActiveRead.pfnCallback(1, pState->sActiveRead.pvContext);
                    //
                    // This is in a critical section, can't return from function here
                    // first need to exit critical section, set this flag to keep track
                    // of the need to return early and do nothing else in this function
                    //
                    bReturnAfterCritical = true;
                }
            }
        }
        else
        {
            //
            // If we're not using an internal queue, we need to read directly
            // from the fifo instead. This should read as many bytes as possible
            // and update ui32ReadSize with the number of bytes we actually got.
            //
            am_hal_uart_fifo_read(pHandle, pui8Dest, ui32BytesLeft,
                                  &ui32ReadSize);

        }

        if ( !bReturnAfterCritical)
        {
            pState->ui32BytesRead += ui32ReadSize;
        }

        AM_CRITICAL_END

        if (bReturnAfterCritical)
        {
            return;
        }

        //
        // At this point, we've moved all the data we can. We need to update
        // the transaction state, and then check to see if this operation has
        // actually completed a transaction. If so, close out the transaction
        // and call the callback.
        //

        if ( pState->sActiveRead.pui32BytesTransferred )
        {
            *pState->sActiveRead.pui32BytesTransferred = pState->ui32BytesRead;
        }

        if (pState->ui32BytesRead == pState->sActiveRead.ui32NumBytes)
        {
            pState->bCurrentlyReading = false;

            if (pState->sActiveRead.pfnCallback)
            {
                pState->sActiveRead.pfnCallback(0, pState->sActiveRead.pvContext);
            }
        }
    }
}

//*****************************************************************************
//
// Wait for all of the traffic in the TX pipeline to be sent.
// This function does not block forever
// returns timeout error, if it detects a timeout
//
//*****************************************************************************
uint32_t
am_hal_uart_tx_flush(void *pHandle)
{
    uint32_t ui32RetStat = AM_HAL_STATUS_SUCCESS;
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    //
    // Ultimately need to wait for tx busy to complete
    // First, wait for tx queue to empty, (the isr or dma should be emptying it into the fifo)
    //
    am_hal_queue_t *pTxQ = &pState->sTxQueue;

    //
    // find the remaining queue size and double the max wait time,
    // allowing for ISR and other unexpected delays
    //
    int32_t i32NumLeftInQueue = (int32_t) (pTxQ->ui32Length * 2);
    if (i32NumLeftInQueue)
    {
        while (pTxQ->ui32Length)
        {
            if ( --i32NumLeftInQueue <= 0)
            {
                ui32RetStat = AM_HAL_STATUS_TIMEOUT;
                break;
            }
            ONE_BYTE_DELAY(pState);
        }
    }

    //
    // now if there is DMA running, wait for that
    //

    if (pState->bDMABusy)
    {
        //
        // pad a little extra time to allow for unexpected delays
        //
        int32_t maxDmaSize = (AM_HAL_MAX_UART_DMA_SIZE + AM_HAL_MAX_UART_DMA_SIZE / 4);
        while (pState->bDMABusy)
        {
            if ( --maxDmaSize <= 0)
            {
                ui32RetStat = AM_HAL_STATUS_TIMEOUT;
                break;
            }
            ONE_BYTE_DELAY(pState);
        }
    }


    //
    // Now wait for fifo to empty.
    // Use generous doubled max fifo size to allow for unexpected delays
    // Waiting the full time is a timeout error.
    //
    int32_t i32GenerousFifoSize = AM_HAL_UART_FIFO_MAX * 2;

    while ( UARTn(ui32Module)->FR_b.BUSY )
    {
        if ( --i32GenerousFifoSize <= 0 )
        {
            ui32RetStat = AM_HAL_STATUS_TIMEOUT;
            break;
        }
        ONE_BYTE_DELAY(pState);
    }

    return ui32RetStat;

}

//*****************************************************************************
//
// Read the UART flags.
//
//*****************************************************************************
uint32_t
am_hal_uart_flags_get(void *pHandle, uint32_t *pui32Flags)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if ( !AM_HAL_UART_CHK_HANDLE(pHandle) )
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    if ( pui32Flags )
    {
        //
        // Set the flags value, then return success.
        //
        *pui32Flags = UARTn(ui32Module)->FR;

        return AM_HAL_STATUS_SUCCESS;
    }
    else
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
}

//*****************************************************************************
//
// Enable interrupts.
//
//*****************************************************************************
uint32_t
am_hal_uart_interrupt_enable(void *pHandle, uint32_t ui32IntMask)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    UARTn(ui32Module)->IER |= ui32IntMask;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Return the set of enabled interrupts.
//
//*****************************************************************************
uint32_t
am_hal_uart_interrupt_enable_get(void *pHandle, uint32_t *pui32IntMask)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    *pui32IntMask = UARTn(ui32Module)->IER;

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_uart_interrupt_enable_get()

//*****************************************************************************
//
// Disable interrupts.
//
//*****************************************************************************
uint32_t
am_hal_uart_interrupt_disable(void *pHandle, uint32_t ui32IntMask)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    UARTn(ui32Module)->IER &= ~ui32IntMask;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Clear interrupt status.
//
//*****************************************************************************
uint32_t
am_hal_uart_interrupt_clear(void *pHandle, uint32_t ui32IntMask)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    UARTn(ui32Module)->IEC = ui32IntMask;
    *(volatile uint32_t*)(&UARTn(ui32Module)->MIS);
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Read interrupt status.
//
//*****************************************************************************
uint32_t
am_hal_uart_interrupt_status_get(void *pHandle, uint32_t *pui32Status,
                                 bool bEnabledOnly)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    //
    // If requested, only return the interrupts that are enabled.
    //
    *pui32Status = bEnabledOnly ? UARTn(ui32Module)->MIS : UARTn(ui32Module)->IES;

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Read interrupt status.
//
//*****************************************************************************
uint32_t
am_hal_uart_interrupt_service(void *pHandle, uint32_t ui32Status)
{
    am_hal_uart_state_t *pState = pHandle;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    uint32_t ui32Module = ((am_hal_uart_state_t*)pHandle)->ui32Module;
    if (pState->bDMABusy)
    {
        if ( pState->Transaction.pui32BytesTransferred  != NULL )
        {
            *(pState->Transaction.pui32BytesTransferred) = pState->Transaction.ui32NumBytes - UARTn(ui32Module)->COUNT_b.TOTCOUNT;
        }
        if (ui32Status & AM_HAL_UART_INT_RX_TMOUT)
        {
            am_hal_uart_rx_abort(pHandle);
        }
        if (ui32Status & AM_HAL_UART_INT_DMAERIS)
        {
            //
            // Clear DMAERR bit.
            //
            am_hal_uart_dma_abort(pHandle);
        }
        if (ui32Status & AM_HAL_UART_INT_DMACPRIS)
        {
            if ( pState->Transaction.pfnCallback != NULL )
            {
                pState->Transaction.pfnCallback(internal_uart_get_int_err(ui32Module, ui32Status), pState->Transaction.pvContext);
                pState->Transaction.pfnCallback = NULL;
            }
            //
            // Complete DMA transaction
            //
            am_hal_uart_dma_transfer_complete(pHandle);
        }
        pState->bDMABusy = false;
    }
    else
    {
        if ( ui32Status & (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT) )
        {
            nonblocking_read_sm(pHandle);
        }

        if ( ui32Status & (AM_HAL_UART_INT_TX) )
        {
            nonblocking_write_sm(pHandle);
        }

        if ( ui32Status & (AM_HAL_UART_INT_TXCMP) )
        {
            pState->bLastTxComplete = true;
        }
    }

    return AM_HAL_STATUS_FAIL;
}


//*****************************************************************************
//
//! @}
// End Doxygen group.
//
//*****************************************************************************
