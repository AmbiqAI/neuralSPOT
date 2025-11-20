//*****************************************************************************
//
//! @file am_devices_tas2560.h
//!
//! @brief Texas Instruments TAS2560 Digital Audio Amplifier Driver with I2C Interface.
//!
//! @addtogroup devices_tas2560 TAS2560 I2C Audio Amplifier Driver
//! @ingroup devices
//! @{
//
//**************************************************************************

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

#ifndef AM_DEVICES_TAS2560_H
#define AM_DEVICES_TAS2560_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "am_mcu_apollo.h"

/* Page Control Register */
#define TAS2560_PAGECTL_REG 0

/* Book Control Register (available in page0 of each book) */
#define TAS2560_BOOKCTL_PAGE 0
#define TAS2560_BOOKCTL_REG  0x7F

#define TAS2560_REG(book, page, reg) (((book * 256 * 128) + (page * 128)) + reg)

#define TAS2560_BOOK_ID(reg)  (reg / (256 * 128))
#define TAS2560_PAGE_ID(reg)  ((reg % (256 * 128)) / 128)
#define TAS2560_BOOK_REG(reg) (reg % (256 * 128))
#define TAS2560_PAGE_REG(reg) ((reg % (256 * 128)) % 128)

#define TAS2560_MUTE_REG TAS2560_REG(0, 0, 7)

#define TAS2560_SR_CTRL1 TAS2560_REG(0, 0, 8)
#define TAS2560_LOAD     TAS2560_REG(0, 0, 9)
#define TAS2560_SR_CTRL2 TAS2560_REG(0, 0, 13) /* B0_P0_R0x0d*/
#define TAS2560_SR_CTRL3 TAS2560_REG(0, 0, 14) /* B0_P0_R0x0e*/

#define TAS2560_DAI_FMT TAS2560_REG(0, 0, 20) /*B0_P0_R0x14*/
#define TAS2560_CHANNEL TAS2560_REG(0, 0, 21) /*B0_P0_R0x15*/

// Left justifying format, 16bit
#define TAS2560_DAI_FMT_VALUE        0x6c
#define TAS2560_DAI_WORD_LENGTH_MASK 0x03

#define AM_DEVICES_TAS2560_I2C_WR 0x0000
#define AM_DEVICES_TAS2560_I2C_RD (1u << 0)

    // I2C message.
    typedef struct i2c_msg
    {
        uint16_t addr;  // Chip address.
        uint16_t flags; // The flag of read or write.
        uint16_t len;   // The length of buffer.
        uint8_t *buf;   // The pointer of buffer.
    } i2c_msg_t;

    // The struct of register table.
    typedef struct
    {
        uint8_t command; // Command or register address.
        uint8_t param;   // Command parameter, or register value.
    } cfg_reg_t;

    // Generic or interface specific status.
    typedef enum
    {
        AM_DEVICES_TAS2560_STATUS_SUCCESS,
        AM_DEVICES_TAS2560_STATUS_FAIL,
        AM_DEVICES_TAS2560_STATUS_INVALID_HANDLE,
        AM_DEVICES_TAS2560_STATUS_IN_USE,
        AM_DEVICES_TAS2560_STATUS_TIMEOUT,
        AM_DEVICES_TAS2560_STATUS_OUT_OF_RANGE,
        AM_DEVICES_TAS2560_STATUS_INVALID_ARG,
        AM_DEVICES_TAS2560_STATUS_INVALID_OPERATION,
        AM_DEVICES_TAS2560_STATUS_MEM_ERR,
        AM_DEVICES_TAS2560_STATUS_HW_ERR,
    } am_devices_tas2560_status_t;

    // TAS2560 channel mode
    typedef enum
    {
        TAS2560_LEFT = 0,   // Left channel
        TAS2560_RIGHT,      // Right channel
        TAS2560_LEFT_RIGHT, // (Left + Right) / 2
        TAS2560_MONO,       // Mono mode
    } am_devices_tas2560_ch_t;

    //  TAS2560 mute status
    typedef enum
    {
        TAS2560_MUTE = 0, // Mute TAS2560
        TAS2560_UNMUTE,   // Unmute TAS2560
    } am_devices_tas2560_mute_t;
    //*****************************************************************************
    //
    // External function definitions.
    //
    //*****************************************************************************

    //*****************************************************************************
    //
    //! @brief Init IOM for the TAS2560 AMP
    //!
    //! @param ui32Module - IOM Module Number.
    //! @param *psIOMSettings - Pointer to the IOM Settings.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t
    am_devices_tas2560_i2c_init(uint32_t             ui32Module,
                                am_hal_iom_config_t *psIOMSettings);

    //*****************************************************************************
    //
    //! @brief Init the Device TAS2560 Amplifier.
    //!
    //! @param chip_addr - Chip I2C device address.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t am_devices_tas2560_init(uint8_t chip_addr);

    //*****************************************************************************
    //
    //! @brief Deinit IOM for the TAS2560 Amplifier.
    //!
    //! @param ui32Module - IOM Module Number.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t am_devices_tas2560_i2c_deinit(uint32_t ui32Module);

    //*****************************************************************************
    //
    //! @brief Deinit the Device TAS2560 Amplifier.
    //!
    //! @param chip_addr - Chip I2C device address.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t am_devices_tas2560_deinit(uint8_t chip_addr);

    //*****************************************************************************
    //
    //! @brief Set TAS2560 Sample Rate.
    //!
    //! @param chip_addr - Chip I2C device address.
    //! @param sample_rate - Sample Rate.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t am_devices_tas2560_set_sample_rate(uint8_t  chip_addr,
                                                       uint32_t sample_rate);

    //*****************************************************************************
    //
    //! @brief Set TAS2560 channel mode.
    //!
    //! @param chip_addr - Chip I2C device address.
    //! @param ch - The number of channel.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t am_devices_tas2560_set_channel(uint8_t chip_addr,
                                                   am_devices_tas2560_ch_t ch);

    //*****************************************************************************
    //
    //! @brief Set TAS2560 mute status.
    //!
    //! @param chip_addr - Chip I2C device address.
    //! @param val - The status of mute.
    //!
    //! @return status - Generic or interface specific status.
    //
    //*****************************************************************************
    extern uint32_t am_devices_tas2560_set_mute(uint8_t chip_addr,
                                                am_devices_tas2560_mute_t val);
#ifdef __cplusplus
}
#endif

#endif

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
