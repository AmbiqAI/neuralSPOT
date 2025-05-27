/**
 * @file ns-uart.c
 * @author Evan Chen
 * @brief NeuralSPOT UART Utilities
 *
 * @version 0.1
 * @date 2024-11-13
 *
 * Helper library for neuralspot features using UART
 * @copyright Copyright (c) 2024
 *
 */

#include "ns_uart.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_timer.h"

// UART handle
ns_uart_handle_t phUART;
volatile bool g_DataAvailable = false;
const ns_core_api_t ns_uart_V0_0_1 = {.apiId = NS_UART_API_ID, .version = NS_UART_V0_0_1};

const ns_core_api_t ns_uart_oldest_supported_version = {
    .apiId = NS_UART_API_ID, .version = NS_UART_V0_0_1};

const ns_core_api_t ns_uart_current_version = {.apiId = NS_UART_API_ID, .version = NS_UART_V0_0_1};

ns_uart_config_t ns_uart_config = {
    .api = &ns_uart_V0_0_1,
    .uart_config = NULL};

extern uint32_t init_uart(am_hal_uart_config_t *uart_config);
extern uint32_t ns_uart_blocking_send_data(ns_uart_config_t * cfg, char *txBuffer, uint32_t size);
extern uint32_t ns_uart_nonblocking_send_data(ns_uart_config_t * cfg, char *txBuffer, uint32_t size);
extern uint32_t ns_uart_blocking_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size);
extern uint32_t ns_uart_nonblocking_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size);


void ns_uart_register_callbacks(ns_uart_handle_t handle, ns_uart_rx_cb rxcb, ns_uart_tx_cb txcb) {
    ((ns_uart_config_t *)handle)->rx_cb = rxcb;
    ((ns_uart_config_t *)handle)->tx_cb = txcb;
}

uint32_t ns_uart_init(ns_uart_config_t *cfg, ns_uart_handle_t * handle) {
#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(cfg->api, &ns_uart_oldest_supported_version, &ns_uart_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
    if(cfg == NULL || cfg->uart_config == NULL) {
        return NS_STATUS_INVALID_CONFIG;
    }
    ns_uart_config.uart_config = cfg->uart_config;
    ns_uart_config.rx_blocking = cfg->rx_blocking;
    ns_uart_config.tx_blocking = cfg->tx_blocking;
    *handle = (void *)&ns_uart_config;
    return init_uart(cfg->uart_config);
}

bool ns_uart_data_available(void) {
    return g_DataAvailable;
}


uint32_t ns_uart_change_baud_rate(ns_uart_handle_t handle, uint32_t baud_rate) {
    am_hal_uart_interrupt_disable(phUART, (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_TX | AM_HAL_UART_INT_RX_TMOUT));
    ((ns_uart_config_t *)handle)->uart_config->ui32BaudRate = baud_rate;
    NS_TRY(am_hal_uart_configure(phUART,
        ((ns_uart_config_t *)handle)->uart_config),
        "Failed to reconfigure baud rate");
    am_hal_uart_interrupt_enable(phUART, (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_TX | AM_HAL_UART_INT_RX_TMOUT));

    return NS_STATUS_SUCCESS;
}