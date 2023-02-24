/**
 * @file basic_rpc_client.h
 * @author Basic RPC Client stuff
 * @brief
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "har.h"
#include "ns_core.h"
#include "ns_rpc_generic_data.h"

// Vars and init the RPC system - note this also inits the USB interface

static char msg_store[30] = "Audio16bPCM_to_WAV";

// Block sent to PC
static dataBlock outBlock = {
    .length = SAMPLES_IN_FRAME * sizeof(int16_t),
    .dType = uint8_e,
    .description = msg_store,
    .cmd = write_cmd,
    .buffer = {.data = (uint8_t *)in16AudioDataBuffer, // point this to audio buffer
               .dataLength = SAMPLES_IN_FRAME * sizeof(int16_t)}};

static ns_rpc_config_t rpcConfig = {.api = &ns_rpc_gdo_V1_0_0,
                                    .mode = NS_RPC_GENERICDATA_CLIENT,
                                    .sendBlockToEVB_cb = NULL,
                                    .fetchBlockFromEVB_cb = NULL,
                                    .computeOnEVB_cb = NULL};
