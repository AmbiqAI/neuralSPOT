

# File ns\_camera.h

[**File List**](files.md) **>** [**includes-api**](dir_01be6a440053b8b57cd3acfc114f4ae9.md) **>** [**ns\_camera.h**](ns__camera_8h.md)

[Go to the documentation of this file](ns__camera_8h.md)


```C++

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
    NS_CAM_IMAGE_MODE_QQVGA = 0x00,   
    NS_CAM_IMAGE_MODE_QVGA = 0x01,    
    NS_CAM_IMAGE_MODE_VGA = 0x02,     
    NS_CAM_IMAGE_MODE_SVGA = 0x03,    
    NS_CAM_IMAGE_MODE_HD = 0x04,      
    NS_CAM_IMAGE_MODE_SXGAM = 0x05,   
    NS_CAM_IMAGE_MODE_UXGA = 0x06,    
    NS_CAM_IMAGE_MODE_FHD = 0x07,     
    NS_CAM_IMAGE_MODE_QXGA = 0x08,    
    NS_CAM_IMAGE_MODE_WQXGA2 = 0x09,  
    NS_CAM_IMAGE_MODE_96X96 = 0x0a,   
    NS_CAM_IMAGE_MODE_128X128 = 0x0b, 
    NS_CAM_IMAGE_MODE_320X320 = 0x0c, 
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
    const ns_core_api_t *api; 
    int8_t iom;               
    uint32_t spiSpeed;        
    ns_camera_hw_e cameraHw;
    ns_image_mode_e imageMode;
    ns_image_pix_fmt_e imagePixFmt;
    ns_spi_config_t spiConfig;
    ns_camera_dma_cb
        dmaCompleteCb; 
    ns_camera_picture_cb
        pictureTakenCb; 

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

uint32_t ns_camera_init(ns_camera_config_t *cfg);

uint32_t ns_start_camera(ns_camera_config_t *cfg);

uint32_t ns_stop_camera(ns_camera_config_t *cfg);

uint32_t ns_take_picture(ns_camera_config_t *cfg);

uint32_t ns_press_shutter_button(ns_camera_config_t *cfg);

int ns_is_camera_capturing();

uint32_t ns_transfer_picture(
    ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t *buffer_offset, uint32_t bufLen);

uint32_t ns_start_dma_read(
    ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t *buffer_offset, uint32_t bufLen);

// uint32_t ns_camera_capture(ns_camera_config_t *cfg, uint8_t *camBuf, uint32_t bufLen);

int camera_decode_image(
    uint8_t *camBuf, uint32_t camLen, uint8_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
    uint32_t scaleFactor);

void ns_camera_adjust_settings(int8_t contrast, int8_t brightness, int8_t ev);

uint32_t ns_chop_off_trailing_zeros(uint8_t *buff, uint32_t length);

#ifdef __cplusplus
}
#endif
#endif // __CAMERA_H
```


