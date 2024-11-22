#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
// #include "am_hal_global.h"
#include "ns_uart.h"
volatile uint32_t ui32LastError;
bool g_bUARTdone = false;
extern uint8_t g_pui8RxBuffer[256];
extern uint8_t g_pui8TxBuffer[256];
// UART0 interrupt handler.
void
am_uart_isr(void)
{
    // // Service the FIFOs as necessary, and clear the interrupts.
    // uint32_t ui32Status;
    // am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    // am_hal_uart_interrupt_clear(phUART, ui32Status);
    // am_hal_uart_interrupt_service(phUART, ui32Status);
    am_hal_uart_interrupt_queue_service(phUART);

}


// non blocking callback function
void uart_done(uint32_t ui32ErrorStatus, void *pvContext)
{
    g_bUARTdone = true;
}

uint32_t init_uart(am_hal_uart_config_t *uart_config)
{

    //
    // Enable the UART pins.
    //
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);
    NS_TRY(am_hal_uart_initialize(0, &phUART), "Failed to initialize the UART");
    NS_TRY(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false), "Failed to power the UART");
    NS_TRY(am_hal_uart_configure(phUART, uart_config),  "Failed to configure the UART");
    NS_TRY(am_hal_uart_buffer_configure(phUART, g_pui8TxBuffer, sizeof(g_pui8TxBuffer), g_pui8RxBuffer, sizeof(g_pui8RxBuffer)), "Failed to configure the UART buffers");

    // Set the default cache configuration
    // am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    // am_hal_cachectrl_enable();

    // Enable interrupts.
    NVIC_SetPriority((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST), AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));
    am_hal_interrupt_master_enable();

    return AM_HAL_STATUS_SUCCESS;
}

void ns_uart_send_data(ns_uart_config_t* cfg, char * pcStr, uint32_t size) {
    uint32_t ui32StrLen = 0;
    uint32_t ui32BytesWritten = 0;

    // // Measure the length of the string.
    // while(pcStr[ui32StrLen] != 0) {
    //     ui32StrLen++;
    // }
    const am_hal_uart_transfer_t sUartWrite =
    {
        .eType = AM_HAL_UART_NONBLOCKING_WRITE,
        .pui8Data = (uint8_t *)pcStr,
        .ui32NumBytes = size,
        .pui32BytesTransferred = &ui32BytesWritten,
        .ui32TimeoutMs = 100, // block for 5 seconds
        .pfnCallback = &uart_done,
        .pvContext = NULL,
        .ui32ErrorStatus = 0
    };    
    g_bUARTdone = false;
    NS_TRY(am_hal_uart_transfer(phUART, &sUartWrite), "Failed to write to UART\n");
    while (!g_bUARTdone);
    if (ui32BytesWritten != ui32StrLen)
    {
        // Couldn't send the whole string!!
        while(1);
    }
    // am_hal_uart_append_tx(phUART, (uint8_t *)g_pui8TxBuffer, (uint32_t)sizeof(g_pui8TxBuffer));

}

void ns_uart_receive_data(ns_uart_config_t *cfg) {
    // uint32_t ui32StrLen = 0;
    // uint32_t ui32BytesRead = 0;

    // const am_hal_uart_transfer_t sUartRead =
    // {
    //     .eType = AM_HAL_UART_NONBLOCKING_READ, // uses blocking read
    //     .pui8Data = (uint8_t *) g_pui8RxBuffer,
    //     .ui32NumBytes = ui32StrLen,
    //     .pui32BytesTransferred = &ui32BytesRead,
    //     .ui32TimeoutMs = 100, // 100 ms blocking
    //     .pfnCallback = &uart_done,
    //     .pvContext = NULL,
    //     .ui32ErrorStatus = &ui32LastError
    // };
    // NS_TRY(am_hal_uart_transfer(phUART, &sUartRead), "Failed to read from UART\n");
    am_hal_uart_get_rx_data(phUART, g_pui8RxBuffer, 14);
}