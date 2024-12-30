

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "am_hal_mcuctrl.h"
#include "ns_uart.h"
#include <stdio.h>
#include <string.h>
#include "ns_peripherals_button.h"

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
    char rxBuffer[12];
    int count = 1;
    char *txBuffer = "";
    for(int i = 0; i < 15; i++) {
        switch (i) {
            case 0:
                ns_uart_send_data(&uart_config, "Press button 0\n", 15);
                break;
            case 1:
                ns_uart_send_data(&uart_config, "Press button 1\n", 15);
                break;
            case 2:
                ns_uart_send_data(&uart_config, "Press button 0 and 1\n", 20);
                break;
            case 3:
                ns_uart_send_data(&uart_config, "Press button 0\n", 15);
                break;
            case 4:
                ns_uart_send_data(&uart_config, "Press button 1\n", 15);
                break;
            case 5:
                ns_uart_send_data(&uart_config, "Press button 0 and 1\n", 20);
                break;
            case 6:
                ns_uart_send_data(&uart_config, "Press button 11111111asfjlas fjasfadslfjasldf0\n", 47);
                break;
            case 7:
                ns_uart_send_data(&uart_config, "P\n", 1);
                break;
            default:
                ns_uart_send_data(&uart_config,rxBuffer, sizeof(rxBuffer));
        }
        // Receive data over UART
        ns_uart_receive_data(&uart_config, rxBuffer, 12);
        // strcpy(txBuffer, rxBuffer);
        ns_delay_us(500000);
    }
}