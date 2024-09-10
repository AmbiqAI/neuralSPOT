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

#include "arm_math.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
int arducam_spi_read(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin);
int arducam_spi_write(
    const void *buf, uint32_t bufLen, uint64_t reg, uint32_t regLen, uint32_t csPin);
void arducam_delay_ms(uint16_t delay);
void arducam_delay_us(uint16_t delay);

int init_camera(void);
int start_camera(void);
int stop_camera(void);
int trigger_camera_capture();
int is_camera_capturing();
uint32_t camera_capture(uint8_t *camBuf, uint32_t bufLen);
int camera_decode_image(
    uint8_t *camBuf, uint32_t camLen, int8_t *imgBuf, uint32_t imgWidth, uint32_t imgHeight,
    uint32_t scaleFactor);
#ifdef __cplusplus
}
#endif
#endif // __CAMERA_H
