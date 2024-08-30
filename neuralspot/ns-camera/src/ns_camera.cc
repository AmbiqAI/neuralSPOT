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
#include "arducam/arducam.h"
#include "arm_math.h"
#include "ns_camera_constants.h"
#include "jpeg-decoder/jpeg_decoder.h"
#include "jpeg-decoder/picojpeg.h"
#include "ns_spi.h"
#include "ns_ambiqsuite_harness.h"
// #include "ns_i2c.h"

#define MAX_SPI_BUF_LEN 64

ns_spi_config_t spiConfig = {.iom = CAM_SPI_IOM};
jpeg_decoder_context_t jpegCtx = {};

ArducamCamera camera;

static int
arducam_spi_read(const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin) {
    uint32_t err;
    uint8_t *bufPtr = (uint8_t *)buf;
    uint32_t bytesLeft = bufLen;
    uint32_t chunkSize;
    while (bytesLeft) {
        chunkSize = bytesLeft > MAX_SPI_BUF_LEN ? MAX_SPI_BUF_LEN : bytesLeft;
        err = ns_spi_read(&spiConfig, bufPtr, chunkSize, reg, regLen, csPin);
        bufPtr += chunkSize;
        bytesLeft -= chunkSize;
    }
    return err;
}

static int
arducam_spi_write(const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin) {
    uint8_t *bufPtr = (uint8_t *)buf;
    uint32_t bytesLeft = bufLen;
    uint32_t chunkSize;
    while (bytesLeft) {
        chunkSize = bytesLeft > MAX_SPI_BUF_LEN ? MAX_SPI_BUF_LEN : bytesLeft;
        ns_spi_write(&spiConfig, bufPtr, chunkSize, reg, regLen, csPin);
        bufPtr += chunkSize;
        bytesLeft -= chunkSize;
    }
    return 0;
}

static void arducam_delay_ms(uint16_t delay) { ns_delay_us(1000 * delay); }

static void arducam_delay_us(uint16_t delay) { ns_delay_us(delay); }

ArducamTransport camTransport = {
    .spiRead = arducam_spi_read,
    .spiWrite = arducam_spi_write,
    .delayMs = arducam_delay_ms,
    .delayUs = arducam_delay_us};

void dump_camera_regs() {
    /**
     * @brief Capture Arducam SPI register values
     *
     */
    uint8_t regs[] = {0,    1,    2,    3,    4,    5,    6,    0x3B, 0x3E,
                      0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47};
    for (size_t i = 0; i < 18; i++) {
        ns_printf("[%x]=%x\n", regs[i], cameraReadReg(&camera, regs[i]));
    }
}

int init_camera(void) {
    /**
     * @brief Initialize and configure camera
     *
     */
    if (ns_spi_interface_init(&spiConfig, CAM_SPI_SPEED, AM_HAL_IOM_SPI_MODE_0)) {
        return 1;
    }
    cameraInit(&camera, AM_BSP_IOM1_CS_CHNL, &camTransport);
    cameraBegin(&camera);
    cameraLowPowerOn(&camera);
    return 0;
}

int start_camera() {
    /**
     * @brief Start camera and take out of standby
     *
     */
    cameraLowPowerOff(&camera);
    ns_delay_us(1000);
    cameraSetBrightness(&camera, CAM_BRIGHTNESS_LEVEL_3);
    cameraSetAutoExposure(&camera, true);
    cameraSetAutoFocus(&camera, true);
    cameraSetAutoISOSensitive(&camera, true);

    return 0;
}

int stop_camera() {
    /**
     * @brief Stop camera and put into low-power standby mode.
     *
     */
    cameraLowPowerOn(&camera);
    return 0;
}

int trigger_camera_capture() {
    /**
     * @brief Trigger capture (Arducam will fetch next frame from image sensor)
     *
     */
    // This version blocks...
    cameraTakePicture(&camera, CAM_IMAGE_MODE_QVGA, CAM_IMAGE_PIX_FMT_JPG);
    return 0;
}

int is_camera_capturing() {
    /**
     * @brief Check if Arducam is still fetching frame
     *
     */
    return !cameraImageAvailable(&camera);
}

uint32_t transfer_camera_capture(uint8_t *camBuf, uint32_t bufLen) {
    /**
     * @brief Transfer captured frame over SPI.
     * NOTE: This routine is blocking and will wait for inflight capture.
     * @param camBuf Camera buffer to store frame
     * @param bufLen Buffer size
     * @return uint32_t
     */
    uint32_t length;
    uint32_t index;
    // Wait for capture to complete
    while (is_camera_capturing()) {
        ns_delay_us(10000);
    }

    // Get FIFO length
    length = cameraReadFifoLength(&camera);
    // if (length == 0 || length >= bufLen) {
    //     return 0;
    // }

    ns_printf("FIFO length: %u\n", length);
    cameraReadBuff(&camera, camBuf, length);

    // Remove trailing zeros from image
    for (index = length - 1; index >= 0; index--) {
        if (camBuf[index] != 0) {
            break;
        }
    }
    length = index + 1;
    return length;
}

/**
 * @brief Convenience routine to capture and transfer next frame.
 *
 * @param camBuf Camera buffer to store frame
 * @param bufLen Buffer size
 * @return uint32_t
 */
uint32_t camera_capture(uint8_t *camBuf, uint32_t bufLen) {
    trigger_camera_capture();
    return transfer_camera_capture(camBuf, bufLen);
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
int camera_decode_image(
    uint8_t *camBuf, uint32_t camLen, img_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
    uint32_t scaleFactor) {

    uint16_t *pImg;
    uint16_t color;
    uint8_t r, g, b;

    jpeg_decoder_init(&jpegCtx, camBuf, camLen);

    const int keep_x_mcus = scaleFactor * imgWidth / jpegCtx.imgInfo.m_MCUWidth;
    const int keep_y_mcus = scaleFactor * imgHeight / jpegCtx.imgInfo.m_MCUHeight;

    const int skip_x_mcus = jpegCtx.imgInfo.m_MCUSPerRow - keep_x_mcus;

    const int skip_start_x_mcus = skip_x_mcus / 2;

    const int skip_end_x_mcu_index = skip_start_x_mcus + keep_x_mcus;

    const int skip_y_mcus = jpegCtx.imgInfo.m_MCUSPerCol - keep_y_mcus;
    const int skip_start_y_mcus = skip_y_mcus / 2;
    const int skip_end_y_mcu_index = skip_start_y_mcus + keep_y_mcus;

    const int scaleImageSize = imgHeight * imgWidth * 3;

    const img_t pixelOffset = -128;

    for (int i = 0; i < scaleImageSize; i++) {
        imgBuf[i] = pixelOffset;
    }

    while (jpeg_decoder_read(&jpegCtx)) {
        // Out of height bounds
        if (jpegCtx.MCUy < skip_start_y_mcus || jpegCtx.MCUy >= skip_end_y_mcu_index) {
            continue;
        }
        // Out of width bounds
        if (jpegCtx.MCUx < skip_start_x_mcus || jpegCtx.MCUx >= skip_end_x_mcu_index) {
            continue;
        }

        pImg = jpegCtx.pImage;

        int relMcuX = jpegCtx.MCUx - skip_start_x_mcus;
        int relMcuY = jpegCtx.MCUy - skip_start_y_mcus;

        int xOrigin = relMcuX * jpegCtx.imgInfo.m_MCUWidth;
        int yOrigin = relMcuY * jpegCtx.imgInfo.m_MCUHeight;

        for (int mcuRow = 0; mcuRow < jpegCtx.imgInfo.m_MCUHeight; mcuRow++) {
            int currentY = yOrigin + mcuRow;
            for (int mcuCol = 0; mcuCol < jpegCtx.imgInfo.m_MCUWidth; mcuCol++) {
                int currentX = xOrigin + mcuCol;
                color = *pImg++;
                if (scaleFactor != 1 &&
                    (currentY % scaleFactor != 0 || currentX % scaleFactor != 0)) {
                    continue;
                }
                r = ((color & 0xF800) >> 11) * 8;
                g = ((color & 0x07E0) >> 5) * 4;
                b = ((color & 0x001F) >> 0) * 8;

                int index = (currentY / scaleFactor) * (imgWidth / 1) + currentX / scaleFactor;
                imgBuf[index * 3 + 0] += r;
                imgBuf[index * 3 + 1] += g;
                imgBuf[index * 3 + 2] += b;
            }
        }
    }
    return 0;
}
