/**
 * @file ns_opus_port.h
 * @author Ambiq
 * @brief 
 * @version 0.1
 * @date 2023-11-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NS_OPUS_PORT
#define NS_OPUS_PORT
    #ifdef __cplusplus
extern "C" {
    #endif
#include "ns_malloc.h"
#include "opus.h"
#include "opus_types.h"

OpusEncoder *ns_opus_encoder_default_create();
int ns_opus_encode_frame(OpusEncoder *st, const opus_int16 *pcm, int frame_size, unsigned char *data, opus_int32 max_data_bytes);

#endif
    #ifdef __cplusplus
}
    #endif