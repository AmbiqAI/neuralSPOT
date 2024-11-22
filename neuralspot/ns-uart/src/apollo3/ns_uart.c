#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_uart.h"

// volatile uint32_t ui32LastError;

// UART0 interrupt handler.
void
am_uart_isr(void)
{
    // Service the FIFOs as necessary, and clear the interrupts.
    uint32_t ui32Status, ui32Idle;
    // am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    // am_hal_uart_interrupt_clear(phUART, ui32Status);
    // am_hal_uart_interrupt_service(phUART, ui32Status, &ui32Idle);
    am_hal_uart_interrupt_queue_service(phUART);
}

uint32_t init_uart(am_hal_uart_config_t *uart_config)
{
    am_util_id_t sIdDevice;
    uint32_t ui32StrBuf;

    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.

    am_bsp_low_power_init();
    // Initialize the printf interface for UART output.
    NS_TRY(am_hal_uart_initialize(UART_ID, &phUART), "Failed to initialize the UART");

    NS_TRY(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false), "Failed to power the UART");

    am_hal_uart_clock_speed_e eUartClockSpeed = eUART_CLK_SPEED_DEFAULT ;
    NS_TRY(am_hal_uart_control(phUART, AM_HAL_UART_CONTROL_CLKSEL, &eUartClockSpeed),  "Failed to set the UART clock speed");
    NS_TRY(am_hal_uart_configure(phUART, uart_config),  "Failed to configure the UART");
    NS_TRY(am_hal_uart_buffer_configure(phUART, uart_config->pui8TxBuffer, sizeof(uart_config->pui8TxBuffer), uart_config->pui8RxBuffer, sizeof(uart_config->pui8RxBuffer)), "Failed to configure the UART buffers");

    // Enable the UART pins.
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    //
    // Enable interrupts.
    //
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));
    am_hal_interrupt_master_enable();

    //
    // Set the main print interface to use the UART print function we defined.
    //
    // am_util_stdio_printf_init(ns_uart_send_data);
    return AM_HAL_STATUS_SUCCESS;
}

void ns_uart_send_data(ns_uart_config_t *cfg, char * pcStr, uint32_t size) {
    uint32_t ui32StrLen = 0;
    uint32_t ui32BytesWritten = 0;

    // // Measure the length of the string.
    // while (pcStr[ui32StrLen] != 0)
    // {
    //     ui32StrLen++;
    // }
    const am_hal_uart_transfer_t sUartWrite =
    {
        .ui32Direction = AM_HAL_UART_WRITE,
        .pui8Data = (uint8_t *)pcStr,
        .ui32NumBytes = size,
        .ui32TimeoutMs = 100,
        .pui32BytesTransferred = &ui32BytesWritten,
    };
    am_hal_uart_transfer(phUART, &sUartWrite);
    // am_hal_uart_append_tx(phUART, cfg->uart_config->pui8TxBuffer, (uint32_t)sizeof(cfg->uart_config->pui8TxBuffer));

}

void ns_uart_receive_data(ns_uart_config_t *cfg) {
    // uint32_t ui32StrLen = 0;
    // uint32_t ui32BytesRead = 0;

    // const am_hal_uart_transfer_t sUartRead =
    // {
    //     .ui32Direction = AM_HAL_UART_READ,
    //     .pui8Data = cfg->uart_config->pui8RxBuffer,
    //     .ui32NumBytes = 0,
    //     .pui32BytesTransferred = &ui32BytesRead,
    //     .ui32TimeoutMs = 100,
    // };
    // NS_TRY(am_hal_uart_transfer(phUART, &sUartRead), "Failed to read to UART\n");
    am_hal_uart_get_rx_data(phUART, cfg->uart_config->pui8RxBuffer, 14);

}