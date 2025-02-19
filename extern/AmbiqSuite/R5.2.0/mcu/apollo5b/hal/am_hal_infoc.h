//*****************************************************************************
//
//! @file am_hal_infoc.h
//!
//! @brief Functions for INFOC functions
//!
//! @addtogroup infoc INFOC - One-Time Programmable
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

#ifndef AM_HAL_INFOC_H
#define AM_HAL_INFOC_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! @brief  Read INFOC word
//!
//! @param  offset -  word aligned offset in INFOC to be read
//! @param  pVal -  Pointer to word for returned data
//!
//! This will retrieve the INFOC information
//!
//! @note The caller is responsible for powering up OTP before calling this
//! function.
//!
//! The current power status of OTP can be determined via
//! am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_OTP, &bEnabled);
//!
//! OTP is enabled or disabled via:
//! am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP);
//! am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);
//!
//! @return Returns AM_HAL_STATUS_SUCCESS on success
//
//*****************************************************************************
extern uint32_t am_hal_infoc_read_word(uint32_t offset, uint32_t *pVal);

//*****************************************************************************
//
//! @brief  Write INFOC word
//!
//! @param  offset -  word aligned offset in INFOC to be read
//! @param  value -  value to be written
//!
//! This will write a word to the supplied offset in the INFOC
//!
//! @note The caller is responsible for powering up OTP before calling this
//! function.
//!
//! The current power status of OTP can be determined via
//! am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_OTP, &bEnabled);
//!
//! OTP is enabled or disabled via:
//! am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP);
//! am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);
//!
//! @return Returns AM_HAL_STATUS_SUCCESS on success
//
//*****************************************************************************
extern uint32_t am_hal_infoc_write_word(uint32_t offset, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_INFOC_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

