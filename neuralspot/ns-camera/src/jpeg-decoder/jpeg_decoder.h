#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "picojpeg.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    // Public
    // int width;
    // int height;
    // int comps;
    // int MCUSPerRow;
    // int MCUSPerCol;
    // pjpeg_scan_type_t scanType;
    // int MCUWidth;
    // int MCUHeight;
    int MCUx;
    int MCUy;
    // Private
    pjpeg_image_info_t imgInfo;
    uint8_t *jpg_data;
    uint8_t status;
    int is_available;
    int mcu_x;
    int mcu_y;
    uint32_t g_nInFileSize;
    uint32_t g_nInFileOfs;
    uint32_t row_pitch;
    // uint32_t decoded_width, decoded_height;
    // uint32_t row_blocks_per_mcu, col_blocks_per_mcu;
    uint16_t pImage[256];
} jpeg_decoder_context_t;

unsigned char pjpeg_callback(
    unsigned char *pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read,
    void *pCallback_data);
int jpeg_decoder_init(jpeg_decoder_context_t *ctx, const uint8_t *array, uint32_t array_size);
int jpeg_decoder_read(jpeg_decoder_context_t *ctx);
void jpeg_decoder_abort(jpeg_decoder_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
