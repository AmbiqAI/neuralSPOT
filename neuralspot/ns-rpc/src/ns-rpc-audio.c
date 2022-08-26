/**
 * @file ns-rpc-audio.c
 * @author Carlos Morales
 * @brief API for Audio RPC
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ns-usb.h"
#include "ns-rpc-audio.h"
#include "erpc_client_setup.h"
#include "ns_audio_rpc.h"
#include "ns_ambiqsuite_harness.h"

#ifdef NS_RPC_AUDIO

ns_usb_config_t g_RpcAudioUSBHandle = {
        .deviceType = NS_USB_CDC_DEVICE,
        .buffer = NULL, // Not needed by RPC
        .bufferLength = 0,
        .rx_cb = NULL,
        .tx_cb = NULL
};

uint16_t ns_rpc_audio_init() {

    usb_handle_t usb_handle = ns_usb_init(&g_RpcAudioUSBHandle);
 
    /* Init eRPC client environment */
    /* USB transport layer initialization */
    erpc_transport_t transport = erpc_transport_usb_cdc_init(usb_handle);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();

    /* eRPC client side initialization */
    erpc_client_init(transport, message_buffer_factory);

    return 1;
}

nsAudioStatus_e ns_rpc_audio_send_buffer(uint8_t *buf, uint32_t len) { 
    // Set up audio dump command
    nsAudioBuffer_t dump = {
        .bufferLength = len,
        .buf = buf
    };

    audioCommand_t cmd = {
        .cmd = nsAudioDisplayAudio,
        .buf = dump
    };
    return erpcDumpAudioBuffer(&cmd);
}

#else
uint16_t ns_rpc_audio_init() {return 1;}
nsAudioStatus_e ns_rpc_audio_send_buffer(uint8_t *buf, uint32_t len) { return 1;}
#endif