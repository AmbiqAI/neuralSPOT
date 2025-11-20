/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	compiler.h
 * @brief	Compiler specific primitives for libmetal.
 */

#ifndef __METAL_COMPILER__H__
#define __METAL_COMPILER__H__

#if defined(__clang__)
# include "./compiler/clang/compiler.h"
#elif defined(__GNUC__)
# include "./compiler/gcc/compiler.h"
#elif defined(__ICCARM__)
# include "./compiler/iar/compiler.h"
#elif defined(__CC_ARM)
# error "MDK-ARM ARMCC compiler requires the GNU extensions to work correctly"
#else
# error "Missing compiler support"
#endif

#endif /* __METAL_COMPILER__H__ */
