/**
 * @file camera.cc
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Camera controller
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ns_camera.h"
#include "ArducamCamera.h"
#include "arm_math.h"
#include "jpeg-decoder/jpeg_decoder.h"
#include "jpeg-decoder/picojpeg.h"
#include "ns_spi.h"
#include "ns_ambiqsuite_harness.h"

const ns_core_api_t ns_camera_V1_0_0 = {.apiId = NS_CAMERA_API_ID, .version = NS_CAMERA_V1_0_0};
const ns_core_api_t ns_camera_oldest_supported_version = {
    .apiId = NS_CAMERA_API_ID, .version = NS_CAMERA_V1_0_0};
const ns_core_api_t ns_camera_current_version = {
    .apiId = NS_CAMERA_API_ID, .version = NS_CAMERA_V1_0_0};

#define MAX_SPI_BUF_LEN 64

jpeg_decoder_context_t jpegCtx = {};
ArducamCamera camera; // Arducam driver assumes this is a global, so :shrug:
ns_spi_config_t *spiHandle = NULL;
ns_camera_config_t ns_camera_config;
bool nsCameraPictureBeingTaken = false;

// Some Arducam functions are re-written in this file, make aliases of registers
#define ARDU_BURST_FIFO_READ 0x3C    // Burst FIFO read operation
#define NS_CAMERA_ARDUCHIP_TRIG 0x44 // Trigger source
#define NS_CAMERA_VSYNC_MASK 0x01
#define NS_CAMERA_SHUTTER_MASK 0x02
#define NS_CAMERA_CAP_DONE_MASK 0x04
#define NS_CAM_REG_FORMAT 0X20
#define NS_CAM_REG_CAPTURE_RESOLUTION 0X21
#define NS_CAM_SET_CAPTURE_MODE (0 << 7)

// DMA read state
// SPI DMA max xfer size is 4095 on AP4, so we have to chunk it
#define MAX_SPI_DMA_LEN 4095
static uint32_t dma_total_requested_length;
static uint32_t dma_current_chunk_length;
static uint32_t dma_offset;
static uint8_t *dma_cambuf;

bool ns_read_done = false;

void ns_camera_buff_read_done(ns_spi_config_t *cfg) {
    // ns_printf("Chunk Read done for chunk starting at offset %d\n", dma_offset);

    // Calculate next chunk offset
    dma_offset += dma_current_chunk_length;

    // Check if we have more to read
    if (dma_offset < dma_total_requested_length) {
        // Calculate next chunk length
        dma_current_chunk_length = (dma_total_requested_length - dma_offset) > MAX_SPI_DMA_LEN
                                       ? MAX_SPI_DMA_LEN
                                       : (dma_total_requested_length - dma_offset);

        // Read next chunk
        // ns_lp_printf("Starting next dma addr %d len %d\n", dma_offset, dma_current_chunk_length);
        ns_spi_read_dma(
            spiHandle, dma_offset + dma_cambuf, dma_current_chunk_length, ARDU_BURST_FIFO_READ, 1,
            camera.csPin);
    } else {
        ns_read_done = true;
        // ns_lp_printf("DMA Read done\n");
        if (ns_camera_config.dmaCompleteCb) {
            ns_camera_config.dmaCompleteCb(&ns_camera_config);
        }
        if (camera.burstFirstFlag == 0) {
            camera.burstFirstFlag = 1;
        }
        camera.receivedLength -= dma_total_requested_length;
    }
}

void ns_camera_check_picture_completion(ns_timer_config_t *timer) {
    if (!nsCameraPictureBeingTaken) {
        // ns_lp_printf("Not taking picture\n");
        return;
    }
    if (getBit(&camera, NS_CAMERA_ARDUCHIP_TRIG, NS_CAMERA_CAP_DONE_MASK) == 0) {
        // ns_lp_printf("Picture not done\n");
        return;
    }
    // ns_lp_printf("Picture done\n");
    camera.receivedLength = readFifoLength(&camera);
    camera.totalLength = camera.receivedLength;
    camera.burstFirstFlag = 0;

    if (ns_camera_config.pictureTakenCb) {
        // ns_lp_printf("Calling picture taken CB\n");
        ns_camera_config.pictureTakenCb(&ns_camera_config);
    }
    nsCameraPictureBeingTaken = false;
}

ns_timer_config_t timerCfg = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_INTERRUPT,
    .enableInterrupt = true,
    .periodInMicroseconds = 80000,
    .callback = ns_camera_check_picture_completion,
};

uint32_t ns_camera_init(ns_camera_config_t *cfg) {
    // Check API
    // Only IOM1 is supported currently - check it
    // Check that buff size is sane
    // Check that picture mode is sane
    // Check that pix format is sane

    spiHandle = &cfg->spiConfig;
    cfg->spiConfig.cb = ns_camera_buff_read_done;
    memcpy(&ns_camera_config, cfg, sizeof(ns_camera_config_t));

    am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_IOM1);
    if (ns_spi_interface_init(&cfg->spiConfig, cfg->spiSpeed, AM_HAL_IOM_SPI_MODE_0)) {
        return NS_STATUS_INIT_FAILED;
    }
    createArducamCamera(AM_BSP_IOM1_CS_CHNL); // inits camera global, other stuff

    // Start a polling timer is needed
    if (cfg->pictureTakenCb != NULL) {
        // ns_lp_printf("Starting camera timer\n");
        NS_TRY(ns_timer_init(&timerCfg), "Failed to init camera timer\n");
    }

    begin(&camera);
    lowPowerOn(&camera);
    return NS_STATUS_SUCCESS;
}

int arducam_spi_read(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin) {
    uint32_t err = 0;
    uint8_t *bufPtr = (uint8_t *)buf;
    uint32_t bytesLeft = bufLen;
    uint32_t chunkSize;
    while (bytesLeft) {
        chunkSize = bytesLeft > MAX_SPI_BUF_LEN ? MAX_SPI_BUF_LEN : bytesLeft;
        err = ns_spi_read(spiHandle, bufPtr, chunkSize, reg, regLen, csPin);
        bufPtr += chunkSize;
        bytesLeft -= chunkSize;
    }
    return err;
}

int arducam_spi_write(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin) {
    uint8_t *bufPtr = (uint8_t *)buf;
    uint32_t bytesLeft = bufLen;
    uint32_t chunkSize;
    while (bytesLeft) {
        chunkSize = bytesLeft > MAX_SPI_BUF_LEN ? MAX_SPI_BUF_LEN : bytesLeft;
        ns_spi_write(spiHandle, bufPtr, chunkSize, reg, regLen, csPin);
        bufPtr += chunkSize;
        bytesLeft -= chunkSize;
    }
    return 0;
}

void arducam_delay_ms(uint16_t delay) { ns_delay_us(1000 * delay); }
void arducam_delay_us(uint16_t delay) { ns_delay_us(delay); }

// Re-implementation of Arducam readBuf using SPI DMA. This just kicks off the DMA.
uint32_t ns_read_buff(ArducamCamera *camera, uint8_t *buff, uint32_t length) {
    if (cameraImageAvailable(camera) == 0 || (length == 0)) {
        return 0;
    }

    if (camera->receivedLength < length) {
        length = camera->receivedLength;
    }
    uint32_t err = ns_spi_read_dma(spiHandle, buff, length, ARDU_BURST_FIFO_READ, 1, camera->csPin);

    if (camera->burstFirstFlag == 0) {
        camera->burstFirstFlag = 1;
    }
    camera->receivedLength -= length;
    return length;
}

uint32_t ns_start_camera(ns_camera_config_t *cfg) {
    /**
     * @brief Start camera and take out of standby
     *
     */
    lowPowerOff(&camera);
    ns_delay_us(1000);
    setBrightness(&camera, CAM_BRIGHTNESS_LEVEL_1);
    setAutoExposure(&camera, true);
    setAutoFocus(&camera, true);
    setAutoISOSensitive(&camera, true);

    return NS_STATUS_SUCCESS;
}
uint32_t ns_stop_camera(ns_camera_config_t *cfg) {
    /**
     * @brief Stop camera and put into low-power standby mode.
     *
     */
    lowPowerOn(&camera);
    return NS_STATUS_SUCCESS;
}

uint32_t ns_take_picture(ns_camera_config_t *cfg) {
    /**
     * @brief Trigger capture (Arducam will fetch next frame from image sensor)
     *
     */

    // This version blocks...
    return takePicture(&camera, cfg->imageMode, cfg->imagePixFmt);
}

// Arducam needs to be polled to check when a capture is done.
// Instead of a tight loop, we use a timer to poll the camera.
// This function just starts the timer and sets state.
uint32_t ns_press_shutter_button(ns_camera_config_t *cfg) {
    ns_image_pix_fmt_e pixel_format = cfg->imagePixFmt;
    ns_image_mode_e mode = cfg->imageMode;

    if (cfg->pictureTakenCb == NULL) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (camera.currentPixelFormat != pixel_format) {
        camera.currentPixelFormat = pixel_format;
        ns_lp_printf("Setting pixel format to %d\n", pixel_format);
        writeReg(&camera, NS_CAM_REG_FORMAT, pixel_format); // set the data format
        waitI2cIdle(&camera);                               // Wait I2c Idle
    }

    if (camera.currentPictureMode != mode) {
        camera.currentPictureMode = mode;
        ns_lp_printf("Setting capture mode to %d\n", mode);
        writeReg(&camera, NS_CAM_REG_CAPTURE_RESOLUTION, NS_CAM_SET_CAPTURE_MODE | mode);
        waitI2cIdle(&camera); // Wait I2c Idle
    }

    cameraClearFifoFlag(&camera);
    cameraStartCapture(&camera);
    nsCameraPictureBeingTaken = true;
    return NS_STATUS_SUCCESS;
}

int ns_is_camera_capturing() {
    /**
     * @brief Check if Arducam is still fetching frame
     *
     */
    return !cameraImageAvailable(&camera);
}

uint32_t ns_start_dma_read(
    ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t *buffer_offset, uint32_t bufLen) {
    // Wait for capture to complete
    while (ns_is_camera_capturing()) {
        ns_delay_us(10000);
        ns_lp_printf("Waiting for camera capture\n"); // should never be here
    }

    // Get FIFO length
    dma_total_requested_length = cameraReadFifoLength(&camera);
    // ns_lp_printf("FIFO length: %u\n", dma_total_requested_length);
    if (cfg->imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG) {
        *buffer_offset = 1;
    } else {
        *buffer_offset = 0;
    }

    // Kick off DMA read
    ns_read_done = false;
    dma_cambuf = camBuf;
    dma_offset = 0;
    dma_current_chunk_length = (dma_total_requested_length > MAX_SPI_DMA_LEN)
                                   ? MAX_SPI_DMA_LEN
                                   : dma_total_requested_length;
    // ns_lp_printf("Starting DMA read of chunk size %d\n", dma_current_chunk_length);
    uint32_t err = ns_spi_read_dma(
        spiHandle, camBuf, dma_current_chunk_length, ARDU_BURST_FIFO_READ, 1, camera.csPin);

    return dma_total_requested_length;
}

uint32_t ns_transfer_picture(
    ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t *buffer_offset, uint32_t bufLen) {
    /**
     * @brief Transfer captured frame over SPI to local buffer
     * NOTE: This routine is blocking and will wait for inflight capture.
     * @param camBuf Camera buffer to store frame
     * @param bufLen Buffer size
     * @return uint32_t
     */
    uint32_t length;
    uint32_t index;
    // Wait for capture to complete
    while (ns_is_camera_capturing()) {
        ns_delay_us(10000);
    }

    // Get FIFO length
    length = cameraReadFifoLength(&camera);
    // if (length == 0 || length >= bufLen) {
    //     return 0;
    // }

    // ns_lp_printf("FIFO length: %u\n", length);
    readBuff(&camera, camBuf, length);

    // Remove trailing zeros from image
    for (index = length - 1; index >= 0; index--) {
        if (camBuf[index] != 0) {
            break;
        }
    }
    length = index + 1;
    // ns_lp_printf("Clipped FIFO length: %u\n", length);

    if (cfg->imagePixFmt == NS_CAM_IMAGE_PIX_FMT_JPEG) {
        *buffer_offset = 1;
    } else {
        *buffer_offset = 0;
    }

    return length;
}

/**
 * @brief Convenience routine to capture and transfer next frame.
 *
 * @param camBuf Camera buffer to store frame
 * @param bufLen Buffer size
 * @return uint32_t
 */
uint32_t ns_camera_capture(ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t bufLen) {
    // ns_trigger_camera_capture(cfg);
    // return transfer_camera_capture(camBuf, bufLen);
    return 0;
}

/**
 * @brief Decode JPEG-encoded camera buffer into RGB image buffer. Optionally scales the image on
 * the fly to reduce memory req. RGB values are placed in range [-128, 127]
 * @param camBuf JPEG-encoded camera buffer
 * @param camLen Camera buffer length
 * @param imgBuf Image buffer to store results
 * @param imgWidth Requested image width. Clip pixels that fall outside.
 * @param imgHeight Requested image height. Clip pixels that fall outside.
 * @param scaleFactor Scaling factor
 * @return 0 if success otherwise error code
 */
// int camera_decode_image(
//     uint8_t *camBuf, uint32_t camLen, img_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
//     uint32_t scaleFactor) {

//     uint16_t *pImg;
//     uint16_t color;
//     uint8_t r, g, b;

//     jpeg_decoder_init(&jpegCtx, camBuf, camLen);

//     const int keep_x_mcus = scaleFactor * imgWidth / jpegCtx.imgInfo.m_MCUWidth;
//     const int keep_y_mcus = scaleFactor * imgHeight / jpegCtx.imgInfo.m_MCUHeight;

//     const int skip_x_mcus = jpegCtx.imgInfo.m_MCUSPerRow - keep_x_mcus;

//     const int skip_start_x_mcus = skip_x_mcus / 2;

//     const int skip_end_x_mcu_index = skip_start_x_mcus + keep_x_mcus;

//     const int skip_y_mcus = jpegCtx.imgInfo.m_MCUSPerCol - keep_y_mcus;
//     const int skip_start_y_mcus = skip_y_mcus / 2;
//     const int skip_end_y_mcu_index = skip_start_y_mcus + keep_y_mcus;

//     const int scaleImageSize = imgHeight * imgWidth * 3;

//     const img_t pixelOffset = -128;

//     for (int i = 0; i < scaleImageSize; i++) {
//         imgBuf[i] = pixelOffset;
//     }

//     while (jpeg_decoder_read(&jpegCtx)) {
//         // Out of height bounds
//         if (jpegCtx.MCUy < skip_start_y_mcus || jpegCtx.MCUy >= skip_end_y_mcu_index) {
//             continue;
//         }
//         // Out of width bounds
//         if (jpegCtx.MCUx < skip_start_x_mcus || jpegCtx.MCUx >= skip_end_x_mcu_index) {
//             continue;
//         }

//         pImg = jpegCtx.pImage;

//         int relMcuX = jpegCtx.MCUx - skip_start_x_mcus;
//         int relMcuY = jpegCtx.MCUy - skip_start_y_mcus;

//         int xOrigin = relMcuX * jpegCtx.imgInfo.m_MCUWidth;
//         int yOrigin = relMcuY * jpegCtx.imgInfo.m_MCUHeight;

//         for (int mcuRow = 0; mcuRow < jpegCtx.imgInfo.m_MCUHeight; mcuRow++) {
//             int currentY = yOrigin + mcuRow;
//             for (int mcuCol = 0; mcuCol < jpegCtx.imgInfo.m_MCUWidth; mcuCol++) {
//                 int currentX = xOrigin + mcuCol;
//                 color = *pImg++;
//                 if (scaleFactor != 1 &&
//                     (currentY % scaleFactor != 0 || currentX % scaleFactor != 0)) {
//                     continue;
//                 }
//                 r = ((color & 0xF800) >> 11) * 8;
//                 g = ((color & 0x07E0) >> 5) * 4;
//                 b = ((color & 0x001F) >> 0) * 8;

//                 int index = (currentY / scaleFactor) * (imgWidth / 1) + currentX / scaleFactor;
//                 imgBuf[index * 3 + 0] += r;
//                 imgBuf[index * 3 + 1] += g;
//                 imgBuf[index * 3 + 2] += b;
//             }
//         }
//     }
//     return 0;
// }
