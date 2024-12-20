#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"
volatile uint32_t ui32LastError;
bool g_bUARTdone = false;
#define MAX_UART_RETRIES 10 // Maximum number of retries
#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256
AM_SHARED_RW uint8_t g_pui8TxBuffer[TX_BUFFER_SIZE]__attribute__((aligned(32)));
AM_SHARED_RW uint8_t g_pui8RxBuffer[RX_BUFFER_SIZE]__attribute__((aligned(32)));

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

void am_uart_isr(void)
{
    // // Service the FIFOs as necessary, and clear the interrupts.
    uint32_t ui32Status;
    am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(phUART, ui32Status);
    am_hal_uart_interrupt_service(phUART, ui32Status);

}


// non blocking callback function
void uart_done(uint32_t ui32ErrorStatus, void *pvContext)
{
    g_bUARTdone = true;
}

uint32_t init_uart(am_hal_uart_config_t *uart_config)
{
    // // Set the default cache configuration
    // am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    // am_hal_cachectrl_enable();

    // // Configure the board for low power operation.
    // am_bsp_low_power_init();

#if defined(AM_PART_APOLLO4L)
    NS_TRY(am_hal_uart_initialize(2, &phUART), "Failed to initialize the UART");
#else
    NS_TRY(am_hal_uart_initialize(0, &phUART), "Failed to initialize the UART");
#endif
    NS_TRY(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false), "Failed to power the UART");
    NS_TRY(am_hal_uart_configure(phUART, uart_config),  "Failed to configure the UART");

    NS_TRY(am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX), "Failed to configure UART TX pin");
    NS_TRY(am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX), "Failed to configure UART RX pin");

    // Enable interrupts.
    NVIC_SetPriority((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));

    return AM_HAL_STATUS_SUCCESS;
}

void ns_uart_send_data(ns_uart_config_t* cfg, char * txBuffer, uint32_t size) {
    uint32_t ui32BytesWritten = 0;
    uint32_t retries = 0;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    while(retries < MAX_UART_RETRIES) {
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
        // g_bUARTdone = false;
        status = am_hal_uart_transfer(phUART, &sUartWrite);
        if (status == AM_HAL_STATUS_SUCCESS && ui32BytesWritten == size) {
            // Successfully sent the whole string
            am_hal_uart_tx_flush(phUART);
            return AM_HAL_STATUS_SUCCESS;
        }
        retries++;
    }
    return status;
    }

void ns_uart_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size) {
    uint32_t retries = 0;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32BytesRead = 0;
    while (retries < MAX_UART_RETRIES) {
        memset(g_pui8RxBuffer, 0, sizeof(g_pui8RxBuffer));
        const am_hal_uart_transfer_t sUartRead =
        {
            .eType = AM_HAL_UART_BLOCKING_READ, // uses blocking read
            .pui8Data = (uint8_t *)rxBuffer,
            .ui32NumBytes = size,
            .pui32BytesTransferred = &ui32BytesRead,
            .ui32TimeoutMs = 1000,
            .pfnCallback = &uart_done,
            .pvContext = NULL,
            .ui32ErrorStatus = &ui32LastError
        };
        status = am_hal_uart_transfer(phUART, &sUartRead);
        if (status == AM_HAL_STATUS_SUCCESS) {
            // Successfully read the whole string
            return AM_HAL_STATUS_SUCCESS;
        }
        retries++;
    }
    return status;
}