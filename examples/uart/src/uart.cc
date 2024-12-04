

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "am_hal_mcuctrl.h"
#include "ns_uart.h"
#include <stdio.h>
#include <string.h>
#include "ns_peripherals_button.h"

#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256
static uint8_t g_pui8TxBuffer[TX_BUFFER_SIZE];
static uint8_t g_pui8RxBuffer[RX_BUFFER_SIZE];
#endif

// UART0 interrupt handler.
am_hal_uart_config_t g_sUartConfig =
{
    // Standard UART settings: 115200-8-N-1
    .ui32BaudRate = 115200,
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
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
#endif
#if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
    .eDataBits = AM_HAL_UART_DATA_BITS_8,
    .eParity = AM_HAL_UART_PARITY_NONE,
    .eStopBits = AM_HAL_UART_ONE_STOP_BIT,
    .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
    .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
    .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
#endif
};
ns_uart_config_t uart_config = {
    .api=&ns_uart_V0_0_1,
    .uart_config = &g_sUartConfig,
};
int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    uint32_t opusBegin, opusEnd;
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0); // nada
#endif
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed.");
    ns_itm_printf_enable();                                     // nada
    
    ns_lp_printf("UART init\n");
    NS_TRY(ns_uart_init(&uart_config), "UART init failed.\n");
    ns_interrupt_master_enable();

    ns_lp_printf("UART init done\n");
    char buffer[] = "hello world";
    char rxBuffer[12];
    while(1) {
        // Send data over UART
        ns_uart_send_data(&uart_config, buffer, (uint32_t)sizeof(buffer) - 1);
        // Receive data over UART
        ns_uart_receive_data(&uart_config, rxBuffer, (uint32_t)sizeof(rxBuffer));
        strcpy(buffer, (char *)rxBuffer);
        ns_delay_us(500000);
    }
}