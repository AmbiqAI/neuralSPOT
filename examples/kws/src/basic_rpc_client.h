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

#include "basic_tf_stub.h"
#include "ns_core.h"
#include "ns_rpc_generic_data.h"

// Vars and init the RPC system - note this also inits the USB interface

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
// RPC uses malloc internally, so we need to declare it here
uint8_t ucHeap[NS_RPC_MALLOC_SIZE_IN_K * 1024] __attribute__((aligned(4)));
#endif

static char msg_store[30] = "Audio16bPCM_to_WAV";

// USB bufffers declared locally
#define MY_USB_RX_BUFSIZE 2048
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];

// Block sent to PC
static dataBlock outBlock = {
    .length = SAMPLES_IN_FRAME * sizeof(int16_t),
    .dType = uint8_e,
    .description = msg_store,
    .cmd = write_cmd,
    .buffer = {
        .data = (uint8_t *)audioDataBuffer, // point this to audio buffer
        .dataLength = SAMPLES_IN_FRAME * sizeof(int16_t)}};

static ns_rpc_config_t rpcConfig = {
    .api = &ns_rpc_gdo_V1_1_0,
    .mode = NS_RPC_GENERICDATA_CLIENT,
    .rx_buf = my_cdc_rx_ff_buf,
    .rx_bufLength = MY_USB_RX_BUFSIZE,
    .tx_buf = my_cdc_tx_ff_buf,
    .tx_bufLength = MY_USB_TX_BUFSIZE,
    .sendBlockToEVB_cb = NULL,
    .fetchBlockFromEVB_cb = NULL,
    .computeOnEVB_cb = NULL,
    .transport = NS_RPC_TRANSPORT_USB};
