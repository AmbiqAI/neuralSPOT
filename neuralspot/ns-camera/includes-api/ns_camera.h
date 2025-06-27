/**
 * @file camera.h
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Camera controller
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __CAMERA_H
#define __CAMERA_H

#include "ns_core.h"
#include "ns_spi.h"
#include "arm_math.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define NS_CAMERA_V1_0_0                                                                           \
    { .major = 1, .minor = 0, .revision = 0 }

#define NS_CAMERA_OLDEST_SUPPORTED_VERSION NS_CAMERA_V1_0_0
#define NS_CAMERA_CURRENT_VERSION NS_CAMERA_V1_0_0
#define NS_CAMERA_API_ID 0xCA000A

extern const ns_core_api_t ns_camera_V1_0_0;
extern const ns_core_api_t ns_camera_oldest_supported_version;
extern const ns_core_api_t ns_camera_current_version;

typedef int8_t img_t;

typedef enum {
    NS_ARDUCAM = 0,
} ns_camera_hw_e;

typedef enum {
    NS_CAM_IMAGE_MODE_QQVGA = 0x00,   /**<160x120 */
    NS_CAM_IMAGE_MODE_QVGA = 0x01,    /**<320x240*/
    NS_CAM_IMAGE_MODE_VGA = 0x02,     /**<640x480*/
    NS_CAM_IMAGE_MODE_SVGA = 0x03,    /**<800x600*/
    NS_CAM_IMAGE_MODE_HD = 0x04,      /**<1280x720*/
    NS_CAM_IMAGE_MODE_SXGAM = 0x05,   /**<1280x960*/
    NS_CAM_IMAGE_MODE_UXGA = 0x06,    /**<1600x1200*/
    NS_CAM_IMAGE_MODE_FHD = 0x07,     /**<1920x1080*/
    NS_CAM_IMAGE_MODE_QXGA = 0x08,    /**<2048x1536*/
    NS_CAM_IMAGE_MODE_WQXGA2 = 0x09,  /**<2592x1944*/
    NS_CAM_IMAGE_MODE_96X96 = 0x0a,   /**<96x96*/
    NS_CAM_IMAGE_MODE_128X128 = 0x0b, /**<128x128*/
    NS_CAM_IMAGE_MODE_320X320 = 0x0c, /**<320x320*/
} ns_image_mode_e;

typedef enum {
    NS_CAM_IMAGE_PIX_FMT_RGB565 = 0x02,
    NS_CAM_IMAGE_PIX_FMT_JPEG = 0x01,
} ns_image_pix_fmt_e;

// Callback def
struct ns_camera_cfg;
typedef void (*ns_camera_dma_cb)(struct ns_camera_cfg *cfg);
typedef void (*ns_camera_picture_cb)(struct ns_camera_cfg *cfg);

typedef struct ns_camera_cfg {
    const ns_core_api_t *api; ///< API prefix
    int8_t iom;               ///< Apollo4 IOM port
    uint32_t spiSpeed;        ///< = CAM_SPI_SPEED;
    ns_camera_hw_e cameraHw;
    ns_image_mode_e imageMode;
    ns_image_pix_fmt_e imagePixFmt;
    ns_spi_config_t spiConfig;
    ns_camera_dma_cb
        dmaCompleteCb; ///< If using DMA, this will be called when DMA chunk is complete
    ns_camera_picture_cb
        pictureTakenCb; ///< If using polling timer, this will be called when pic is ready to xfer

    // Internal state
    uint32_t dmaBufferOffset;
    uint32_t dmaBufferLength;

} ns_camera_config_t;

// Should only be used by Arducam driver
int arducam_spi_read(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin);
int arducam_spi_write(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin);
void arducam_delay_ms(uint16_t delay);
void arducam_delay_us(uint16_t delay);

/**
 * @brief Initialize the camera
 *
 * @param cfg
 * @return uint32_t
 */
uint32_t ns_camera_init(ns_camera_config_t *cfg);

/**
 * @brief Start the camera (take out of low power mode)
 *
 * @param cfg
 * @return uint32_t
 */
uint32_t ns_start_camera(ns_camera_config_t *cfg);

/**
 * @brief Stop the camera (put into low power mode)
 *
 * @param cfg
 * @return uint32_t
 */
uint32_t ns_stop_camera(ns_camera_config_t *cfg);

/**
 * @brief Take a picture in mode specified by cfg
 * Once picture is taken, it can be transferred using ns_transfer_picture
 * @param cfg
 * @return uint32_t
 */
uint32_t ns_take_picture(ns_camera_config_t *cfg);

/**
 * @brief Press the shutter button
 * Start the capture process, turns on a timer to poll the camera for completion
 * @param cfg
 * @return uint32_t
 */
uint32_t ns_press_shutter_button(ns_camera_config_t *cfg);

/**
 * @brief Check if camera is still capturing
 * This is a helper function, it should typically only be used by ns_camera
 * @return int
 */
int ns_is_camera_capturing();

/**
 * @brief Transfer captured frame over SPI to local buffer
 * NOTE: This routine is blocking and will wait for inflight capture.
 * @param cfg
 * @param camBuf Camera buffer to store frame
 * @param buffer_offset JPG images are stored 1 byte offset from start of buffer, so use this to
 * find it
 * @param bufLen Buffer size
 * @return uint32_t
 */
uint32_t ns_transfer_picture(
    ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t *buffer_offset, uint32_t bufLen);

/**
 * @brief Start a DMA read of the camera buffer
 *
 * @param cfg
 * @param camBuf Buffer to store image
 * @param buffer_offset Returned value of buffer offset
 * @param bufLen Length of buffer
 * @return uint32_t Total size of image in bytes
 */
uint32_t ns_start_dma_read(
    ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t *buffer_offset, uint32_t bufLen);

// uint32_t ns_camera_capture(ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t bufLen);

/**
 * @brief Converts a JPG to an RGB565 image
 *
 * @param camBuf Buffer containing JPG image
 * @param camLen Length of JPG image in bytes
 * @param imgBuf Buffer to store RGB565 image
 * @param imgWidth Width of RGB565 image
 * @param imgHeight Height of RGB565 image
 * @param scaleFactor Scale factor to reduce image size
 * @return int
 */
int camera_decode_image(
    uint8_t *camBuf, uint32_t camLen, uint8_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
    uint32_t scaleFactor);

/**
 * @brief Adjust camera settings
 *
 * @param contrast
 * @param brightness
 * @param ev
 */

void ns_camera_adjust_settings(int8_t contrast, int8_t brightness, int8_t ev);

/**
 * @brief Chop off trailing zeros from a buffer
 *
 * @param buff JPG buffer
 * @param length Adjusted length after trailing zeros are removed
 * @return uint32_t
 */
uint32_t ns_chop_off_trailing_zeros(uint8_t *buff, uint32_t length);

#ifdef __cplusplus
}
#endif
#endif // __CAMERA_H
