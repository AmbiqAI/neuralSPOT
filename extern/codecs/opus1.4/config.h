
#ifndef CONFIG_H
#define CONFIG_H

#define USE_ALLOCA            1

/* Comment out the next line for floating-point code */
/*#define FIXED_POINT           1 */

#define OPUS_BUILD            1

// #if defined(_M_IX86) || defined(_M_X64)
// /* Can always compile SSE intrinsics (no special compiler flags necessary) */
// #define OPUS_X86_MAY_HAVE_SSE
// #define OPUS_X86_MAY_HAVE_SSE2
// #define OPUS_X86_MAY_HAVE_SSE4_1

// /* Presume SSE functions, if compiled to use SSE/SSE2/AVX (note that AMD64 implies SSE2, and AVX
//    implies SSE4.1) */
// #if defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 1)) || defined(__AVX__)
// #define OPUS_X86_PRESUME_SSE 1
// #endif
// #if defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2)) || defined(__AVX__)
// #define OPUS_X86_PRESUME_SSE2 1
// #endif
// #if defined(__AVX__)
// #define OPUS_X86_PRESUME_SSE4_1 1
// #endif

// #if !defined(OPUS_X86_PRESUME_SSE4_1) || !defined(OPUS_X86_PRESUME_SSE2) || !defined(OPUS_X86_PRESUME_SSE)
// #define OPUS_HAVE_RTCD 1
// #endif

// #endif

#include "version.h"

#endif /* CONFIG_H */