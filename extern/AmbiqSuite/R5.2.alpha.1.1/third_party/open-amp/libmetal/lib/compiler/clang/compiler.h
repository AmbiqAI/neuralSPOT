
/*
 * @file	clang/compiler.h
 * @brief	Clang specific primitives for libmetal.
 */

#ifndef __METAL_CLANG_COMPILER__H__
#define __METAL_CLANG_COMPILER__H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wgnu"

#define restrict __restrict__
#define metal_align(n) __attribute__((aligned(n)))
#define metal_weak __attribute__((weak))

#define METAL_PACKED_BEGIN
#define METAL_PACKED_END __attribute__((__packed__))

#define metal_asm __asm__

#ifndef __deprecated
#define __deprecated	__attribute__((deprecated))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __METAL_CLANG_COMPILER__H__ */
