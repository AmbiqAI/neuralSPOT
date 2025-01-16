/**
 * @file ns_rpc_generic_data.h
 * @author Carlos Morales
 * @brief API for using generic data RPC interface
 * @version 0.1
 * @date 2022-08-26
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-rpc-generic-data
 * @{
 * @ingroup ns-rpc
 *
 */

#ifndef NS_RPC_GENERIC_DATA_H
    #define NS_RPC_GENERIC_DATA_H

    #include "GenericDataOperations_EvbToPc.h"
    #include "GenericDataOperations_PcToEvb.h"
    #include "erpc_server_setup.h"
    #ifdef __cplusplus
extern "C" {
    #endif
    #include "ns_core.h"
#ifdef NS_USB_PRESENT
    #include "ns_usb.h"
#endif
    #include "ns_uart.h"
    #define NS_RPC_GDO_V0_0_1                                                                      \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_RPC_GDO_V1_0_0                                                                      \
        { .major = 1, .minor = 0, .revision = 0 }
    #define NS_RPC_GDO_V1_1_0                                                                      \
        { .major = 1, .minor = 1, .revision = 0 }

    #define NS_RPC_GDO_OLDEST_SUPPORTED_VERSION NS_RPC_GDO_V0_0_1
    #define NS_RPC_GDO_CURRENT_VERSION NS_RPC_GDO_V1_1_0
    #define NS_RPC_GDO_API_ID 0xCA0100

    #define NS_RPC_MALLOC_SIZE_IN_K 8
extern const ns_core_api_t ns_rpc_gdo_V0_0_1;
extern const ns_core_api_t ns_rpc_gdo_V1_0_0;
extern const ns_core_api_t ns_rpc_gdo_V1_1_0;
extern const ns_core_api_t ns_rpc_gdo_oldest_supported_version;
extern const ns_core_api_t ns_rpc_gdo_current_version;

typedef status (*ns_rpc_data_sendBlockToEVB_cb)(const dataBlock *block);

typedef status (*ns_rpc_data_fetchBlockFromEVB_cb)(dataBlock *block);

typedef status (*ns_rpc_data_computeOnEVB_cb)(const dataBlock *in_block, dataBlock *result_block);

typedef enum { NS_RPC_GENERICDATA_CLIENT, NS_RPC_GENERICDATA_SERVER } rpcGenericDataMode_e;

typedef enum { NS_RPC_TRANSPORT_USB, NS_RPC_TRANSPORT_UART } ns_rpc_transport_e;
/**
 * @brief RPC Configuration Struct
 *
 */
typedef struct {
    const ns_core_api_t *api;  ///< API prefix
    rpcGenericDataMode_e mode; ///< Client or Server mode
    uint8_t *rx_buf;        ///< Pointer to allocated buffer which USB CDC will use for rx transfers
    uint32_t rx_bufLength;  ///< Length of rx buffer
    uint8_t *tx_buf;        ///< Pointer to allocated buffer which USB CDC will use for tx transfers
    uint32_t tx_bufLength;  ///< Length of tx buffer
#ifdef NS_USB_PRESENT
    usb_handle_t usbHandle; ///< USB handle
#endif
    ns_uart_handle_t uartHandle; ///< UART handle
    ns_rpc_data_sendBlockToEVB_cb sendBlockToEVB_cb;       ///< Callback for sendBlockToEVB
    ns_rpc_data_fetchBlockFromEVB_cb fetchBlockFromEVB_cb; ///< Callback for fetchBlockFromEVB
    ns_rpc_data_computeOnEVB_cb computeOnEVB_cb;           ///< Callback for computeOnEVB
    ns_rpc_transport_e transport; ///< Transport type USB or UART
} ns_rpc_config_t;

/**
 * @brief Initialize the GenericDataOperations RPC system
 *
 * @param cfg
 * @return uint16_t Status
 */
extern uint16_t ns_rpc_genericDataOperations_init(ns_rpc_config_t *cfg);

extern uint16_t ns_rpc_genericDataOperationsClient_reset(ns_rpc_config_t *cfg);

/**
 * @brief Helper function for printing a block's contents
 *
 * @param block to be printed
 */
extern void ns_rpc_genericDataOperations_printDatablock(const dataBlock *block);

/**
 * @brief Helper function to be called after processing the block from ns_rpc_data_fetchBlockFromPC
 * or result block from ns_rpc_data_computeOnPC. This will free() the description
 * and buffer.data block struct members, so only do it after you're done with those!
 *
 * @param block Block to be freed
 */
extern void ns_rpc_data_clientDoneWithBlockFromPC(const dataBlock *block);

/**
 * @brief Enable RPC server and prepare to receive RPC calls
 *
 * @param cfg
 */
extern void ns_rpc_genericDataOperations_enableServerPoll(ns_rpc_config_t *cfg);

/**
 * @brief Disable RPC server and stop receiving RPC calls
 *
 * @param cfg
 */
extern void ns_rpc_genericDataOperations_disableServerPoll(ns_rpc_config_t *cfg);

/**
 * @brief Polls the server for incoming RPC calls. Must be called
 * to handle incoming RPC calls (for example, when a USB or timer event
 * occurs)
 *
 * @param cfg
 */
extern void ns_rpc_genericDataOperations_pollServer(ns_rpc_config_t *cfg);

    #ifdef __cplusplus
}
    #endif
#endif    // NS_RPC_GENERIC_DATA_H
/** @} */ // end of ns-rpc-generic-data
