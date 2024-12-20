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

const ns_core_api_t ns_uart_V0_0_1 = {.apiId = NS_UART_API_ID, .version = NS_UART_V0_0_1};

const ns_core_api_t ns_uart_oldest_supported_version = {
    .apiId = NS_UART_API_ID, .version = NS_UART_V0_0_1};

const ns_core_api_t ns_uart_current_version = {.apiId = NS_UART_API_ID, .version = NS_UART_V0_0_1};

// // UART interrupt handler.
// am_hal_uart_config_t g_sUartConfig =
// {
//     // Standard UART settings: 115200-8-N-1
//     .ui32BaudRate = 115200,
// #if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
//     .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
//     // Set TX and RX FIFOs to interrupt at half-full.
//     .ui32Parity = AM_HAL_UART_PARITY_NONE,
//     .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
//     .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
//     .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
//                        AM_HAL_UART_RX_FIFO_1_2),
//     // Buffers
//     .pui8TxBuffer = g_pui8TxBuffer,
//     .ui32TxBufferSize = (uint32_t) sizeof(g_pui8TxBuffer),
//     .pui8RxBuffer = g_pui8RxBuffer,
//     .ui32RxBufferSize = (uint32_t) sizeof(g_pui8RxBuffer),
// #endif
// #if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
//     .eDataBits = AM_HAL_UART_DATA_BITS_8,
//     .eParity = AM_HAL_UART_PARITY_NONE,
//     .eStopBits = AM_HAL_UART_ONE_STOP_BIT,
//     .eFlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
//     .eTXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
//     .eRXFifoLevel = AM_HAL_UART_FIFO_LEVEL_16,
// #endif
// };

ns_uart_config_t ns_uart_config = {
    .api = &ns_uart_V0_0_1,
    .uart_config = NULL};

extern uint32_t init_uart(am_hal_uart_config_t *uart_config);
extern void ns_uart_send_data(ns_uart_config_t * cfg, char *txBuffer, uint32_t size);
extern void ns_uart_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size);
uint32_t ns_uart_init(ns_uart_config_t *cfg) {
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
    return init_uart(cfg->uart_config);
}
