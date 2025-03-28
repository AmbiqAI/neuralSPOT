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

// Hann window array for BUFFER_LEN=512
static float hannWindow_512[512];

static void init_hann_window(void) {
    for (int i = 0; i < 512; i++) {
        // Hann window: w(n) = 0.5 * (1 - cos(2*pi*n/(N-1))) 
        // or 0.5*(1 - cos(2*pi*i/511)) if strictly Hann for 512 points
        // If you prefer the same as your code, you had:
        // (1 - cos(2 * PI * i / BUFFER_LEN)) * 0.5f
        hannWindow_512[i] = 0.5f * (1.0f - cosf((2.0f * PI * i) / 512.0f));
    }
}

void algorithm_init(struct algorithm *instance) {
    instance->time = 0;
    for (int i = 0; i < MAX_INPUT_CHANNELS; i++) {
        buffer_init(&instance->buffer[i]);
    }
    // Initialize Hann window
    init_hann_window();
    // arm_rfft_fast_init_1024_f32(&fft);
}

static inline void buffer_copy_and_window_mve(const struct buffer *inBuf,
                                              float *outBuf,
                                              uint32_t bufLen)
{
    // We assume bufLen == 512
    // We'll do a 4-float wide loop using MVE intrinsics

    int pos = inBuf->position;
    // Read from [pos..end-1], then [0..pos-1] in the ring buffer.
    // Apply window in a single pass.

    // First chunk: from pos..(bufLen-1)
    int firstChunkSize = bufLen - pos;
    // Second chunk: from 0..(pos-1)

    // We handle them in 4-float steps
    // 1) First chunk
    int i = 0;
    while (i < firstChunkSize) {
        // Load up to 4 samples from ring buffer
        mve_pred16_t p = vctp32q(firstChunkSize - i);
        float32x4_t inputVec = vldrwq_z_f32(&inBuf->values[pos + i], p);
        float32x4_t wVec     = vldrwq_z_f32(&hannWindow_512[i], p);
        // Multiply
        float32x4_t outVec   = vmulq_f32(inputVec, wVec);
        // Store to outBuf
        vstrwq_p_f32(&outBuf[i], outVec, p);
        i += 4;
    }

    // 2) Second chunk
    int secondChunkSize = pos;
    int offset = firstChunkSize;   // where we left off in outBuf
    i = 0;
    while (i < secondChunkSize) {
        mve_pred16_t p = vctp32q(secondChunkSize - i);
        float32x4_t inputVec = vldrwq_z_f32(&inBuf->values[i], p);
        float32x4_t wVec     = vldrwq_z_f32(&hannWindow_512[offset + i], p);
        float32x4_t outVec   = vmulq_f32(inputVec, wVec);
        vstrwq_p_f32(&outBuf[offset + i], outVec, p);
        i += 4;
    }
}

bool __attribute__((section(".itcm_text"))) algorithm(struct algorithm *instance,
// bool algorithm(struct algorithm *instance,
        struct algorithm_input *input,
    float *tempBuffer,
    float *output)
{
    // 1) Acquire new samples in ring buffer
    for (int ch = 0; ch < input->num_channels; ch++) {
        buffer_input(&instance->buffer[ch], input->channels[ch]);
    }

    // 2) Check if we should do processing
    instance->time += input->sample_interval;
    if (instance->time < CALCULATION_INTERVAL) {
        return false;
    }
    instance->time -= CALCULATION_INTERVAL;

    // 3) Clear sBuffer to accumulate multiple channel FFT results
    float *sBuffer = tempBuffer;  // size=1024
    arm_fill_f32(0.0f, sBuffer, 1024);

    // 4) For each channel: copy+window -> FFT -> accumulate
    for (int ch = 0; ch < input->num_channels; ch++) {
        float *tBuffer = &tempBuffer[1024];      // size=1024
        float *fBuffer = &tempBuffer[2 * 1024];  // size=1024

        // Copy ring buffer -> tBuffer & multiply Hann window
        buffer_copy_and_window_mve(&instance->buffer[ch], tBuffer, BUFFER_LEN);

        // Zero pad from BUFFER_LEN..1023
        arm_fill_f32(0.0f, &tBuffer[BUFFER_LEN], 1024 - BUFFER_LEN);

        // Perform FFT
        static arm_rfft_fast_instance_f32 fftInstance; 
        // You might initialize once globally instead of every call
        arm_rfft_fast_init_1024_f32(&fftInstance);
        arm_rfft_fast_f32(&fftInstance, tBuffer, fBuffer, 0);

        // Accumulate into sBuffer
        arm_add_f32(sBuffer, fBuffer, sBuffer, 1024);
    }

    // 5) Take magnitude (512 complex bins)
    float *mBuffer = &tempBuffer[3 * 1024]; // size=512
    arm_cmplx_mag_f32(sBuffer, mBuffer, 512);

    // 6) Find max
    float32_t maxValue = 0.0f;
    uint32_t maxIndex  = 0;
    arm_max_f32(mBuffer, 512, &maxValue, &maxIndex);

    // 7) Output fundamental frequency estimate
    output[0] = (float)maxIndex / (1024.0f * input->sample_interval);

    return true;
}

/*-------------------- Circular buffer function definitions ------------------*/
inline void buffer_init(struct buffer *instance) {
    instance->position = 0;
    for (int i = 0; i < BUFFER_LEN; i++) {
        instance->values[i] = 0;
    }
}

inline void buffer_input(struct buffer *instance, float value) {
    instance->values[instance->position] = value;
    instance->position += 1;
    if (instance->position >= BUFFER_LEN) {
        instance->position = 0;
    }
}

inline void buffer_copy(struct buffer *instance, float *output) {
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

inline float generate_sine(struct input_generator *self) {
    float result = self->amplitude *
        sinf(2 * PI * self->frequency * self->time + self->phase);

    self->time += 1 / self->sample_rate;
    if (self->time > 1 / self->frequency)
        self->time -= 1 / self->frequency;

    return result;
}

inline float generate_white_noise(float amplitude) {
    return amplitude * (2 * ((float)rand() / RAND_MAX) - 1);
}


/* 
O1 Prompt:


*/