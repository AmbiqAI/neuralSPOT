//*****************************************************************************
//
//! @file am_hal_uart.h
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

#ifndef AM_HAL_UART_H
#define AM_HAL_UART_H

#include <stdint.h>
#include <stdbool.h>


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

//*****************************************************************************
//
//! UART error codes.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_UART_STATUS_SUCCESS    = AM_HAL_STATUS_SUCCESS,
    AM_HAL_UART_STATUS_BUS_ERROR = AM_HAL_STATUS_MODULE_SPECIFIC_START,
    AM_HAL_UART_STATUS_RX_QUEUE_FULL,
    AM_HAL_UART_STATUS_CLOCK_NOT_CONFIGURED,
    AM_HAL_UART_STATUS_BAUDRATE_NOT_POSSIBLE,
    AM_HAL_UART_STATUS_TX_CHANNEL_BUSY,
    AM_HAL_UART_STATUS_RX_CHANNEL_BUSY,
    AM_HAL_UART_STATUS_RX_TIMEOUT,
    AM_HAL_UART_STATUS_FRAME_ERROR,
    AM_HAL_UART_STATUS_PARITY_ERROR,
    AM_HAL_UART_STATUS_BREAK_ERROR,
    AM_HAL_UART_STATUS_OVERRUN_ERROR,
    AM_HAL_UART_STATUS_DMA_ERROR,
    AM_HAL_UART_ERR_DMA_NO_INIT,
    AM_HAL_UART_ERR_BUFFER_OVERFILL,
    AM_HAL_UART_ERR_MEMORY_ERROR_01,
    AM_HAL_UART_ERR_MEMORY_ERROR_02,
    AM_HAL_UART_ERR_MEMORY_ERROR_03,
    AM_HAL_UART_ERR_END,            //<! Keep this the last entry in this enum list

}
am_hal_uart_errors_t;

//*****************************************************************************
//
//! @name UART status.
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_RSR_DMAERR              UART0_RSR_DMAERR_Msk
#define AM_HAL_UART_RSR_DMACPL              UART0_RSR_DMACPL_Msk
#define AM_HAL_UART_RSR_OESTAT              UART0_RSR_OESTAT_Msk
#define AM_HAL_UART_RSR_BESTAT              UART0_RSR_BESTAT_Msk
#define AM_HAL_UART_RSR_PESTAT              UART0_RSR_PESTAT_Msk
#define AM_HAL_UART_RSR_FESTAT              UART0_RSR_FESTAT_Msk

//*****************************************************************************
//
//! @name UART interrupts.
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_INT_DMAERIS             UART0_IER_DMAEIM_Msk
#define AM_HAL_UART_INT_DMACPRIS            UART0_IER_DMACPIM_Msk
#define AM_HAL_UART_INT_OVER_RUN            UART0_IER_OEIM_Msk
#define AM_HAL_UART_INT_BREAK_ERR           UART0_IER_BEIM_Msk
#define AM_HAL_UART_INT_PARITY_ERR          UART0_IER_PEIM_Msk
#define AM_HAL_UART_INT_FRAME_ERR           UART0_IER_FEIM_Msk
#define AM_HAL_UART_INT_RX_TMOUT            UART0_IER_RTIM_Msk
#define AM_HAL_UART_INT_TX                  UART0_IER_TXIM_Msk
#define AM_HAL_UART_INT_RX                  UART0_IER_RXIM_Msk
#define AM_HAL_UART_INT_DSRM                UART0_IER_DSRMIM_Msk
#define AM_HAL_UART_INT_DCDM                UART0_IER_DCDMIM_Msk
#define AM_HAL_UART_INT_CTSM                UART0_IER_CTSMIM_Msk
#define AM_HAL_UART_INT_TXCMP               UART0_IER_TXCMPMIM_Msk
#define AM_HAL_UART_INT_ALL                 0xFFFFFFFF
//! @}

//*****************************************************************************
//
//! @name UART Flag Register
//! @{
//! Macro definitions for UART Flag Register Bits.
//! They may be used with the \e am_hal_uart_flags_get() function.
//
//*****************************************************************************
#define AM_HAL_UART_FR_TX_BUSY              UART0_FR_TXBUSY_Msk
#define AM_HAL_UART_FR_TX_EMPTY             UART0_FR_TXFE_Msk
#define AM_HAL_UART_FR_RX_FULL              UART0_FR_RXFF_Msk
#define AM_HAL_UART_FR_TX_FULL              UART0_FR_TXFF_Msk
#define AM_HAL_UART_FR_RX_EMPTY             UART0_FR_RXFE_Msk
#define AM_HAL_UART_FR_BUSY                 UART0_FR_BUSY_Msk
#define AM_HAL_UART_FR_DCD_DETECTED         UART0_FR_DCD_Msk
#define AM_HAL_UART_FR_DSR_READY            UART0_FR_DSR_Msk
#define AM_HAL_UART_FR_CTS                  UART0_FR_CTS_Msk
//! @}

//*****************************************************************************
//
//! @brief Transfer callback function prototype
//
//*****************************************************************************
typedef void (*am_hal_uart_callback_t)(uint32_t transactionStatus, void *pCallbackCtxt);

//*****************************************************************************
//
// UART configuration options.
//
//*****************************************************************************

//
//! Fifo interrupt level.
//
typedef enum
{
    AM_HAL_UART_FIFO_LEVEL_4  = 0,
    AM_HAL_UART_FIFO_LEVEL_8  = 1,
    AM_HAL_UART_FIFO_LEVEL_16 = 2,
    AM_HAL_UART_FIFO_LEVEL_24 = 3,
    AM_HAL_UART_FIFO_LEVEL_28 = 4,
}
am_hal_uart_fifo_level_e;

//
//! Number of data bits per UART frame.
//
typedef enum
{
    AM_HAL_UART_DATA_BITS_5 = 0,
    AM_HAL_UART_DATA_BITS_6 = 1,
    AM_HAL_UART_DATA_BITS_7 = 2,
    AM_HAL_UART_DATA_BITS_8 = 3,
}
am_hal_uart_data_bits_e;

//
//! Parity options.
//
typedef enum
{
    AM_HAL_UART_PARITY_ODD,
    AM_HAL_UART_PARITY_EVEN,
    AM_HAL_UART_PARITY_NONE,
}
am_hal_uart_parity_e;

//
//! Stop bit options.
//
typedef enum
{
    AM_HAL_UART_ONE_STOP_BIT = 0,
    AM_HAL_UART_TWO_STOP_BITS = 1,
}
am_hal_uart_stop_bits_e;

//
//! Flow control options.
//
typedef enum
{
    AM_HAL_UART_FLOW_CTRL_NONE     = 0,
    AM_HAL_UART_FLOW_CTRL_CTS_ONLY = UART0_CR_CTSEN_Msk,
    AM_HAL_UART_FLOW_CTRL_RTS_ONLY = UART0_CR_RTSEN_Msk,
    AM_HAL_UART_FLOW_CTRL_RTS_CTS  = (UART0_CR_CTSEN_Msk | UART0_CR_RTSEN_Msk),
}
am_hal_uart_flow_control_e;

//
//! Clock sources.
//
typedef enum
{
    AM_HAL_UART_CLOCK_SRC_HFRC    = 0,
    AM_HAL_UART_CLOCK_SRC_SYSPLL  = 1,
}
am_hal_uart_clock_src_e;

//
//! DMA async mode defs,
//! These are modes that are supported by the uart async functions
//
typedef enum
{
    AM_HAL_UART_DMA_NONE,
    AM_HAL_UART_DMA_TX_DOUBLE_BUFFER,
    AM_HAL_UART_DMA_TX_SINGLE_BUFFER,
    AM_HAL_UART_DMA_RX,                //!< This is not supported in Async Calls
    AM_HAL_UART_DMA_RX_TX_DOUBLE,      //!< This is not supported
    AM_HAL_UART_DMA_RX_TX_SINGLE,      //!< This is not supported
    AM_HAL_UART_DMA_MODE_ENTRIES,
}
am_hal_uart_async_dma_mode_e;

//
//! UART Config
//
typedef struct
{
    uint32_t                        ui32BaudRate;
    am_hal_uart_data_bits_e         eDataBits;     //!< Number of bits per frame
    am_hal_uart_parity_e            eParity;       //!< UART parity
    am_hal_uart_stop_bits_e         eStopBits;     //!< Number of stop bits
    am_hal_uart_flow_control_e      eFlowControl;  //!< Flow control option
    am_hal_uart_fifo_level_e        eTXFifoLevel;  //!< TX fifo interrupt level
    am_hal_uart_fifo_level_e        eRXFifoLevel;  //!< RX fifo interrupt level
    am_hal_uart_clock_src_e         eClockSrc;     //!< Clock source
    am_hal_uart_async_dma_mode_e    eAsyncDMAMode; //!< DmaMode
}
am_hal_uart_config_t;


//! @{
//! Structs to manage DMA tx queueing
#define AM_HAL_DMA_QUEUE_MAX_ENTRIES 3


//
//! defines a tx buffer descriptor used
//! for DMA transmissions in the async functions
//! These buffer queuing methods work because uart DMA does
//! not require an aligned start address
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
am_hal_uart_dma_tx_descriptor_entry_t;


//
//! This is the
//! for DMA transmissions in the async functions
//
typedef struct
{
    //
    //! buffer descriptors for a uart
    //!
    am_hal_uart_dma_tx_descriptor_entry_t tDescriptor[AM_HAL_DMA_QUEUE_MAX_ENTRIES];
    uint32_t                              queueStartAddr;        //!< start of the circular buffer
    uint32_t                              queueEndAddr;          //!< end of circular buffer
    am_hal_uart_dma_tx_descriptor_entry_t *activeDmaTxDesc;      //!< active dma queue, 0 when no DMA ongoing
    am_hal_uart_dma_tx_descriptor_entry_t *nextDmaWrtDesc;       //!< used in SingleBuffer mode

    am_hal_uart_async_dma_mode_e          eAsyncDmaMode;         //!< the dma mode used for this uart
    bool                                  bDmaQueueInited;       //!< true when data in this struct is inited

    uint8_t     align[2];
}
am_hal_uart_dma_tx_queue_t;

//! @}   circular buffer dma queue structs

//! @}  // Structs to manage DMA tx queueing


//*****************************************************************************
//
//! @brief The type of transfer to execute.
//! This is for the non-async (blocking and non-blocking) functions
//!
//! For blocking transfers, the CPU will poll until the requested number of
//! bytes have been transferred, or the timeout interval elapses, whichever
//! happens first.
//!
//! For non-blocking transfers, the CPU will read or write as many bytes as
//! possible immediately, and then the interrupt service routine will handle the
//! rest.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_UART_BLOCKING_WRITE,
    AM_HAL_UART_BLOCKING_READ,
    AM_HAL_UART_NONBLOCKING_WRITE,
    AM_HAL_UART_NONBLOCKING_READ,
}
am_hal_uart_transfer_type_e;

//
//! Transmit or receive enumerations.
//
typedef enum
{
    AM_HAL_UART_TX,
    AM_HAL_UART_RX,
}
am_hal_uart_dir_e;

//*****************************************************************************
//
//! UART transfer structure.
//
//*****************************************************************************
typedef struct
{

    //
    //! Data location to use for this transaction.
    //
    uint8_t *pui8Data;

    //
    //! How many bytes should we send?
    //
    uint32_t ui32NumBytes;

    //
    //! When the transaction is complete, this will be set to the number of
    //! bytes we read.
    //
    uint32_t *pui32BytesTransferred;

    //
    //! For blocking transactions, this determines how long the UART HAL should
    //! wait before aborting the transaction.
    //
    uint32_t ui32TimeoutMs;

    //
    //! For non-blocking transfers, the UART HAL will call this callback
    //! function as soon as the requested action is complete.
    //
    am_hal_uart_callback_t pfnCallback;

    //
    //! This context variable will be saved and provided to the callback
    //! function when it is called.
    //
    void *pvContext;

    //
    //! This context variable will be saved and provided to the callback
    //! function when it is called.
    //
    uint32_t ui32ErrorStatus;


    //
    //! Buffer
    //
    uint32_t *pui32TxBuffer;
    uint32_t *pui32RxBuffer;
    uint32_t ui32FdRxNumBytes;

    //
    //! DMA: RX variables for fullduplex
    //
    uint32_t *pui32FdRxBytesTransferred;

    am_hal_uart_callback_t pfnFdRxCallback; //!< read callback fcn
    void *pvFdRxContext;

    //
    //! Is this a write or a read?
    //
    am_hal_uart_transfer_type_e eType;

    //
    //! Transfer Direction (Transmit/Receive)
    //
    am_hal_uart_dir_e eDirection;
    //
    //! DMA: Priority 0 = Low (best effort); 1 = High (service immediately)
    //
    uint8_t  ui8Priority;

}
am_hal_uart_transfer_t;


//*****************************************************************************
//
//! UART error codes.
//! This is returned from the async uart ISR service
//
//*****************************************************************************
typedef enum
{
    AM_HAL_UART_ASYNC_STATUS_SUCCESS            = 0,
    AM_HAL_UART_ASYNC_STATUS_RX_QUEUE_FULL      = 0x0001,
    AM_HAL_UART_ASYNC_STATUS_RX_DATA_AVAIL      = 0x0002,
    AM_HAL_UART_ASYNC_STATUS_TX_QUEUE_FULL      = 0x0004,
    AM_HAL_UART_ASYNC_STATUS_TX_COMPLETE        = 0x0008,
    AM_HAL_UART_ASYNC_STATUS_TX_BUSY            = 0x0010,
    AM_HAL_UART_ASYNC_STATUS_TX_DMA_BUSY        = 0x0020,
    AM_HAL_UART_ASYNC_STATUS_TX_DMA_COMPLETE    = 0x0020,
    AM_HAL_UART_ASYNC_STATUS_DMA_ERROR          = 0x0040,
    AM_HAL_UART_ASYNC_STATUS_INTERNAL_DMA_ERROR = 0x0080,
    AM_HAL_UART_ASYNC_STATUS_FRM_ERROR          = UART0_DR_FEDATA_Msk,  // 0x0100 <-> 0x0800 are reserved for hardware outputs
    AM_HAL_UART_ASYNC_STATUS_PRTY_ERROR         = UART0_DR_PEDATA_Msk,
    AM_HAL_UART_ASYNC_STATUS_BRK_ERROR          = UART0_DR_BEDATA_Msk,
    AM_HAL_UART_ASYNC_STATUS_OVRN_ERROR         = UART0_DR_OEDATA_Msk,
    AM_HAL_UART_ASYNC_STATUS_INTRNL_MSK         = (AM_HAL_UART_ASYNC_STATUS_FRM_ERROR
                                                   | AM_HAL_UART_ASYNC_STATUS_OVRN_ERROR
                                                   | AM_HAL_UART_ASYNC_STATUS_PRTY_ERROR
                                                   | AM_HAL_UART_ASYNC_STATUS_BRK_ERROR),

    AM_HAL_UART_ASYNC_STATUS_x32                = 0x80000000,  // force all compilers to use 32bit
}
am_hal_uart_async_status_t;


//
//! UART Blocking Write Defaults.
//
#define AM_HAL_UART_BLOCKING_WRITE_DEFAULTS                                   \
{                                                                             \
    .eType = AM_HAL_UART_BLOCKING_WRITE,                                      \
    .pui8Data = 0,                                                            \
    .ui32NumBytes = 0,                                                        \
    .pui32BytesTransferred = 0,                                               \
    .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,                                \
    .pfnCallback = 0,                                                         \
    .pvContext = 0,                                                           \
    .ui32ErrorStatus = 0,                                                     \
};

//
//! UART Blocking Read Defaults.
//
#define AM_HAL_UART_BLOCKING_READ_DEFAULTS                                    \
{                                                                             \
    .eType = AM_HAL_UART_BLOCKING_READ,                                       \
    .pui8Data = 0,                                                            \
    .ui32NumBytes = 0,                                                        \
    .pui32BytesTransferred = 0,                                               \
    .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,                                \
    .pfnCallback = 0,                                                         \
    .pvContext = 0,                                                           \
    .ui32ErrorStatus = 0,                                                     \
}

//
//! UART Non-Blocking Write Defaults.
//
#define AM_HAL_UART_NONBLOCKING_WRITE_DEFAULTS                                \
{                                                                             \
    .eType = AM_HAL_UART_NONBLOCKING_WRITE,                                   \
    .pui8Data = 0,                                                            \
    .ui32NumBytes = 0,                                                        \
    .pui32BytesTransferred = 0,                                               \
    .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,                                \
    .pfnCallback = 0,                                                         \
    .pvContext = 0,                                                           \
    .ui32ErrorStatus = 0,                                                     \
}

//
//! UART Non-Blocking Read Defaults.
//
#define AM_HAL_UART_NONBLOCKING_READ_DEFAULTS                                 \
{                                                                             \
    .eType = AM_HAL_UART_NONBLOCKING_READ,                                    \
    .pui8Data = 0,                                                            \
    .ui32NumBytes = 0,                                                        \
    .pui32BytesTransferred = 0,                                               \
    .ui32TimeoutMs = AM_HAL_UART_WAIT_FOREVER,                                \
    .pfnCallback = 0,                                                         \
    .pvContext = 0,                                                           \
    .ui32ErrorStatus = 0,                                                     \
}

//
//! Enforce DMA hardware capability limits
//
#define AM_HAL_MAX_UART_DMA_SIZE           4095

//
//! Use this value if you want to keep a UART transaction blocking forever.
//
#define AM_HAL_UART_WAIT_FOREVER            0xFFFFFFFF

//
//! Clock frequency when using SYSPLL as clock source
//
#define AM_HAL_UART_PLLCLK_FREQ             49152000

//*****************************************************************************
//
//! @brief UART enable macro
//!
//! This will allow the user a more standard function call for enabling the UART
//! If the user wishes to retain the state, use am_hal_uart_power_control(..., true)
//!
//! @return HAL status code.
//
//*****************************************************************************
#define am_hal_uart_power_enable(phandle) am_hal_uart_power_control(phandle, AM_HAL_SYSCTRL_WAKE, false);

//*****************************************************************************
//
//! @brief UART disable macro
//!
//! This will allow the user a more standard function call for disabling the UART
//! If the user wishes to retain the state, use am_hal_uart_power_control(..., true)
//!
//! @return HAL status code.
//
//*****************************************************************************
#define am_hal_uart_power_disable(phandle) am_hal_uart_power_control(phandle, AM_HAL_SYSCTRL_DEEPSLEEP, false);

//*****************************************************************************
//
// External functions.
//
//*****************************************************************************

//*****************************************************************************
//
//! @brief Initialize the UART internal RX/TX queue
//!
//! @param pHandle is a pointer to a UART handle to initialize.
//! @param pui8TxBuffer is the pointer to buffer with length of ui32TxBufferSize.
//! @param ui32TxBufferSize is the length of Tx buffer.
//! @param pui8RxBuffer is the pointer to buffer with length of ui32RxBufferSize.
//! @param ui32RxBufferSize is the length of Rx buffer.
//!
//! This function enables the internal UART queue (FIFO) for UART Tx/Rx. When
//! Tx or Rx Queue are enabled, the UART data is stored internally when UART
//! hardware FIFO is full.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_buffer_configure(void *pHandle,
                                             uint8_t *pui8TxBuffer,
                                             uint32_t ui32TxBufferSize,
                                             uint8_t *pui8RxBuffer,
                                             uint32_t ui32RxBufferSize);

//*****************************************************************************
//
//! @brief Initialize the UART
//!
//! @param ui32Module is the module number of the UART to use.
//! @param ppHandle is a pointer to a UART handle to initialize.
//!
//! This function prepares a UART module for use, and returns an initialized
//! handle for use by the application.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_initialize(uint32_t ui32Module, void **ppHandle);

//*****************************************************************************
//
//! @brief Deinitialize the UART interface.
//!
//! @param pHandle is a previously initialized UART handle.
//!
//! This function effectively disables future calls to interact with the UART
//! referred to by \e Handle. The user may call this function if UART operation
//! is no longer desired.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_deinitialize(void *pHandle);

//*****************************************************************************
//
//! @brief Change the power state of the UART module.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param ePowerState is the desired power state of the UART.
//! @param bRetainState is a flag to ask the HAL to save UART registers.
//!
//! This function can be used to switch the power to the UART on or off. If \e
//! bRetainState is true during a powerdown operation, it will store the UART
//! configuration registers to SRAM, so it can restore them on power-up.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_power_control(void *pHandle,
                                          uint32_t ePowerState,
                                          bool bRetainState);

//*****************************************************************************
//
//! @brief Used to configure basic UART settings.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param psConfig is a structure of UART configuration options.
//!
//! This function takes the options from an \e am_hal_uart_config_t structure,
//! and applies them to the UART referred to by \e pHandle.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_configure(void *pHandle,
                                      const am_hal_uart_config_t *psConfig);

//*****************************************************************************
//
//! @brief Read from the UART RX FIFO.
//!
//! @param pHandle is the UART handle to use.
//! @param pui8Data is the buffer to read bytes into.
//! @param ui32NumBytes is the number of bytes to try to read.
//! @param pui32NumBytesRead will be set to the number of bytes actually read.
//!
//! This function reads bytes directly from the UART FIFO into the specified
//! data location.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_fifo_read(void *pHandle,
                                      uint8_t *pui8Data,
                                      uint32_t ui32NumBytes,
                                      uint32_t *pui32NumBytesRead);

//*****************************************************************************
//
//! @brief Write to the UART TX FIFO
//!
//! @param pHandle is the UART handle to use.
//! @param pui8Data is the buffer to write bytes from.
//! @param ui32NumBytes is the number of bytes to attempt to write.
//! @param pui32NumBytesWritten will be set to the number of bytes written.
//!
//! This function writes bytes from memory into the UART FIFO.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_fifo_write(void *pHandle,
                                       uint8_t *pui8Data,
                                       uint32_t ui32NumBytes,
                                       uint32_t *pui32NumBytesWritten);

//*****************************************************************************
//
//! @brief Drain the UART RX FIFO.
//!
//! @param pHandle is the UART handle to use.
//!
//! This function reads bytes directly from the UART FIFO and discards them.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_rx_fifo_drain(void *pHandle);

//*****************************************************************************
//
//! @brief Wait for the UART TX to become idle
//!
//! @param pHandle is the handle for the UART to operate on.
//!
//! This function waits (polling) for all data in the UART TX FIFO and UART TX
//! buffer (if configured) to be fully sent on the physical UART interface.
//! This is not the most power-efficient way to wait for UART idle, but it can be
//! useful in simpler applications, or where power-efficiency is less important.
//!
//! Once this function returns, the UART can be safely disabled without
//! interfering with any previous transmissions.
//!
//! For a more power-efficient way to shut down the UART, check the
//! \e am_hal_uart_interrupt_service() function.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_tx_flush(void *pHandle);

//*****************************************************************************
//
//! @brief Abort active uart tx transaction
//!
//! @param pHandle is the handle for the UART to operate on.
//!
//! This function reset uart tx states and abort active tx transaction, it should
//! be called after am_hal_uart_tx_flush.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern void am_hal_uart_tx_abort(void *pHandle);

//*****************************************************************************
//
//! @brief Abort active uart rx transaction
//!
//! @param pHandle is the handle for the UART to operate on.
//!
//! This function reset uart rx states and abort active rx transaction.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern void am_hal_uart_rx_abort(void *pHandle);

//*****************************************************************************
//
//! @brief Run a UART transaction.
//!
//! @param pHandle is the UART handle to use.
//! @param psTransfer is a structure defining the desired transfer.
//!
//! This function is used for all UART transactions. See the documentation for
//! \e am_hal_uart_transfer_t for more information about the options.
//!
//! @return HAL status code.
//
//*****************************************************************************
extern uint32_t am_hal_uart_transfer(void *pHandle,
                                     const am_hal_uart_transfer_t *psTransfer);

//*****************************************************************************
//
//! @brief Abort UART DMA transaction.
//!
//! @param pHandle is the UART handle to use.
//!
//! This function disables and clears UART DMA.
//!
//! @return void.
//
//*****************************************************************************
extern void am_hal_uart_dma_abort(void *pHandle);

//*****************************************************************************
//
//! @brief UART dma transfer function
//!
//! @param pHandle      - handle for the interface.
//! @param psTransaction - pointer to the uniform transaction control structure.
//!
//! This function performs a transaction on the UART in DMA mode.
//!
//! @return status      - generic or interface specific status.
//
//*****************************************************************************
extern uint32_t am_hal_uart_dma_transfer(void *pHandle, am_hal_uart_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief UART dma full duplex transfer function
//!
//! @param pTXHandle       - handle for the TX UART.
//! @param pRXHandle       - handle for the RX UART.
//! @param psTransaction   - pointer to the uniform transaction control structure.
//!
//! This function performs a transaction on the UART in full duplex DMA mode.
//!
//! @return success or error code
//
//*****************************************************************************
extern uint32_t am_hal_uart_dma_fullduplex_transfer(void *pTXHandle, void *pRXHandle, am_hal_uart_transfer_t *psTransaction);

//*****************************************************************************
//
//! @brief Read the UART flags.
//!
//! @param pHandle is the UART handle to use.
//! @param pui32Flags is the destination pointer for the UART flags.
//!
//! The UART hardware provides some information about the state of the physical
//! interface at all times. This function provides a way to read that data
//! directly. Below is a list of all possible UART flags.
//!
//! These correspond directly to the bits in the UART_FR register.
//!
//! @code
//!
//! AM_HAL_UART_FR_TX_EMPTY
//! AM_HAL_UART_FR_RX_FULL
//! AM_HAL_UART_FR_TX_FULL
//! AM_HAL_UART_FR_RX_EMPTY
//! AM_HAL_UART_FR_BUSY
//! AM_HAL_UART_FR_DCD_DETECTED
//! AM_HAL_UART_FR_DSR_READY
//! AM_HAL_UART_FR_CTS
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_flags_get(void *pHandle, uint32_t *pui32Flags);

//*****************************************************************************
//
//! @brief Enable interrupts.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param ui32IntMask is the bitmask of interrupts to enable.
//!
//! This function enables the UART interrupt(s) given by ui32IntMask. If
//! multiple interrupts are desired, they can be OR'ed together.
//!
//! @note This function need not be called for UART FIFO interrupts if the UART
//! buffer service provided by \e am_hal_uart_buffer_configure() and \e
//! am_hal_uart_interrupt_service() is already in use. Non-FIFO-related
//! interrupts do require the use of this function.
//!
//! The full list of interrupts is given by the following:
//!
//! @code
//!
//! AM_HAL_UART_INT_OVER_RUN
//! AM_HAL_UART_INT_BREAK_ERR
//! AM_HAL_UART_INT_PARITY_ERR
//! AM_HAL_UART_INT_FRAME_ERR
//! AM_HAL_UART_INT_RX_TMOUT
//! AM_HAL_UART_INT_TX
//! AM_HAL_UART_INT_RX
//! AM_HAL_UART_INT_DSRM
//! AM_HAL_UART_INT_DCDM
//! AM_HAL_UART_INT_CTSM
//! AM_HAL_UART_INT_TXCMP
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_enable(void *pHandle,
                                             uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief Disable interrupts.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param ui32IntMask is the bitmask of interrupts to disable.
//!
//! This function disables the UART interrupt(s) given by ui32IntMask. If
//! multiple interrupts need to be disabled, they can be OR'ed together.
//!
//! @note This function need not be called for UART FIFO interrupts if the UART
//! buffer service provided by \e am_hal_uart_buffer_configure() and \e
//! am_hal_uart_interrupt_service() is already in use. Non-FIFO-related
//! interrupts do require the use of this function.
//!
//! The full list of interrupts is given by the following:
//!
//! @code
//!
//! AM_HAL_UART_INT_OVER_RUN
//! AM_HAL_UART_INT_BREAK_ERR
//! AM_HAL_UART_INT_PARITY_ERR
//! AM_HAL_UART_INT_FRAME_ERR
//! AM_HAL_UART_INT_RX_TMOUT
//! AM_HAL_UART_INT_TX
//! AM_HAL_UART_INT_RX
//! AM_HAL_UART_INT_DSRM
//! AM_HAL_UART_INT_DCDM
//! AM_HAL_UART_INT_CTSM
//! AM_HAL_UART_INT_TXCMP
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_disable(void *pHandle,
                                              uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief Clear interrupt status.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param ui32IntMask is the bitmask of interrupts to clear.
//!
//! This function clears the UART interrupt(s) given by ui32IntMask. If
//! multiple interrupts need to be cleared, they can be OR'ed together.
//!
//! The full list of interrupts is given by the following:
//!
//! @code
//!
//! AM_HAL_UART_INT_OVER_RUN
//! AM_HAL_UART_INT_BREAK_ERR
//! AM_HAL_UART_INT_PARITY_ERR
//! AM_HAL_UART_INT_FRAME_ERR
//! AM_HAL_UART_INT_RX_TMOUT
//! AM_HAL_UART_INT_TX
//! AM_HAL_UART_INT_RX
//! AM_HAL_UART_INT_DSRM
//! AM_HAL_UART_INT_DCDM
//! AM_HAL_UART_INT_CTSM
//! AM_HAL_UART_INT_TXCMP
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_clear(void *pHandle,
                                            uint32_t ui32IntMask);

//*****************************************************************************
//
//! @brief Read interrupt status.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param pui32Status is the returned interrupt status (all bits OR'ed
//! together)
//! @param bEnabledOnly determines whether to read interrupts that were not
//! enabled.
//!
//! This function reads the status the UART interrupt(s) if \e bEnabled is
//! true, it will only return the status of the enabled interrupts. Otherwise,
//! it will return the status of all interrupts, enabled or disabled.
//!
//! The full list of interrupts is given by the following:
//!
//! @code
//!
//! AM_HAL_UART_INT_OVER_RUN
//! AM_HAL_UART_INT_BREAK_ERR
//! AM_HAL_UART_INT_PARITY_ERR
//! AM_HAL_UART_INT_FRAME_ERR
//! AM_HAL_UART_INT_RX_TMOUT
//! AM_HAL_UART_INT_TX
//! AM_HAL_UART_INT_RX
//! AM_HAL_UART_INT_DSRM
//! AM_HAL_UART_INT_DCDM
//! AM_HAL_UART_INT_CTSM
//! AM_HAL_UART_INT_TXCMP
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_status_get(void *pHandle,
                                                 uint32_t *pui32Status,
                                                 bool bEnabledOnly);

//*****************************************************************************
//
//! @brief Check to see which interrupts are enabled.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param pui32IntMask is the current set of interrupt enable bits (all bits
//!                     OR'ed together)
//!
//! This function checks the UART Interrupt Enable Register to see which UART
//! interrupts are currently enabled. The result will be an interrupt mask with
//! one bit set for each of the currently enabled UART interrupts.
//!
//! The full set of UART interrupt bits is given by the list below:
//!
//! @code
//!
//! AM_HAL_UART_INT_OVER_RUN
//! AM_HAL_UART_INT_BREAK_ERR
//! AM_HAL_UART_INT_PARITY_ERR
//! AM_HAL_UART_INT_FRAME_ERR
//! AM_HAL_UART_INT_RX_TMOUT
//! AM_HAL_UART_INT_TX
//! AM_HAL_UART_INT_RX
//! AM_HAL_UART_INT_DSRM
//! AM_HAL_UART_INT_DCDM
//! AM_HAL_UART_INT_CTSM
//! AM_HAL_UART_INT_TXCMP
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_enable_get(void *pHandle, uint32_t *pui32IntMask);
//*****************************************************************************
//
//! @brief transfer data between hardware and software queue in the background.
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param ui32Status is the interrupt flag and can be any combination of
//!  AM_HAL_UART_INT_RX, AM_HAL_UART_INT_RX_TMOUT, or AM_HAL_UART_INT_TX.
//!
//! This function should be called from the ISR and then receive/transmit data
//! from/to hardware FIFO.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_service(void *pHandle,
                                              uint32_t ui32Status);

//*****************************************************************************
//
//! @brief //! Manage UART ISR used when uart fifos and tx and rx queues are enabled
//! This is a nonblocking, non-signalling uart driver, it saves incoming rx data
//! in the rx queue, and transmits tx data from the queue.
//!
//! @param pHandle  uart handle
//!
//! @return am_hal_uart_async_status_t
//
//*****************************************************************************
extern am_hal_uart_async_status_t am_hal_uart_interrupt_queue_service(void *pHandle);

//*****************************************************************************
//
//! @brief Choose correct function based on DMA mode for tx append.
//! @param pHandle  pointer to uart handle
//! @param pui8Buff  pointer to tx buffer
//! @param ui32NumBytes number of bytes to transmit
//!
//! @return standard hal status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_async_uart_append_tx(void *pHandle,
                                            uint8_t *pui8Buff,
                                            uint32_t ui32NumBytes);

//*****************************************************************************
//
//
//! @brief Append data into the uart tx output queue
//! using fifos not DMA
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param pui8Buff  pointer to data buffer
//! @param ui32NumBytes  number of bytes to transmit
//!
//! @return  uart hal status (standard hal status)
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_append_tx_fifo( void *pHandle,
                                       uint8_t *pui8Buff,
                                       uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief queue data to transmit via UART DMA
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param pui8Buff pointer to data buffer
//! @param ui32NumBytes  number of bytes to send
//!
//! @return uart status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_append_tx_double(void *pHandle,
                                             uint8_t *pui8Buff,
                                             uint32_t ui32NumBytes);
//*****************************************************************************
//
//! @brief queue data to transmit via UART DMA
//!
//! @param pHandle is the handle for the UART to operate on.
//! @param pui8Buff pointer to data buffer
//! @param ui32NumBytes  number of bytes to send
//!
//! @return uart status
//
//*****************************************************************************
extern am_hal_uart_errors_t am_hal_uart_append_tx_single(void *pHandle,
                                             uint8_t *pui8Buff,
                                             uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief Get Rx Data from Async buffered data
//! This function will unload data from the queue and load the data into
//! a user supplied buffer.
//!
//! @param pHandle              pointer to handle for the UART to operate on.
//! @param pui8DestBuff         pointer to rx data buffer
//! @param ui32MaxBytesToRead   max number of bytes to read
//!
//! @return number of bytes loaded into data buffer
//
//*****************************************************************************
extern uint32_t am_hal_uart_async_get_rx_data(void *pHandle,
                                                uint8_t *pui8DestBuff,
                                                uint32_t ui32MaxBytesToRead);

//*****************************************************************************
//
//! @brief // Setup the DMA queue buffer descriptors depending on the dma mode chosen
//!
//! @param pHandle       Is pointer to the handle for this UART
//! @param eAsyncDmaMode The dma mode used for this UART
//! @param psDmaQ        Pointer to a global the calling program must allocate (cannot be on the stack)
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t am_hal_uart_dmaQueueInit(void *pHandle,
                                         am_hal_uart_async_dma_mode_e eAsyncDmaMode,
                                         am_hal_uart_dma_tx_queue_t *psDmaQ);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_UART_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

