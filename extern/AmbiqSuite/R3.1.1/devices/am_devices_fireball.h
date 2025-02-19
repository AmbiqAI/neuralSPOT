//*****************************************************************************
//
//! @file am_devices_fireball.h
//!
//! @brief Control the Ambiq FIREBALL board.
//!
//! This module contains functions for controlling the Ambiq internal FIREBALL
//! board.
//!
//! @addtogroup fireball FIREBALL Board Driver
//! @ingroup devices
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2023, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_3_1_1-10cda4b5e0 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_DEVICES_FIREBALL_H
#define AM_DEVICES_FIREBALL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//
//! Fireball driver is included.
//
//#define AM_DEVICES_FIREBALL     1

//
//! Fireball version info.
//! These 16-bit values are returned by a call to:
//!    - am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_ID_GET, &x);
//
#define FIREBALL_ID         0x7710
#define FIREBALL2_ID        0x7712
#define FIREBALL3_ID        0x7713


//*****************************************************************************
//
//! Enums for am_devices_fireball_control()
//
//*****************************************************************************
typedef enum
{
    //
    //! @name General
    //! @{
    //
    AM_DEVICES_FIREBALL_STATE_FBGEN_GET = 1,    //!< 1=Orig Fireball, 2=Fireball2
    AM_DEVICES_FIREBALL_STATE_ID_GET,           //!< Get actual ID from Fireball
    AM_DEVICES_FIREBALL_STATE_VER_GET,          //!< Get Fireball firmware version
    AM_DEVICES_FIREBALL_STATE_LED_BLINK,        //!< Blink LED n times (n is the call parameter)
    //! @}

    //
    //! @name Fireball device resets
    //! @{
    //
    AM_DEVICES_FIREBALL_STATE_ALL_RESET,
    AM_DEVICES_FIREBALL_STATE_SX9300_RESET,
    AM_DEVICES_FIREBALL_STATE_BNO055_RESET,
    AM_DEVICES_FIREBALL_STATE_MSPI_RESET,
    AM_DEVICES_FIREBALL_STATE_MKB1_RESET,
    AM_DEVICES_FIREBALL_STATE_MKB2_RESET,
    //! @}

    //
    //! @name States for Fireball
    //! @{
    //
    AM_DEVICES_FIREBALL_STATE_SPI_FLASH,
    AM_DEVICES_FIREBALL_STATE_SPI_FRAM,
    AM_DEVICES_FIREBALL_STATE_I2C_IOM0,
    AM_DEVICES_FIREBALL_STATE_I2C_IOM1,
    AM_DEVICES_FIREBALL_STATE_I2C_IOM2,
    AM_DEVICES_FIREBALL_STATE_I2C_IOM3,
    AM_DEVICES_FIREBALL_STATE_I2C_IOM4,
    AM_DEVICES_FIREBALL_STATE_I2C_IOM5,
    AM_DEVICES_FIREBALL_STATE_OCTAL_FLASH_CE0,
    AM_DEVICES_FIREBALL_STATE_OCTAL_FLASH_CE1,
    AM_DEVICES_FIREBALL_STATE_TWIN_QUAD_CE0_CE1,
    //! @}

    //
    //! @name Fireball2 device resets
    //! @{
    //
    AM_DEVICES_FIREBALL2_STATE_GLOBAL_RESET,    //!< New for Fireball2 global reset
    AM_DEVICES_FIREBALL2_STATE_SX9300_RESET,
    AM_DEVICES_FIREBALL2_STATE_BNO055_RESET,
    AM_DEVICES_FIREBALL2_STATE_MSPI_RESET,
    AM_DEVICES_FIREBALL2_STATE_MKB1_RESET,
    AM_DEVICES_FIREBALL2_STATE_MKB2_RESET,
    AM_DEVICES_FIREBALL2_STATE_GP30_RESET,
    AM_DEVICES_FIREBALL2_STATE_ALL_RESET,       //!< Reset all devices individually
    //! @}

    //
    //! @name States for Fireball2
    //! @{
    //
    AM_DEVICES_FIREBALL2_STATE_SPI_FRAM_PSRAM_1P8,
    AM_DEVICES_FIREBALL2_STATE_SPI_PSRAM_FLASH_3P3,
    AM_DEVICES_FIREBALL2_STATE_I2C_IOM0,
    AM_DEVICES_FIREBALL2_STATE_I2C_IOM1,
    AM_DEVICES_FIREBALL2_STATE_I2C_IOM2,
    AM_DEVICES_FIREBALL2_STATE_I2C_IOM3,
    AM_DEVICES_FIREBALL2_STATE_I2C_IOM4,
    AM_DEVICES_FIREBALL2_STATE_I2C_IOM5,
    AM_DEVICES_FIREBALL2_STATE_MSPI_FRAM_PSRAM_FLASH_1P8,
    AM_DEVICES_FIREBALL2_STATE_MSPI_PSRAM_FLASH_3P3,
    AM_DEVICES_FIREBALL2_STATE_SC_8_9_16,
    AM_DEVICES_FIREBALL2_STATE_SC_17_32_26,
    AM_DEVICES_FIREBALL2_STATE_SC_19_18_46,
    AM_DEVICES_FIREBALL2_STATE_SC_31_37_21,
    AM_DEVICES_FIREBALL2_STATE_PDM_10_29,
    AM_DEVICES_FIREBALL2_STATE_PDM_12_15,
    AM_DEVICES_FIREBALL2_STATE_PDM_14_11,
    AM_DEVICES_FIREBALL2_STATE_PDM_22_34,
    AM_DEVICES_FIREBALL2_STATE_PDM_37_36,
    AM_DEVICES_FIREBALL2_STATE_PDM_46_45,
    AM_DEVICES_FIREBALL2_STATE_PDM_AMP_IN,
    AM_DEVICES_FIREBALL2_STATE_I2S_DAC,
    AM_DEVICES_FIREBALL2_STATE_STEPPER,
    //
    //! @note Most are the same as for FB2 except for a few new ones.\n
    //! However, the ordering is important such that FB2 states should
    //!  map to similar FB3 states by simply adding an offset.
    //! @}

    //
    //! @name FB3 device reset states
    //! @{
    //
    AM_DEVICES_FIREBALL3_STATE_GLOBAL_RESET,
    AM_DEVICES_FIREBALL3_STATE_SX9300_RESET,
    AM_DEVICES_FIREBALL3_STATE_BNO055_RESET,
    AM_DEVICES_FIREBALL3_STATE_MSPI_RESET,
    AM_DEVICES_FIREBALL3_STATE_MKB1_RESET,
    AM_DEVICES_FIREBALL3_STATE_MKB2_RESET,
    AM_DEVICES_FIREBALL3_STATE_GP30_RESET,
    AM_DEVICES_FIREBALL3_STATE_ALL_RESET,       //!< Reset all devices individually

    AM_DEVICES_FIREBALL3_STATE_SPI_FRAM_PSRAM_1P8,
    AM_DEVICES_FIREBALL3_STATE_SPI_PSRAM_FLASH_3P3,
    AM_DEVICES_FIREBALL3_STATE_I2C_IOM0,
    AM_DEVICES_FIREBALL3_STATE_I2C_IOM1,
    AM_DEVICES_FIREBALL3_STATE_I2C_IOM2,
    AM_DEVICES_FIREBALL3_STATE_I2C_IOM3,
    AM_DEVICES_FIREBALL3_STATE_I2C_IOM4,
    AM_DEVICES_FIREBALL3_STATE_I2C_IOM5,
    AM_DEVICES_FIREBALL3_STATE_MSPI0_PSRAM_1P8,
    AM_DEVICES_FIREBALL3_STATE_MSPI0_PSRAM_3P3,
    AM_DEVICES_FIREBALL3_STATE_SC_8_9_16,
    AM_DEVICES_FIREBALL3_STATE_SC_17_32_26,
    AM_DEVICES_FIREBALL3_STATE_SC_19_18_46,
    AM_DEVICES_FIREBALL3_STATE_SC_31_37_21,
    AM_DEVICES_FIREBALL3_STATE_PDM_10_29,
    AM_DEVICES_FIREBALL3_STATE_PDM_12_15,
    AM_DEVICES_FIREBALL3_STATE_PDM_14_11,
    AM_DEVICES_FIREBALL3_STATE_PDM_22_34,
    AM_DEVICES_FIREBALL3_STATE_PDM_37_36,
    AM_DEVICES_FIREBALL3_STATE_PDM_46_45,
    AM_DEVICES_FIREBALL3_STATE_PDM_AMP_IN,
    AM_DEVICES_FIREBALL3_STATE_I2S_DAC,
    AM_DEVICES_FIREBALL3_STATE_STEPPER,
    //! @}

    //
    //! @name New states for FB3
    //! @{
    //
    AM_DEVICES_FIREBALL3_STATE_MSPI0_FLASH_1P8,
    AM_DEVICES_FIREBALL3_STATE_MSPI0_FT812,
    AM_DEVICES_FIREBALL3_STATE_MSPI0_DISPLAY,
    AM_DEVICES_FIREBALL3_STATE_MSPI1_PSRAM_1P8,
    AM_DEVICES_FIREBALL3_STATE_MSPI1_FLASH_1P8,
    AM_DEVICES_FIREBALL3_STATE_MSPI1_PSRAM_3P3,
    AM_DEVICES_FIREBALL3_STATE_MSPI1_FT812,
    AM_DEVICES_FIREBALL3_STATE_MSPI1_DISPLAY,
    AM_DEVICES_FIREBALL3_STATE_MSPI2_PSRAM_1P8,
    AM_DEVICES_FIREBALL3_STATE_MSPI2_FLASH_1P8,
    AM_DEVICES_FIREBALL3_STATE_MSPI2_PSRAM_3P3,
    AM_DEVICES_FIREBALL3_STATE_MSPI2_FT812,
    AM_DEVICES_FIREBALL3_STATE_MSPI2_DISPLAY,
    //! @}

    //
    //! End of valid states
    //
    AM_DEVICES_FIREBALL_STATE_INVALID

} am_devices_fireball_control_e;


//
//! @name Define valid first/last states for each board.
//! @{
//
#define AM_DEVICES_FIREBALL_STATE_FIRST     AM_DEVICES_FIREBALL_STATE_ALL_RESET
#define AM_DEVICES_FIREBALL_STATE_LAST      AM_DEVICES_FIREBALL_STATE_TWIN_QUAD_CE0_CE1

#define AM_DEVICES_FIREBALL2_STATE_FIRST    AM_DEVICES_FIREBALL2_STATE_GLOBAL_RESET
#define AM_DEVICES_FIREBALL2_STATE_LAST     AM_DEVICES_FIREBALL2_STATE_STEPPER

#define AM_DEVICES_FIREBALL3_STATE_FIRST    AM_DEVICES_FIREBALL3_STATE_GLOBAL_RESET
#define AM_DEVICES_FIREBALL3_STATE_LAST     AM_DEVICES_FIREBALL3_STATE_STEPPER

//! @}


//*****************************************************************************
//
//! @brief Set FIREBALL state.
//!
//! @param eState - Designate which state to put the FIREBALL.  The state
//! number indicates which peripherals are muxed in.
//! @param pArgs
//
//*****************************************************************************
extern uint32_t am_devices_fireball_control(am_devices_fireball_control_e eState,
                                            void *pArgs);


#ifdef __cplusplus
}
#endif

#endif // AM_DEVICES_FIREBALL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************

