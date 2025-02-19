//*****************************************************************************
//
//! @file system_apollo5b.c
//!
//! @brief Ambiq Micro Apollo510 MCU specific functions.
//
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
// This is part of revision release_sdk5p2-040c7863bb of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include <stdint.h>
#include "system_apollo5b.h"
#include "apollo5b.h"

//*****************************************************************************
//
// Defines
//
//*****************************************************************************

//
// Clocks
//
#define __HSI             (6000000UL)
#define __XTAL            (32768UL)         // Crystal Oscillator frequency
#define __SYS_OSC_CLK     (96000000)        // Main oscillator frequency
#define __SYSTEM_CLOCK    (1 * __SYS_OSC_CLK)

//
// Initialize SystemCoreClock with the system core clock frequency value
// achieved after system intitialization.
// This means system core clock frequency after call to SystemInit()
//
uint32_t SystemCoreClock = __SYSTEM_CLOCK;  // System Clock Frequency (Core Clock)

//---------------------------------------------------------------------------
//  Exception / Interrupt Vector table
//---------------------------------------------------------------------------*/
extern const VECTOR_TABLE_Type __VECTOR_TABLE[496];

//*****************************************************************************
//
//! @brief Set the global clock frequncy.
//!
//! This function sets the global clock frequency.
//!
//! @return None.
//
//*****************************************************************************
void
SystemCoreClockUpdate(void)
{
    //
    // Calculate the system frequency based upon the current register settings.
    // This function can be used to retrieve the system core clock frequeny
    // after user changed register sittings.
    //
    SystemCoreClock = __SYS_OSC_CLK;
}

//*****************************************************************************
//
//! @brief Initialize the system.
//!
//! This function sets up the microcontroller system.
//!
//! @return None.
//
//*****************************************************************************
void
SystemInit(void)
{

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t)(&__VECTOR_TABLE[0]);
#endif

#if (defined (__FPU_USED) && (__FPU_USED == 1U)) || \
    (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
  SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */

  /* Set low-power state for PDEPU                */
  /*  0b00  | ON, PDEPU is not in low-power state */
  /*  0b01  | ON, but the clock is off            */
  /*  0b10  | RET(ention)                         */
  /*  0b11  | OFF                                 */

  /* Clear ELPSTATE, value is 0b11 on Cold reset */
  PWRMODCTL->CPDLPSTATE &= ~(PWRMODCTL_CPDLPSTATE_ELPSTATE_Msk);

  /* Favor best FP/MVE performance by default, avoid EPU switch-ON delays */
  /* PDEPU ON, Clock OFF */
  PWRMODCTL->CPDLPSTATE |= 0x1 << PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos;
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

  /* Enable Loop and branch info cache */
  SCB->CCR |= SCB_CCR_LOB_Msk;
  __DSB();
  __ISB();

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  TZ_SAU_Setup();
#endif

  SystemCoreClock = __SYSTEM_CLOCK;

//
// Set the SSRAM non-cacheable for application explicitly requested SSRAM_NON_CACHEABLE
//
#ifdef SSRAM_NON_CACHEABLE
  __DSB();
  //
  // Set up non-cachable MPU region attributes.
  //
  ARM_MPU_SetMemAttr (
    7, // use the last MPU attribute slot
    ARM_MPU_ATTR (
      ARM_MPU_ATTR_MEMORY_ (0, 1, 0, 0),
      ARM_MPU_ATTR_MEMORY_ (0, 1, 0, 0)
    )
  );

  //
  // Set the whole SSRAM non-cacheable 
  //
  ARM_MPU_Region_t region;
  region.RBAR = ((0x20080000 & MPU_RBAR_BASE_Msk) |
                 (ARM_MPU_SH_NON << MPU_RBAR_SH_Pos) |
                 (ARM_MPU_AP_(0, 1) << MPU_RBAR_AP_Pos) |
                 (1 << MPU_RBAR_XN_Pos));
  region.RLAR = ((0x2037FFFF & MPU_RLAR_LIMIT_Msk) |
                 (7 << MPU_RLAR_AttrIndx_Pos) |
                 (1));
  ARM_MPU_Load (
    15, // use the last MPU region 
    (ARM_MPU_Region_t const*)&region, 1);

  //
  // Enable MPU
  //
  SCB_CleanInvalidateDCache();
  ARM_MPU_Enable((1 << MPU_CTRL_HFNMIENA_Pos) |
                 (1 << MPU_CTRL_PRIVDEFENA_Pos));
#endif  // SSRAM_NON_CACHEABLE
}
