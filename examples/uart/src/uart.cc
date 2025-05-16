

#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_power.h"
#include "ns_timer.h"
#include "am_hal_mcuctrl.h"
#include "ns_uart.h"
#include <stdio.h>
#include <string.h>
#include "ns_peripherals_button.h"
#include <stdlib.h>

ns_uart_config_t uart_config = {
    .api=&ns_uart_V0_0_1,
    .uart_config = &g_sUartConfig,
    .tx_blocking = true,
    .rx_blocking = true,
};
char txbuffer[256];

void my_rx_cb(ns_uart_transaction_t *t)
{
    static int i = 0;
    char data;
    if(ns_uart_nonblocking_receive_data(&uart_config, &data, 1) == AM_HAL_STATUS_SUCCESS)
    {
        txbuffer[i++] = data;
        ns_uart_blocking_send_data(&uart_config, &txbuffer[i-1], 1);

        txbuffer[i++] = data;
        if (i >= sizeof(txbuffer)) i = 0;
    }
}

ns_uart_handle_t uart_handle = NULL;
int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0); // nada
#endif
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    ns_itm_printf_enable();                                     // nada
    ns_lp_printf("UART init\n");

    NS_TRY(ns_uart_init(&uart_config, &uart_handle), "UART init failed.\n");
    ns_uart_register_callbacks(uart_handle, my_rx_cb, NULL);
    ns_interrupt_master_enable();

    ns_lp_printf("UART init done\n");
    

    while (1) {
    }

}