//*****************************************************************************
//
//! @file am_hal_tpiu.h
//!
//! @brief Support functions for the ARM TPIU module
//!
//! Provides support functions for configuring the ARM TPIU module
//!
//! @addtogroup tpiu4 TPIU - Trace Port Interface Unit
//! @ingroup apollo5b_hal
//! @{
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
#ifndef AM_HAL_TPIU_H
#define AM_HAL_TPIU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! Various TPI fields and values that may not be defined in core_cm55.h
//
//*****************************************************************************
#define TPI_SPPR_TXMODE_UART       2    // SWO NRZ (UART)
#define TPI_SPPR_TXMODE_MANCHESTER 1    // SWO Manchester
#define TPI_SPPR_TXMODE_PARALLEL   0    // Parallel port

#define TPI_CSPSR_CWIDTH_1BIT      1    // 1-bit trace port
#define TPI_CSPSR_CWIDTH_2BIT      2    // 2-bit trace port
#define TPI_CSPSR_CWIDTH_4BIT      4    // 4-bit trace port

#ifndef TPI_ACPR_SWOSCALER_Pos
#define TPI_ACPR_SWOSCALER_Pos              0U                                         /*!< TPI ACPR: SWOSCALER Position */
#define TPI_ACPR_SWOSCALER_Msk             (0xFFFFUL /*<< TPI_ACPR_SWOSCALER_Pos*/)    /*!< TPI ACPR: SWOSCALER Mask */
#endif

//*****************************************************************************
//
//! @name TPIU bit rate defines.
//! @{
//
//*****************************************************************************
#define AM_HAL_TPIU_BAUD_57600      (115200 / 2)
#define AM_HAL_TPIU_BAUD_115200     (115200 * 1)
#define AM_HAL_TPIU_BAUD_230400     (115200 * 2)
#define AM_HAL_TPIU_BAUD_460800     (115200 * 4)
#define AM_HAL_TPIU_BAUD_250000     (1000000 / 4)
#define AM_HAL_TPIU_BAUD_500000     (1000000 / 2)
#define AM_HAL_TPIU_BAUD_1M         (1000000 * 1)
#define AM_HAL_TPIU_BAUD_2M         (1000000 * 2)
#define AM_HAL_TPIU_BAUD_DEFAULT    (AM_HAL_TPIU_BAUD_1M)
//! @}

//*****************************************************************************
//
//! @brief Configure the TPIU
//!
//! This function configures TPIU parameters.
//! Since the TPIU is used by different peripherals such as ITM, DWT, PMU, etc.,
//! each user can call this function to configure for specific needs.
//
//*****************************************************************************
extern uint32_t am_hal_tpiu_config(uint32_t ui32DbgTpiuClksel,
                                   uint32_t ui32FFCR,
                                   uint32_t ui32CSPSR,
                                   uint32_t ui32PinProtocol,
                                   uint32_t ui32SWOscaler);

//*****************************************************************************
//
//! @brief Enables the TPIU
//!
//! This function enables the ARM TPIU by setting the TPIU registers and then
//! enabling the TPIU clock source in MCU control register.
//!
//! @param ui32SetItmBaud - Deprecated. Instead use am_hal_itm_parameters_set().
//
//*****************************************************************************
extern uint32_t am_hal_tpiu_enable(uint32_t ui32DeprecatedSetItmBaud);

//*****************************************************************************
//
//! @brief Disables the TPIU
//!
//! This function disables the ARM TPIU by disabling the TPIU clock source
//! in MCU control register.
//
//*****************************************************************************
extern uint32_t am_hal_tpiu_disable(void);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_TPIU_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

