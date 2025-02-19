/**
 * @file ns_port.c
 * @author Ambiq
 * @brief Wrapper and ports for opus1.4
 * @version 0.1
 * @date 2023-11-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ns_core.h"
#include "ns_malloc.h"
#include "opus.h"
#include "opus_types.h"
#include "ns_ambiqsuite_harness.h"

void* alloca(int size)
{
    ns_lp_printf("alloca asked for %d bytes\n", size);
    char *ptr = ns_malloc(size);
    ns_lp_printf("alloca returning %p\n", ptr);
    return (void*)ptr;
    // return ns_malloc(size);
}

OpusEncoder *ns_opus_encoder_default_create()
{
    // Create a default encoder typical to neuralspot applications
    // 16khz, 1 channel, 20ms frame size, low complexity
    int sample_rate = 16000;
    int frame_size = 320;
    int complexity = 3;
    int channels = 1;
    int application = OPUS_APPLICATION_VOIP;

    OpusEncoder *enc = opus_encoder_create(sample_rate, channels, application, NULL);
    if (enc == NULL)
    {
        ns_lp_printf("Failed to create an opus encoder\n");
        return NULL;
    }
    // Set the encoder parameters
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_BITRATE(sample_rate)),"OPUS_SET_BITRATE");
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_VBR(0)), "OPUS_SET_VBR");
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(0)), "OPUS_SET_VBR_CONSTRAINT");
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(complexity)), "OPUS_SET_COMPLEXITY");
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE)), "OPUS_SET_SIGNAL");
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_MAX_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND)), "OPUS_SET_MAX_BANDWIDTH");
    NS_TRY(opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_20_MS)), "OPUS_SET_EXPERT_FRAME_DURATION");

    return enc;
} 

int ns_opus_encode_frame(OpusEncoder *st, const opus_int16 *pcm, int frame_size, unsigned char *data, opus_int32 max_data_bytes)
{
    return opus_encode(st, pcm, frame_size, data, max_data_bytes);
}

