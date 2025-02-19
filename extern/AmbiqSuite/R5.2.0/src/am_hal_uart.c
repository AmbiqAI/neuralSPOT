//*****************************************************************************
//
//! @file am_hal_uart.c
//!
//! @brief Hardware abstraction for the UART
//!
//! @addtogroup uart UART Functionality
//! @ingroup apollo5b_hal
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include <string.h>



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
        delayus = 1000000 / pState->ui32BaudRate;   /* Delay for 1 bit  */  \
        delayus += 1;                               /* Round up         */  \
        delayus *= 10;                              /* 8 bits,start/stop*/  \
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
    //! am_hal_uart_dmaQueueInit
    //! @note Since it is allocated and passed in, this usually shouldn't be on the stack
    //
    am_hal_uart_dma_tx_queue_t *psDmaQueue;

    //
    //! Most recently configured baud rate.
    //
    uint32_t ui32BaudRate;

    am_hal_uart_transfer_t sActiveRead;
    volatile uint32_t ui32BytesRead;

    am_hal_uart_transfer_t sActiveWrite;
    volatile uint32_t ui32BytesWritten;

    am_hal_queue_t sTxQueue;
    am_hal_queue_t sRxQueue;

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
    am_hal_clkmgr_clock_id_e eClkSrc;

    //
    //! UART Dma mode
    //

    am_hal_uart_async_dma_mode_e eAsyncDmaMode;

    bool bCurrentlyWriting;

    //
    //! State information about nonblocking transfers.
    //
    bool bCurrentlyReading;

    //
    //! DMA transaction in progress.
    //
    bool                bDMABusy;
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
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    am_hal_pwrctrl_periph_e eUARTPowerModule = ((am_hal_pwrctrl_periph_e)
                                                (AM_HAL_PWRCTRL_PERIPH_UART0 +
                                                 ui32Module));

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
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

            //
            // Enable power control.
            //
            am_hal_pwrctrl_periph_enable(eUARTPowerModule);

            if (bRetainState)
            {
                if ( ( pState->ui32BaudRate > 1500000 ) && ( APOLLO5_GE_B1 ) )
                {
                    // Resume D2ASPARE, force uart_gate.clken to be 1, making uart.pclk to be always-on
                    MCUCTRL->D2ASPARE_b.D2ASPARE |= (0x1UL << (22 + ui32Module));
                }
                am_hal_clkmgr_clock_request(pState->eClkSrc, (am_hal_clkmgr_user_id_e)(AM_HAL_CLKMGR_USER_ID_UART0 + ui32Module));

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

            if ( ( pState->ui32BaudRate > 1500000 ) && ( APOLLO5_GE_B1 ) )
            {
                // Clear D2ASPARE to save power
                MCUCTRL->D2ASPARE_b.D2ASPARE &= ~(0x1UL << (22 + ui32Module));
            }
            am_hal_clkmgr_clock_release(pState->eClkSrc, (am_hal_clkmgr_user_id_e)(AM_HAL_CLKMGR_USER_ID_UART0 + ui32Module));

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

    // For 3Mbps, make sure to use 2 stop bits for TX and 1 stop bit for RX
    if ((pUARTTXState->ui32BaudRate == 3000000) && (pUARTTXState->ui32BaudRate == pUARTRXState->ui32BaudRate))
    {
        UARTn(ui32TxModule)->LCRH_b.STP2 = AM_HAL_UART_TWO_STOP_BITS;
        UARTn(ui32RxModule)->LCRH_b.STP2 = AM_HAL_UART_ONE_STOP_BIT;
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
config_baudrate(uint32_t ui32Module, uint32_t ui32DesiredBaudrate, uint32_t *pui32ActualBaud)
{
    uint32_t ui32UartClkFreq;

    //
    // Check that the baudrate is in range.
    //
    switch ( UARTn(ui32Module)->CR_b.CLKSEL )
    {
        case UART0_CR_CLKSEL_PLL_CLK:
            ui32UartClkFreq = 49152000;
            break;

        case UART0_CR_CLKSEL_HFRC_48MHZ:
            ui32UartClkFreq = 48000000;
            break;

        case UART0_CR_CLKSEL_HFRC_24MHZ:
            ui32UartClkFreq = 24000000;
            break;

        case UART0_CR_CLKSEL_HFRC_12MHZ:
            ui32UartClkFreq = 12000000;
            break;

        case UART0_CR_CLKSEL_HFRC_6MHZ:
            ui32UartClkFreq = 6000000;
            break;

        case UART0_CR_CLKSEL_HFRC_3MHZ:
            ui32UartClkFreq = 3000000;
            break;

        default:
            *pui32ActualBaud = 0;
            return AM_HAL_UART_STATUS_CLOCK_NOT_CONFIGURED;
    }

    //
    // Calculate register values.
    //
    {
        uint32_t ui32BaudClk             = BAUDCLK * ui32DesiredBaudrate;
        uint32_t ui32IntegerDivisor      = (ui32UartClkFreq / ui32BaudClk);
        uint64_t ui64IntermediateLong    = ((uint64_t) ui32UartClkFreq * 64) / ui32BaudClk; // Q58.6
        uint64_t ui64FractionDivisorLong = ui64IntermediateLong - ((uint64_t) ui32IntegerDivisor * 64); // Q58.6
        uint32_t ui32FractionDivisor     = (uint32_t) ui64FractionDivisorLong;

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
    }
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
    do
    {
        ONE_BYTE_DELAY(pState);
    }while ( UARTn(ui32Module)->FR_b.BUSY );
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
// Used to configure basic UART settings.
//
//*****************************************************************************
uint32_t
am_hal_uart_configure(void *pHandle, const am_hal_uart_config_t *psConfig)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    uint32_t ui32ErrorStatus;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check to make sure this is a valid handle.
    //
    if (!AM_HAL_UART_CHK_HANDLE(pHandle))
    {
        return AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    //
    // Reset the CR register to a known value.
    //
    UARTn(ui32Module)->CR = 0;

    //
    // Start by enabling the clocks, which needs to happen in a critical
    // section.
    //
    UARTn(ui32Module)->CR_b.CLKEN = 1;

    pState->eAsyncDmaMode = psConfig->eAsyncDMAMode;

    // B0 does not support SYSPLL
    if ( ( psConfig->eClockSrc > AM_HAL_UART_CLOCK_SRC_SYSPLL ) ||
         (( psConfig->eClockSrc == AM_HAL_UART_CLOCK_SRC_SYSPLL ) && ( APOLLO5_B0 )) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
    pState->eClkSrc = (psConfig->eClockSrc == AM_HAL_UART_CLOCK_SRC_HFRC) ? AM_HAL_CLKMGR_CLK_ID_HFRC : AM_HAL_CLKMGR_CLK_ID_SYSPLL;
    if ( psConfig->ui32BaudRate > 1500000 )
    {
        if ( APOLLO5_GE_B1 )
        {
            // Set D2ASPARE, force uart_gate.clken to be 1, making uart.pclk to be always-on
            MCUCTRL->D2ASPARE_b.D2ASPARE |= (0x1UL << (22 + ui32Module));
        }
        UARTn(ui32Module)->CR_b.CLKSEL = (pState->eClkSrc == AM_HAL_CLKMGR_CLK_ID_SYSPLL) ? UART0_CR_CLKSEL_PLL_CLK : UART0_CR_CLKSEL_HFRC_48MHZ;
    }
    else
    {
        UARTn(ui32Module)->CR_b.CLKSEL = (pState->eClkSrc == AM_HAL_CLKMGR_CLK_ID_SYSPLL) ? UART0_CR_CLKSEL_PLL_CLK : UART0_CR_CLKSEL_HFRC_24MHZ;
    }
    am_hal_clkmgr_clock_request(pState->eClkSrc, (am_hal_clkmgr_user_id_e)(AM_HAL_CLKMGR_USER_ID_UART0 + ui32Module));

    //
    // Disable the UART.
    //
    // AM_CRITICAL_BEGIN
    UARTn(ui32Module)->CR_b.UARTEN = 0;
    UARTn(ui32Module)->CR_b.RXE    = 0;
    UARTn(ui32Module)->CR_b.TXE    = 0;
    // AM_CRITICAL_END

    //
    // Set the baud rate.
    //
    ui32ErrorStatus = config_baudrate(ui32Module,
                                      psConfig->ui32BaudRate,
                                      &(pState->ui32BaudRate));

    if (ui32ErrorStatus != AM_HAL_STATUS_SUCCESS)
    {
        return ui32ErrorStatus;
    }

    //
    // Set the flow control options
    //
    // AM_CRITICAL_BEGIN
    UARTn(ui32Module)->CR_b.RTSEN = 0;
    UARTn(ui32Module)->CR_b.CTSEN = 0;
    UARTn(ui32Module)->CR |= psConfig->eFlowControl;
    // AM_CRITICAL_END

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
    // Set the data format.
    //
    // AM_CRITICAL_BEGIN
    UARTn(ui32Module)->LCRH_b.BRK  = 0;
    UARTn(ui32Module)->LCRH_b.PEN  = ui32ParityEnable;
    UARTn(ui32Module)->LCRH_b.EPS  = ui32EvenParity;
    UARTn(ui32Module)->LCRH_b.STP2 = psConfig->eStopBits;
    UARTn(ui32Module)->LCRH_b.FEN  = 1;
    UARTn(ui32Module)->LCRH_b.WLEN = psConfig->eDataBits;
    UARTn(ui32Module)->LCRH_b.SPS  = 0;
    // AM_CRITICAL_END

    //
    // Set the FIFO levels.
    //
    // AM_CRITICAL_BEGIN
    UARTn(ui32Module)->IFLS_b.TXIFLSEL = psConfig->eTXFifoLevel;
    UARTn(ui32Module)->IFLS_b.RXIFLSEL = psConfig->eRXFifoLevel;
    // AM_CRITICAL_END

    //
    // Enable the UART, RX, and TX.
    //
    // AM_CRITICAL_BEGIN
    UARTn(ui32Module)->CR_b.UARTEN = 1;
    UARTn(ui32Module)->CR_b.RXE = 1;
    UARTn(ui32Module)->CR_b.TXE = 1;
    // AM_CRITICAL_END

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
//
//*****************************************************************************
uint32_t
am_hal_uart_tx_flush(void *pHandle)
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
    uint32_t ui32Module = pState->ui32Module;

    //
    // If our state variable says we're done, we're done.
    //
    if (pState->bLastTxComplete == true)
    {
        return AM_HAL_STATUS_SUCCESS;
    }

    //
    // If our state variable doesn't say we're done, we need to check to make
    // sure this program is actually capable of using the state variable.
    // Checking the interrupt enable bit isn't a perfect test, but it will give
    // us a reasonable guess about whether the bLastTxComplete flag might be
    // updated in the future.
    //
    if (UARTn(ui32Module)->IER & AM_HAL_UART_INT_TXCMP)
    {
        while(pState->bLastTxComplete == false)
        {
            ONE_BYTE_DELAY(pState);
        }
    }
    else
    {
        //
        // If we don't know the UART status by now, the best we can do is check
        // to see if the queue is empty, or it the busy bit is still set.
        //
        // If we have a TX queue, we should wait for it to empty.
        //
        if (pState->bEnableTxQueue)
        {
            while (am_hal_queue_data_left(&(pState->sTxQueue)))
            {
                ONE_BYTE_DELAY(pState);
            }
        }

        //
        // Wait for the TX busy bit to go low.
        //
        while ( UARTn(ui32Module)->FR_b.BUSY )
        {
            ONE_BYTE_DELAY(pState);
        }
    }

    return AM_HAL_STATUS_SUCCESS;
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
// Manage UART ISR used when uart fifos and tx and rx queues are enabled
// This is a nonblocking, non-signaling uart driver, it saves incoming rx data
// in the rx queue, and transmits tx data from the queue.via fifo or DMA
//
//*****************************************************************************
am_hal_uart_async_status_t
am_hal_uart_interrupt_queue_service(void *pHandle)
{
    am_hal_uart_state_t *pState = pHandle;
    volatile UART0_Type *pUart = UARTn(pState->ui32Module);

    am_hal_uart_async_status_t ui32RetStat = AM_HAL_UART_ASYNC_STATUS_SUCCESS;

    //
    // manage rx fifo data
    //
    if (pUart->MIS & (UART0_MIS_RTMIS_Msk | UART0_MIS_RXMIS_Msk))
    {
        //
        // read the fifo data, save into the rx queue
        //
        am_hal_queue_t *pRxQ = &pState->sRxQueue;
        uint32_t ui32QueSize = pRxQ->ui32Capacity;
        uint8_t *pui8QueBuff = pRxQ->pui8Data;
        //
        // @note: if the output buffer is read from a higher priority ISR
        //  this next block should be in a critical section
        //
        {
            uint32_t ui32WrtIdx = pRxQ->ui32WriteIndex;
            uint32_t ui32NumInQue = pRxQ->ui32Length;

            //
            // loop while there is data in the queue
            // and there is storage to save the incoming data
            //
            while (!(pUart->FR & UART0_FR_RXFE_Msk))
            {
                uint32_t ui32RdDr = pUart->DR;

                //
                // capture any read error flags
                //
                ui32RetStat |= (ui32RdDr & AM_HAL_UART_ASYNC_STATUS_INTRNL_MSK);
                pui8QueBuff[ui32WrtIdx] = (uint8_t) ui32RdDr;
                if (++ui32NumInQue > ui32QueSize)
                {
                    //
                    // queue is at the limit, can't write this data
                    //
                    ui32NumInQue = ui32QueSize;
                    ui32RetStat |= AM_HAL_UART_ASYNC_STATUS_RX_QUEUE_FULL;
                    break;
                }
                if (++ui32WrtIdx >= ui32QueSize)
                {
                    ui32WrtIdx = 0;
                }
            }
            pRxQ->ui32WriteIndex = ui32WrtIdx;
            if (pRxQ->ui32Length != ui32NumInQue)
            {
                //
                // new data has been added to the rx buffer
                //
                ui32RetStat |= AM_HAL_UART_ASYNC_STATUS_RX_DATA_AVAIL;
                pRxQ->ui32Length = ui32NumInQue;
            }

            //
            // Clear these Interrupts (rx fifo and rx timeout)
            //
            pUart->IEC = (UART0_IEC_RTIC_Msk | UART0_IEC_RXIC_Msk);
        }

    } // ui32IES_int & (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT

    //
    // =============== tx management ================================
    //

    //
    // tx dma support
    // rx dma is not supported in this function
    //
    if (pUart->MIS & UART0_MIS_DMAEMIS_Msk)
    {
        //
        // DMA error? This is not ideal.
        // Shut all DMA off
        //
        pUart->DCR      = 0;  // stop dma
        pUart->IER      &= ~(UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk | UART0_IER_TXIM_Msk | UART0_IER_TXCMPMIM_Msk);
        pUart->IEC      = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk | UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

        ui32RetStat     |= AM_HAL_UART_ASYNC_STATUS_DMA_ERROR;
    } // pUart->MIS & UART0_MIS_DMAEMIS_Msk, dma error


    if (pUart->MIS & UART0_MIS_DMACPMIS_Msk)
    {
        //
        // DMA complete interrupt
        //
        am_hal_uart_dma_tx_queue_t *psDmaQ = pState->psDmaQueue;

        am_hal_uart_dma_tx_descriptor_entry_t *activeDesc = psDmaQ->activeDmaTxDesc;
#ifndef AM_HAL_DISABLE_API_VALIDATION
        if (activeDesc == NULL)
        {
            // This is an unexpected internal error, disable this DMA transfer.
            // @note, if a dma complete interrupt occurs, there should be an active descriptor.
            pUart->DCR      = 0;  // stop dma
            pUart->IER      &= ~(UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk | UART0_IER_TXIM_Msk | UART0_IER_TXCMPMIM_Msk);
            pUart->IEC      = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk | UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

            ui32RetStat     |= AM_HAL_UART_ASYNC_STATUS_INTERNAL_DMA_ERROR;
            return ui32RetStat;
        }
#endif
        if (activeDesc->ui32NumBytes == 0)
        {
            //
            // All the data in this descriptor has been transmitted, switch to the next descriptor.
            // The next descriptor may also contain no data: That will be taken care of below.
            //
            activeDesc->ui32NumDmaQueued = 0;
            activeDesc->ui32StartAddress = activeDesc->ui32BuffBaseAddr; // reset start address
            activeDesc               = activeDesc->nextDesc;
            //
            // }else(activeDesc->ui32NumBytes != 0){
            // if activeDesc->ui32NumBytes is non zero:
            // this descriptor still contains data to send (probably due to dma size limit on the previous DMA)
            //
        }

        if (psDmaQ->eAsyncDmaMode == AM_HAL_UART_DMA_TX_SINGLE_BUFFER)
        {
            //
            // This is a circular buffer scheme
            // check if there is another descriptor/buffer to start transmitting
            //
            am_hal_uart_dma_tx_descriptor_entry_t *currDesc = activeDesc;

            if (currDesc->ui32NumBytes)
            {
                //
                // The next buffer contains data, so start sending that now.
                // Make it the new current buffer, and set that buffer active.
                //
                psDmaQ->activeDmaTxDesc = currDesc;

                pUart->DCR              = 0;  // stop dma
                pUart->TARGADDR         = currDesc->ui32StartAddress;  // set new address
                uint32_t  ui32TxCount   = currDesc->ui32NumBytes;      //
                if (ui32TxCount > AM_HAL_MAX_UART_DMA_SIZE)
                {
                    ui32TxCount         = AM_HAL_MAX_UART_DMA_SIZE;
                }

                pUart->COUNT                = ui32TxCount;
                currDesc->ui32NumDmaQueued  = ui32TxCount;
                currDesc->ui32NumBytes      -= ui32TxCount;
                currDesc->ui32StartAddress  += ui32TxCount;

                __DMB();
                //
                // disable tx complete interrupts, enable dma interrupts
                //
                pUart->IER      = (pUart->IER & ~(UART0_IER_TXIM_Msk | UART0_IER_TXCMPMIM_Msk)) |
                                  (UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk);
                //
                // clear and setup interrupts
                //
                pUart->IEC      = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk |
                                  UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

                pUart->DCR      = UART0_DCR_DMAPRI_Msk | UART0_DCR_TXDMAE_Msk;  // enable dma

                if (psDmaQ->nextDmaWrtDesc == currDesc)
                {
                    //
                    // The current (active DMA) descr is now the same as the append descriptor,
                    // That is invalid (but expected).
                    // Now need to advance and init the append descriptor.
                    //
                    am_hal_uart_dma_tx_descriptor_entry_t *nextw = currDesc->nextDesc;  // next append descriptor
                    //
                    // Init the new append descriptor
                    // Take the write address (start + number) for the existing current descriptor
                    // and use that in the next append descriptor.
                    //
                    uint32_t nextStartAddr      = currDesc->ui32StartAddress;
                    if (nextStartAddr >= psDmaQ->queueEndAddr )
                    {
                        nextStartAddr           = psDmaQ->queueStartAddr;
                    }
                    nextw->ui32StartAddress     = nextStartAddr;
                    nextw->ui32NumBytes         = 0; // should already be zero
                    psDmaQ->nextDmaWrtDesc      = nextw;


                }
                ui32RetStat |= AM_HAL_UART_ASYNC_STATUS_TX_DMA_BUSY;
            }
            else
            {
                //
                // no data left
                //
                if (psDmaQ->activeDmaTxDesc)
                {
                    psDmaQ->activeDmaTxDesc->ui32NumDmaQueued = 0;
                    psDmaQ->activeDmaTxDesc = NULL;
                }

                pUart->DCR              = 0;  // stop dma

                //
                // clear and disable tx and DMA interrupts
                // non DMA code, don't clear tx complete, it may have completed before this code has been run
                //
                pUart->IER = (pUart->IER & ~(UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk | UART0_IER_TXIM_Msk)) |
                             (UART0_IER_TXCMPMIM_Msk);
                pUart->IEC = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk | UART0_IEC_TXIC_Msk;
                ui32RetStat |= AM_HAL_UART_ASYNC_STATUS_TX_DMA_COMPLETE;
            }
        }
        else
        {
            //
            // This is a double buffer scheme
            //
            if (activeDesc->ui32NumBytes)
            {
                //
                // The buffer contains data, so start sending that now.
                //
                psDmaQ->activeDmaTxDesc = activeDesc;
                pUart->DCR              = 0;  // stop dma
                pUart->TARGADDR         = activeDesc->ui32StartAddress;  // set new address

                uint32_t count = activeDesc->ui32NumBytes;      // set new dma size
                if ( count > AM_HAL_MAX_UART_DMA_SIZE)
                {
                    count = AM_HAL_MAX_UART_DMA_SIZE;
                    //
                    // do not advance to next dma descriptor, there is data left
                    //
                }

                pUart->COUNT                 = count;      // set new dma size
                activeDesc->ui32NumDmaQueued = count;
                activeDesc->ui32NumBytes     -= count;
                activeDesc->ui32StartAddress += count;
                __DMB();                //

                //
                // disable tx complete interrupts, enable dma interrupts
                //
                pUart->IER              = (pUart->IER & ~(UART0_IER_TXIM_Msk | UART0_IER_TXCMPMIM_Msk)) |
                                          (UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk);
                //
                // clear and setup interrupts
                //
                pUart->IEC              = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk |
                                          UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

                pUart->DCR              = UART0_DCR_DMAPRI_Msk | UART0_DCR_TXDMAE_Msk;  // enable dma
                ui32RetStat             |= AM_HAL_UART_ASYNC_STATUS_TX_DMA_BUSY;
            }
            else
            {
                //
                // no more data, DMA complete, stop dma
                //
                if (psDmaQ->activeDmaTxDesc)
                {
                    psDmaQ->activeDmaTxDesc->ui32NumDmaQueued = 0;
                    psDmaQ->activeDmaTxDesc = NULL;
                }

                pUart->DCR              = 0;  // stop dma

                //
                // clear and disable tx and DMA interrupts
                // Tx fifo is probably still emptying, don't stop tx, optionally enable tx complete interrupt,
                // check tx comp handling code, may need to be rewritten, which could cause compatibility headache with older
                // non DMA code, don't clear tx complete, it may have completed before this code has been run
                //
                pUart->IER              =
                    (pUart->IER & ~(UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk | UART0_IER_TXIM_Msk)) |
                    (UART0_IER_TXCMPMIM_Msk);
                pUart->IEC              = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk | UART0_IEC_TXIC_Msk;
                ui32RetStat             |= AM_HAL_UART_ASYNC_STATUS_TX_DMA_COMPLETE;
            }
        } // single or double buffer mode
    } // pUart->MIS & UART0_MIS_DMACPMIS_Msk :: big dma complete interrupt block

    //
    // manage tx fifo data
    //
    if (pUart->MIS & UART0_MIS_TXMIS_Msk)
    {
        //
        // When here, the tx interrupt is enabled
        // and the interrupt is active
        //
        am_hal_queue_t *pTxQ = &pState->sTxQueue;
        //
        // @note: This critical section is not needed if:
        // uart send isn't called in higher priority ISRs
        //
        AM_CRITICAL_BEGIN
            uint32_t ui32NumInQue = pTxQ->ui32Length;
            if (ui32NumInQue)
            {
                //
                // There is data to transmit
                // move data from the tx queue to the tx fifo
                //
                uint8_t *pui8QueBuff = pTxQ->pui8Data;
                uint32_t ui32QueSize = pTxQ->ui32Capacity;
                uint32_t ui32RdIdx = pTxQ->ui32ReadIndex;
                //
                // Clear these Tx Interrupts
                //
                pUart->IEC = UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

                while (ui32NumInQue && !(pUart->FR & UART0_FR_TXFF_Msk))
                {
                    pUart->DR = pui8QueBuff[ui32RdIdx];
                    if (++ui32RdIdx >= ui32QueSize)
                    {
                        ui32RdIdx = 0;
                    }
                    ui32NumInQue--;
                }
                pTxQ->ui32ReadIndex = ui32RdIdx;
                pTxQ->ui32Length = ui32NumInQue;
                if (ui32NumInQue == 0)
                {
                    //
                    // Nothing left in queue, disable this interrupt
                    // enable the tx complete interrupt
                    //
                    pUart->IER = (pUart->IER & ~UART0_IER_TXIM_Msk) | UART0_IER_TXCMPMIM_Msk;
                }
                else
                {
                    //
                    // There is still data in the queue,
                    // so at least one more txim interrupt is needed
                    // tx complete interrupt is not needed until the queue is empty
                    //
                    pUart->IER = (pUart->IER & ~UART0_IER_TXCMPMIM_Msk) | UART0_IER_TXIM_Msk;
                }
                ui32RetStat |= AM_HAL_UART_ASYNC_STATUS_TX_BUSY;
            }
            else
            {
                //
                // there is nothing in the queue
                // clear and disable this interrupt, this code should not be executed
                // there could still be some data in the tx fifo
                //
                pUart->IEC = UART0_IEC_TXIC_Msk;
                pUart->IER &= ~UART0_IER_TXIM_Msk;

            }
        AM_CRITICAL_END

    } // ui32IES_int & UART0_IER_TXIM_Msk

    if (pUart->MIS & UART0_MIS_TXCMPMMIS_Msk)
    {

        //
        // tx complete, clear and disable this interrupt
        //
        pUart->IEC = UART0_IEC_TXCMPMIC_Msk;
        //
        // @note: This critical section is not needed if:
        // uart send isn't called in higher priority ISRs
        //
        AM_CRITICAL_BEGIN
            pUart->IER &= ~UART0_IER_TXCMPMIM_Msk;
        AM_CRITICAL_END
        ui32RetStat &= ~AM_HAL_UART_ASYNC_STATUS_TX_BUSY;
        ui32RetStat |= AM_HAL_UART_ASYNC_STATUS_TX_COMPLETE;
    } // pUart->MIS & UART0_MIS_TXCMPMMIS_Msk

    return ui32RetStat;

} // am_hal_uart_interrupt_queue_service

//*****************************************************************************
//
// Choose the correct function based on DMA mode for tx append.
//
//*****************************************************************************
am_hal_uart_errors_t
am_hal_async_uart_append_tx(void *pHandle, uint8_t *pui8Buff, uint32_t ui32NumBytes)
{
    if ( pHandle )
    {
        am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;
        am_hal_uart_async_dma_mode_e eDmaMode = pState->eAsyncDmaMode;
        switch(eDmaMode)
        {
            case AM_HAL_UART_DMA_NONE:
                return am_hal_uart_append_tx_fifo(pHandle, pui8Buff, ui32NumBytes);

            case AM_HAL_UART_DMA_TX_DOUBLE_BUFFER:
                return am_hal_uart_append_tx_double(pHandle, pui8Buff, ui32NumBytes);

            case AM_HAL_UART_DMA_TX_SINGLE_BUFFER:
                return am_hal_uart_append_tx_single(pHandle, pui8Buff, ui32NumBytes);

            case AM_HAL_UART_DMA_RX:
            case AM_HAL_UART_DMA_RX_TX_DOUBLE:
            case AM_HAL_UART_DMA_RX_TX_SINGLE:
            case AM_HAL_UART_DMA_MODE_ENTRIES:
                break;
        }
    }

    return (am_hal_uart_errors_t) AM_HAL_STATUS_INVALID_ARG;
}

//*****************************************************************************
//
// Append data into the uart tx output queue, not using DMA
//
//*****************************************************************************
am_hal_uart_errors_t
am_hal_uart_append_tx_fifo( void *pHandle, uint8_t *pui8Buff, uint32_t ui32NumBytes)
{
    if (ui32NumBytes == 0)
    {
        return AM_HAL_UART_STATUS_SUCCESS;
    }

    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

    if (pState == NULL)
    {
        return (am_hal_uart_errors_t) AM_HAL_STATUS_INVALID_HANDLE;
    }

    am_hal_queue_t *pTxQ = &pState->sTxQueue;
    if (pTxQ->pui8Data == NULL)
    {
        //
        // the user needs to define the queue
        //
        return AM_HAL_UART_ERR_DMA_NO_INIT;
    }

    volatile UART0_Type *pUart = UARTn(pState->ui32Module);

    am_hal_uart_errors_t eReturnStat = AM_HAL_UART_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN
    do
    {
        //
        // clear pending tx interrupts
        //
        pUart->IEC = UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

        uint32_t bytesInQueue = pTxQ->ui32Length;

        //
        // Only write to fifo once, before or after filling the queue:
        // if queue is empty write buffer fifo first, then reducing
        // (or eliminating) the need to save that data in the queue.
        //
        bool fifoFilled = false;
        if (bytesInQueue == 0)
        {
            fifoFilled = true;
            //
            // nothing in the queue, so start by dumping incoming data into fifo
            //
            while (ui32NumBytes && !(pUart->FR & UART0_FR_TXFF_Msk))
            {
                pUart->DR = *pui8Buff++;
                ui32NumBytes--;
            }
        }

        uint8_t  *txQueueBuff = pTxQ->pui8Data;
        uint32_t ui32Wi       = pTxQ->ui32WriteIndex;
        uint32_t ui32Maxi    = pTxQ->ui32Capacity;

        if (ui32NumBytes)
        {
            //
            // put remainder in queue
            //
            bytesInQueue += ui32NumBytes;
            if (bytesInQueue > ui32Maxi)
            {
                eReturnStat = AM_HAL_UART_ERR_BUFFER_OVERFILL;
                break;
            }

            do
            {
                //
                // fill circular buffer
                //
                txQueueBuff[ui32Wi] = *pui8Buff++;
                if (++ui32Wi >= ui32Maxi)
                {
                    ui32Wi = 0;
                }
            }
            while (--ui32NumBytes);
            pTxQ->ui32WriteIndex = ui32Wi;
        }

        if (!fifoFilled)
        {
            //
            // fifo has not been filled this pass, so
            // fill fifo with data from queue
            //
            uint32_t ui32RdIdx = pTxQ->ui32ReadIndex;

            while (bytesInQueue && !(pUart->FR & UART0_FR_TXFF_Msk))
            {
                //
                // move data from circular buffer into queue
                //
                pUart->DR = txQueueBuff[ui32RdIdx];
                if (++ui32RdIdx >= ui32Maxi)
                {
                    ui32RdIdx = 0;
                }
                bytesInQueue--;
            }
            pTxQ->ui32ReadIndex = ui32RdIdx;
        }
        pTxQ->ui32Length     = bytesInQueue;

        if (bytesInQueue)
        {
            //
            // enable the tx fifo low interrupt, that will manage fifo filling from queue
            //
            pUart->IER = (pUart->IER & ~UART0_IER_TXCMPMIM_Msk) | UART0_IER_TXIM_Msk;
        }
        else
        {
            //
            // queue is empty, but fifo has data, enable transmit complete interrupt
            //
            pUart->IER = (pUart->IER & ~UART0_IER_TXIM_Msk) | UART0_IER_TXCMPMIM_Msk;
        }
    }
    while(false);

    AM_CRITICAL_END

    return eReturnStat;
} // am_hal_uart_append_tx
//*****************************************************************************
//
// Append data into the uart tx output queue
//
//*****************************************************************************

//****************************** DMA tx async code ********************************

//*****************************************************************************
//
// Set-up the DMA queue buffer descriptors depending on the dma mode chosen
//
//*****************************************************************************
uint32_t
am_hal_uart_dmaQueueInit(void *pHandle,
                         am_hal_uart_async_dma_mode_e eAsyncDmaMode,
                         am_hal_uart_dma_tx_queue_t *psDmaQ)
{

    if ( (eAsyncDmaMode != AM_HAL_UART_DMA_TX_DOUBLE_BUFFER) &&
         (eAsyncDmaMode != AM_HAL_UART_DMA_TX_SINGLE_BUFFER ) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( pState == NULL)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif

    pState->psDmaQueue      = psDmaQ;
    pState->eAsyncDmaMode   = eAsyncDmaMode;

    psDmaQ->activeDmaTxDesc = 0;
    psDmaQ->nextDmaWrtDesc  = &psDmaQ->tDescriptor[0];
    psDmaQ->eAsyncDmaMode   = eAsyncDmaMode;

    am_hal_uart_dma_tx_descriptor_entry_t *qe = psDmaQ->tDescriptor;

    if (pState->sTxQueue.pui8Data == NULL)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    memset(qe, 0, sizeof(psDmaQ->tDescriptor));
    uint32_t txQueueAddr    = (uint32_t) pState->sTxQueue.pui8Data;

    qe[0].descIdx           = 0;
    qe[1].descIdx           = 1;
    qe[2].descIdx           = 2;

    if (eAsyncDmaMode == AM_HAL_UART_DMA_TX_SINGLE_BUFFER)
    {
        //
        // Using three queue descriptors for circular buffer.
        //
        psDmaQ->bDmaQueueInited = true;
        qe[0].nextDesc          = &qe[1];           // close the linked list
        qe[1].nextDesc          = &qe[2];
        qe[2].nextDesc          = &qe[0];

        psDmaQ->queueStartAddr  = txQueueAddr;
        //
        // This is a circular buffer,
        // pre-compute the end of buffer address
        //
        psDmaQ->queueEndAddr    = psDmaQ->queueStartAddr + pState->sTxQueue.ui32Capacity;
    }
    else if (eAsyncDmaMode == AM_HAL_UART_DMA_TX_DOUBLE_BUFFER)
    {
        //
        // Using two queue descriptors, one for each buffer
        //
        psDmaQ->bDmaQueueInited = true;
        qe[0].nextDesc          = &qe[1];        // close the linked list
        qe[1].nextDesc          = &qe[0];

        qe[0].ui32StartAddress = txQueueAddr;
        qe[0].ui32BuffBaseAddr = txQueueAddr;

        //
        // split the tx buffer in half for double buffering
        //
        uint32_t ui32SubBufferSize = (pState->sTxQueue.ui32Capacity / 2) & ~0x03;
        uint32_t ui32SecondBufferStartAddr = txQueueAddr + ui32SubBufferSize;
        qe[1].ui32StartAddress = ui32SecondBufferStartAddr;
        qe[1].ui32BuffBaseAddr = ui32SecondBufferStartAddr;

        qe[0].ui32BufferSize = ui32SubBufferSize;
        qe[1].ui32BufferSize = ui32SubBufferSize;
    }

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Append tx data with DMA
// this is using the "one buffer / triple descriptor" method (circular buffer)
//
//*****************************************************************************
am_hal_uart_errors_t
am_hal_uart_append_tx_single( void *pHandle,
                              uint8_t *pui8Buff,
                              uint32_t ui32NumBytes)
{
    if (ui32NumBytes == 0)
    {
        //
        // no data, nothing to do here
        //
        return (am_hal_uart_errors_t) AM_HAL_STATUS_SUCCESS;
    }

    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (pState == NULL)
    {
        return (am_hal_uart_errors_t) AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif

    am_hal_uart_dma_tx_queue_t *psDma = pState->psDmaQueue;
    if (psDma == NULL || !psDma->bDmaQueueInited)
    {
        return AM_HAL_UART_ERR_DMA_NO_INIT;
    }

    am_hal_uart_dma_tx_descriptor_entry_t *q = psDma->tDescriptor;
    am_hal_uart_errors_t eReturnVal = AM_HAL_UART_STATUS_SUCCESS;

    //
    // Since the uart interrupt is also working on these DMA data structures,
    // start a critical section (disable interrupts) when queueing DMA data
    //
    AM_CRITICAL_BEGIN

    do
    {
        //
        //! compute number of bytes currently in use.
        //
        uint32_t ui32BytesQueued = q[0].ui32NumBytes + q[1].ui32NumBytes + q[2].ui32NumBytes;
        if (psDma->activeDmaTxDesc)
        {
            ui32BytesQueued += psDma->activeDmaTxDesc->ui32NumDmaQueued;
        }

        //
        // compute total allocated buffer size
        //
        uint32_t ui32BuffSize = psDma->queueEndAddr - psDma->queueStartAddr;

        //
        // compute how many bytes can be queued
        //
        uint32_t ui32RoomLeft = ui32BuffSize - ui32BytesQueued;

        if (ui32RoomLeft < ui32NumBytes)
        {
            //
            // return not enough room for data
            //
            eReturnVal = AM_HAL_UART_ERR_BUFFER_OVERFILL;
            break;
        }

        //
        // if here, the data will fit in the buffer allocated
        // find next copy location
        //
        am_hal_uart_dma_tx_descriptor_entry_t *psNextQ;
        bool bStartDMA = false;
        if (psDma->activeDmaTxDesc == 0)
        {
            //
            // no active dma
            //
            if ( ui32BytesQueued )
            {
                //
                // Error condition.
                // This is inconsistent/invalid behavior, NO active DMA,
                // but data is queued.
                //
                eReturnVal = AM_HAL_UART_ERR_MEMORY_ERROR_01;
                break;
            }

            if (ui32NumBytes <= AM_HAL_UART_FIFO_MAX)
            {
                volatile UART0_Type *pUart = UARTn(pState->ui32Module);
                //
                // contents can fit if fifo is empty
                // is fifo empty then no need for DMA
                //
                if (pUart->FR & UART0_FR_TXFE_Msk)
                {

                    pUart->DCR      = 0;  // stop dma

                    //
                    // fifo is empty, dump data into fifo
                    //
                    do
                    {
                        pUart->DR = *pui8Buff++;
                    }
                    while (--ui32NumBytes);

                    //
                    // setup interrupts
                    // set TX complete interrupt, this could be optional
                    // no need for DMA interrupt
                    //
                    pUart->IER      = (pUart->IER & ~(UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk | UART0_IER_TXIM_Msk)) |
                                      (UART0_IER_TXCMPMIM_Msk);

                    pUart->IEC      = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk |
                                      UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;
                    break; // exit critical section and this function
                }
            } // if ui32NumBytes <= AM_HAL_UART_FIFO_MAX

            psNextQ                     = &psDma->tDescriptor[0];  // use the first descriptor
            psNextQ->ui32StartAddress   = psDma->queueStartAddr;   // start at the beginning of circ buffer
            //psDma->nextDmaWrtDesc       = psNextQ->nextDesc;       // set active write descriptor
            bStartDMA                   = true;                    // since dma is not running, dma needs to be started
            psDma->activeDmaTxDesc      = psNextQ;
            psDma->nextDmaWrtDesc       = psNextQ;                 // once dma is startted will need to advance this

        }
        else
        {
            //
            // there is dma running, so grab the current append/write descriptor
            // there will be no need to start dma in this function
            //
            psNextQ                     = psDma->nextDmaWrtDesc;
        }

        //
        // add new data at the end of any existing data
        //
        uint32_t ui32CopyStartAddr = psNextQ->ui32StartAddress + psNextQ->ui32NumBytes;
        uint32_t ui32BuffHardEnd   = psDma->queueEndAddr;  // end of the cirular buffer
        if (ui32CopyStartAddr >= ui32BuffHardEnd)
        {
            //
            // this is a serious error, existing buffer is overlimit (off the end of the array)
            // for some reason
            //
            eReturnVal = AM_HAL_UART_ERR_MEMORY_ERROR_02;
            break;
        }

        //
        // find the end of the new data in the accumulating descriptor
        //
        uint32_t ui32NewEnd   = ui32CopyStartAddr + ui32NumBytes;  // updated end of data in from start
        uint32_t ui32CopySize = ui32NumBytes;

        if (ui32NewEnd >= ui32BuffHardEnd)
        {
            //
            // this copy will run off the end of the ciruclar buffer
            // need to use two buffers to make this fit
            //  reduce the size of the first copy
            //
            ui32CopySize = ui32BuffHardEnd - ui32CopyStartAddr;

            psNextQ->ui32NumBytes += ui32CopySize;
            //psNextQ->bWrapped      = true;  // is this used?

            //
            // will still need to do another copy, below, for the remainder of the data
            //
            memcpy((void *) ui32CopyStartAddr, pui8Buff, ui32CopySize);
            ui32NumBytes -= ui32CopySize;
            if (ui32NumBytes & 0x80000000)
            {
                //
                // This value has gone negative, a serious error. This shouldn't happen
                // This test can be removed if code proves reliable
                //
                eReturnVal = AM_HAL_UART_ERR_MEMORY_ERROR_03;
                break;
            }

            //
            // advance the write/append queue, since this wrapped
            //
            psNextQ                     = psNextQ->nextDesc;
            psNextQ->ui32StartAddress   = psDma->queueStartAddr; // start at the beginning of the buffer
            psNextQ->ui32NumBytes       = ui32NumBytes;
            psDma->nextDmaWrtDesc       = psNextQ;

            if (ui32NumBytes)
            {
                // buffer wrap
                // so that ui32CopyStartAddr address is always the beginning of the buffer
                // this buffer should be empty at this point

                pui8Buff                += ui32CopySize;  // advance buffer pointer
                memcpy((void *) psNextQ->ui32StartAddress, pui8Buff, ui32NumBytes);
            } // ui32Numbytes != 0: second buffer copy
        }
        else // ui32NewEnd >= ui32BuffHardEnd
        {
            //
            // all data will fit in buffer with one copy
            //
            memcpy((void *) ui32CopyStartAddr, pui8Buff, ui32CopySize);
            psNextQ->ui32NumBytes += ui32CopySize;
        } // ui32NewEnd >= ui32BuffHardEnd

        if (bStartDMA)
        {
            am_hal_uart_dma_tx_descriptor_entry_t *psActiveDesc = psDma->activeDmaTxDesc;

            volatile UART0_Type *pUart = UARTn(pState->ui32Module);

            pUart->DCR      = 0;  // stop dma

            pUart->TARGADDR             = psActiveDesc->ui32StartAddress;
            uint32_t  ui32TxCount       = psActiveDesc->ui32NumBytes;
            if (ui32TxCount > AM_HAL_MAX_UART_DMA_SIZE)
            {
                ui32TxCount                 = AM_HAL_MAX_UART_DMA_SIZE;
            }
            pUart->COUNT                    = ui32TxCount;
            psActiveDesc->ui32NumDmaQueued  = ui32TxCount;
            psActiveDesc->ui32NumBytes      -= ui32TxCount;
            psActiveDesc->ui32StartAddress  += ui32TxCount;

            __DMB();        // Ensure data is copied before starting DMA (it is necessary)

            //
            // clear and setup interrupts
            //
            pUart->IER      = (pUart->IER & ~(UART0_IER_TXCMPMIM_Msk | UART0_IER_TXIM_Msk)) |
                              (UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk);
            pUart->IEC      = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk |
                              UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

            pUart->DCR      = UART0_DCR_DMAPRI_Msk | UART0_DCR_TXDMAE_Msk;  // start tx dma

            if (psActiveDesc == psDma->nextDmaWrtDesc)
            {
                //
                // the active buffer cannot also be the append buffer, need to advance the append buffer
                //
                am_hal_uart_dma_tx_descriptor_entry_t *psAppendDesc = psActiveDesc->nextDesc;
                uint32_t ui32StartAddr = psActiveDesc->ui32StartAddress + psActiveDesc->ui32NumBytes;
                if ( ui32StartAddr >= ui32BuffHardEnd )
                {
                    ui32StartAddr = psDma->queueStartAddr;  // static code analysis says this can't happen
                }
                psAppendDesc->ui32StartAddress  = ui32StartAddr;
                psAppendDesc->ui32NumBytes      = 0;
                psDma->nextDmaWrtDesc           = psAppendDesc;
            } // psActiveDesc == psDma->nextDmaWrtDesc :  end of advance append desc

        } // end of start DMA
    }
    while( false );


    AM_CRITICAL_END

    return eReturnVal;

} // am_hal_uart_append_tx

//*****************************************************************************
//
// This is for appending tx data while doing double buffered DMA
//
//*****************************************************************************
am_hal_uart_errors_t
am_hal_uart_append_tx_double( void *pHandle, uint8_t *pui8Buff, uint32_t ui32NumBytes)
{
    if (ui32NumBytes == 0)
    {
        // no data, nothing to do here
        return AM_HAL_UART_STATUS_SUCCESS;
    }

    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if (pState == NULL)
    {
        return (am_hal_uart_errors_t) AM_HAL_STATUS_INVALID_HANDLE;
    }
#endif
    am_hal_uart_dma_tx_queue_t *psDma = pState->psDmaQueue;
    if (psDma == NULL || !psDma->bDmaQueueInited)
    {
        return AM_HAL_UART_ERR_DMA_NO_INIT;
    }

    am_hal_uart_errors_t eRetVal = AM_HAL_UART_STATUS_SUCCESS;

    AM_CRITICAL_BEGIN
        do
        {
            //
            // find the current descriptor
            //
            am_hal_uart_dma_tx_descriptor_entry_t *currD = psDma->activeDmaTxDesc;

            //
            // find the descriptor that will be used to copy data into
            //
            am_hal_uart_dma_tx_descriptor_entry_t *psPendD;
            if (currD == NULL)
            {
                //
                // nothing transmitting
                //
                if (ui32NumBytes <= AM_HAL_UART_FIFO_MAX)
                {
                    //
                    // contents can fit if the fifo is empty.
                    // is fifo empty?
                    // no need for DMA
                    //
                    volatile UART0_Type *pUart = UARTn(pState->ui32Module);

                    pUart->DCR = 0;  // stop dma

                    if (pUart->FR & UART0_FR_TXFE_Msk)
                    {
                        //
                        // fifo is empty, dump data into fifo
                        //
                        do
                        {
                            pUart->DR = *pui8Buff++;
                        }
                        while (--ui32NumBytes);

                        //
                        // setup interrupts
                        // set TX complete interrupt:
                        // tx complete interrupt could be used to disable uar transmitter
                        // for power savings.
                        //
                        pUart->IER =
                            (pUart->IER & ~(UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk | UART0_IER_TXIM_Msk)) |
                            (UART0_IER_TXCMPMIM_Msk);

                        pUart->IEC = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk |
                                     UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

                        break; // exit critical section and function

                    }
                } // if ui32NumBytes <= AM_HAL_UART_FIFO_MAX

                psPendD                     = &psDma->tDescriptor[0];
                uint32_t ui32BaseAddr       = psPendD->ui32BuffBaseAddr;
                psPendD->ui32StartAddress   = ui32BaseAddr;
                psPendD->ui32NumBytes       = 0;
            }
            else //currD == NULL
            {
                //
                // DMA is active, so choose the other buffer
                //
                psPendD = currD->nextDesc;
            } //currD == NULL

            //
            // compute unused buffer size
            //
            uint32_t ui32NumInQueue = psPendD->ui32NumBytes;
            uint32_t ui32RoomLeft   = psPendD->ui32BufferSize - ui32NumInQueue;
            if (ui32RoomLeft < ui32NumBytes)
            {
                //
                // this data won't fit
                // exit with error
                //
                eRetVal = AM_HAL_UART_ERR_BUFFER_OVERFILL;
                break;  // exit critical section and function
            }

            //
            // there is room left to save data in the output buffer
            //
            uint32_t ui32CopyStartAddr = psPendD->ui32StartAddress + ui32NumInQueue;  // start address in buffer
            memcpy((void *) ui32CopyStartAddr, pui8Buff, ui32NumBytes);          // copy data into buffer
            psPendD->ui32NumBytes = ui32NumInQueue + ui32NumBytes;             // update buffer size

            if (currD == NULL)
            {
                //
                // dma is not running, start dma
                //
                psDma->activeDmaTxDesc    = psPendD;

                volatile UART0_Type *pUart = UARTn(pState->ui32Module);

                pUart->DCR                = 0;  // stop dma
                pUart->TARGADDR           = psPendD->ui32StartAddress;  // set start address
                uint32_t ui32TxCount      = psPendD->ui32NumBytes;      // number of bytes for DMA
                if (ui32TxCount > AM_HAL_MAX_UART_DMA_SIZE)
                {
                    ui32TxCount           = AM_HAL_MAX_UART_DMA_SIZE;
                }
                psPendD->ui32StartAddress += ui32TxCount;       // advance to next starting address
                psPendD->ui32NumBytes     -= ui32TxCount;       // subtract number of bytes queued via DMA
                pUart->COUNT              = ui32TxCount;        // queue this many bytes

                __DMB();        // This is needed here to ensure data copy is complete before DMA start

                //
                // setup interrupts
                //
                pUart->IER      = (pUart->IER & ~(UART0_IER_TXCMPMIM_Msk | UART0_IER_TXIM_Msk)) |
                                  (UART0_IER_DMAEIM_Msk | UART0_IER_DMACPIM_Msk);
                pUart->IEC      = UART0_IEC_DMAEIC_Msk | UART0_IEC_DMACPIC_Msk |
                                  UART0_IEC_TXCMPMIC_Msk | UART0_IEC_TXIC_Msk;

                pUart->DCR      = UART0_DCR_DMAPRI_Msk | UART0_DCR_TXDMAE_Msk;  // start DMA

            } // currD == NULL
        }
        while (false); // critical section do/while

    AM_CRITICAL_END

    return eRetVal;
}

//*****************************************************************************
//
// Get Rx Data
//
// This function will unload data from the queue and load the data into
// a user supplied buffer.
//
//*****************************************************************************
uint32_t
am_hal_uart_async_get_rx_data(void *pHandle,
                              uint8_t *pui8DestBuff,
                              uint32_t ui32MaxBytesToRead )
{
    am_hal_uart_state_t *pState = (am_hal_uart_state_t *) pHandle;

#ifdef OPTIONAL_UNLOAD_FIFO
    //
    // If the rx data timeout is enabled, reading the fifo here is unnecessary
    // If using an RX queue, take all the pui8Data from the RX FIFO and add
    // it to the internal queue.
    //
    if (pState->bEnableRxQueue)
    {
        rx_queue_update(pHandle);
    }
#endif

    am_hal_queue_t *psRxQueue = &(pState->sRxQueue);

    //
    // read from queue (loaded via uart rx interrupt)
    //
    uint32_t ui32Wi    = psRxQueue->ui32WriteIndex;
    uint32_t ui32Ri    = psRxQueue->ui32ReadIndex;
    uint32_t ui32Size  = psRxQueue->ui32Capacity;
    uint8_t  *pui8Data = psRxQueue->pui8Data;

    if (ui32Ri >= ui32Size )
    {
        ui32Ri = 0;
    }

    uint32_t ui32NumRead = 0;

    __DMB();

    while(true)
    {
        if (ui32Ri == ui32Wi)
        {
            //
            // queue is empty
            //
            break;
        }
        *pui8DestBuff++ = pui8Data[ui32Ri];
        if (++ui32Ri >= ui32Size)
        {
            ui32Ri = 0;
        }
        if (++ui32NumRead >= ui32MaxBytesToRead)
        {
            //
            // read limit reached
            //
            break;
        }
    }

    if (ui32NumRead)
    {
        //
        // Update number of bytes left in queue.
        //
        AM_CRITICAL_BEGIN
            psRxQueue->ui32ReadIndex = ui32Ri;
            uint32_t ui32NumInQueue = psRxQueue->ui32Length;
            if (ui32NumInQueue > ui32NumRead)
            {
                ui32NumInQueue -= ui32NumRead;
            }
            else
            {
                ui32NumInQueue = 0;
            }
            psRxQueue->ui32Length = ui32NumInQueue;
        AM_CRITICAL_END
    }

    return ui32NumRead;
}

//*****************************************************************************
//
//! @}
// End Doxygen group.
//
//*****************************************************************************
