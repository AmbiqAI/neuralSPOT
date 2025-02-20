# **NeuralSpot NNSP: Programmer’s Guide**

This guide provides an overview of the **NeuralSpot NNSP** (Neural Network Speech Processing) library’s API for fixed-point neural-network, audio, and speech-processing functions. It focuses on the public headers found under `ns-nnsp/includes-api/`. Each header provides a well-defined set of C/C++ functions, data structures, and constants to facilitate signal processing, neural network inference, speech feature extraction, and more, on resource-constrained devices.

---
## **Table of Contents**
1. [Overview](#overview)
2. [Header-by-Header Reference](#header-by-header-reference)  
   2.1 [activation.h](#activationh)  
   2.2 [affine.h](#affineh)  
   2.3 [affine_acc32b.h](#affine_acc32bh)  
   2.4 [ambiq_nnsp_const.h](#ambiq_nnsp_consth)  
   2.5 [ambiq_nnsp_debug.h](#ambiq_nnsp_debugh)  
   2.6 [ambiq_stdint.h](#ambiq_stdinth)  
   2.7 [basic_mve.h](#basic_mveh)  
   2.8 [complex.h](#complexh)  
   2.9 [debug_files.h](#debug_filesh)  
   2.10 [feature_module.h](#feature_moduleh)  
   2.11 [fft_arm.h](#fft_armh)  
   2.12 [fft.h](#ffth)  
   2.13 [fixlog10.h](#fixlog10h)  
   2.14 [iir.h](#iirh)  
   2.15 [lstm.h](#lstmh)  
   2.16 [melSpecProc.h](#melspecproch)  
   2.17 [minmax.h](#minmaxh)  
   2.18 [neural_nets.h](#neural_netsh)  
   2.19 [nn_speech.h](#nn_speechh)  
   2.20 [nnid_class.h](#nnid_classh)  
   2.21 [nnsp_identification.h](#nnsp_identificationh)  
   2.22 [s2i_const.h](#s2i_consth)  
   2.23 [spectrogram_module.h](#spectrogram_moduleh)  
3. [Example: Putting It All Together](#example-putting-it-all-together)

---

## **Overview**

The **NNSP** library provides:

- **Fixed-point Activation Functions** (ReLU6, Tanh, Sigmoid, Linear).
- **Affine (Fully-Connected) Layers** with different accumulators (32-bit or 64-bit).
- **LSTM** routines optimized for fixed-point arithmetic.
- **STFT/Spectrogram** routines, including windowing functions and FFT-based transforms.
- **Mel Spectrogram** computations.
- **Feature extraction** modules for audio signals (e.g., log-mel spectrogram).
- **IIR Filtering** utilities.
- **Debug & Logging** functionalities.
- **NeuralNet** and **NNSPClass** structures for building speech-related pipelines (e.g., KWS, VAD, S2I, SE).

You can build and link these APIs into your embedded or desktop application to implement fixed-point deep learning tasks, speech feature extraction, or classical signal-processing pipelines.

---

## **Header-by-Header Reference**

### 2.1 <a name="activationh"></a> **`activation.h`**

**Location**: `ns-nnsp/includes-api/activation.h`  
**Purpose**: Provides fixed-point activation functions.

#### **Functions**

1. ```c
   void *relu6_fix(int16_t *y, int32_t *x, int len);
   ```
   - **Description**: Applies the ReLU6 activation elementwise.  
     \[
       \text{ReLU6}(x) = \min(\max(x, 0), 6).
     \]
     Internally, values are in Q-format (see code for specific shifts).
   - **Parameters**:  
     - `y`: Output array (int16_t).  
     - `x`: Input array (int32_t).  
     - `len`: Number of elements.
   - **Returns**: A pointer to the end of the output array (`y + len`).

2. ```c
   void *linear_fix(int32_t *y, int32_t *x, int len);
   ```
   - **Description**: Elementwise “linear” activation. Basically copies `x` into `y`.
   - **Parameters**:  
     - `y`: Output array (int32_t).  
     - `x`: Input array (int32_t).  
     - `len`: Number of elements.
   - **Returns**: A pointer to `(y + len)`.

3. ```c
   void *tanh_fix(int16_t *y, int32_t *x, int len);
   ```
   - **Description**: Fixed-point \(\tanh\) function.
   - **Parameters**:  
     - `y`: Output array (int16_t).  
     - `x`: Input array (int32_t).  
     - `len`: Number of elements.
   - **Returns**: Pointer to `(y + len)`.

4. ```c
   void *sigmoid_fix(int16_t *y, int32_t *x, int len);
   ```
   - **Description**: Fixed-point \(\sigma(x)\) function using \(\tanh(x/2)\). 
   - **Parameters**:  
     - `y`: Output array (int16_t).  
     - `x`: Input array (int32_t).  
     - `len`: Number of elements.
   - **Returns**: Pointer to `(y + len)`.

5. ```c
   typedef enum { relu6, ftanh, sigmoid, linear } ACTIVATION_TYPE;
   ```
   - **Description**: An enum for identifying these activations.

#### **Usage Example**
```c
#include "activation.h"
#include <stdio.h>

int main(void) {
    int len = 5;
    int32_t input[] = {1000, -5000, 32000, 8000, 60000}; // Q15-like
    int16_t output[5];

    // Apply ReLU6
    relu6_fix(output, input, len);
    for(int i = 0; i < len; i++) {
        printf("ReLU6 out[%d] = %d\n", i, output[i]);
    }
    return 0;
}
```

---

### 2.2 <a name="affineh"></a> **`affine.h`**

**Location**: `ns-nnsp/includes-api/affine.h`  
**Purpose**: Implements matrix-vector multiplication with 64-bit accumulators for neural network layers (e.g., fully-connected, RNN-like).

#### **Functions**

1. ```c
   int affine_Krows_8x16(
       int16_t dim_output,
       int16_t **pp_output,
       int8_t **pp_kernel,
       int16_t **pp_bias,
       int16_t *input,
       int16_t dim_input,
       int16_t qbit_kernel,
       int16_t qbit_bias,
       int16_t qbit_input,
       int64_t *pt_accum,
       int8_t is_out,
       void *(*act)(void *, int32_t *, int));
   ```
   - **Description**: Core function to multiply a (dim_output x dim_input) 8-bit weight matrix by a 16-bit input vector, accumulate in 64-bit, optionally add bias, and apply an activation function.  
   - **Parameters**:  
     - `dim_output`: Number of output neurons (rows).  
     - `pp_output`: Pointer to a pointer of the output array.  
     - `pp_kernel`: Pointer to pointer of the weight matrix in 8-bit format.  
     - `pp_bias`: Pointer to pointer of the bias vector in 16-bit format (optional).  
     - `input`: 16-bit input vector.  
     - `dim_input`: Length of `input`.  
     - `qbit_kernel`, `qbit_bias`, `qbit_input`: Q-format shifts for kernel, bias, and input, used for fixed-point scaling.  
     - `pt_accum`: Array used for partial accumulators.  
     - `is_out`: If nonzero, apply the activation function and finalize the output.  
     - `act`: Activation function callback (e.g., `relu6_fix`).  
   - **Returns**: `0` on success.

2. ```c
   int rc_Krows_8x16(...);
   int fc_8x16(...);
   int rc_8x16(...);
   void shift_64b(int64_t *x, int8_t shift, int len);
   ```
   - **`rc_Krows_8x16`**: Similar to `affine_Krows_8x16`, but for recurrent connections (input + recurrent input).  
   - **`fc_8x16`**: Simplified “fully-connected” interface.  
   - **`rc_8x16`**: Recurrent version (no 64-bit accum, it calls `rc_Krows_8x16`).  
   - **`shift_64b`**: Utility to shift a 64-bit array by a fixed amount.

#### **Usage Example**
```c
#include "affine.h"
#include "activation.h"
#include <stdio.h>

// Activation callback
static void *my_relu(void *out, int32_t *accum, int len) {
    int16_t *out16 = (int16_t *)out;
    for(int i = 0; i < len; i++) {
        int32_t val = accum[i];
        if(val < 0) val = 0;
        // saturate to 16-bit
        if(val > 32767) val = 32767;
        out16[i] = (int16_t)val;
    }
    return (void *)(out16 + len);
}

int main(void) {
    int16_t input[4] = {100, 200, 300, 400};
    int8_t kernel_data[16] = {/* 4x4 weights in row-major 8-bit*/};
    int16_t bias[4] = {10, 10, 10, 10};
    int16_t output[4];
    int16_t *p_out = output;
    int8_t *p_kernel = kernel_data;
    int16_t *p_bias = bias;
    int64_t accum[4] = {0};
    // Example usage of a single 4x4 fully-connected
    affine_Krows_8x16(
       4, 
       &p_out, &p_kernel, &p_bias, 
       input, 4, 
       /*qbit_kernel=*/8, /*qbit_bias=*/8, /*qbit_input=*/8,
       accum, /*is_out=*/1, my_relu);

    // output now has the activation result
    for(int i=0; i<4; i++){
        printf("Output[%d] = %d\n", i, output[i]);
    }
    return 0;
}
```

---

### 2.3 <a name="affine_acc32bh"></a> **`affine_acc32b.h`**

**Location**: `ns-nnsp/includes-api/affine_acc32b.h`  
**Purpose**: Similar to **affine.h** but uses **32-bit** accumulators (less memory usage, but risk of overflow).

**Main Functions**:
- `affine_Krows_8x16_acc32b`
- `rc_Krows_8x16_acc32b`
- `fc_8x16_acc32b`
- `rc_8x16_acc32b`
- `shift_32b`

All usage patterns are analogous to those in [`affine.h`](#affineh). The difference is the internal accumulator size (32-bit vs. 64-bit).

---

### 2.4 <a name="ambiq_nnsp_consth"></a> **`ambiq_nnsp_const.h`**

**Purpose**: Defines constants used throughout the library, particularly for audio feature extraction.

```c
#define LEN_FFT_NNSP 512
#define LEN_STFT_WIN_COEFF 480
#define LEN_STFT_HOP 160
#define NUM_MELBANKS 40
#define NUM_FEATURE_CONTEXT 6
#define MAX_SIZE_FEATURE 80
#define DIMEMSION_FEATURE NUM_MELBANKS
#define SAMPLING_RATE 16000
```
These are typical audio settings (e.g., 16 kHz sampling).

**Usage**: Simply `#include "ambiq_nnsp_const.h"` and use the constants directly.

---

### 2.5 <a name="ambiq_nnsp_debugh"></a> **`ambiq_nnsp_debug.h`**

**Purpose**: Debugging macros that control levels of optimization and logging.

```c
#define AMBIQ_NNSP_DEBUG 0
/*
 ARM_OPTIMIZED:
   0: no optimization
   1: fully optimization
   2: verify to optimization
   3: optimization on Helium (MVE for M55)
*/
#define ARM_OPTIMIZED 3
#define ARM_FFT 1
#define DEBUG_NNID 0
```

**Usage**:  
- Edit `AMBIQ_NNSP_DEBUG` or `ARM_OPTIMIZED` if you want to toggle debug prints or use MVE optimization. Typically, you recompile after changing these macros.

---

### 2.6 <a name="ambiq_stdinth"></a> **`ambiq_stdint.h`**

Contains redefinitions and macros for fixed-width integer constraints:
```c
#define MAX_INT32_T ((int32_t)0x7fffffff)
#define MIN_INT32_T ((int32_t)0x80000000)
#define MAX_INT16_T ((int16_t)0x7fff)
#define MIN_INT16_T ((int16_t)0x8000)
#define ONE_N32_Q15 ((int32_t)32768)
```
**Usage**:  
- For saturating arithmetic or easy reference to max/min.  
- `ONE_N32_Q15` is `1.0` in Q15.

---

### 2.7 <a name="basic_mveh"></a> **`basic_mve.h`**

**Purpose**: Contains Helium MVE-optimized vector operations (for ARM Cortex-M55).

**Functions**:
```c
void vec16_vec16_mul_32b(int32_t *y, int16_t *x1, int16_t *x2, int16_t len);
void move_data_16b(int16_t *src, int16_t *dst, int len);
void set_zero_32b(int32_t *dst, int len);
int64_t interproduct_32x16(int32_t *in32, int16_t *in16, int16_t len);
```
- **vec16_vec16_mul_32b**: Elementwise multiply two 16-bit vectors into a 32-bit result array.  
- **move_data_16b**: Vector copy with MVE acceleration.  
- **set_zero_32b**: Sets a 32-bit array to zero with MVE.  
- **interproduct_32x16**: Dot product of a 32-bit vector with a 16-bit vector, returning 64-bit.

**Usage Example**:
```c
#include "basic_mve.h"

int main(void){
    int16_t x1[8] = {1,2,3,4,5,6,7,8};
    int16_t x2[8] = {1,2,3,4,5,6,7,8};
    int32_t out[8];
    vec16_vec16_mul_32b(out, x1, x2, 8);
    // out[i] = x1[i]*x2[i]

    int64_t dot = interproduct_32x16(out, x1, 8);
    // ...
    return 0;
}
```

---

### 2.8 <a name="complexh"></a> **`complex.h`**

**Purpose**: Supports 16/32-bit complex data types and basic operations.

**Data Structures**:
```c
typedef struct {
    int32_t real;
    int32_t imag;
} COMPLEX32;

typedef struct {
    int16_t real;
    int16_t imag;
} COMPLEX16;
```

**Key Functions**:
- `complex32_copy`, `complex32_add`, `complex32_sub`, `complex32_mul`, etc.
- `complex32_interprod`: Dot product of two COMPLEX32 arrays.
- `complex32_complex16_elmtprod`: Elementwise multiply a COMPLEX32 array by a COMPLEX16 array.

**Usage Example**:
```c
#include "complex.h"
#include <stdio.h>

int main(void){
    COMPLEX32 a = {1000, 2000};
    COMPLEX32 b = {500, -1000};
    COMPLEX32 out;

    complex32_mul(&out, &a, &b);
    printf("out = (%d, %d)\n", out.real, out.imag);
    return 0;
}
```

---

### 2.9 <a name="debug_filesh"></a> **`debug_files.h`**

**Purpose**: If `AMBIQ_NNSP_DEBUG == 1`, provides file pointers and functions to open/close debug logs.

```c
extern FILE *file_spec_c;
extern FILE *file_pspec_c;
...
void open_debug_files();
void close_debug_files();
```
**Usage**:
```c
#if AMBIQ_NNSP_DEBUG == 1
#include "debug_files.h"
int main(){
    open_debug_files();
    // Debug logging ...
    close_debug_files();
}
#endif
```

---

### 2.10 <a name="feature_moduleh"></a> **`feature_module.h`**

**Purpose**: Offers a **FeatureClass** for STFT-based feature extraction (e.g., Mel-spectrogram + normalization).

```c
typedef struct {
    stftModule state_stftModule;
    int32_t feature[MAX_SIZE_FEATURE];
    int16_t num_mfltrBank;
    const int16_t *p_melBanks;
    ...
    int16_t normFeatContext[NUM_FEATURE_CONTEXT * MAX_SIZE_FEATURE];
    ...
    const int32_t *pt_norm_mean;
    const int32_t *pt_norm_stdR;
    ...
} FeatureClass;

void FeatureClass_construct(...);
void FeatureClass_setDefault(...);
void FeatureClass_execute(...);
```

**High-Level Flow**:
1. `FeatureClass_construct(...)`: Initialize the STFT module, melbank pointer, normalization arrays, etc.  
2. `FeatureClass_setDefault(...)`: Clears internal buffers.  
3. `FeatureClass_execute(...)`: Takes raw PCM, runs STFT, mel-scaling, log10, normalization, and updates `normFeatContext`.

**Usage Example**:
```c
#include "feature_module.h"
#include "ambiq_nnsp_const.h" // for constants
#include <stdio.h>

int main(void){
    FeatureClass feat;
    // Suppose we have precomputed norm arrays
    static const int32_t norm_mean[NUM_MELBANKS] = { ... };
    static const int32_t norm_std[NUM_MELBANKS]  = { ... };

    PARAMS_NNSP params = {
        .num_mfltrBank = 40,
        .winsize_stft  = 480,
        .hopsize_stft  = 160,
        .fftsize       = 512,
        .pt_stft_win_coeff = stft_win_coeff_w480_h160 // from library
        // ...
    };

    FeatureClass_construct(&feat, norm_mean, norm_std, /*qbit_output=*/8,
                           params.num_mfltrBank, params.winsize_stft, 
                           params.hopsize_stft, params.fftsize,
                           params.pt_stft_win_coeff);

    FeatureClass_setDefault(&feat);

    // For each 160-sample frame:
    int16_t audio_frame[160];
    // Fill audio_frame with PCM data...
    FeatureClass_execute(&feat, audio_frame);

    // The result is in feat.normFeatContext, 
    // or feat.feature for the latest frame
    // ...
    return 0;
}
```

---

### 2.11 <a name="fft_armh"></a> **`fft_arm.h`**

**Purpose**: Wraps CMSIS DSP RFFT functionalities for Q31 usage.

**Functions**:
```c
void arm_fft_init(void *p_fft_st_t, uint32_t is_ifft, int16_t fftsize);
void arm_fft_exec(void *p_fft_st_t, int32_t *y, int32_t *x);
```

- **arm_fft_init**: Initialize an `arm_rfft_instance_q31` struct for forward or inverse FFT.  
- **arm_fft_exec**: Execute the RFFT on `x` -> `y`.

**Usage Example**:
```c
#include "fft_arm.h"
#include <arm_math.h>

int main(void){
    arm_rfft_instance_q31 rfft;
    int16_t fft_size = 512;
    arm_fft_init(&rfft, /*is_ifft=*/0, fft_size);

    // Suppose x[] is your real input in Q30
    int32_t x[512], y[512];
    // fill x[] ...
    arm_fft_exec(&rfft, y, x);
    // y[] now holds the complex frequency bins in a Q31 format
    return 0;
}
```

---

### 2.12 <a name="ffth"></a> **`fft.h`**

**Purpose**: Simple fixed-point FFT/RFFT functions if `ARM_FFT==0`.  
- `fft(int exp_nfft, void *input, void *output)`
- `rfft(int num_rfft, int32_t *input, void *output)`

**Usage**: For builds without ARM CMSIS DSP support, you can use these implementations.

```c
#include "fft.h"

int main(void){
    int32_t input[256*2]; // real/imag interleaved?
    // fill input ...
    COMPLEX32 output[256];
    fft(/*exp_nfft=*/8, (void*)input, (void*)output);
    return 0;
}
```

---

### 2.13 <a name="fixlog10h"></a> **`fixlog10.h`**

**Purpose**: Fixed-point log10 approximations.  
- `void norm_oneTwo(int32_t x, int32_t *y, int8_t *shift);`
- `void my_log10(int32_t *out, int32_t x);`
- `void log10_vec(int32_t *out, int32_t *x, int32_t len, int16_t bit_frac_in);`

**Usage**:
```c
#include "fixlog10.h"
#include <stdio.h>

int main(void) {
    int32_t x = 32768; // Q15 '1.0'
    int32_t out;
    my_log10(&out, x);
    printf("log10(1.0) = %ld (Q15-based)\n", out);
    return 0;
}
```

---

### 2.14 <a name="iirh"></a> **`iir.h`**

**Purpose**: A simple IIR filter class with a single pole/zero.

```c
typedef struct {
    int16_t a; // feedback coefficient
    int16_t b; // feedforward coefficient
    int32_t state;
} IIR_CLASS;

void IIR_CLASS_init(IIR_CLASS *inst);
void IIR_CLASS_reset(IIR_CLASS *inst);
void IIR_CLASS_exec(IIR_CLASS *inst, int16_t *outputs, int16_t *inputs, int len);
```

**Usage Example**:
```c
#include "iir.h"
#include <stdio.h>

int main(){
    IIR_CLASS iir;
    IIR_CLASS_init(&iir);
    iir.a = -31630; // your design
    iir.b = -32768;

    // filter 100 samples
    int16_t in[100], out[100];
    // fill in[] ...
    IIR_CLASS_exec(&iir, out, in, 100);

    return 0;
}
```

---

### 2.15 <a name="lstmh"></a> **`lstm.h`**

**Purpose**: LSTM cell functions with fixed-point arithmetic. 8-bit weights, 16-bit states.

```c
int lstm_8x16(...);
int lstm_8x16_acc32b(...);
```

**Usage**:
1. Provide input vector (`int16_t *input`), hidden-state vector (`int16_t *h_state`), cell-state vector (`int32_t *c_state`), weights/bias.
2. The function updates `h_state` and `c_state` for the next step, outputs the new hidden-state in `p_output`.

**Basic Example** (pseudocode):
```c
#include "lstm.h"
#include "activation.h"

int main(void){
    // Suppose we have an LSTM with dim_input=10, dim_output=8
    int16_t input[10];
    int16_t h_state[8] = {0};
    int32_t c_state[8] = {0};
    int8_t kernel[8*10*4], kernel_rec[8*8*4]; 
    int16_t bias[8*4];
    // ... fill weights

    int16_t output[8];
    lstm_8x16(
      output, 
      kernel, kernel_rec, bias, 
      input, h_state, c_state,
      /*dim_output=*/8, 
      /*dim_input=*/10,
      /*dim_input_rec=*/8,
      /*qbit_kernel=*/8, /*qbit_bias=*/8, /*qbit_input=*/8, /*qbit_input_rec=*/8,
      ftanh, // activation for gates
      (void *(*)(void*, int32_t*, int)) &tanh_fix
    );

    // output and h_state[] are updated
    return 0;
}
```

---

### 2.16 <a name="melspecproch"></a> **`melSpecProc.h`**

**Purpose**: Convert a power spectrum into a Mel-scaled spectrogram.

```c
void melSpecProc(
   int32_t *specs,    // input power spectrum
   int32_t *melSpecs, // output mel-spectral array
   const int16_t *p_melBanks,
   int16_t num_mfltrBank);
```

**Usage**:
```c
#include "melSpecProc.h"

int main(void){
    int32_t specs[257]; // e.g. 512-point FFT => half +1
    int32_t mel[40];
    // fill specs ...
    melSpecProc(specs, mel, mfltrBank_coeff_nfilt40_fftsize512, 40);
    // mel[] now has mel-spectral energies
    return 0;
}
```

---

### 2.17 <a name="minmaxh"></a> **`minmax.h`**

Defines macros:
```c
#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
```
**Usage**: Basic min/max macros used throughout the library.

---

### 2.18 <a name="neural_netsh"></a> **`neural_nets.h`**

**Purpose**: Definition of a higher-level `NeuralNetClass` structure to orchestrate multi-layer (FC, LSTM, etc.) networks.

```c
typedef enum { fc, lstm } NET_LAYER_TYPE;

typedef struct {
    int8_t numlayers;
    int16_t size_layer[11];
    NET_LAYER_TYPE net_layer_type[10];
    int8_t qbit_kernel[10];
    int8_t qbit_input[10];
    int8_t qbit_bias[10];
    ACTIVATION_TYPE activation_type[10];
    int32_t *pt_cstate[10];
    int16_t *pt_hstate[10];
    void *(*act_func[10])(void *, int32_t *, int);
    int *(*layer_func[10])();
    int8_t *pt_kernel[10];
    int16_t *pt_bias[10];
    int8_t *pt_kernel_rec[10];
} NeuralNetClass;
```
**Functions**:
- `NeuralNetClass_init(...)`
- `NeuralNetClass_setDefault(...)`
- `NeuralNetClass_exe(...)`

**Usage**:
1. Populate `size_layer[i]`, layer types, pointers to weights, etc.
2. Call `NeuralNetClass_exe(...)` to run the forward pass on the input vector.

---

### 2.19 <a name="nn_speechh"></a> **`nn_speech.h`**

**Purpose**: High-level “speech pipeline” structure, `NNSPClass`, combining feature extraction + neural net + post-processing (KWS, VAD, S2I, etc.).

```c
typedef struct {
    char nn_id;
    void *pt_net;
    void *pt_feat;
    int8_t slides;
    int16_t trigger;
    int16_t *pt_thresh_prob;
    int16_t counts_category[8];
    int16_t *pt_th_count_trigger;
    int16_t outputs[3];
    ...
    PARAMS_NNSP *pt_params;
} NNSPClass;

int NNSPClass_init(...);
int NNSPClass_reset(...);
int NNSPClass_exec(NNSPClass *pt_inst, int16_t *rawPCM);
...
```

**Usage**:
1. Construct + initialize your feature class + network + thresholds.  
2. Call `NNSPClass_exec(...)` each new audio frame.  
3. Check `pt_inst->trigger` or `pt_inst->outputs[]` for classification results.

**Example**:
```c
#include "nn_speech.h"
#include <stdio.h>

int main(void){
    NNSPClass speech;
    // Suppose we have an existing neural net instance (NeuralNetClass),
    // and a FeatureClass instance, plus thresholds.
    // You set them up, then do:
    NNSPClass_init(&speech, net, feat, /*nn_id=*/kws_galaxy_id, ...);

    // Provide raw PCM each iteration:
    int16_t audio_buf[160];
    // fill audio_buf ...
    NNSPClass_exec(&speech, audio_buf);

    if(speech.trigger){
       printf("Detected event!\n");
    }
    return 0;
}
```

---

### 2.20 <a name="nnid_classh"></a> **`nnid_class.h`**

**Purpose**: Provides a structure and functions for “NN-based ID” (speaker ID / embeddings).

```c
typedef struct {
    int32_t *pt_embd;
    int16_t dim_embd;
    int16_t is_get_corr;
    int16_t thresh_get_corr;
    int16_t total_enroll_ppls;
    float thresh_trigger;
    float corr[5];
} NNID_CLASS;

void nnidClass_get_cos(
   int32_t *pt_nn_est,
   int32_t *pt_embds,
   int16_t len_embd,
   int16_t ppls,
   float *corr);
void nnidClass_reset_states(NNID_CLASS *pt_nnid);
```

**Usage**:  
- Typically part of a pipeline for speaker identification.  
- `nnidClass_get_cos(...)` computes the cosine similarity of `pt_nn_est` embedding with a set of reference embeddings.

---

### 2.21 <a name="nnsp_identificationh"></a> **`nnsp_identification.h`**

Defines an enum for known tasks:
```c
typedef enum {
   s2i_id = 0,
   vad_id,
   kws_galaxy_id,
   se_id,
   nnid_id,
   num_NNSP_IDS
} NNSP_ID;
```

**Usage**: For identifying the type of speech neural net or pipeline (S2I, VAD, KWS, etc.).

---

### 2.22 <a name="s2i_consth"></a> **`s2i_const.h`**

```c
#define DIM_INTENTS 7
#define DIM_SLOTS 17
```
Used in an S2I (speech-to-intent) model.

---

### 2.23 <a name="spectrogram_moduleh"></a> **`spectrogram_module.h`**

**Purpose**: STFT-based spectrogram generation using overlap-and-add.

```c
typedef struct {
    int16_t len_win;
    int16_t hop;
    int16_t len_fft;
    int16_t *dataBuffer;
    int32_t *odataBuffer;
    const int16_t *window;
    // ...
    int32_t *spec;
} stftModule;

int stftModule_construct(...);
int stftModule_setDefault(...);
int stftModule_analyze(...);
int stftModule_analyze_arm(...);
int stftModule_synthesize_arm(...);
```

**Example** (for ARM FFT):
```c
#include "spectrogram_module.h"
#include "fft_arm.h"
#include <stdio.h>

int main(void){
    stftModule stft;
    // Construct for a 480-sample window, 160 hop, 512-FFT
    stftModule_construct(&stft, 480, 160, 512, stft_win_coeff_w480_h160);

    stftModule_setDefault(&stft);

    // Each 160-sample input:
    int16_t audio_frame[160];
    int32_t spec[1024];
    stftModule_analyze_arm((void*)&stft, audio_frame, spec, stft.len_fft, NULL);
    
    // spec now has the frequency bins
    return 0;
}
```

---

## **3. Example: Putting It All Together**

Below is a conceptual example that ties together **feature extraction**, **neural net** inference, and the **NNSPClass** pipeline for a keyword spotting scenario:

```c
#include "nn_speech.h"
#include "feature_module.h"
#include "neural_nets.h"
#include "activation.h"
#include "affine.h"
#include "lstm.h"
#include <stdio.h>

// 1) Allocate and fill your NeuralNetClass with layer info
static NeuralNetClass g_net;
static int32_t cstate_layer0[64]; 
static int16_t hstate_layer0[64];
// Similarly for other layers if needed
// ... Fill weights, biases in global arrays

// 2) Feature extraction
static FeatureClass g_feat;
// Provide norm arrays, melbanks, etc

// 3) NNSP container
static NNSPClass g_ns;

int main(void){
    // Setup stft parameters
    PARAMS_NNSP params = {
       .samplingRate    = 16000,
       .fftsize         = 512,
       .winsize_stft    = 480,
       .hopsize_stft    = 160,
       .num_mfltrBank   = 40,
       .pt_stft_win_coeff = stft_win_coeff_w480_h160,
       .is_dcrm         = 0,
       .pre_gain_q1     = 2
       // ...
    };

    // (A) Initialize FeatureClass
    int32_t norm_mean[40] = { /* ... */ };
    int32_t norm_stdR[40] = { /* ... */ };
    FeatureClass_construct(&g_feat, norm_mean, norm_stdR, /*qbit_output=*/8,
                           params.num_mfltrBank, params.winsize_stft, 
                           params.hopsize_stft, params.fftsize, 
                           params.pt_stft_win_coeff);
    FeatureClass_setDefault(&g_feat);

    // (B) Initialize NeuralNetClass
    g_net.numlayers = 2; 
    g_net.size_layer[0] = 240; // e.g. 40 * 6 context frames
    g_net.size_layer[1] = 64; 
    g_net.size_layer[2] = 2;  // Binary classifier
    g_net.net_layer_type[0] = fc;  // layer 0
    g_net.net_layer_type[1] = fc;  // layer 1
    // fill qbit_kernel[], qbit_bias[], act_func[], etc
    g_net.pt_cstate[0] = cstate_layer0; // or NULL for FC
    g_net.pt_hstate[0] = hstate_layer0; // or NULL for FC
    // fill kernel arrays g_net.pt_kernel[0], biases, etc
    NeuralNetClass_setDefault(&g_net);

    // (C) NNSPClass: combine feature + net
    int16_t thresh_prob = 16384; // 0.5 in Q15
    int16_t th_count_trigger = 10;
    NNSPClass_init(&g_ns, &g_net, &g_feat, kws_galaxy_id,
                   norm_mean, norm_stdR, 
                   &thresh_prob, &th_count_trigger, &params);

    // (D) Repeatedly process audio
    int16_t audio_frame[160];
    while( getMicrophoneFrame(audio_frame, 160) ){
        // Run pipeline
        int16_t trigger = NNSPClass_exec(&g_ns, audio_frame);
        if(trigger){
           printf("KWS triggered!\n");
        }
    }

    return 0;
}
```

This completes a high-level example. In practice, you must carefully set your weight arrays, Q-format shifts, and handle partial/rolling classification logic.

---

**Enjoy developing with NeuralSpot NNSP!**  
This guide covered each API from `includes-api/` with usage hints and minimal code snippets. For more detailed examples, consult the library’s tests/demos and the reference source files in `ns-nnsp/src/`.