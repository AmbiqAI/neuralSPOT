//*****************************************************************************
//
//! @file am_hal_info.h
//!
//! @brief INFO helper functions
//!
//! @addtogroup info5 INFO Functionality
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
#ifndef AM_HAL_INFO_H
#define AM_HAL_INFO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
//! @name Enumerations for the eInfoSpace argument of am_hal_info0_read()
//! and am_hal_info1_read().
//! @{
//
//*****************************************************************************
typedef enum
{
    AM_HAL_INFO_INFOSPACE_CURRENT_INFO0,    // Currently active INFO0
    AM_HAL_INFO_INFOSPACE_CURRENT_INFO1,    // Currently active INFO1
    AM_HAL_INFO_INFOSPACE_OTP_INFO0,        // INFO0 from OTP (regardless of current)
    AM_HAL_INFO_INFOSPACE_OTP_INFO1,        // INFO1 from OTP (regardless of current)
    AM_HAL_INFO_INFOSPACE_MRAM_INFO0,       // INFO0 from MRAM (regardless of current)
    AM_HAL_INFO_INFOSPACE_MRAM_INFO1,       // INFO1 from MRAM (regardless of current)
} am_hal_info_infospace_e;


//! @}

//*****************************************************************************
//
//! @brief Apollo5b INFO0 signature values.
//
//*****************************************************************************
#define INFO0_SIGNATURE0    0x48EAAD88
#define INFO0_SIGNATURE1    0xC9705737
#define INFO0_SIGNATURE2    0x0A6B8458
#define INFO0_SIGNATURE3    0xE41A9D74

//*****************************************************************************
//
//! @brief Determine if INFO0 is valid.
//
//*****************************************************************************
extern bool am_hal_info0_valid(void);

//*****************************************************************************
//
//! @brief Read data from INFO0.
//!
//! This function reads INFO0 data from the device.
//! The data to be read depends on the eInfoSpace parameter.
//!
//! @param eInfoSpace - Specifies which info space to be read.
//!     AM_HAL_INFO_INFOSPACE_CURRENT_INFO0     // Currently active INFO0
//!     AM_HAL_INFO_INFOSPACE_OTP_INFO0         // INFO0 from OTP  (regardless of current)
//!     AM_HAL_INFO_INFOSPACE_MRAM_INFO0        // INFO0 from MRAM (regardless of current)
//!
//! @param ui32wordOffset - Word offset of the first data word to be read.
//!     - For the "CURRENT" types, the offset is ALWAYS specified by the OTP
//!       offset (as if the data were definitely stored in OTP).
//!     - For the "OTP" or "MRAM" types, the user is expected to supply
//!       the appropriate offset.
//!     - Note that this value is a word offset, that is, the byte offset divided by 4.
//!
//! @param ui32NumWords - The number of words to be read from INFOx into the caller's buffer.
//!
//! @param piu32Dst - Pointer to the caller's buffer.
//!
//! @note Refer to am_hal_pwrctrl_INFO1_populate() for details about
//!       the power down of OTP.
//!       Caller is responsible for powering up OTP as necessary before
//!       calling this function. OTP can then be safely disabled after
//!       exit from this function.
//!
//! When AM_HAL_INFO_INFOSPACE_CURRENT_INFO0 is specified, the current usage
//! of INFO0 (OTP or MRAM) is determined via
//! MCUCTRL->SHADOWVALID_b.INFO0SELOTP
//!
//! The current power status of OTP can be determined via
//! am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_OTP, &bEnabled);
//!
//! OTP is enabled or disabled via:
//! am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP);
//! am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);
//!
//! @return 0 for success, non-zero for failure.
//
//*****************************************************************************
extern uint32_t
am_hal_info0_read(am_hal_info_infospace_e eInfoSpace,
                  uint32_t ui32wordOffset,
                  uint32_t ui32NumWords,
                  uint32_t *pui32Dst);

//*****************************************************************************
//
//! @brief Read data from INFO1.
//!
//! This function reads INFO1 data from the device.
//! The data to be read depends on the eInfoSpace parameter.
//!
//! @param eInfoSpace - Specifies which info space to be read.
//!     AM_HAL_INFO_INFOSPACE_CURRENT_INFO1     // Currently active INFO1
//!     AM_HAL_INFO_INFOSPACE_OTP_INFO1         // INFO1 from OTP  (regardless of current)
//!     AM_HAL_INFO_INFOSPACE_MRAM_INFO1        // INFO1 from MRAM (regardless of current)
//!
//! @param ui32wordOffset - Word offset of the first data word to be read.
//!     - For the "CURRENT" types, the offset is ALWAYS specified by the OTP
//!       offset (as if the data were definitely stored in OTP).
//!     - For the "OTP" or "MRAM" types, the user is expected to supply
//!       the appropriate offset.
//!     - Note that this value is a word offset, that is, the byte offset divided by 4.
//!
//! @param ui32NumWords - The number of words to be read from INFOx into the caller's buffer.
//!
//! @param piu32Dst - Pointer to the caller's buffer.
//!
//! @note OTP is powered down in am_hal_pwrctrl_low_power_init(). Therefore if
//!       INFO1 is OTP, caller is responsible for making sure OTP is powered up
//!       before calling this function. OTP can then be safely disabled after
//!       exit from this function. See following for determining status.
//!
//! When AM_HAL_INFO_INFOSPACE_CURRENT_INFO1 is specified, the current usage
//! of INFO1 (OTP or MRAM) is determined via
//! MCUCTRL->SHADOWVALID_b.INFO1SELOTP
//!
//! The current power status of OTP can be determined via
//! am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_OTP, &bEnabled);
//!
//! OTP is enabled or disabled via:
//! am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP);
//! am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);
//!
//! @return 0 for success, non-zero for failure.
//
//*****************************************************************************
extern uint32_t
am_hal_info1_read(am_hal_info_infospace_e eInfoSpace,
                  uint32_t ui32wordOffset,
                  uint32_t ui32NumWords,
                  uint32_t *pui32Dst);

//*****************************************************************************
//
//! @brief This programs up to N words of INFO0.
//!
//! @param eInfoSpace - Specifies which info space to be read.
//!     AM_HAL_INFO_INFOSPACE_CURRENT_INFO0     // Currently active INFO0
//!     AM_HAL_INFO_INFOSPACE_OTP_INFO0         // INFO0 from OTP  (regardless of current)
//!     AM_HAL_INFO_INFOSPACE_MRAM_INFO0        // INFO0 from MRAM (regardless of current)
//! @param ui32InfoKey - The programming key, AM_HAL_MRAM_INFO_KEY.
//! @param pui32Src - Pointer to word aligned array of data to program into
//! INFO0.
//! @param ui32Offset - Pointer to the word aligned INFO0 offset where
//! programming of the INFO0 is to begin.
//! @param ui32NumWords - The number of words to be programmed.
//!
//! This function will program multiple words in INFO0
//!
//! @note Refer to am_hal_pwrctrl_INFO1_populate() for details about
//!       the power down of OTP.
//!       Caller is responsible for powering up OTP as necessary before
//!       calling this function. OTP can then be safely disabled after
//!       exit from this function.
//!
//! When AM_HAL_INFO_INFOSPACE_CURRENT_INFO0 is specified, the current usage
//! of INFO0 (OTP or MRAM) is determined via
//! MCUCTRL->SHADOWVALID_b.INFO0SELOTP
//!
//! The current power status of OTP can be determined via
//! am_hal_pwrctrl_periph_enabled(AM_HAL_PWRCTRL_PERIPH_OTP, &bEnabled);
//!
//! OTP is enabled or disabled via:
//! am_hal_pwrctrl_periph_enable(AM_HAL_PWRCTRL_PERIPH_OTP);
//! am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_OTP);
//!
//! @return 0 for success, non-zero for failure.
//!     1   ui32InfoKey is invalid.
//!     2   ui32Offset is invalid.
//!     3   Dst addressing range would be exceeded.  That is, (ui32Offset +
//!         (ui32NumWords * 4)) is greater than the last valid address.
//!     4   pui32Src is invalid.
//!     5   Src addressing range would be exceeded.  That is, (pui32Src +
//!         (ui32NumWords * 4)) is greater than the last valid address.
//!     6   Flash controller hardware timeout.
//!     9   OTP is not active.
//
//*****************************************************************************
extern uint32_t
am_hal_info0_program(am_hal_info_infospace_e eInfoSpace,
                     uint32_t ui32InfoKey, uint32_t *pui32Src,
                     uint32_t ui32Offset,  uint32_t ui32NumWords);


#ifdef __cplusplus
}
#endif

#endif // AM_HAL_INFO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

