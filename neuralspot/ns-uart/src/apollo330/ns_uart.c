#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"
#include "ns_ambiqsuite_harness.h"

volatile uint32_t ui32LastError;
bool g_bUARTdone = false;
#define MAX_UART_RETRIES 10 // Maximum number of retries

am_hal_uart_config_t g_sUartConfig =
{
    // Standard UART settings: 115200-8-N-1
    .ui32BaudRate = 115200,
    .eDataBits = AM_HAL_UART_DATA_BITS_8,
    .eParity = AM_HAL_UART_PARITY_NONE,
    .eStopBits = AM_HAL_UART_ONE_STOP_BIT,
    .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
    .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
    .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
};

ns_uart_transaction_t g_sUartTransaction =
{
    .status = 0,
};

void am_uart_isr(void)
{
    // // Service the FIFOs as necessary, and clear the interrupts.
    uint32_t ui32Status;
    am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(phUART, ui32Status);
    am_hal_uart_interrupt_service(phUART, ui32Status);

    ns_uart_config_t * ctx = &ns_uart_config;

    // Set the data available flag if RX interrupt is set
    if (ui32Status & (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT))
    {
        g_sUartTransaction.status = AM_HAL_UART_INT_RX;
        if (ctx->rx_cb != NULL)
        {
            ctx->rx_cb(&g_sUartTransaction);
        }
        g_DataAvailable = true;
    }
    else if (ui32Status & AM_HAL_UART_INT_TX)
    {
        g_sUartTransaction.status = AM_HAL_UART_INT_TX;
        if (ctx->tx_cb != NULL)
        {
            ctx->tx_cb(&g_sUartTransaction);
        }
    }
}

void am_uart2_isr(void) {
    // // Service the FIFOs as necessary, and clear the interrupts.
    uint32_t ui32Status;
    am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(phUART, ui32Status);
    am_hal_uart_interrupt_service(phUART, ui32Status);
    // Set the data available flag if RX interrupt is set
    if (ui32Status & AM_HAL_UART_INT_RX)
    {
        g_DataAvailable = true;
    }
}


// non blocking callback function
void uart_done(uint32_t ui32ErrorStatus, void *pvContext)
{
    g_bUARTdone = true;
}

uint32_t init_uart(am_hal_uart_config_t *uart_config)
{
    NS_TRY(am_hal_uart_initialize(0, &phUART), "Failed to initialize the UART");
    NS_TRY(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false), "Failed to power the UART");
    NS_TRY(am_hal_uart_configure(phUART, uart_config),  "Failed to configure the UART");

    NS_TRY(am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX), "Failed to configure UART TX pin");
    NS_TRY(am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX), "Failed to configure UART RX pin");

    // Enable interrupts.
    NVIC_SetPriority((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));
    am_hal_uart_interrupt_enable(phUART, (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_TX | AM_HAL_UART_INT_RX_TMOUT));
    return AM_HAL_STATUS_SUCCESS;
}

uint32_t ns_uart_blocking_send_data(ns_uart_config_t* cfg, char * txBuffer, uint32_t size) {
    uint32_t ui32BytesWritten = 0;
    uint32_t retries = MAX_UART_RETRIES;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    while(retries > 0) {
        const am_hal_uart_transfer_t sUartWrite =
        {
            .eType = AM_HAL_UART_BLOCKING_WRITE,
            .pui8Data = (uint8_t *)txBuffer,
            .ui32NumBytes = size,
            .pui32BytesTransferred = &ui32BytesWritten,
            .ui32TimeoutMs = 1000,
            .pfnCallback = &uart_done,
            .pvContext = NULL,
            .ui32ErrorStatus = 0
        };    
        status = am_hal_uart_transfer(phUART, &sUartWrite);
        if (status == AM_HAL_STATUS_SUCCESS && ui32BytesWritten == size) {
            // Successfully sent the whole string
            am_hal_uart_tx_flush(phUART);
            return AM_HAL_STATUS_SUCCESS;
        }
        retries--;
    }
    // If we reach here, it means send operation failed all retries
    ns_lp_printf("[ERROR] ns_uart_send_data exhausted retries\n");
    return status;
}


uint32_t ns_uart_nonblocking_send_data(ns_uart_config_t* cfg, char * txBuffer, uint32_t size) {
    uint32_t ui32BytesWritten = 0;
    uint32_t retries = MAX_UART_RETRIES;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    while(retries > 0) {
        const am_hal_uart_transfer_t sUartWrite =
        {
            .eType = AM_HAL_UART_NONBLOCKING_WRITE,
            .pui8Data = (uint8_t *)txBuffer,
            .ui32NumBytes = size,
            .pui32BytesTransferred = &ui32BytesWritten,
            .ui32TimeoutMs = 0,
            .pfnCallback = &uart_done,
            .pvContext = NULL,
            .ui32ErrorStatus = 0
        };    
        status = am_hal_uart_transfer(phUART, &sUartWrite);
        if (status == AM_HAL_STATUS_SUCCESS && ui32BytesWritten == size) {
            // Successfully sent the whole string
            am_hal_uart_tx_flush(phUART);
            return AM_HAL_STATUS_SUCCESS;
        }
        retries--;
    }
    // If we reach here, it means send operation failed all retries
    ns_lp_printf("[ERROR] ns_uart_send_data exhausted retries\n");
    return status;
}


uint32_t ns_uart_blocking_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size) {
    uint32_t retries = MAX_UART_RETRIES;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32BytesRead = 0;
    while (retries > 0) {
        const am_hal_uart_transfer_t sUartRead =
        {
            .eType = AM_HAL_UART_BLOCKING_READ, // uses blocking read
            .pui8Data = (uint8_t *)rxBuffer,
            .ui32NumBytes = size,
            .pui32BytesTransferred = &ui32BytesRead,
            .ui32TimeoutMs = 1000,
            .pfnCallback = &uart_done,
            .pvContext = NULL,
            .ui32ErrorStatus = ui32LastError
        };
        status = am_hal_uart_transfer(phUART, &sUartRead);
        if (status == AM_HAL_STATUS_SUCCESS && ui32BytesRead == size) {
            // Successfully read the whole string
            g_DataAvailable = false; // Clear the data available flag
            return AM_HAL_STATUS_SUCCESS;
        }
        retries--;
    }
    if(ui32BytesRead < size) {
        ns_lp_printf("[ERROR] RX error, asked for %d, got %d\n", size, ui32BytesRead);
    }
    // If we reach here, it means receive operation failed all retries
    ns_lp_printf("[ERROR] ns_uart_receive_data exhausted retries\n");
    return status;
}



uint32_t ns_uart_nonblocking_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size) {
    uint32_t retries = MAX_UART_RETRIES;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32BytesRead = 0;
    while (retries > 0) {
        const am_hal_uart_transfer_t sUartRead =
        {
            .eType = AM_HAL_UART_NONBLOCKING_READ, // uses blocking read
            .pui8Data = (uint8_t *)rxBuffer,
            .ui32NumBytes = size,
            .pui32BytesTransferred = &ui32BytesRead,
            .ui32TimeoutMs = 0,
            .pfnCallback = &uart_done,
            .pvContext = NULL,
            .ui32ErrorStatus = ui32LastError
        };
        status = am_hal_uart_transfer(phUART, &sUartRead);
        if (status == AM_HAL_STATUS_SUCCESS && ui32BytesRead == size) {
            // Successfully read the whole string
            g_DataAvailable = false; // Clear the data available flag
            return AM_HAL_STATUS_SUCCESS;
        }
        retries--;
    }
    if(ui32BytesRead < size) {
        ns_lp_printf("[ERROR] RX error, asked for %d, got %d\n", size, ui32BytesRead);
    }
    // If we reach here, it means receive operation failed all retries
    ns_lp_printf("[ERROR] ns_uart_receive_data exhausted retries\n");
    return status;
}