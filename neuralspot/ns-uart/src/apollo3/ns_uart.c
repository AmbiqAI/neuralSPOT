#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"

#define MAX_UART_RETRIES 10 // Maximum number of retries
#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256
static uint8_t g_pui8TxBuffer[TX_BUFFER_SIZE];
static uint8_t g_pui8RxBuffer[RX_BUFFER_SIZE];

am_hal_uart_config_t g_sUartConfig =
{
    // Standard UART settings: 115200-8-N-1
    .ui32BaudRate = 115200,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    // Set TX and RX FIFOs to interrupt at half-full.
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
    .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
    .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                       AM_HAL_UART_RX_FIFO_1_2),
    // Buffers
    .pui8TxBuffer = g_pui8TxBuffer,
    .ui32TxBufferSize = (uint32_t) sizeof(g_pui8TxBuffer),
    .pui8RxBuffer = g_pui8RxBuffer,
    .ui32RxBufferSize = (uint32_t) sizeof(g_pui8RxBuffer),
};
void am_uart_isr(void)
{
    // Service the FIFOs as necessary, and clear the interrupts.
    uint32_t ui32Status, ui32Idle;
    am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(phUART, ui32Status);
    am_hal_uart_interrupt_service(phUART, ui32Status, &ui32Idle);
}

uint32_t init_uart(am_hal_uart_config_t *uart_config)
{
    NS_TRY(am_hal_uart_initialize(UART_ID, &phUART), "Failed to initialize the UART");

    NS_TRY(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false), "Failed to power the UART");

    am_hal_uart_clock_speed_e eUartClockSpeed = eUART_CLK_SPEED_DEFAULT ;
    NS_TRY(am_hal_uart_control(phUART, AM_HAL_UART_CONTROL_CLKSEL, &eUartClockSpeed),  "Failed to set the UART clock speed");
    NS_TRY(am_hal_uart_configure(phUART, uart_config),  "Failed to configure the UART");

    // Enable the UART pins.
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    // Enable interrupts.
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));
    am_hal_interrupt_master_enable();

    return AM_HAL_STATUS_SUCCESS;
}

uint32_t ns_uart_send_data(ns_uart_config_t *cfg, char *txBuffer, uint32_t size) {
    uint32_t ui32BytesWritten = 0;
    uint32_t retries = MAX_UART_RETRIES;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    while (retries > 0) {
        const am_hal_uart_transfer_t sUartWrite = {
            .ui32Direction = AM_HAL_UART_WRITE,
            .pui8Data = (uint8_t *)txBuffer,
            .ui32NumBytes = size,
            .ui32TimeoutMs = 1000,
            .pui32BytesTransferred = &ui32BytesWritten,
        };

        status = am_hal_uart_transfer(phUART, &sUartWrite);

        if (status == AM_HAL_STATUS_SUCCESS  && ui32BytesWritten == size) {
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

uint32_t ns_uart_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size) {
    uint32_t retries = MAX_UART_RETRIES;
    uint32_t status = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32BytesRead = 0;
    while (retries > 0) {
        const am_hal_uart_transfer_t sUartRead =
        {
            .ui32Direction = AM_HAL_UART_READ,
            .pui8Data = rxBuffer,
            .ui32NumBytes = size,
            .pui32BytesTransferred = &ui32BytesRead,
            .ui32TimeoutMs = 1000,
        };
        status = am_hal_uart_transfer(phUART, &sUartRead);
            if (status == AM_HAL_STATUS_SUCCESS  && ui32BytesRead == size) {
                // Successfully read the whole string
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