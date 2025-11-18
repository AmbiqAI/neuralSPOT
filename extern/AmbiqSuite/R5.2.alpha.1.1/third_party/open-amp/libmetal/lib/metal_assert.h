/*
 * Copyright (c) 2018, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	metal_assert.h
 * @brief	Assertion support.
 */

#ifndef __METAL_ASSERT__H__
#define __METAL_ASSERT__H__

// Include the metal-specific assertion support
#include "./system/freertos/metal_freertos_assert.h"

/**
 * @brief Assertion macro.
 * @param cond Condition to test.
 */
#define metal_assert(cond) metal_sys_assert(cond)

#endif /* __METAL_ASSERT_H__ */

