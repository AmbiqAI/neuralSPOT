//*****************************************************************************
//
//! @file am_hal_uart_stream.h
//!
//! @brief UART streaming mode Hardware abstraction
//!
//! @addtogroup uart_stream_ap510L UART Stream Functionality
//! @ingroup apollo330P_hal
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
#ifndef AM_HAL_UART_STREAM_H
#define AM_HAL_UART_STREAM_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Global definitions
//
//*****************************************************************************
#define UARTn(n)    ((UART0_Type*)(UART0_BASE + (n * (UART1_BASE - UART0_BASE))))

//
//! Contains computed values from uart config.
//! Used in uart state struct.
//
typedef struct
{
    //
    //! Computed uart baud-rate
    //
    uint32_t ui32BaudRate;
    //
    //! UART clock source.
    //
    am_hal_uart_clock_src_e eClkSrc;

    //
    //! UART clock Divider.
    //
    am_hal_uart_clock_div_e eClkDiv;

}
am_hal_uart_config_out_t;

//
//! DMA streaming mode defs,
//! These are modes that are supported by the uart streaming functions
//
typedef enum
{
    AM_HAL_UART_DMA_NONE,
    AM_HAL_UART_DMA_TX_DOUBLE_BUFFER,
    AM_HAL_UART_DMA_TX_SINGLE_BUFFER,
    AM_HAL_UART_DMA_RX_SINGLE,
    AM_HAL_UART_DMA_RX_DOUBLE,
    //AM_HAL_UART_DMA_RX_TX_SINGLE,      //!< This is not supported
    //AM_HAL_UART_DMA_RX_TX_DOUBLE,      //!< This is not supported
    AM_HAL_UART_DMA_MODE_ENTRIES,
}
am_hal_uart_streaming_dma_mode_e;

#define AM_HAL_DMA_TX_QUEUE_MAX_ENTRIES 3  //!< this value shouldn't be changed
#define AM_HAL_DMA_RX_QUEUE_MAX_ENTRIES 2  //!< this value shouldn't be changed

//*****************************************************************************
//
//! UART error codes.
//! This is returned from the streaming uart ISR service
//
//*****************************************************************************
typedef enum
{
    AM_HAL_UART_STREAM_STATUS_SUCCESS            = 0,
    AM_HAL_UART_STREAM_STATUS_RX_QUEUE_FULL      = 0x0001,
    AM_HAL_UART_STREAM_STATUS_RX_DATA_AVAIL      = 0x0002,
    AM_HAL_UART_STREAM_STATUS_TX_QUEUE_FULL      = 0x0004,
    AM_HAL_UART_STREAM_STATUS_TX_COMPLETE        = 0x0008,
    AM_HAL_UART_STREAM_STATUS_TX_BUSY            = 0x0010,
    AM_HAL_UART_STREAM_STATUS_TX_DMA_BUSY        = 0x0020,
    AM_HAL_UART_STREAM_STATUS_TX_DMA_COMPLETE    = 0x0020,
    AM_HAL_UART_STREAM_STATUS_DMA_ERROR          = 0x0040,
    AM_HAL_UART_STREAM_STATUS_INTERNAL_DMA_ERROR = 0x0080,
    AM_HAL_UART_STREAM_STATUS_FRM_ERROR          = UART0_DR_FEDATA_Msk,  // 0x0100 <-> 0x0800 are reserved for hardware outputs
    AM_HAL_UART_STREAM_STATUS_PRTY_ERROR         = UART0_DR_PEDATA_Msk,
    AM_HAL_UART_STREAM_STATUS_BRK_ERROR          = UART0_DR_BEDATA_Msk,
    AM_HAL_UART_STREAM_STATUS_OVRN_ERROR         = UART0_DR_OEDATA_Msk,
    AM_HAL_UART_STREAM_STATUS_INTRNL_MSK         = (AM_HAL_UART_STREAM_STATUS_FRM_ERROR
                                                   | AM_HAL_UART_STREAM_STATUS_OVRN_ERROR
                                                   | AM_HAL_UART_STREAM_STATUS_PRTY_ERROR
                                                   | AM_HAL_UART_STREAM_STATUS_BRK_ERROR),
    AM_HAL_UART_RX_DMA_ERROR                     = 0x01000,
    AM_HAL_UART_RX_DMA_COMPLETE                  = 0x02000,
    AM_HAL_UART_RX_DMA_BUSY                      = 0x04000,
    AM_HAL_UART_RX_DMA_TIMEOUT                   = 0x08000,
    AM_HAL_UART_RX_DMA_OVERFLOW                  = 0x10000,
    AM_HAL_UART_STREAM_DMA_CFG_ERROR             = 0x20000,
    AM_HAL_UART_STREAM_STATUS_BUSY_WAIT_CMPL     = 0x40000,
    AM_HAL_UART_INVALID_HANDLE                   = 0x80000,
    AM_HAL_UART_STREAM_STATUS_x32                = 0x7F000000,  // force all compilers to use 32bit
}
am_hal_uart_stream_status_t;

//
//! defines a tx buffer descriptor used
//! for DMA transmissions in the streaming functions
//! The DMA does not require an aligned start address
//! This is part of the stream state
//
typedef struct uart_dma_tx_qe
{
    struct uart_dma_tx_qe *nextDesc;
    uint32_t              ui32BuffBaseAddr;    //!< used in double buffer mode to save base address
    uint32_t              ui32StartAddress;    //!< next starting address of this section
    uint32_t              ui32BufferSize;      //!< the size of this descriptor buffer
    uint32_t              ui32NumBytes;        //!< num bytes saved in this section, other than sending DMA
    uint32_t              ui32NumDmaQueued;    //!< number of bytes in buffer, sending via DMA
    uint8_t               descIdx;
    uint8_t               align[3];
}
am_hal_uart_stream_dma_tx_descriptor_entry_t;

//
//!
//
typedef enum
{
    eAM_HAL_TX_COMPL_NO_NOTIFICATION = 0,
    eAM_HAL_TX_COMPL_DMA_COMPLETE    = 0x01,
    eAM_HAL_TX_COMPL_TX_COMPLETE     = 0x02,
    eAM_HAL_TX_COMPL_BUSY_WAIT_COMPLETE = 0x04,
    eAM_HAL_TX_COMPL_x32 = 0x80000000,  //!< Force 4 byte variable

} am_hal_uart_stream_tx_complete_options_t;

//
//! defines a rx buffer descriptor used
//! for DMA transmissions in the streaming functions
//! The DMA does not require an aligned start address
//! This is part of the stream state
//
typedef struct uart_dma_rx_qe
{
    struct uart_dma_rx_qe *nextDesc;
    uint32_t              ui32BuffBaseAddr;    //!< used in double buffer mode to save base address
    uint32_t              ui32BufferSize;      //!< the size of this descriptor buffer
    uint32_t              ui32BytesToRead;
    uint32_t              ui32NumBytes;        //!< num bytes to rx for this DMA transaction
    uint32_t              ui32BuffHasData;     //!< when set to 1 the buffer has data in it
    uint8_t               descIdx;
    bool                  bLastComplete;        //!< when double-buffering, this can be used to find the oldest
    uint8_t               align[2];
}
am_hal_uart_stream_dma_rx_descriptor_entry_t;

//! define argument returned via callback
typedef struct
{
    uint8_t     *pui8Buffer;          //!< pointer to the beginning of buffer
    uint32_t    ui32BufferSize;       //!< number of bytes available
    volatile uint32_t *hasData;     //!< pointer to buffer data ready, clear this when data is used, DMA only
    am_hal_uart_stream_status_t eStatus; //!< transfer status
}
am_hal_uart_stream_callback_data_t;

typedef struct
{
    volatile uint32_t ui32WriteIndex;  //!< queue write index
    volatile uint32_t ui32ReadIndex;   //!< queue read index
    volatile uint32_t ui32Length;      //!< number of bytes in queue
    uint32_t ui32Capacity;             //!< total size of queue buffer
    uint32_t ui32ItemSize;             //!< number of bytes per item
    uint8_t *pui8Data;                 //!< pointer to start of queue buffer
}
am_hal_stream_queue_t;

//
//! define the callback form
//
typedef void (*am_hal_uart_stream_cb_t)(am_hal_uart_stream_callback_data_t *psCBData);

//
//! This is the tx parameters definition
//! This is part of the stream state
//
typedef struct
{
    am_hal_stream_queue_t               sTxQueue;

    am_hal_uart_stream_cb_t             pfTxCompleteCallback;

    //
    //! State used to trigger callback
    //
    am_hal_uart_stream_tx_complete_options_t eTxCallbackNotificationOptions;
    //
    am_hal_uart_stream_tx_complete_options_t eTxCompleteMode;

    //
    //! This is set when a callback is made and must be cleared to re-enable the callback
    //
    uint32_t                            ui32TxCallbackPending;

    //
    //! buffer descriptors for a uart
    //!
    am_hal_uart_stream_dma_tx_descriptor_entry_t tDescriptor[AM_HAL_DMA_TX_QUEUE_MAX_ENTRIES];
    uint32_t                              queueStartAddr;        //!< start of the circular buffer
    uint32_t                              queueEndAddr;          //!< end of circular buffer
    am_hal_uart_stream_dma_tx_descriptor_entry_t *activeDmaTxDesc;      //!< active dma queue, 0 when no DMA ongoing
    am_hal_uart_stream_dma_tx_descriptor_entry_t *nextDmaWrtDesc;       //!< used in SingleBuffer mode

    bool                                  bDmaQueueInited;       //!< true when data in this struct is inited
    //
    //! Queued write/read implementation
    //
    bool bEnableTxQueue;

    uint8_t     align[2];
}
am_hal_uart_stream_tx_params_t;

//
//! simple struct that associates buffer pointer with buff size.
//
typedef struct
{
    uint8_t *pui8Buff;
    uint32_t ui32BufferSize;
}
am_hal_uart_stream_buff_t;

//
//! The rx dma config struct, used to init the rx dma and associated parameters
//! INIT struct (used to init the system)
//
typedef struct
{
    am_hal_uart_stream_buff_t sRxBuffer;

    uint32_t ui32NumBytesToRead;    //!< number of dma byes to read

    //
    //! Callback function used when RX DMA completes. Called from ISR
    //! Also called when not in RX dma when the number of bytes in the buffer
    //! exceeds the threshold.
    //
    am_hal_uart_stream_cb_t pfRxCallback;

    //
    //! pfRxCallback is called when the number of rxed bytes matches this value, disabled when zero
    //
    uint32_t ui32RxCallbackThreshold;
    uint32_t ui32TimeoutMs;         //!< Specify timeout in milliseconds, not implemented
    bool bClearRxFifo;              //!< When starting RX DMA, setting this flag will clear the RX fifo
    uint8_t align[3];
}
am_hal_uart_stream_rx_config_t;

//
//! This is the rx streaming and streaming DMA definitions
//! for DMA transmissions in the streaming functions
//! This is part of the stream state struct
//
typedef struct
{
    //
    //! rx buffer queue
    //
    am_hal_stream_queue_t               sRxQueue;

    //
    //! RX complete callback, Called from ISR
    //
    am_hal_uart_stream_cb_t             pfRxCompleteCallback;

    //
    //! Threshold used for rx callback, Number of bytes in queue
    //! Threshold is used when using RX in FIFO mode (no DMA)
    //
    uint32_t                            ui32RxCallbackThreshold;

    //
    //! This is set when a callback is made and must be cleared to re-enable the callback
    //! Clear this once the data has been consumed.
    //
    uint32_t                            ui32RxCallbackPending;

    //
    //! buffer descriptors for a uart
    //! These are used in for double buffer management.
    //
    am_hal_uart_stream_dma_rx_descriptor_entry_t tDescriptor[AM_HAL_DMA_RX_QUEUE_MAX_ENTRIES];
    am_hal_uart_stream_dma_rx_descriptor_entry_t *activeDmaRxDesc;      //!< active dma queue, 0 when no DMA ongoing

    uint32_t                              ui32RxTimeoutmSec;     //!< Specify timeout in milliseconds, reserved
    bool                                  bDmaQueueInited;       //!< true when data in this struct is inited
    bool                                  bEnableRxQueue;

    uint8_t     align[2];
}
am_hal_uart_stream_rx_params_t;  // @todo change name

//
//! enum specifying active dma mode
//
typedef enum
{
    eAM_HAL_UART_NO_ACTIVE_DMA,
    eAM_HAL_UART_TX_ACTIVE_DMA,
    eAM_HAL_UART_RX_ACTIVE_DMA,
}
am_hal_uart_stream_active_queue;

//
//! used in the dma code,
//! This is part of the stream state
//
typedef struct
{
    volatile bool                         bDMAActive;
    am_hal_uart_stream_active_queue       eActiveQueueType;
    uint8_t                               align[2];
}
am_hal_uart_dma_config_t;

//
//!
//! INIT Used am_hal_uart_stream_data_config_t
//
typedef struct
{
    //
    //! contains buffer pointer and size
    //
    am_hal_uart_stream_buff_t sTxBuffer;

    //
    //! Callback function used when TX completes. Called from ISR
    //
    am_hal_uart_stream_cb_t pfTxCallback;

    //
    //! This is when the isr will notify (nothing, check tx complete, or tx busy)
    //
    am_hal_uart_stream_tx_complete_options_t eTxCompleteNotificationAction;

    //
    //! This is what the isr will do (nothing, check tx complete, or tx busy)
    //
    am_hal_uart_stream_tx_complete_options_t eTxCompleteMode;

    uint8_t align[2];
}
am_hal_uart_stream_tx_config_t;

//
//!
//! This is used to configure tx and rx parameters
//! it can be const, this is read-only
//
typedef struct
{
    //
    //! can be const, struct containing rx setup params
    //
    am_hal_uart_stream_rx_config_t sRxStreamConfig;
    //
    //! can be const, struct containing tx setup params
    //
    am_hal_uart_stream_tx_config_t        sTxStreamConfig;
    am_hal_uart_streaming_dma_mode_e      eStreamingDmaMode;         //!< the dma mode used for this uart
    uint8_t align[3];

}
am_hal_uart_stream_data_config_t;

//*****************************************************************************
//
//! @brief Manage UART ISR used when using uart streaming api
//! This is a nonblocking, non-signaling (streaming) uart driver, it saves incoming rx data
//! in the rx queue, and transmits tx data from the queue via fifo with interrupts or DMA
//!
//! @note This takes care of all interrupt management, the calling application does not
//! interact with the processor interrupts.
//!
//! @param pUartHandle Pointer to the UART handle
//!
//! @return am_hal_uart_stream_status_t
//
//*****************************************************************************
extern am_hal_uart_stream_status_t am_hal_uart_interrupt_stream_service(void *pUartHandle);

//*****************************************************************************
//
//! @brief Choose the correct function based on DMA mode for tx append.
//!
//! @param pUartHandle Pointer to the UART handle
//! @param pui8Buff  pointer to tx buffer
//! @param ui32NumBytes number of bytes to transmit
//!
//! @return standard hal status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_stream_uart_append_tx(void *pUartHandle,
                                                         uint8_t *pui8Buff,
                                                         uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Append data into the uart tx output queue using Tx FIFO not DMA
//!
//! @param pUartHandle Pointer to the UART handle
//! @param pui8Buff  pointer to data buffer
//! @param ui32NumBytes  number of bytes to transmit
//!
//! @return  uart hal status (standard hal status)
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_append_tx_fifo(void *pUartHandle,
                                                       uint8_t *pui8Buff,
                                                       uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief queue data to transmit via UART DMA
//!
//! @param pUartHandle Pointer to the UART handle
//! @param pui8Buff pointer to data buffer
//! @param ui32NumBytes  number of bytes to send
//!
//! @return uart status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_append_tx_double(void *pUartHandle,
                                                         uint8_t *pui8Buff,
                                                         uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief queue data to transmit via UART DMA
//!
//! @param pUartHandle  Pointer to the UART handle
//! @param pui8Buff     pointer to data buffer
//! @param ui32NumBytes  number of bytes to send
//!
//! @return uart status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_append_tx_single(void *pUartHandle,
                                                         uint8_t *pui8Buff,
                                                         uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief get number of bytes in rx buffer
//!
//! @note Using this function only makes sense when not using RX DMA
//! as the uart driver has no visibility of much data the RX buffer contains
//! as DMA transfers are ongoing.
//! Use the RX callback mechanism for RX DMA
//!
//! @note. This function won't include any bytes sitting in the RX FIFO
//!
//! @param pUartHandle Pointer to the UART handle
//! @param bUnloadRxFifo when any data in the fifo will be unloaded first
//!
//! @return The number of bytes loaded into the rx buffer
//
//*****************************************************************************
extern uint32_t am_hal_uart_stream_get_num_rx_bytes_in_buffer(void *pUartHandle,
                                                              bool bUnloadRxFifo);

//*****************************************************************************
//
//! @brief Get Rx Data from Streaming buffered data
//! This function will unload data from the queue and load the data into
//! a user supplied buffer.
//!
//! @param pUartHandle          Pointer to the UART handle
//! @param pui8DestBuff         Pointer to rx data buffer
//! @param ui32MaxBytesToRead   max number of bytes to read
//! @param bUnloadRxFifo        when true, the function will first unload
//!                             any data in the rx fifo
//!
//! @return number of bytes loaded into data buffer
//
//*****************************************************************************
extern uint32_t  am_hal_uart_stream_get_rx_data(void *pUartHandle,
                                  uint8_t *pui8DestBuff,
                                  uint32_t ui32MaxBytesToRead,
                                  bool bUnloadRxFifo);

//*****************************************************************************
//
//! @brief configure the streaming driver for TX operation
//!
//! @param pUartHandle Pointer to the UART handle
//! @param ptxConfig  pointer to pre-defined struct containing tx config params
//!
//! @return AM_HAL_UART_STATUS_SUCCESS if the configuration is successful.
//!         Returns a UART error code (am_hal_uart_errors_t) if an error is detected
//!         during RX or TX stream configuration.
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_stream_configure_tx(void *pUartHandle,
    const am_hal_uart_stream_tx_config_t *ptxConfig);

//*****************************************************************************
//
//! @brief configure the streaming driver for RX operation
//!
//! @param pUartHandle Pointer to the UART handle
//! @param pRxConfig  pointer to pre-defined struct containing rx config params
//!
//! @return module hal status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_stream_configure_rx(void *pUartHandle,
                                                          const am_hal_uart_stream_rx_config_t *pRxConfig);

//*****************************************************************************
//
//! @brief Enables or disables RX DMA streaming for a specified UART module.
//!
//! This function configures the RX DMA settings of a UART module to enable or disable
//! RX DMA streaming.
//!  It clears the UART FIFO buffer if requested,
//!  It sets up DMA buffers, and initializes RX DMA if enabled.
//!
//! @param pUartHandle Pointer to the UART streaming state structure
//!                (am_hal_uart_stream_state_t). This structure contains the state
//!                of the UART streaming and DMA configurations.
//! @param bEnableRxDMA Boolean flag to specify whether to enable (true) or disable (false)
//!                     RX DMA streaming.
//! @param bClearFifo Boolean flag to indicate whether to clear the RX FIFO buffer.
//!                   Set this to 'true' to clear the buffer before enabling RX DMA streaming.
//!
//! @return AM_HAL_UART_STATUS_SUCCESS if the RX DMA is successfully enabled or disabled.
//!         Other UART-specific error codes (am_hal_uart_errors_t) in case of a failure,
//!
//
//*****************************************************************************
 extern am_hal_uart_errors_t  am_hal_uart_stream_enable_rxDma(void *pUartHandle,
                bool bEnableRxDMA, bool bClearFifo);

//*****************************************************************************
//
//! @brief Initializes a UART stream instance for a specified module.
//!
//! @note The provided am_hal_uart_stream_state_t instance cannot not be NULL.
//!       It cannot be a stack variable, it must be a pointer to a global.
//!
//! This function sets up initial configurations and internal states for the
//! specified UART module.
//! It ensures that the requested module is valid and not already initialized
//! before associating it with the provided UART stream state structure.
//!
//! @param ui32Module The UART module number to initialize. Must be less than
//!                   the total number of UART modules supported.
//!
//! @param ppUartHandle Pointer uart handle pointer
//!                the pointer is set in this function
//!
//! @return AM_HAL_STATUS_SUCCESS if the initialization is successful.
//!         AM_HAL_STATUS_INVALID_ARG if the module number is invalid.
//!         AM_HAL_STATUS_INVALID_OPERATION if the module is already initialized.
//!
//
//*****************************************************************************
extern uint32_t am_hal_uart_stream_initialize(uint32_t ui32Module,
    void **ppUartHandle);

//*****************************************************************************
//
//! @brief Configures UART streaming data settings for a specified module.
//!
//! This sets up the UART streaming data configurations, including RX and TX
//! streaming configurations.
//!
//! @param pUartHandle Pointer to the UART handle
//! @param psDataCfg Pointer to the UART streaming data config struct.
//!                  (am_hal_uart_stream_data_config_t) Which contains setup settings,
//!                  including stream configurations for RX and TX.
//!
//! @return AM_HAL_UART_STATUS_SUCCESS if the configuration is successful.
//!         Returns a UART error code (am_hal_uart_errors_t) if an error is detected
//!         during RX or TX stream configuration.
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_stream_data_configure(void *pUartHandle,
    const am_hal_uart_stream_data_config_t *psDataCfg);

//*****************************************************************************
//
//! @brief configure uart hardware settings
//!
//! @param pUartHandle Pointer to the UART handle
//! @param psConfig pointer to uart config struct
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t am_hal_uart_stream_configure(void *pUartHandle,
                                         const am_hal_uart_config_t *psConfig);

//*****************************************************************************
//
//! @brief set low power state for uart
//!
//! @param pUartHandle Pointer to the UART handle
//! @param ePowerState power state
//! @param bRetainState retain the register state through low power
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t am_hal_uart_stream_power_control(void *pUartHandle,
                                                 am_hal_sysctrl_power_state_e ePowerState,
                                                 bool bRetainState);

//*****************************************************************************
//!
//! @brief  This function will set and clear interrupt enable bits
//!         for the specified uart.
//!
//! @param pUartHandle Pointer to the UART streaming state handle
//! @param ui32Clear these bits will be cleared in the uart interrupt enable register
//! @param ui32Set these bits will be set in the uart interrupt enable register
//!
//! @return AM_HAL_STATUS_SUCCESS always success
//
//*****************************************************************************
extern uint32_t am_hal_uart_stream_interrupt_clr_set( void *pUartHandle,
                                                     uint32_t ui32Clear,
                                                     uint32_t ui32Set);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_UART_STREAM_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
