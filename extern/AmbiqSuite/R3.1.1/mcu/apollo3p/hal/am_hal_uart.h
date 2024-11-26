//*****************************************************************************
//
//! @file am_hal_uart.h
//!
//! @brief Functions for accessing and configuring the UART.
//!
//! @addtogroup uart3p UART - UART Functionality
//! @ingroup apollo3p_hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_UART_H
#define AM_HAL_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! CMSIS-style macro for handling a variable MSPI module number.
//
//*****************************************************************************
#define UARTn(n)    ((UART0_Type*)(UART0_BASE + (n * (UART1_BASE - UART0_BASE))))

// #### INTERNAL BEGIN ####
#if 0
//*****************************************************************************
//
// UART clock speed defs
//
//*****************************************************************************
typedef enum
{
    eUART_CLK_SPEED_DEFAULT = 0,
    eUART_CLK_SPEED_NOCLK,
    eUART_CLK_SPEED_3MHZ,
    eUART_CLK_SPEED_6MHZ,
    eUART_CLK_SPEED_12MHZ,
    eUART_CLK_SPEED_24MHZ,
    eUART_CLK_SPEED_X32 = 0x80000000,
}
am_hal_uart_clock_speed_e;
#endif
// #### INTERNAL END ####


//*****************************************************************************
//
//! Control operations.
//
//*****************************************************************************
typedef enum
{
    AM_HAL_UART_CONTROL_CLKSEL,
} am_hal_uart_control_e;

//*****************************************************************************
//
//! Module clock speed options
//
//*****************************************************************************
typedef enum
{
    eUART_CLK_SPEED_DEFAULT = 0,
    eUART_CLK_SPEED_NOCLK   = UART0_CR_CLKSEL_NOCLK + 1,
    eUART_CLK_SPEED_24MHZ   = UART0_CR_CLKSEL_24MHZ + 1,
    eUART_CLK_SPEED_12MHZ   = UART0_CR_CLKSEL_12MHZ + 1,
    eUART_CLK_SPEED_6MHZ    = UART0_CR_CLKSEL_6MHZ + 1,
    eUART_CLK_SPEED_3MHZ    = UART0_CR_CLKSEL_3MHZ + 1,
    eUART_CLK_SPEED_INVALID,
} am_hal_uart_clock_speed_e;



//*****************************************************************************
//
//! UART configuration options.
//
//*****************************************************************************
typedef struct
{
    //
    //! Standard UART options.
    //
    uint32_t ui32BaudRate;
    uint32_t ui32DataBits;
    uint32_t ui32Parity;
    uint32_t ui32StopBits;
    uint32_t ui32FlowControl;

    //
    //! Additional options.
    //
    uint32_t ui32FifoLevels;

    //
    //! Buffers
    //
    uint8_t *pui8TxBuffer;
    uint32_t ui32TxBufferSize;
    uint8_t *pui8RxBuffer;
    uint32_t ui32RxBufferSize;
// #### INTERNAL BEGIN ####
#if 0
    //
    // Clock speed setting
    //
    am_hal_uart_clock_speed_e e32ClockSpeed;
#endif
// #### INTERNAL END ###
}
am_hal_uart_config_t;

//*****************************************************************************
//
//! @brief UART transfer structure.
//
//! This structure describes a UART transaction that can be performed by \e
//! am_hal_uart_transfer()
//
//*****************************************************************************
typedef struct
{
    //! Determines whether data should be read or written.
    //!
    //! Should be either AM_HAL_UART_WRITE or AM_HAL_UART_READ
    uint32_t ui32Direction;

    //! Pointer to data to be sent, or space to fill with received data.
    uint8_t *pui8Data;

    //! Number of bytes to send or receive.
    uint32_t ui32NumBytes;

    //! Timeout in milliseconds.
    //!
    //! Given a timeout value, the \e am_hal_uart_transfer() function will keep
    //! trying to transfer data until either the number of bytes is satisfied,
    //! or the time runs out. If provided with a value of zero, the transfer
    //! function will only send as much data as it can immediately deal with.
    //! If provided with a timeout value of \e AM_HAL_UART_WAIT_FOREVER, the
    //! function will block until either the final "read" byte is received or
    //! the final "write" byte is placed in the output buffer.
    uint32_t ui32TimeoutMs;

    //! Number of bytes successfully transferred.
    uint32_t *pui32BytesTransferred;
}
am_hal_uart_transfer_t;

//*****************************************************************************
//
//! Maximum baudrate supported is 921600 for Apollo3-A1 and 1.5Mbaud for
//! Apollo3-B0.
//
//*****************************************************************************
#define AM_HAL_UART_MAXIMUM_BAUDRATE        921600

//*****************************************************************************
//
//! @name Uart transfer options.
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_WRITE                   0
#define AM_HAL_UART_READ                    1
#define AM_HAL_UART_WAIT_FOREVER            0xFFFFFFFF
//! @}

//*****************************************************************************
//
//! UART conficuration option values.
//
//*****************************************************************************

//*****************************************************************************
//
//! @name Data bits Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_DATA_BITS_8             (_VAL2FLD(UART0_LCRH_WLEN, 3))
#define AM_HAL_UART_DATA_BITS_7             (_VAL2FLD(UART0_LCRH_WLEN, 2))
#define AM_HAL_UART_DATA_BITS_6             (_VAL2FLD(UART0_LCRH_WLEN, 1))
#define AM_HAL_UART_DATA_BITS_5             (_VAL2FLD(UART0_LCRH_WLEN, 0))
//! @}

//*****************************************************************************
//
//! @name Parity Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_PARITY_NONE             0
#define AM_HAL_UART_PARITY_ODD              UART0_LCRH_PEN_Msk
#define AM_HAL_UART_PARITY_EVEN             (UART0_LCRH_PEN_Msk |   \
                                             UART0_LCRH_EPS_Msk)
//! @}

//*****************************************************************************
//
//! @name Stop Bits Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_ONE_STOP_BIT            (_VAL2FLD(UART0_LCRH_STP2, 0))
#define AM_HAL_UART_TWO_STOP_BITS           (_VAL2FLD(UART0_LCRH_STP2, 1))
//! @}

//*****************************************************************************
//
//! @name Flow control Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_FLOW_CTRL_NONE          0
#define AM_HAL_UART_FLOW_CTRL_CTS_ONLY      UART0_CR_CTSEN_Msk
#define AM_HAL_UART_FLOW_CTRL_RTS_ONLY      UART0_CR_RTSEN_Msk
#define AM_HAL_UART_FLOW_CTRL_RTS_CTS       (UART0_CR_CTSEN_Msk |   \
                                             UART0_CR_RTSEN_Msk)
//! @}
//*****************************************************************************
//
//! @name FIFO enable/disable Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_FIFO_ENABLE             (_VAL2FLD(UART0_LCRH_FEN, 1))
#define AM_HAL_UART_FIFO_DISABLE            (_VAL2FLD(UART0_LCRH_FEN, 0))
//! @}

//*****************************************************************************
//
//! @name TX FIFO interrupt level settings Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_TX_FIFO_1_8             (_VAL2FLD(UART0_IFLS_TXIFLSEL, 0))
#define AM_HAL_UART_TX_FIFO_1_4             (_VAL2FLD(UART0_IFLS_TXIFLSEL, 1))
#define AM_HAL_UART_TX_FIFO_1_2             (_VAL2FLD(UART0_IFLS_TXIFLSEL, 2))
#define AM_HAL_UART_TX_FIFO_3_4             (_VAL2FLD(UART0_IFLS_TXIFLSEL, 3))
#define AM_HAL_UART_TX_FIFO_7_8             (_VAL2FLD(UART0_IFLS_TXIFLSEL, 4))
//! @}

//*****************************************************************************
//
//! @name RX FIFO interrupt level settings Macros
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_RX_FIFO_1_8             (_VAL2FLD(UART0_IFLS_RXIFLSEL, 0))
#define AM_HAL_UART_RX_FIFO_1_4             (_VAL2FLD(UART0_IFLS_RXIFLSEL, 1))
#define AM_HAL_UART_RX_FIFO_1_2             (_VAL2FLD(UART0_IFLS_RXIFLSEL, 2))
#define AM_HAL_UART_RX_FIFO_3_4             (_VAL2FLD(UART0_IFLS_RXIFLSEL, 3))
#define AM_HAL_UART_RX_FIFO_7_8             (_VAL2FLD(UART0_IFLS_RXIFLSEL, 4))
//! @}

//*****************************************************************************
//
//! @name UART interrupts Macros
//! @{
//
//*****************************************************************************
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
//! @}

//*****************************************************************************
//
//! @name UART Flag Register
//! @brief Macro definitions for UART Flag Register Bits.
//!
//! They may be used with the \e am_hal_uart_flags_get() function.
//!
//! @{
//
//*****************************************************************************
#define AM_HAL_UART_FR_TX_EMPTY             (_VAL2FLD(UART0_FR_TXFE, UART0_FR_TXFE_XMTFIFO_EMPTY))
#define AM_HAL_UART_FR_RX_FULL              (_VAL2FLD(UART0_FR_RXFF, UART0_FR_RXFF_RCVFIFO_FULL))
#define AM_HAL_UART_FR_TX_FULL              (_VAL2FLD(UART0_FR_TXFF, UART0_FR_TXFF_XMTFIFO_FULL))
#define AM_HAL_UART_FR_RX_EMPTY             (_VAL2FLD(UART0_FR_RXFE, UART0_FR_RXFE_RCVFIFO_EMPTY))
#define AM_HAL_UART_FR_BUSY                 (_VAL2FLD(UART0_FR_BUSY, UART0_FR_BUSY_BUSY))
#define AM_HAL_UART_FR_DCD_DETECTED         (_VAL2FLD(UART0_FR_DCD,  UART0_FR_DCD_DETECTED))
#define AM_HAL_UART_FR_DSR_READY            (_VAL2FLD(UART0_FR_DSR,  UART0_FR_DSR_READY))
#define AM_HAL_UART_FR_CTS                  UART0_FR_CTS_Msk
//! @}

//*****************************************************************************
//
//! UART FIFO size for Apollo3.
//
//*****************************************************************************
#define AM_HAL_UART_FIFO_MAX                32

//*****************************************************************************
//
//! Turn timeouts into indefinite waits.
//
//*****************************************************************************
#define AM_HAL_UART_WAIT_FOREVER            0xFFFFFFFF

typedef enum
{
    AM_HAL_UART_STATUS_SUCCESS   = 0,
    AM_HAL_UART_STATUS_RX_QUEUE_FULL = 0x0001,
    AM_HAL_UART_STATUS_RX_DATA_AVAIL = 0x0002,
    AM_HAL_UART_STATUS_TX_QUEUE_FULL = 0x0004,
    AM_HAL_UART_STATUS_TX_COMPLETE   = 0x0008,
    AM_HAL_UART_STATUS_TX_BUSY       = 0x0010,
    AM_HAL_UART_STATUS_FRM_ERROR     = UART0_DR_FEDATA_Msk,
    AM_HAL_UART_STATUS_PRTY_ERROR    = UART0_DR_PEDATA_Msk,
    AM_HAL_UART_STATUS_BRK_ERROR     = UART0_DR_BEDATA_Msk,
    AM_HAL_UART_STATUS_OVRN_ERROR    = UART0_DR_OEDATA_Msk,
    AM_HAL_UART_STATUS_INTRNL_MSK    = (AM_HAL_UART_STATUS_FRM_ERROR
                                        | AM_HAL_UART_STATUS_OVRN_ERROR
                                        | AM_HAL_UART_STATUS_PRTY_ERROR
                                        | AM_HAL_UART_STATUS_BRK_ERROR),

    AM_HAL_UART_STATUS_x32           = 0x80000000,
}
am_hal_uart_status_t;

typedef enum
{
    AM_HAL_UART_ERR_BUS_ERROR = AM_HAL_STATUS_MODULE_SPECIFIC_START,
    AM_HAL_UART_ERR_RX_QUEUE_FULL,
    AM_HAL_UART_ERR_CLOCK_NOT_CONFIGURED,
    AM_HAL_UART_ERR_BAUDRATE_NOT_POSSIBLE,
}
am_hal_uart_errors_t;


//*****************************************************************************
//
//! @brief Initialize the UART interface.
//!
//! @param ui32Module - The module number for the UART to initialize.
//! @param ppHandle - The location to write the UART handle.
//!
//! This function sets internal tracking variables associated with a specific
//! UART module. It should be the first UART API called for each UART module in
//! use. The handle can be used to interact with the UART
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_initialize(uint32_t ui32Module, void **ppHandle);

//*****************************************************************************
//
//! @brief Deinitialize the UART interface.
//!
//! @param pHandle - A previously initialized UART handle.
//!
//! This function effectively disables future calls to interact with the UART
//! refered to by \e pHandle. The user may call this function if UART operation
//! is no longer desired.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_deinitialize(void *pHandle);

//*****************************************************************************
//
//! @brief Change the power state of the UART module.
//!
//! @param pHandle - The handle for the UART to operate on.
//! @param ePowerState - the desired power state of the UART.
//! @param bRetainState - A flag to ask the HAL to save UART registers.
//!
//! This function can be used to switch the power to the UART on or off. If \e
//! bRetainState is true during a powerdown operation, it will store the UART
//! configuration registers to SRAM, so it can restore them on power-up.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_power_control(void *pHandle,
                                          am_hal_sysctrl_power_state_e ePowerState,
                                          bool bRetainState);

//*****************************************************************************
//
//! @brief Used to configure basic UART settings.
//!
//! @param pHandle - The handle for the UART to operate on.
//! @param psConfig - A structure of UART configuration options.
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
//! @brief Transfer data through the UART interface.
//!
//! @param pHandle - The handle for the UART to operate on.
//! @param pTransfer - A structure describing the operation.
//!
//! This function executes a transaction as described by the \e
//! am_hal_uart_transfer_t structure. It can either read or write, and it will
//! take advantage of any buffer space provided by the \e
//! am_hal_uart_configure() function.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_transfer(void *pHandle,
                                     const am_hal_uart_transfer_t *pTransfer);

//*****************************************************************************
//
//! @brief Wait for the UART TX to become idle
//!
//! @param pHandle - The handle for the UART to operate on.
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
//! @brief Read the UART flags.
//!
//! @param pHandle - The handle for the UART to operate on.
//! @param pui32Flags - The destination pointer for the UART flags.
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
//! @brief Read the UART FIFO directly.
//!
//! @param pHandle          - The handle for the UART to operate on.
//! @param pui8Data         - A pointer where the UART data should be written.
//! @param ui32NumBytes     - The number of bytes to transfer.
//! @param pui32NumBytesRead - The number of bytes actually transferred.
//!
//! This function reads the UART FIFO directly, and writes the resulting bytes
//! to pui8Data. Since the UART FIFO hardware has no direct size indicator, the
//! caller can only specify the maximum number of bytes they can handle. This
//! function will try to read as many bytes as possible. At the end of the
//! transfer, the number of bytes actually transferred will be written to the
//! pui32NumBytesRead parameter.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART error.
//
//*****************************************************************************
extern uint32_t am_hal_uart_fifo_read(void *pHandle,
                                      uint8_t *pui8Data,
                                      uint32_t ui32NumBytes,
                                      uint32_t *pui32NumBytesRead);

//*****************************************************************************
//
//! @brief Write the UART FIFO directly.
//!
//! @param pHandle          - The handle for the UART to operate on.
//! @param pui8Data         - A pointer where the UART data should be written.
//! @param ui32NumBytes     - The number of bytes to transfer.
//! @param pui32NumBytesWritten - The number of bytes actually transferred.
//!
//! This function reads from pui8Data, and writes the requested number of bytes
//! directly to the UART FIFO. Since the UART FIFO hardware has no register to
//! tell us how much free space it has, the caller can only specify the number
//! of bytes they would like to send. This function will try to write as many
//! bytes as possible up to the requested number. At the end of the transfer,
//! the number of bytes actually transferred will be written to the
//! pui32NumBytesWritten parameter.
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART error.
//
//*****************************************************************************
extern uint32_t am_hal_uart_fifo_write(void *pHandle,
                                       uint8_t *pui8Data,
                                       uint32_t ui32NumBytes,
                                       uint32_t *pui32NumBytesWritten);

//*****************************************************************************
//
//! @brief This function handles the UART buffers during UART interrupts.
//!
//! @param pHandle          - The handle for the UART to operate on.
//! @param ui32Status       - The interrupt status at the time of ISR entry.
//! @param pui32UartTxIdle  - Can be used to store the UART idle status.
//!
//! The main purpose of this function is to manage the UART buffer system. Any
//! buffers configured by \e am_hal_uart_buffer_configure will be managed by
//! this service routine. Data queued for transmit will be added to the UART TX
//! FIFO as space allows, and data stored in the UART RX FIFO will be copied
//! out and stored in the RX buffer. This function will skip this transfer for
//! any buffer that has not been configured.
//!
//! In addition, this function can be used to alert the caller when the UART
//! becomes idle via the optional \e pui32UartTxIdle argument. This function
//! will set this variable any time it completes its operation and the UART TX
//! channel is no longer in use (including both the FIFO and any configured
//! buffer). To make sure this happens as early as possible, the user may
//! enable the UART_TXCMP interrupt as shown below.
//!
//! For RTOS-enabled cases, this function does not necessarily need to be
//! called inside the actual ISR for the UART, but it should be called promptly
//! in response to the receipt of a UART TX, RX, or RX timeout interrupt. If
//! the service routine is not called quickly enough, the caller risks an RX
//! FIFO overflow (data can be lost here), or a TX FIFO underflow (usually not
//! harmful).
//!
//! @code
//!
//! void
//! am_uart_isr(void)
//! {
//!     //
//!     // Service the FIFOs as necessary, and clear the interrupts.
//!     //
//!     uint32_t ui32Status;
//!     uint32_t ui32UartIdle;
//!
//!     am_hal_uart_interrupt_status_get(UART, &ui32Status, true);
//!     am_hal_uart_interrupt_clear(UART, ui32Status);
//!     am_hal_uart_interrupt_service(UART, ui32Status, &ui32UartIdle);
//!
//!     ui32TXDoneFlag = ui32UartIdle;
//! }
//!
//! int
//! main(void)
//! {
//!     ...
//!
//!     // Initialize, power up, and configure the UART.
//!     am_hal_uart_initialize(0, &UART);
//!     am_hal_uart_power_control(UART, AM_HAL_SYSCTRL_WAKE, false);
//!     am_hal_uart_configure(UART, &sUartConfig);
//!
//!     ...
//!
//!     // Enable the UART0 interrupt vector.
//!     am_hal_uart_interrupt_enable(UART, AM_REG_UART_IER_TXCMPMIM_M);
//!     am_hal_interrupt_enable(AM_HAL_INTERRUPT_UART0);
//!     am_hal_interrupt_master_enable();
//! }
//!
//! @endcode
//!
//! @return AM_HAL_STATUS_SUCCESS or applicable UART errors.
//
//*****************************************************************************
extern uint32_t am_hal_uart_interrupt_service(void *pHandle,
                                              uint32_t ui32Status,
                                              uint32_t *pui32UartTxIdle);

//*****************************************************************************
//
//! @brief Enable interrupts.
//!
//! @param pHandle      - The handle for the UART to operate on.
//! @param ui32IntMask  - The bitmask of interrupts to enable.
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
//! @param pHandle     - The handle for the UART to operate on.
//! @param ui32IntMask - The bitmask of interrupts to disable.
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
//! @param pHandle     - The handle for the UART to operate on.
//! @param ui32IntMask - The bitmask of interrupts to clear.
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
//! @param pHandle     - The handle for the UART to operate on.
//! @param pui32Status - The returned interrupt status (all bits OR'ed
//! together)
//! @param bEnabledOnly - determines whether to read interrupts that were not
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
//! @param pHandle  - The handle for the UART to operate on.
//!
//! @param pui32IntMask - The current set of interrupt enable bits (all bits
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
//! @brief Apply various specific commands/controls on the UART module
//!
//! @param pHandle - The handle for the UART to operate on.
//! @param eControl
//! @param pArgs
//! @return uint32_t
//*****************************************************************************
extern uint32_t am_hal_uart_control(void *pHandle, am_hal_uart_control_e eControl, void *pArgs) ;

//*****************************************************************************
//
//! @brief
//!
//! @param pHandle
//! @param pui8TxBuffer
//! @param ui32TxBufferSize
//! @param pui8RxBuffer
//! @param ui32RxBufferSize
//!
//! @return
//
//*****************************************************************************
extern uint32_t am_hal_uart_buffer_configure(void *pHandle,
                                             uint8_t *pui8TxBuffer,
                                             uint32_t ui32TxBufferSize,
                                             uint8_t *pui8RxBuffer,
                                             uint32_t ui32RxBufferSize);


//*****************************************************************************
//
//! @brief called from uart ISR
//!
//! @details this code process uart tx, txcomplete, and rx interrupts, it is
//! designed to be used with uart fifos enabled and tx and rx queues enabled
//! The application will add data to the tx queue with the am_hal_uart_append_tx
//! call. The application will read data from the rx queue with the
//! am_hal_uart_get_rx_data
//!
//! @param pHandle is the handle for the UART to operate on.
//!
//! This function should be called from the ISR and then recieve/transmit data
//! from/to hardware FIFO.
//!
//! @return am_hal_uart_status_t this is a bitfield
//
//*****************************************************************************
extern am_hal_uart_status_t am_hal_uart_interrupt_queue_service(void *pHandle);

//*****************************************************************************
//
//! @brief append data to uart tx output queue
//!
//! @details this code is used in conjunction with am_hal_uart_interrupt_queue_service
//! it is designed to be used with uart fifos enabled and tx and rx queues enabled
//! It will add data to the uart tx queue and get uart tx running
//!
//! @note the uart fifos and tx queue must be enabled before calling this
//!
//! @param pHandle      - The handle for the UART to operate on.
//! @param pui8Buff     - Pointer to data buffer
//! @param ui32NumBytes - The Number of bytes to send
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t am_hal_uart_append_tx( void *pHandle,
                                       uint8_t *pui8Buff,
                                       uint32_t ui32NumBytes);

//*****************************************************************************
//
//! @brief move data from rx queue (filled via ISR) into user supplied buffer
//!
//! @details this code is used in conjunction with am_hal_uart_interrupt_queue_service
//! it is designed to be used with uart fifos enabled and tx and rx queues enabled
//! It will add data to the uart tx queue and get uart tx running
//!
//! @note the uart fifos and rx queue must be enabled before calling this
//!
//! @param pHandle              - UART handle
//! @param pui8DestBuff         - data is moved into this buffer (user buffer)
//! @param ui32MaxBytes         - max number of bytes that can be moved
//!
//! @return     - actual number of bytes loaded into user buffer
//
//*****************************************************************************
extern int32_t am_hal_uart_get_rx_data( void *pHandle,
                                        uint8_t *pui8DestBuff,
                                        uint32_t ui32MaxBytes);


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
