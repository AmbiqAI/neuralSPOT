
#include "jpeg_decoder.h"
#include "picojpeg.h"

#ifndef jpg_min
    #define jpg_min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

unsigned char pjpeg_callback(
    unsigned char *pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read,
    void *pCallback_data) {
    uint32_t n;
    jpeg_decoder_context_t *ctx = (jpeg_decoder_context_t *)pCallback_data;

    n = jpg_min(ctx->g_nInFileSize - ctx->g_nInFileOfs, buf_size);
    for (int i = 0; i < n; i++) {
        pBuf[i] = *ctx->jpg_data++;
    }

    *pBytes_actually_read = (uint8_t)(n);
    ctx->g_nInFileOfs += n;
    return 0;
}

int jpeg_decoder_decode_mcu(jpeg_decoder_context_t *ctx) {
    ctx->status = pjpeg_decode_mcu();
    if (ctx->status) {
        ctx->is_available = 0;
        if (ctx->status != PJPG_NO_MORE_BLOCKS) {
            return -1;
        }
    }
    return 1;
}

int jpeg_decoder_read(jpeg_decoder_context_t *ctx) {
    int y, x;
    uint16_t *pDst_row;

    if (ctx->is_available == 0 || ctx->mcu_y >= ctx->imgInfo.m_MCUSPerCol) {
        jpeg_decoder_abort(ctx);
        return 0;
    }

    pDst_row = ctx->pImage;
    for (y = 0; y < ctx->imgInfo.m_MCUHeight; y += 8) {

        const int by_limit =
            jpg_min(8, ctx->imgInfo.m_height - (ctx->mcu_y * ctx->imgInfo.m_MCUHeight + y));

        for (x = 0; x < ctx->imgInfo.m_MCUWidth; x += 8) {
            uint16_t *pDst_block = pDst_row + x;

            uint32_t src_ofs = (x * 8U) + (y * 16U);
            const uint8_t *pSrcR = ctx->imgInfo.m_pMCUBufR + src_ofs;
            const uint8_t *pSrcG = ctx->imgInfo.m_pMCUBufG + src_ofs;
            const uint8_t *pSrcB = ctx->imgInfo.m_pMCUBufB + src_ofs;

            const int bx_limit =
                jpg_min(8, ctx->imgInfo.m_width - (ctx->mcu_x * ctx->imgInfo.m_MCUWidth + x));

            if (ctx->imgInfo.m_scanType == PJPG_GRAYSCALE) {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++) {
#ifdef SWAP_BYTES
                        *pDst++ = (*pSrcR & 0xF8) | (*pSrcR & 0xE0) >> 5 | (*pSrcR & 0xF8) << 5 |
                                  (*pSrcR & 0x1C) << 11;
#else
                        *pDst++ = (*pSrcR & 0xF8) << 8 | (*pSrcR & 0xFC) << 3 | *pSrcR >> 3;
#endif
                        pSrcR++;
                    }

                    pSrcR += (8 - bx_limit);

                    pDst_block += ctx->row_pitch;
                }
            } else {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++) {
#ifdef SWAP_BYTES
                        *pDst++ = (*pSrcR & 0xF8) | (*pSrcG & 0xE0) >> 5 | (*pSrcB & 0xF8) << 5 |
                                  (*pSrcG & 0x1C) << 11;
#else
                        *pDst++ = (*pSrcR & 0xF8) << 8 | (*pSrcG & 0xFC) << 3 | *pSrcB >> 3;
#endif
                        pSrcR++;
                        pSrcG++;
                        pSrcB++;
                    }

                    pSrcR += (8 - bx_limit);
                    pSrcG += (8 - bx_limit);
                    pSrcB += (8 - bx_limit);

                    pDst_block += ctx->row_pitch;
                }
            }
        }
        pDst_row += (ctx->row_pitch * 8);
    }

    ctx->MCUx = ctx->mcu_x;
    ctx->MCUy = ctx->mcu_y;

    ctx->mcu_x++;
    if (ctx->mcu_x == ctx->imgInfo.m_MCUSPerRow) {
        ctx->mcu_x = 0;
        ctx->mcu_y++;
    }

    if (jpeg_decoder_decode_mcu(ctx) == -1) {
        ctx->is_available = 0;
    }

    return 1;
}

int jpeg_decoder_read_swapped_bytes(jpeg_decoder_context_t *ctx) {
    int y, x;
    uint16_t *pDst_row;

    if (ctx->is_available == 0 || ctx->mcu_y >= ctx->imgInfo.m_MCUSPerCol) {
        jpeg_decoder_abort(ctx);
        return 0;
    }

    pDst_row = ctx->pImage;
    for (y = 0; y < ctx->imgInfo.m_MCUHeight; y += 8) {

        const int by_limit =
            jpg_min(8, ctx->imgInfo.m_height - (ctx->mcu_y * ctx->imgInfo.m_MCUHeight + y));

        for (x = 0; x < ctx->imgInfo.m_MCUWidth; x += 8) {
            uint16_t *pDst_block = pDst_row + x;

            uint32_t src_ofs = (x * 8U) + (y * 16U);
            const uint8_t *pSrcR = ctx->imgInfo.m_pMCUBufR + src_ofs;
            const uint8_t *pSrcG = ctx->imgInfo.m_pMCUBufG + src_ofs;
            const uint8_t *pSrcB = ctx->imgInfo.m_pMCUBufB + src_ofs;

            const int bx_limit =
                jpg_min(8, ctx->imgInfo.m_width - (ctx->mcu_x * ctx->imgInfo.m_MCUWidth + x));

            if (ctx->imgInfo.m_scanType == PJPG_GRAYSCALE) {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++) {

                        *pDst++ = (*pSrcR & 0xF8) | (*pSrcR & 0xE0) >> 5 | (*pSrcR & 0xF8) << 5 |
                                  (*pSrcR & 0x1C) << 11;

                        pSrcR++;
                    }
                }
            } else {
                int bx, by;
                for (by = 0; by < by_limit; by++) {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++) {

                        *pDst++ = (*pSrcR & 0xF8) | (*pSrcG & 0xE0) >> 5 | (*pSrcB & 0xF8) << 5 |
                                  (*pSrcG & 0x1C) << 11;

                        pSrcR++;
                        pSrcG++;
                        pSrcB++;
                    }

                    pSrcR += (8 - bx_limit);
                    pSrcG += (8 - bx_limit);
                    pSrcB += (8 - bx_limit);

                    pDst_block += ctx->row_pitch;
                }
            }
        }
        pDst_row += (ctx->row_pitch * 8);
    }

    ctx->MCUx = ctx->mcu_x;
    ctx->MCUy = ctx->mcu_y;

    ctx->mcu_x++;
    if (ctx->mcu_x == ctx->imgInfo.m_MCUSPerRow) {
        ctx->mcu_x = 0;
        ctx->mcu_y++;
    }

    if (jpeg_decoder_decode_mcu(ctx) == -1) {
        ctx->is_available = 0;
    }

    return 1;
}

int jpeg_decoder_init(jpeg_decoder_context_t *ctx, const uint8_t *array, uint32_t array_size) {
    ctx->mcu_x = 0;
    ctx->mcu_y = 0;
    ctx->status = 0;
    ctx->is_available = 0;
    ctx->g_nInFileOfs = 0;
    ctx->jpg_data = (uint8_t *)array;
    ctx->g_nInFileSize = array_size;

    ctx->status = pjpeg_decode_init(&ctx->imgInfo, pjpeg_callback, ctx, 0);

    if (ctx->status) {
        return 0;
    }

    ctx->row_pitch = ctx->imgInfo.m_MCUWidth;

    memset(
        ctx->pImage, 0, ctx->imgInfo.m_MCUWidth * ctx->imgInfo.m_MCUHeight * sizeof(*ctx->pImage));

    ctx->is_available = 1;

    return jpeg_decoder_decode_mcu(ctx);
}

void jpeg_decoder_abort(jpeg_decoder_context_t *ctx) {
    ctx->mcu_x = 0;
    ctx->mcu_y = 0;
    ctx->is_available = 0;
}
