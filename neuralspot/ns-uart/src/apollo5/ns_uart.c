//*****************************************************************************
//
//! @file uart_async_common.c
//!
//! @brief Common code and deinitions for uart_async example
//!
//! @addtogroup uart_async
//! @{
//! @defgroup uart_async_common_c UART Asynchronous Common Example
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

#include "uart_async_common.h"
#include "am_util.h"


//
//! Uart pin definitions
//! This defaults with standard BSP pin definitions.
//
static const uart_defs_t gs_UartPinDefs[eMAX_UARTS] =
{
#if UART0_AVAIL
    [eUART0] =
    {
    .uartId = eUART0,
    .rxPin = {AM_BSP_GPIO_COM_UART_RX, &g_AM_BSP_GPIO_COM_UART_RX},
    .txPin = {AM_BSP_GPIO_COM_UART_TX, &g_AM_BSP_GPIO_COM_UART_TX},
    .uartHwIndex = 0,

    },
#endif
#if UART1_AVAIL
    [eUART1] =
    {
        .uartId = eUART1,
        .rxPin = {AM_BSP_GPIO_UART1_RX, &g_AM_BSP_GPIO_UART1_RX},
        .txPin = {AM_BSP_GPIO_UART1_TX, &g_AM_BSP_GPIO_UART1_TX},
        .uartHwIndex = 1,
    },
#endif
#if UART2_AVAIL
    [eUART2] =
    {
        .uartId = eUART2,
        .rxPin = {AM_BSP_GPIO_UART2_RX, &g_AM_BSP_GPIO_UART2_RX},
        .txPin = {AM_BSP_GPIO_UART2_TX, &g_AM_BSP_GPIO_UART2_TX},
        .uartHwIndex = 2,
    },
#endif
#if UART3_AVAIL
    [eUART3] =
    {
        .uartId = eUART3,
        .rxPin = {AM_BSP_GPIO_UART3_RX, &g_AM_BSP_GPIO_UART3_RX},
        .txPin = {AM_BSP_GPIO_UART3_TX, &g_AM_BSP_GPIO_UART3_TX},
        .uartHwIndex = 3,
    },
#endif
};

//
//! uart error capture struct
//
typedef struct
{
    uint32_t loopCountErr;
    uint32_t posx;
    am_hal_uart_errors_t lastError;

} error_cap_t;

//
//! this is to look at with debugger if printing is disabled
//
volatile error_cap_t ec;

//
//! decode standard error names for printing
//
static const char * const uartCmnErrStatus[] =
{
    "SUCCESS",
    "FAIL",
    "INVALID_HANDLE",
    "IN_USE",
    "TIMEOUT",
    "OUT_OF_RANGE",
    "INVALID_ARG",
    "INVALID_OPERATION",
    "MEM_ERR",
    "HW_ERR",
};

//
//! decode extended error names for printing
//
static const char * const uartDriverErrorStrings[] =
{
    "US_BUS_ERROR",
    "US_RX_QUEUE_FULL",
    "US_CLOCK_NOT_CONFIGURED",
    "US_BAUDRATE_NOT_POSSIBLE",
    "US_TX_CHANNEL_BUSY",
    "US_RX_CHANNEL_BUSY",
    "US_RX_TIMEOUT",
    "US_FRAME_ERROR",
    "US_PARITY_ERROR",
    "US_BREAK_ERROR",
    "US_OVERRUN_ERROR",
    "US_DMA_ERROR",
    "DMA_NO_INIT",
    "BUFFER_OVERFILL",
    "MEMORY_ERROR_01",
    "MEMORY_ERROR_02",
    "MEMORY_ERROR_03",
};


static uart_async_local_vars_t *g_uartIsrMap[4];  //<! one for each uart
//
//! Place DMA ram in SRAM and align the region for better MPU access
//! The mpu will be used to disable the cache in this region
//
__attribute__ ((section ("SHARED_RW")))
__attribute__((aligned(32))) uart_dma_rx_tx_DMA_buffers_t g_dma_buff[NUM_UARTS_USED];


void am_uart_isr(void);
void am_uart1_isr(void);
void am_uart2_isr(void);
void am_uart3_isr(void);
static void am_isr_common(uart_async_local_vars_t *pUartDescriptor);

//*****************************************************************************
//
//! @brief Uart isr handlers
//
//*****************************************************************************
void am_uart_isr(void)
{
#if UART0_AVAIL
    am_isr_common(g_uartIsrMap[0]);
#endif
}

void am_uart1_isr(void)
{
#if UART1_AVAIL
    am_isr_common(g_uartIsrMap[1]);
#endif
}

void am_uart2_isr(void)
{
#if UART2_AVAIL
    am_isr_common(g_uartIsrMap[2]);
#endif
}

void am_uart3_isr(void)
{
#if UART3_AVAIL
    am_isr_common(g_uartIsrMap[3]);
#endif
}

//*****************************************************************************
//
//! @brief Common uart isr handler
//!
//! @param pUartDescriptor
//
//*****************************************************************************
static void am_isr_common(uart_async_local_vars_t *pUartDescriptor)
{
    pUartDescriptor->e32Status |= am_hal_uart_interrupt_queue_service(pUartDescriptor->pvUART);
    if (pUartDescriptor->e32Status != AM_HAL_UART_ASYNC_STATUS_SUCCESS)
    {
        pUartDescriptor->ui32ISrErrorCount++;
    }
}

//*****************************************************************************
//
// called to configure MPU for dma data
//
//*****************************************************************************
uint32_t uart_async_cmn_mpuConfig(void)
{
    return mpuConfig( (uint32_t) g_dma_buff, sizeof(g_dma_buff) );
}

//*****************************************************************************
//
//  will check and log run-time errors
//
//*****************************************************************************
void errorCheck(am_hal_uart_errors_t eErrx, uint32_t ui32LoopCount, uint32_t ui32ErrorPos)
{

    if (eErrx)
    {
        uint32_t ui32ErrIdx = eErrx;
        const char *pcEerrorInfoStr;
        if (ui32ErrIdx <= AM_HAL_STATUS_HW_ERR )
        {
            pcEerrorInfoStr = uartCmnErrStatus[ui32ErrIdx];
        }
        else if ( ui32ErrIdx >= AM_HAL_STATUS_MODULE_SPECIFIC_START && ui32ErrIdx < AM_HAL_UART_ERR_END )
        {
            ui32ErrIdx -= AM_HAL_STATUS_MODULE_SPECIFIC_START;
            pcEerrorInfoStr = uartDriverErrorStrings[ui32ErrIdx];
        }
        else
        {
            pcEerrorInfoStr = "Unknown Error";
        }

        ec.lastError    = eErrx;
        ec.loopCountErr = ui32LoopCount;
        ec.posx = ui32ErrorPos;
        am_util_stdio_printf("error capture: %s 0x%08x %d %d\r\n", pcEerrorInfoStr, eErrx, ui32LoopCount, ui32ErrorPos);
        while (1);
    }
}

//*****************************************************************************
//
//! @brief Initialize a UART instance
//!
//! @param uartId  the uart number (0<->(N-1))
//! @param pUartLocalVar ram allocated for this uart
//!
//! @return standard hal status
//
//*****************************************************************************
static uint32_t init_uart(uart_id_e uartId,
                          uart_async_local_vars_t *pUartLocalVar )
{
    //
    // get and check the uart number
    //
    if (uartId >= eMAX_UARTS)
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    am_hal_uart_async_dma_mode_e eUartTxMode = pUartLocalVar->psUartConfig->eAsyncDMAMode;

    if ( eUartTxMode > AM_HAL_UART_DMA_TX_SINGLE_BUFFER )
    {
        return AM_HAL_STATUS_INVALID_OPERATION;
    }

    g_uartIsrMap[pUartLocalVar->psUartPinDefs->uartHwIndex] = pUartLocalVar;
    //
    // associate this instance with this uart ID
    // (so don't loop on IDs, loop through the instance structures)
    //

    //
    // setup rx and tx pins
    //
    uint32_t uiStatus;
    do
    {
        const uart_defs_t *pUartDefs = pUartLocalVar->psUartPinDefs;
        uiStatus = am_hal_gpio_pinconfig(pUartDefs->txPin.pinNUmber, *(pUartDefs->txPin.pinCfg));
        if (uiStatus)
        {
            break;
        }
        uiStatus = am_hal_gpio_pinconfig(pUartDefs->rxPin.pinNUmber, *(pUartDefs->rxPin.pinCfg));
        if (uiStatus)
        {
            break;
        }

        am_hal_uart_initialize(UART_ID, &pUartLocalVar->pvUART);
        uiStatus = am_hal_uart_power_control(pUartLocalVar->pvUART, AM_HAL_SYSCTRL_WAKE, false);
        if (uiStatus)
        {
            break;
        }

        const am_hal_uart_config_t *pUartConfig = pUartLocalVar->psUartConfig;

        uiStatus = am_hal_uart_configure(pUartLocalVar->pvUART, pUartConfig);
        if (uiStatus)
        {
            break;
        }

        uiStatus = am_hal_uart_buffer_configure(pUartLocalVar->pvUART,
                                                pUartLocalVar->psRxTxBuffers.pui8UARTTXBuffer,
                                                pUartLocalVar->psRxTxBuffers.ui32TxBuffSize,
                                                pUartLocalVar->psRxTxBuffers.pui8UARTRXBuffer,
                                                pUartLocalVar->psRxTxBuffers.ui32RxBuffSize);
        if (uiStatus)
        {
            break;
        }

        am_hal_uart_async_dma_mode_e eAsyncDmaMode = pUartConfig->eAsyncDMAMode;

        if ((eAsyncDmaMode == AM_HAL_UART_DMA_TX_DOUBLE_BUFFER) ||
            (eAsyncDmaMode == AM_HAL_UART_DMA_TX_SINGLE_BUFFER))
        {
            uiStatus = am_hal_uart_dmaQueueInit(pUartLocalVar->pvUART,
                                                eAsyncDmaMode,
                                                &pUartLocalVar->dmaTxQueue);
            if (uiStatus)
            {
                break;
            }
        }

        //
        // Make sure to enable the interrupts for RX, since the HAL doesn't already
        // know we intend to use them., this is done later
        //
        uint32_t uartInstance = pUartDefs->uartHwIndex;
        NVIC_SetPriority((IRQn_Type) (UART0_IRQn + uartInstance), AM_IRQ_PRIORITY_DEFAULT);
        NVIC_EnableIRQ((IRQn_Type) (UART0_IRQn + uartInstance));
    }
    while( false );

    return uiStatus;
}

//*****************************************************************************
//
//! @brief Initialize all the serial modules and associated tx and tx pins
//
//*****************************************************************************
uint32_t
uart_cmn_serial_interface_init(uart_async_local_vars_t *pUartDesc,
                               uint8_t *pui8TxBuff,
                               uint32_t ui32TxBuffSize,
                               uint8_t *pui8RxBuff,
                               uint32_t ui32RxBuffSize)
{
    //
    // Start the UARTs that are enabled here
    //

    uart_rx_tx_buffers_t *psRxTx = &pUartDesc->psRxTxBuffers;
    if (pui8TxBuff)
    {
        psRxTx->pui8UARTTXBuffer = pui8TxBuff;
        psRxTx->ui32TxBuffSize   = ui32TxBuffSize;
    }
    else
    {
        psRxTx->pui8UARTTXBuffer = g_dma_buff[0].pui8UARTTXDMABuffer;
        psRxTx->ui32TxBuffSize   = ((ui32TxBuffSize > TX_BUFFER_SIZE) || (ui32TxBuffSize == 0)) ?
                                    TX_BUFFER_SIZE : ui32TxBuffSize;

    }
    if (pui8RxBuff)
    {
        psRxTx->pui8UARTRXBuffer = pui8RxBuff;
        psRxTx->ui32RxBuffSize   = ui32RxBuffSize;
    }
    else
    {
        psRxTx->pui8UARTRXBuffer = g_dma_buff[0].pui8UARTRXDMABuffer;
        psRxTx->ui32RxBuffSize   = ((ui32RxBuffSize > RX_BUFFER_SIZE) || (ui32RxBuffSize == 0)) ?
                                    RX_BUFFER_SIZE : ui32RxBuffSize;

    }

    pUartDesc->psUartPinDefs = &gs_UartPinDefs[pUartDesc->uartId];

    uint32_t ui32status = init_uart(pUartDesc->uartId, pUartDesc);

    return ui32status;

} //serial_interface_init

//*****************************************************************************
//
// MPU config. Confgiure DMA region for as non-cacheable
//
//*****************************************************************************
uint32_t mpuConfig(uint32_t ui32BaseAddress, uint32_t ui32DmaSize)
{
    ARM_MPU_Disable();

    uint8_t ui32AttrIndex = 3;
    uint8_t ui32CachePolicy = (ARM_MPU_ATTR_NON_CACHEABLE << 4) | ARM_MPU_ATTR_NON_CACHEABLE;

    //
    // define mpu attributes for no cache
    //
    ARM_MPU_SetMemAttr(ui32AttrIndex, ui32CachePolicy);

    uint32_t endAddr     = ui32BaseAddress + ui32DmaSize -1;

    //
    // define MPU region
    //
    am_hal_mpu_region_config_t mpuRegsion = {
        .ui32RegionNumber = 3,
        .ui32BaseAddress  = ui32BaseAddress,
        .eShareable = ARM_MPU_SH_INNER,
        .eAccessPermission = RW_NONPRIV,
        .bExecuteNever = true,
        .ui32LimitAddress = endAddr,
        .ui32AttrIndex = ui32AttrIndex,
        .bEnable = true,
    };

    uint32_t  ui32Status = am_hal_mpu_region_configure( &mpuRegsion, 1 );
    if (ui32Status)
    {
        return ui32Status;
    }

    return am_hal_mpu_enable(true, true);
}

//*****************************************************************************
//
// will process returned ISR data, called in background loop
//
//*****************************************************************************
uint32_t serial_process_isr_status(uart_async_local_vars_t *psAsyncLVars, uint32_t ui32IsrStatus)
{
    //
    // print isr activity/status messages
    //

    uart_id_e eUartId = psAsyncLVars->uartId;

    if (ui32IsrStatus & AM_HAL_UART_ASYNC_STATUS_TX_COMPLETE)
    {
        am_util_stdio_printf("txComplete on uart %d\r\n", eUartId );
    }
    if (ui32IsrStatus & AM_HAL_UART_ASYNC_STATUS_INTRNL_MSK)
    {
        am_util_stdio_printf("uart error on uart %d : 0x%x\r\n", eUartId,
                             ui32IsrStatus & AM_HAL_UART_ASYNC_STATUS_INTRNL_MSK);
    }
    if (ui32IsrStatus & AM_HAL_UART_ASYNC_STATUS_RX_DATA_AVAIL)
    {
        uint8_t rxBuf[128];
        //
        // read uart after interrupt
        // here the uart will accumulate data until the fifo is full or a rx timeout occurs
        // Note: the rx-timeout interrupt must be enabled for the rx timeout to occur
        //
        uint32_t numBytesRead = am_hal_uart_async_get_rx_data(psAsyncLVars->pvUART, rxBuf, sizeof(rxBuf) );

        if (numBytesRead > 0)
        {
            am_util_stdio_printf("num bytes rxed on uart %d : %d\r\n", eUartId, numBytesRead);
        }
    }
#ifdef POLL_UART
    else
    {
        // this iwll poll the uart and will dig data out of the fifo, depending on polling and data reate,
        // possibly before it causes an rx interrupt. This is typically used if the RX-TIMEOUT interrups is
        // NOT enabled.

        int32_t i32numRx = am_hal_uart_async_get_rx_data(pUartDesc->pvUART, rxBuf, sizeof(rxBuf));

        if (i32numRx < 0)
        {
            am_util_stdio_printf("noRxIsr flag: rxError2 on uart %d : %d\r\n", eUartId, -i32numRx);
        }
        else if (i32numRx > 0)
        {
            am_util_stdio_printf("noRxIsr flag: num bytes rxed on uart %d : %d %c\r\n", eUartId, i32numRx, rxBuf[0] );
        }

    }
#endif
    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// enable standard interrupts for uart async modes
//
//*****************************************************************************
uint32_t serial_interrupt_enable( uart_async_local_vars_t *psAsyncLVars )
{
    return am_hal_uart_interrupt_enable( psAsyncLVars->pvUART,
        (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT));
}
//*****************************************************************************
//
// End Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
