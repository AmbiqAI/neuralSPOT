#ifndef __AMBIQ_NNSP_DEBUG__
#define __AMBIQ_NNSP_DEBUG__
#ifdef __cplusplus
extern "C" {
#endif
#define AMBIQ_NNSP_DEBUG 0

/* 
    ARM_OPTIMIZED:
        0: no optimizaiton
        1: fully optimization.
        2: verify to optimization
        3: optimization on mve_m55 
*/

#if defined(AM_PART_APOLLO5B) || defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO510B)
#define ARM_OPTIMIZED 3
#else
#define ARM_OPTIMIZED 1
#endif // AM_PART_APOLLO5B || AM_PART_APOLLO510L

#define ARM_FFT 1       // fft using CMSIS
#define DEBUG_NNID 0
#ifdef __cplusplus
}
#endif
#endif
