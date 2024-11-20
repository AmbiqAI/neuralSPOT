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

ns_uart_config_t ns_uart_config = {
    .api = &ns_uart_V0_0_1,
    .uart_config = NULL};

extern uint32_t init_uart(am_hal_uart_config_t *uart_config);
extern void uart_write(char *pcStr, uint32_t ui32StrLen, uint32_t ui32BytesWritten);


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



//*****************************************************************************
//
// UART print string
//
//*****************************************************************************
void
ns_uart_write(char *pcStr)
{
    uint32_t ui32StrLen = 0;
    uint32_t ui32BytesWritten = 0;

    // Measure the length of the string.
    while (pcStr[ui32StrLen] != 0)
    {
        ui32StrLen++;
    }

    uart_write(pcStr, ui32StrLen, ui32BytesWritten);
}

void ns_uart_read(ns_uart_config_t *cfg) {
    uint32_t ui32StrLen = 0;
    uint32_t ui32BytesRead = 0;

    const am_hal_uart_transfer_t sUartRead =
    {
        .ui32Direction = AM_HAL_UART_READ,
        .pui8Data = (uint8_t *) cfg->uart_config->pui8RxBuffer,
        .ui32NumBytes = ui32StrLen,
        .ui32TimeoutMs = 0,
    };

    NS_TRY(am_hal_uart_transfer(phUART, &sUartRead), "Failed to read to UART\n");

}