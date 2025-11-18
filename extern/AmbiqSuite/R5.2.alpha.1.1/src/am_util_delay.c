//*****************************************************************************
//
//! @file am_util_delay.c
//!
//! @brief Delay Utility Functions
//!
//! @addtogroup delay_utils Delay Utility Functions
//! @ingroup utils
//! @{
//!
//! Purpose: This module provides delay utility functions for timing control
//!          across Ambiq Micro devices. It enables accurate software delays,
//!          cycle counting, and timing operations for embedded applications
//!          requiring precise timing control. The utilities support both
//!          microsecond and millisecond delays with calibration options.
//!
//! @section delay_features Key Features
//!
//! 1. @b Cycle @b Delays: Precise cycle-based timing control.
//! 2. @b Millisecond @b Delays: High-level millisecond delay functions.
//! 3. @b Microsecond @b Delays: Precise microsecond timing operations.
//! 4. @b Frequency @b Adaptive: Automatic timing adjustment based on system clock.
//! 5. @b Device @b Specific: Optimized implementations for different Apollo families.
//! 6. @b Power @b Efficient: Low-power delay operations for battery applications.
//!
//! @section delay_functionality Functionality
//!
//! - Provide cycle-based delay functions for precise timing
//! - Support millisecond and microsecond delay operations
//! - Adapt timing based on system clock frequency
//! - Optimize delays for different Apollo device families
//! - Enable power-efficient timing operations
//! - Support hardware-specific delay implementations
//! - Provide consistent timing across device variations
//! - Handle clock frequency variations automatically
//!
//! @section delay_usage Usage
//!
//! 1. Use am_util_delay_cycles() for precise cycle-based delays
//! 2. Call am_util_delay_ms() for millisecond timing
//! 3. Use am_util_delay_us() for microsecond precision
//! 4. System automatically adapts to device clock frequency
//! 5. No manual configuration required for different devices
//!
//! @section delay_configuration Configuration
//!
//! - @b Device @b Support: Apollo3, Apollo4, Apollo5 family compatibility
//! - @b Clock @b Detection: Automatic system clock frequency detection
//! - @b Hardware @b Optimization: Device-specific delay implementations
//! - @b Power @b Management: Low-power delay operations
//! - @b Timing @b Precision: Cycle-accurate timing control
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include  <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_util_delay.h"

//*****************************************************************************
//
// Delays for a desired amount of loops.
//
//*****************************************************************************
void
am_util_delay_cycles(uint32_t ui32Iterations)
{
    //
    // Call the BOOTROM cycle delay function
    //
#if defined(AM_PART_APOLLO4_API) || defined(AM_PART_APOLLO5_API)
    am_hal_delay_us( ui32Iterations);
#else
    am_hal_flash_delay(ui32Iterations);
#endif // AM_PART_APOLLO4_API || AM_PART_APOLLO5_API
}

//*****************************************************************************
//
// Delays for a desired amount of milliseconds.
//
//*****************************************************************************
void
am_util_delay_ms(uint32_t ui32MilliSeconds)
{
#if defined(AM_PART_APOLLO4_API) || defined(AM_PART_APOLLO5_API)
    am_hal_delay_us( ui32MilliSeconds * 1000);
#else // AM_PART_APOLLO4_API || AM_PART_APOLLO5_API
    uint32_t ui32Loops, ui32HFRC;
#if AM_APOLLO3_CLKGEN
    am_hal_clkgen_status_t sClkgenStatus;
    am_hal_clkgen_status_get(&sClkgenStatus);
    ui32HFRC = sClkgenStatus.ui32SysclkFreq;
#else // AM_APOLLO3_CLKGEN
    ui32HFRC = am_hal_clkgen_sysclk_get();
#endif // AM_APOLLO3_CLKGEN
    ui32Loops = ui32MilliSeconds * (ui32HFRC / 3000);

    //
    // Call the BOOTROM cycle delay function
    //
    am_hal_flash_delay(ui32Loops);
#endif // AM_PART_APOLLO4_API || AM_PART_APOLLO5_API
}

//*****************************************************************************
//
// Delays for a desired amount of microseconds.
//
//*****************************************************************************
void
am_util_delay_us(uint32_t ui32MicroSeconds)
{
#if defined(AM_PART_APOLLO4_API) || defined(AM_PART_APOLLO5_API)
    am_hal_delay_us( ui32MicroSeconds );
#else // AM_PART_APOLLO4_API || AM_PART_APOLLO5_API
    uint32_t ui32Loops, ui32HFRC;

#if AM_APOLLO3_CLKGEN
    am_hal_clkgen_status_t sClkgenStatus;
    am_hal_clkgen_status_get(&sClkgenStatus);
    ui32HFRC = sClkgenStatus.ui32SysclkFreq;
#else // AM_APOLLO3_CLKGEN
    ui32HFRC = am_hal_clkgen_sysclk_get();
#endif // AM_APOLLO3_CLKGEN
    ui32Loops = ui32MicroSeconds * (ui32HFRC / 3000000);

    //
    // Call the BOOTROM cycle delay function
    //
    am_hal_flash_delay(ui32Loops);
#endif // AM_PART_APOLLO4_API || AM_PART_APOLLO5_API
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

