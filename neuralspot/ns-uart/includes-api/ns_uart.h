/**
 * @file ns-uart.h
 * @author Evan Chen
 * @brief API definition for NeuralSPOT UART
 * @version 0.1
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 *
 * \addtogroup ns-uart
 * @{
 *
 */

#ifndef NS_UART
    #define NS_UART

    #ifdef __cplusplus
extern "C" {
    #endif
    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"
    #include "ns_core.h"

    #define NS_UART_V0_0_1                                                                          \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_UART_API_ID 0xCA000B  
    #define UART_ID                            (0)

extern const ns_core_api_t ns_uart_V0_0_1;
extern const ns_core_api_t ns_uart_oldest_supported_version;
extern const ns_core_api_t ns_uart_current_version;
extern am_hal_uart_config_t g_sUartConfig;
typedef void * ns_uart_handle_t;
extern ns_uart_handle_t phUART;

/**
 * @brief UART Configuration Struct used to configure hardware settings
 *
 */
typedef struct
{
    const ns_core_api_t *                 api;                            ///< API prefix
    am_hal_uart_config_t *          uart_config;                    ///< UART Configuration
}
ns_uart_config_t;

/**
 * @brief Initialize the UART system
 *
 * @param cfg
 * @param handle
 * @return uint32_t Status
 */
extern uint32_t ns_uart_init(ns_uart_config_t * cfg);


/**
 * @brief Send data through UART tx buffer
 *
 * @param pcStr
 */
extern void ns_uart_send_data(ns_uart_config_t * cfg, char *txBuffer, uint32_t size);

/**
 * @brief Read from the UART rx buffer
 *
 * @param cfg
 */
extern void ns_uart_receive_data(ns_uart_config_t *cfg, char * rxBuffer, uint32_t size);


    #ifdef __cplusplus
}
    #endif
#endif
/** @}*/
