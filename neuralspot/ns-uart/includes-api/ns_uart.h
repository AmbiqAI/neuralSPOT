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
    #define TIMER_NUM       0               ///< Timer number used in the example
    #define MAX_UART_PACKET_SIZE            (2048)
    #define UART_RX_TIMEOUT_MS              (5)
    #define NUM_UARTS_USED 1
    // #define NS_UART_OLDEST_SUPPORTED_VERSION NS_UART_V0_0_1
    // #define NS_UART_CURRENT_VERSION NS_UART_V0_0_1
    // #define TEST_UART_TX_MODULE     3
    // #define TEST_UART_RX_MODULE     0

    // #define CUST_UART0RX_PIN        62 // VERIFY ALL APOLLO PINS
    // #define CUST_UART0RTS_PIN       58 // VERIFY ALL APOLLO PINS

    // #define UART_DMA_ENABLED        1
    // #define UART_BUFFER_SIZE        2048
    // #define UART_QUEUE_SIZE         2048
    // #define UART_TEST_PACKET_SIZE   2048

// AM_SHARED_RW uint8_t ui8TxBuffer[UART_BUFFER_SIZE] __attribute__((aligned(32)));
// AM_SHARED_RW uint8_t ui8RxBuffer[UART_BUFFER_SIZE] __attribute__((aligned(32)));

extern const ns_core_api_t ns_uart_V0_0_1;
extern const ns_core_api_t ns_uart_oldest_supported_version;
extern const ns_core_api_t ns_uart_current_version;

typedef void *ns_uart_handle_t;
extern ns_uart_handle_t phUART;

// typedef void (*ns_uart_rx_cb)(ns_uart_transaction_t *);
// typedef void (*ns_uart_tx_cb)(ns_uart_transaction_t *);
// typedef void (*ns_uart_service_cb)(uint8_t);


/**
 * @brief UART Configuration Struct used to configure hardware settings
 *
 */
typedef struct
{
    const ns_core_api_t *                 api;                            ///< API prefix
    am_hal_uart_config_t *          uart_config;                    ///< UART Configuration
// #if defined(AM_PART_APOLLO4) || defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L) // The Apollo4 am_hal_uart_config_t struct does not contain the rx or tx buffers, so we declare them here
//     uint8_t * rxBuffer; // RX buffer 
// #endif
}
ns_uart_config_t;

/**
 * @brief Initialize the USB system
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
extern void ns_uart_send_data(ns_uart_config_t * cfg, char *pcStr, uint32_t size);

/**
 * @brief Read from the UART rx buffer
 *
 * @param cfg
 */
extern void ns_uart_receive_data(ns_uart_config_t *cfg);


    #ifdef __cplusplus
}
    #endif
#endif
/** @}*/
