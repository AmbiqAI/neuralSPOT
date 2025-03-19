#include "algorithm.h"
#include <arm_math.h>
#include <math.h>
#include <stdlib.h>

/*-------------------- Circular buffer function declarations -----------------*/
void buffer_init(struct buffer *instance);
void buffer_input(struct buffer *instance, float value);
void buffer_copy(struct buffer *instance, float *output);

/*-------------------- Algorithm private constants ---------------------------*/
#define CALCULATION_INTERVAL 1

static float hann_window[1024];
// static arm_rfft_fast_instance_f32 fft;

/*-------------------- Algorithm function definitions ------------------------*/
void algorithm_init(struct algorithm *instance) {
    instance->time = 0;
    for (int i = 0; i < MAX_INPUT_CHANNELS; i++) {
        buffer_init(&instance->buffer[i]);
    }
    // Initialize Hann window
    for (int i = 0; i < 1024; i++) {
        hann_window[i] = 0.5f * (1 - cosf(2 * PI * i / 1024));
    }
    // arm_rfft_fast_init_1024_f32(&fft);
}

bool algorithm(struct algorithm *instance,
               struct algorithm_input *input,
               float *tempBuffer,
               float *output) {
    for (int channel = 0; channel < input->num_channels; channel++) {
        buffer_input(&instance->buffer[channel], input->channels[channel]);
    }

    instance->time += input->sample_interval;
    if (instance->time < CALCULATION_INTERVAL) {
        return false;
    }
    instance->time -= CALCULATION_INTERVAL;

    float *sBuffer = &tempBuffer[0]; // 1024
    arm_fill_f32(0, sBuffer, 1024);

    for (int channel = 0; channel < input->num_channels; channel++) {
        float *tBuffer = &tempBuffer[1024]; // 1024
        float *fBuffer = &tempBuffer[2 * 1024]; // 1024

        buffer_copy(&instance->buffer[channel], tBuffer);

        arm_rfft_fast_instance_f32 fft;
        arm_rfft_fast_init_1024_f32(&fft);
        arm_mult_f32(tBuffer, hann_window, tBuffer, BUFFER_LEN);
        arm_fill_f32(0, tBuffer + BUFFER_LEN, 1024 - BUFFER_LEN);
        arm_rfft_fast_f32(&fft, tBuffer, fBuffer, 0);
        arm_add_f32(fBuffer, sBuffer, sBuffer, 1024);
    }

    float *mBuffer = &tempBuffer[3 * 1024]; // 512

    arm_cmplx_mag_squared_f32(sBuffer, mBuffer, 512);

    float maxValue = 0;
    uint32_t maxIndex = 0;
    arm_max_f32(mBuffer, 512, &maxValue, &maxIndex);
    output[0] = maxIndex / (1024 * input->sample_interval);

    return true;
}

/*-------------------- Circular buffer function definitions ------------------*/
void buffer_init(struct buffer *instance) {
    instance->position = 0;
    for (int i = 0; i < BUFFER_LEN; i++) {
        instance->values[i] = 0;
    }
}

void buffer_input(struct buffer *instance, float value) {
    instance->values[instance->position] = value;
    instance->position += 1;
    if (instance->position >= BUFFER_LEN) {
        instance->position = 0;
    }
}

void buffer_copy(struct buffer *instance, float *output) {
    arm_copy_f32(&instance->values[instance->position], output, BUFFER_LEN - instance->position);
    arm_copy_f32(&instance->values[0], output + BUFFER_LEN - instance->position, instance->position);
}

/*-------------------- Input generator function definitions ------------------*/
void input_generator_init(struct input_generator *self,
                          float frequency,
                          float amplitude,
                          float phase,
                          float sample_rate) {
    self->frequency = frequency;
    self->amplitude = amplitude;
    self->phase = phase;
    self->sample_rate = sample_rate;
    self->time = 0;
}

float generate_sine(struct input_generator *self) {
    float result = self->amplitude *
        sinf(2 * PI * self->frequency * self->time + self->phase);

    self->time += 1 / self->sample_rate;
    if (self->time > 1 / self->frequency)
        self->time -= 1 / self->frequency;

    return result;
}

float generate_white_noise(float amplitude) {
    return amplitude * (2 * ((float)rand() / RAND_MAX) - 1);
}
