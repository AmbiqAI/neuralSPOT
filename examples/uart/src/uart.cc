

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
};
int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    
    ns_lp_printf("UART init\n");
    NS_TRY(ns_uart_init(&uart_config), "UART init failed.\n");
    ns_interrupt_master_enable();

    ns_lp_printf("UART init done\n");
    
    char buffer[256];
    char txbuffer[256];
    char size[3];
    int num_bytes = 0;
    while(1) {
        // Receive number of bytes to read
        uint32_t status = ns_uart_receive_data(&uart_config, size, 3);
        if (status == AM_HAL_STATUS_SUCCESS) {
            num_bytes = atoi(size); // Convert the received size to an integer
            status = ns_uart_receive_data(&uart_config, buffer, num_bytes);
            int buffer_size = strlen(buffer);
            if(status == AM_HAL_STATUS_SUCCESS && num_bytes == buffer_size) {
                memcpy(txbuffer, buffer, num_bytes);
                ns_uart_send_data(&uart_config, txbuffer, num_bytes);
                memset(buffer, 0, num_bytes);
                memset(txbuffer, 0, num_bytes);
            }
            ns_delay_us(10000);
    }
    }
}