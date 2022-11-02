/**
 * @file basic_tf_stub.h
 * @author Carlos Morales
 * @brief 
 * @version 0.1
 * @date 2022-10-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/// Assorted Configs and helpers
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/// High level audio parameters
#define NUM_CHANNELS 1
#define NUM_FRAMES 49 // 20ms frame shift
#define SAMPLES_IN_FRAME 320
#define SAMPLE_RATE 16000