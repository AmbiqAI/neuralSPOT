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

#ifndef NS_RPC_AUDIO_H
#define NS_RPC_AUDIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ns_audio_rpc.h"

extern uint16_t ns_rpc_audio_init();
extern nsAudioStatus_e ns_rpc_audio_send_buffer(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // NS_RPC_AUDIO_H