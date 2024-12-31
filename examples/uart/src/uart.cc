

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

//     char buffer[256];
//     while (1) {
//         // Receive data over UART
//         uint32_t status = ns_uart_receive_data(&uart_config, buffer, sizeof(buffer));
//         if (status == AM_HAL_STATUS_SUCCESS) {
//             ns_lp_printf("Received: %s\n", buffer);

//             // Send acknowledgment to the Python script
//             ns_uart_send_data(&uart_config, "ACK", 3);

//             // Prepare a response based on the received command
//             if (strcmp(buffer, "Hello, MCU!") == 0) {
//                 strcpy(buffer, "Hello, Python!\n");
//             } else if (strcmp(buffer, "How are you? A B C D E F G H I J K L M N O P Q R S T U V W X Y Z") == 0) {
//                 strcpy(buffer, "I'm fine, thank you!\n");
//             } else {
//                 // Generate a random response
//                 int response_size = rand() % 256;
//                 for (int i = 0; i < response_size; i++) {
//                     buffer[i] = 'A' + (rand() % 26);
//                 }
//                 buffer[response_size] = '\0';
//             }

//             // Send the response back to the Python script
//             ns_uart_send_data(&uart_config, buffer, strlen(buffer));
//         } else {
//             ns_lp_printf("Failed to receive data over UART. Error code: %u\n", status);
//         }

//         // Small delay before the next iteration
//         ns_delay_us(500000);
//     }

//     return 0;
    char buffer[256];
    char * size;
    int count = 1;
    char *txBuffer = "";
    while(1) {
        // Receive data over UART
        uint32_t status = ns_uart_receive_data(&uart_config, buffer, sizeof(buffer));
        if(status == AM_HAL_STATUS_SUCCESS) {
            ns_uart_send_data(&uart_config, buffer, strlen(buffer)-1);
        }
        // strcpy(txBuffer, rxBuffer);
        ns_delay_us(50000);
    }
    return 0;
}