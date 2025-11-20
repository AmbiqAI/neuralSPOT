/*
 * Copyright (c) 2020 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file      errno.h
 * @brief     error specific primitives for libmetal.
 */

#ifndef __METAL_ERRNO__H__
#define __METAL_ERRNO__H__

#if defined(__clang__)
#include "./compiler/clang/errno.h"
#elif defined(__GNUC__)
#include "./compiler/gcc/errno.h"
#elif defined(__ICCARM__)
#include "./compiler/iar/errno.h"
#else
#include <errno.h>
#endif

#endif /* __METAL_ERRNO__H__ */
