

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "am_hal_mcuctrl.h"
#include "ns_uart.h"


uint8_t g_pui8TxBuffer[256];
uint8_t g_pui8RxBuffer[256];

am_hal_uart_config_t g_sUartConfig =
{
    //
    // Standard UART settings: 115200-8-N-1
    //
    .ui32BaudRate = 115200,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .ui32Parity = AM_HAL_UART_PARITY_NONE,
    .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
    .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,

    //
    // Set TX and RX FIFOs to interrupt at half-full.
    //
    .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                       AM_HAL_UART_RX_FIFO_1_2),

    //
    // Buffers
    //
    .pui8TxBuffer = g_pui8TxBuffer,
    .ui32TxBufferSize = sizeof(g_pui8TxBuffer),
    .pui8RxBuffer = g_pui8RxBuffer,
    .ui32RxBufferSize = sizeof(g_pui8RxBuffer),
};

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    ns_core_init(&ns_core_cfg);
    uint32_t opusBegin, opusEnd;
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0); // nada
    ns_itm_printf_enable();                                     // nada
    ns_power_config(&ns_development_default);
    // am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    // am_hal_cachectrl_enable();
    // NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

    // am_hal_mcuctrl_control(AM_HAL_MCUCTRL_CONTROL_SRAM_PREFETCH);
    am_hal_sysctrl_fpu_enable();              // nada
    am_hal_sysctrl_fpu_stacking_enable(true); // nada
    ns_uart_config_t uart_config = {
        .api=&ns_uart_V0_0_1,
        .uart_config = &g_sUartConfig
    };
    ns_lp_printf("UART init\n");
    NS_TRY(ns_uart_init(&uart_config), "UART init failed.\n");

    while (1) {
        ns_uart_write("Hello World!\n");
        ns_uart_read(&uart_config);
        ns_lp_printf("%s\n", g_pui8RxBuffer);
        ns_delay_us(1000000);
    }
}
