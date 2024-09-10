/**
 * @file model.h
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Performs inference using TFLM
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __MODEL_H
#define __MODEL_H

#include "arm_math.h"

int init_model();
int model_inference(float32_t *y);

#endif // __MODEL_H
