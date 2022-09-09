/**
 * @file ns-rpc-audio.h
 * @author Carlos Morales
 * @brief API for using Audio RPC interface
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef NS_RPC_GENERIC_DATA_H
#define NS_RPC_GENERIC_DATA_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "erpc_server_setup.h"
#include "../src/GenericDataOperations_EvbToPc.h"
#include "../src/GenericDataOperations_PcToEvb.h"

extern uint16_t ns_rpc_genericDataOperations_init();

#ifdef __cplusplus
}
#endif

#endif // NS_RPC_GENERIC_DATA_H