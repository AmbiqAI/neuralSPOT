//*****************************************************************************
//
//! @file dcd_apollo4.h
//!
//! @brief Addional Functions for USB module not defined in dcd.h
//!
//! @addtogroup tinyUSBAmbiq Functionality
//! @{
//
//*****************************************************************************
//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_5_0-a1ef3b89f9 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef _DCD_APOLLO4_H
#define _DCD_APOLLO4_H

#include "tusb_option.h"

#if CFG_TUD_ENABLED

#include "device/dcd.h"
#include "am_mcu_apollo.h"
#include "am_util_delay.h"
#include "am_bsp.h"         // Declare BSP functions am_bsp_external_vddusb33_switch() and am_bsp_external_vddusb0p9_switch()

//
//! This is to enable extra functions not defined in dcd.h
//! These functions are prototyped below
//
#define TUSB_ADDED_FUNCTIONS 1

//*****************************************************************************
//
//! @brief This is called for a graceful USB power-up.
//!
//! @note this will typically be called as an even callback from the tud_task
//!
//! @param rhport unused (usb number)
//! @param force  when true, forces the code to execute the power up sequence
//
//*****************************************************************************
extern void dcd_powerup(uint8_t rhport, bool force );

//*****************************************************************************
//! @brief this is called for a graceful USB powerdown
//!
//! @note this will typically be called as an even callback from the tud_task
//!
//! @param rhport unused (usb number)
//! @param force  when true, force the code to execute the power down sequence
//*****************************************************************************
extern void dcd_powerdown(uint8_t rhport, bool force );

//*****************************************************************************
//
//! @brief This is called when VUSB changes state
//!
//! @note the primary use to set VUSB is so the init function will run
//! on system startup
//!
//! @param rhport         USB port number
//! @param vddIsPresent   true if VUSB is present
//! @param takeAction     will run the init functions (recommended to usually be false)
//!
//
//*****************************************************************************
extern void dvd_set_vddPresent(
    uint8_t rhport,
    bool vddIsPresent,
    bool takeAction);

//*****************************************************************************
//
//! @brief shutdown and powerdown the usb
//!
//! @param rhport
//
//*****************************************************************************
extern void dcd_deInit( uint8_t rhport) ;

#endif // CFG_TUD_ENABLED

#endif //_DCD_APOLLO4_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
