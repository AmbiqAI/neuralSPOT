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

typedef struct {
    const ns_core_api_t *api; ///< API prefix
    int8_t iom;               ///< Apollo4 IOM port
    uint32_t spiSpeed;        // = CAM_SPI_SPEED;
    ns_camera_hw_e cameraHw;
    ns_image_mode_e imageMode;
    ns_image_pix_fmt_e imagePixFmt;
    ns_spi_config_t spiConfig; // = {.iom = CAM_SPI_IOM};
} ns_camera_config_t;

int arducam_spi_read(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin);
int arducam_spi_write(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin);
void arducam_delay_ms(uint16_t delay);
void arducam_delay_us(uint16_t delay);

uint32_t ns_camera_init(ns_camera_config_t *cfg);
uint32_t ns_start_camera(ns_camera_config_t *cfg);
uint32_t ns_stop_camera(ns_camera_config_t *cfg);
uint32_t ns_trigger_camera_capture(ns_camera_config_t *cfg);
int is_camera_capturing();
uint32_t ns_transfer_camera_capture(uint8_t *camBuf, uint32_t bufLen);

uint32_t ns_camera_capture(ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t bufLen);

int camera_decode_image(
    uint8_t *camBuf, uint32_t camLen, int8_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
    uint32_t scaleFactor);
#ifdef __cplusplus
}
#endif
#endif // __CAMERA_H
