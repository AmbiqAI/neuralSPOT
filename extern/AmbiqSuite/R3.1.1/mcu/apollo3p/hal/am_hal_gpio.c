//*****************************************************************************
//
//! @file am_hal_gpio.c
//!
//! @brief Functions for Interfacing with the GPIO module
//!
//! @addtogroup gpio3p GPIO - GPIO Functions
//! @ingroup apollo3p_hal
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

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

//*****************************************************************************
//! Local defines.
//*****************************************************************************
//
// Generally define GPIO PADREG and GPIOCFG bitfields
//
#define PADREG_FLD_76_S         6
#define PADREG_FLD_FNSEL_S      3
#define PADREG_FLD_DRVSTR_S     2
#define PADREG_FLD_INPEN_S      1
#define PADREG_FLD_PULLUP_S     0

#define GPIOCFG_FLD_INTD_S      3
#define GPIOCFG_FLD_OUTCFG_S    1
#define GPIOCFG_FLD_INCFG_S     0

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
//*****************************************************************************
//  Define some common GPIO configurations.
//*****************************************************************************
const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_DISABLE =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_TRISTATE =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_TRISTATE
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN
};

//
// Input with various pullups (weak, 1.5K, 6K, 12K, 24K)
// The 1.5K - 24K pullup values are valid for select I2C enabled pads.
// For Apollo3 these pins are 0-1,5-6,8-9,25,27,39-40,42-43,48-49.
// The "weak" value is used for almost every other pad except pin 20.
//
const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .ePullup        = AM_HAL_GPIO_PIN_PULLUP_WEAK
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_1_5 =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .ePullup        = AM_HAL_GPIO_PIN_PULLUP_1_5K
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_6 =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .ePullup        = AM_HAL_GPIO_PIN_PULLUP_6K
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_12 =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .ePullup        = AM_HAL_GPIO_PIN_PULLUP_12K
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_24 =
{
    .uFuncSel       = 3,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN,
    .ePullup        = AM_HAL_GPIO_PIN_PULLUP_24K
};

//
// Variations of output (drive strengths, read, etc)
//
const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_4 =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_4MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_8 =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_8MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_12 =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
};

const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_WITH_READ =
{
    .uFuncSel       = 3,
    .eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
    .eGPInput       = AM_HAL_GPIO_PIN_INPUT_ENABLE,
    .eGPRdZero      = AM_HAL_GPIO_PIN_RDZERO_READPIN
};

//*****************************************************************************
//
//  g_ui8Inpen[]
//  This lookup table determines whether the INPEN bit is required based on
//  the pin number and FNSEL.
//
//*****************************************************************************
static const uint8_t
g_ui8Inpen[AM_HAL_GPIO_MAX_PADS] =
{
    //0     1     2     3     4     5     6     7     8     9
    0x23, 0x23, 0x27, 0x62, 0xA1, 0x03, 0x87, 0x10, 0x03, 0x53, // Pins 0-9
    0x00, 0xE1, 0x51, 0x81, 0x41, 0x55, 0x05, 0xC4, 0x80, 0x40, // Pins 10-19
    0x01, 0xB1, 0x40, 0x41, 0x14, 0x31, 0xA0, 0x31, 0x00, 0xF1, // Pins 20-29
    0x80, 0x11, 0x91, 0x21, 0xC1, 0x11, 0xE5, 0x11, 0x45, 0x30, // Pins 30-39
    0x37, 0x00, 0x30, 0x31, 0x00, 0x71, 0x00, 0x40, 0x30, 0x31, // Pins 40-49
    0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // Pins 50-59
    0x01, 0xA0, 0x50, 0xA0, 0x01, 0x01, 0x01, 0x01, 0x01, 0xA0, // Pins 60-69
    0xA0, 0xA0, 0xA0, 0x50                                      // Pins 70-73
};

//*****************************************************************************
//
//  g_ui8Bit76Capabilities[]
//  This lookup table specifies capabilities of each pad for PADREG bits 7:6.
//
//*****************************************************************************
#define CAP_PUP     0x01    // PULLUP
#define CAP_PDN     0x08    // PULLDOWN (pin 20 only)
#define CAP_VDD     0x02    // VDD PWR (power source)
#define CAP_VSS     0x04    // VSS PWR (ground sink)
#define CAP_RSV     0x80    // bits 7:6 are reserved for this pin
static const uint8_t
g_ui8Bit76Capabilities[AM_HAL_GPIO_MAX_PADS] =
{
    //0        1        2        3        4        5        6        7        8        9
    CAP_PUP, CAP_PUP, CAP_RSV, CAP_VDD, CAP_RSV, CAP_PUP, CAP_PUP, CAP_RSV, CAP_PUP, CAP_PUP,   // Pins 0-9
    CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV,   // Pins 10-19
    CAP_PDN, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_PUP, CAP_RSV, CAP_PUP, CAP_RSV, CAP_RSV,   // Pins 20-29
    CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_VDD, CAP_VSS, CAP_RSV, CAP_PUP,   // Pins 30-39
    CAP_PUP, CAP_VSS, CAP_PUP, CAP_PUP, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_PUP, CAP_PUP,   // Pins 40-49
    CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV,   // Pins 50-59
    CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV,   // Pins 60-69
    CAP_RSV, CAP_RSV, CAP_RSV, CAP_RSV                                                          // Pins 70-73
};

//*****************************************************************************
//
// g_ui8nCEpins[]
// This lookup table lists the nCE funcsel value as a function of the pin.
//  Almost every pad has a nCE function.  Every one of those nCE functions
// can select a polarity (active low or high) via the INTD field.
// All non-nCE functions use INCFG and INTD to select interrupt transition types.
// A lookup will return 0-7 if the pin supports nCE, and 8 if it does not.
//
// The truth table summarizes behavior.  For the purposes of this table, assume
//  "A" is the funcsel that selects nCE (and thus polarity is needed) for the
//  given pad.  Then "!A" is any other funcsel and selects interrupt transition.
//
//  funcsel     INCFG       INTD        Behavior
//    !A        0           0           Interrupt on L->H transition.
//    !A        0           1           Interrupt on H->L transition.
//    !A        1           0           No interrupts.
//    !A        1           1           Interrupt either direction.
//     A        x           0           nCE polarity active low.
//     A        x           1           nCE polarity active high.
//
//*****************************************************************************
static const uint8_t
g_ui8nCEpins[AM_HAL_GPIO_MAX_PADS] =
{
    // 0     1     2     3     4     5     6     7     8     9
    0x07, 0x07, 0x07, 0x02, 0x02, 0x08, 0x08, 0x00, 0x02, 0x02,     // Pads 0-9
    0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,     // Pads 10-19
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,     // Pads 20-29
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x08,     // Pads 30-39
    0x08, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,     // Pads 40-49
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,     // Pads 50-59
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,     // Pads 60-69
    0x01, 0x01, 0x01, 0x01                                          // Pads 70-73
};

//*****************************************************************************
//
// g_ui8NCEtable[]
// This lookup table lists all available NCEs. It basically reproduces the
//  "NCE Encoding Table" from the datasheet.
//
// The format of this table is:
//  [7:7] IOM=1, MSPI=0
//  [6:4] NCE number (0-3)
//  [3:0] IOM number (0-5) or MSPI number (0-2)
//  Every 4 bytes (1 word) of the table represent an index to the next GPIO pin.
//
//*****************************************************************************
//
// In NCETBL entries, either IOM or MSPI should be 0.
//
#define NCETBL(IOM, NUM, NCE)      ((IOM << 7) | (NCE << 4) | (NUM << 0))

static const uint8_t
g_ui8NCEtable[AM_HAL_GPIO_MAX_PADS][4] =
{
     // 0       1       2       3    = OUTCFG
    {NCETBL(1, 3, 2), NCETBL(1, 4, 2), NCETBL(0, 2, 1), NCETBL(0, 1, 0)},   // NCE0
    {NCETBL(1, 0, 2), NCETBL(1, 1, 2), NCETBL(1, 2, 2), NCETBL(1, 4, 2)},   // NCE1
    {NCETBL(1, 3, 3), NCETBL(0, 2, 0), NCETBL(1, 5, 3), NCETBL(1, 2, 1)},   // NCE2
    {NCETBL(1, 3, 0), NCETBL(1, 4, 0), NCETBL(1, 5, 0), NCETBL(1, 2, 0)},   // NCE3
    {NCETBL(1, 3, 1), NCETBL(1, 4, 1), NCETBL(0, 1, 0), NCETBL(1, 1, 1)},   // NCE4
    {0xFF,          0xFF,          0xFF,          0xFF},                // NCE5
    {0xFF,          0xFF,          0xFF,          0xFF},                // NCE6
    {NCETBL(0, 1, 0), NCETBL(0, 2, 1), NCETBL(1, 5, 1), NCETBL(0, 0, 0)},   // NCE7
    {NCETBL(1, 3, 0), NCETBL(1, 4, 0), NCETBL(0, 2, 0), NCETBL(1, 0, 0)},   // NCE8
    {NCETBL(0, 1, 0), NCETBL(1, 4, 3), NCETBL(1, 5, 3), NCETBL(1, 2, 3)},   // NCE9
    {NCETBL(1, 3, 2), NCETBL(1, 4, 2), NCETBL(0, 1, 1), NCETBL(0, 0, 0)},   // NCE10
    {NCETBL(1, 0, 0), NCETBL(1, 1, 0), NCETBL(1, 2, 0), NCETBL(1, 3, 0)},   // NCE11
    {NCETBL(1, 3, 0), NCETBL(0, 2, 0), NCETBL(1, 5, 0), NCETBL(0, 0, 1)},   // NCE12
    {NCETBL(1, 3, 1), NCETBL(1, 4, 1), NCETBL(1, 5, 1), NCETBL(1, 0, 1)},   // NCE13
    {NCETBL(1, 0, 2), NCETBL(0, 2, 1), NCETBL(1, 2, 2), NCETBL(1, 4, 2)},   // NCE14
    {NCETBL(0, 1, 0), NCETBL(1, 1, 3), NCETBL(1, 2, 3), NCETBL(0, 0, 0)},   // NCE15
    {NCETBL(1, 0, 0), NCETBL(0, 2, 0), NCETBL(1, 2, 3), NCETBL(1, 5, 0)},   // NCE16
    {NCETBL(1, 0, 1), NCETBL(1, 1, 1), NCETBL(0, 2, 0), NCETBL(1, 4, 1)},   // NCE17
    {NCETBL(0, 2, 0), NCETBL(1, 1, 2), NCETBL(1, 2, 2), NCETBL(1, 3, 2)},   // NCE18
    {NCETBL(1, 0, 3), NCETBL(0, 1, 1), NCETBL(1, 3, 3), NCETBL(0, 0, 0)},   // NCE19
    {NCETBL(0, 2, 1), NCETBL(0, 0, 0), NCETBL(1, 5, 1), NCETBL(1, 2, 1)},   // NCE20
    {NCETBL(1, 3, 2), NCETBL(1, 4, 2), NCETBL(0, 2, 0), NCETBL(0, 1, 0)},   // NCE21
    {NCETBL(1, 3, 3), NCETBL(1, 4, 3), NCETBL(1, 5, 3), NCETBL(0, 2, 0)},   // NCE22
    {NCETBL(1, 0, 0), NCETBL(1, 1, 0), NCETBL(1, 2, 0), NCETBL(1, 4, 0)},   // NCE23
    {NCETBL(1, 0, 1), NCETBL(0, 1, 0), NCETBL(1, 2, 1), NCETBL(1, 5, 1)},   // NCE24
    {NCETBL(1, 3, 2), NCETBL(0, 1, 0), NCETBL(1, 5, 2), NCETBL(1, 0, 2)},   // NCE25
    {NCETBL(1, 3, 3), NCETBL(1, 4, 3), NCETBL(1, 5, 3), NCETBL(1, 1, 3)},   // NCE26
    {NCETBL(0, 2, 0), NCETBL(1, 4, 0), NCETBL(0, 0, 0), NCETBL(1, 1, 0)},   // NCE27
    {NCETBL(1, 3, 1), NCETBL(1, 4, 1), NCETBL(1, 5, 1), NCETBL(0, 0, 0)},   // NCE28
    {NCETBL(1, 3, 2), NCETBL(0, 2, 1), NCETBL(1, 5, 2), NCETBL(1, 1, 2)},   // NCE29
    {NCETBL(0, 0, 0), NCETBL(1, 4, 3), NCETBL(0, 2, 0), NCETBL(1, 0, 3)},   // NCE30
    {NCETBL(0, 1, 0), NCETBL(0, 2, 0), NCETBL(1, 2, 0), NCETBL(1, 4, 0)},   // NCE31
    {NCETBL(1, 0, 1), NCETBL(1, 1, 3), NCETBL(0, 1, 0), NCETBL(0, 0, 1)},   // NCE32
    {NCETBL(1, 0, 2), NCETBL(0, 1, 1), NCETBL(1, 2, 1), NCETBL(1, 5, 2)},   // NCE33
    {NCETBL(1, 0, 3), NCETBL(0, 2, 0), NCETBL(1, 2, 3), NCETBL(0, 1, 1)},   // NCE34
    {NCETBL(0, 1, 0), NCETBL(1, 1, 0), NCETBL(1, 2, 0), NCETBL(0, 2, 0)},   // NCE35
    {NCETBL(1, 3, 1), NCETBL(1, 4, 1), NCETBL(1, 5, 1), NCETBL(0, 0, 1)},   // NCE36
    {NCETBL(1, 3, 1), NCETBL(1, 4, 2), NCETBL(1, 5, 2), NCETBL(0, 0, 0)},   // NCE37
    {NCETBL(1, 0, 3), NCETBL(1, 1, 3), NCETBL(0, 2, 1), NCETBL(1, 5, 3)},   // NCE38
    {0xFF,          0xFF,          0xFF,          0xFF},                // NCE39
    {0xFF,          0xFF,          0xFF,          0xFF},                // NCE40
    {NCETBL(0,2,0), NCETBL(1,1,1), NCETBL(0,1,1), NCETBL(0,0,1)},       // NCE41
    {NCETBL(1,0,0), NCETBL(0,1,0), NCETBL(1,2,0), NCETBL(1,5,0)},       // NCE42
    {NCETBL(1,0,1), NCETBL(1,1,1), NCETBL(0,2,0), NCETBL(0,0,1)},       // NCE43
    {NCETBL(1,0,2), NCETBL(0,0,0), NCETBL(1,2,2), NCETBL(1,5,2)},       // NCE44
    {NCETBL(1,3,3), NCETBL(1,4,3), NCETBL(1,5,0), NCETBL(1,1,3)},       // NCE45
    {NCETBL(1,3,0), NCETBL(0,1,0), NCETBL(0,2,0), NCETBL(0,0,1)},       // NCE46
    {NCETBL(0,1,1), NCETBL(1,1,0), NCETBL(1,2,1), NCETBL(1,3,1)},       // NCE47
    {NCETBL(0,1,0), NCETBL(1,1,2), NCETBL(1,2,2), NCETBL(0,2,1)},       // NCE48
    {NCETBL(1,0,3), NCETBL(0,1,1), NCETBL(1,2,3), NCETBL(1,1,0)},       // NCE49
    {NCETBL(0,1,0), NCETBL(0,2,0), NCETBL(1,2,3), NCETBL(1,0,0)},       // NCE50
    {NCETBL(1,4,2), NCETBL(1,1,1), NCETBL(1,2,1), NCETBL(1,0,0)},       // NCE51
    {NCETBL(1,0,1), NCETBL(1,1,2), NCETBL(0,0,0), NCETBL(1,2,1)},       // NCE52
    {NCETBL(1,0,2), NCETBL(1,1,3), NCETBL(0,2,0), NCETBL(1,2,0)},       // NCE53
    {NCETBL(0,2,0), NCETBL(1,4,0), NCETBL(1,5,0), NCETBL(1,3,0)},       // NCE54
    {NCETBL(1,3,0), NCETBL(0,0,0), NCETBL(1,5,1), NCETBL(1,4,0)},       // NCE55
    {NCETBL(1,3,2), NCETBL(1,4,3), NCETBL(1,5,2), NCETBL(0,0,1)},       // NCE56
    {NCETBL(0,0,0), NCETBL(1,4,3), NCETBL(1,5,3), NCETBL(1,0,1)},       // NCE57
    {NCETBL(1,3,3), NCETBL(1,1,0), NCETBL(0,0,0), NCETBL(1,5,3)},       // NCE58
    {NCETBL(0,1,1), NCETBL(1,1,1), NCETBL(1,2,1), NCETBL(0,0,0)},       // NCE59
    {NCETBL(0,0,1), NCETBL(1,1,2), NCETBL(1,2,2), NCETBL(1,0,3)},       // NCE60
    {NCETBL(0,0,0), NCETBL(1,1,3), NCETBL(0,2,0), NCETBL(0,1,0)},       // NCE61
    {NCETBL(0,0,1), NCETBL(1,4,0), NCETBL(1,5,0), NCETBL(0,1,1)},       // NCE62
    {NCETBL(0,1,0), NCETBL(1,4,1), NCETBL(0,2,0), NCETBL(0,0,0)},       // NCE63
    {NCETBL(1,0,0), NCETBL(1,4,2), NCETBL(1,5,2), NCETBL(0,0,1)},       // NCE64
    {NCETBL(1,0,1), NCETBL(0,0,0), NCETBL(1,5,2), NCETBL(1,3,1)},       // NCE65
    {NCETBL(0,1,0), NCETBL(1,1,0), NCETBL(1,2,0), NCETBL(1,4,1)},       // NCE66
    {NCETBL(1,0,3), NCETBL(1,1,1), NCETBL(1,2,3), NCETBL(1,5,1)},       // NCE67
    {NCETBL(1,3,0), NCETBL(0,0,0), NCETBL(1,2,2), NCETBL(1,0,2)},       // NCE68
    {NCETBL(1,3,1), NCETBL(1,1,3), NCETBL(0,0,0), NCETBL(0,1,0)},       // NCE69
    {NCETBL(1,3,2), NCETBL(0,2,0), NCETBL(1,5,0), NCETBL(0,1,1)},       // NCE70
    {NCETBL(1,3,3), NCETBL(1,4,1), NCETBL(1,0,3), NCETBL(1,1,2)},       // NCE71
    {NCETBL(0,0,0), NCETBL(0,2,0), NCETBL(0,1,0), NCETBL(1,2,2)},       // NCE72
    {NCETBL(0,1,0), NCETBL(1,4,3), NCETBL(1,5,3), NCETBL(0,2,0)},       // NCE73
};

//*****************************************************************************
//
// Array of function pointers for handling GPIO interrupts.
//
//*****************************************************************************
static am_hal_gpio_handler_t gpio_ppfnHandlers[AM_HAL_GPIO_MAX_PADS];
static void                  *gpio_pHandlerCtxt[AM_HAL_GPIO_MAX_PADS];

//*****************************************************************************
//
// Helper functions
//  popcount()   - Determine how many bits are set in the given bitmasks.
//  pincfg_equ() - compare 2 am_hal_gpio_pincfg_t structures for equality.
//
//*****************************************************************************
//*****************************************************************************
// @brief compare 32bit values at two pointers
//
// @param cfg1
// @param cfg2
// @return true
// @return false
//*****************************************************************************
static bool
pincfg_equ(void *cfg1, void *cfg2)
{
    uint32_t ui32A, ui32B;

    //
    // We're assuming that am_hal_gpio_pincfg_t boils down to a uint32_t,
    // which is its intent.
    //
    ui32A = *((uint32_t*)cfg1);
    ui32B = *((uint32_t*)cfg2);

    return ui32A == ui32B ? true : false;

} // pincfg_equ()

//*****************************************************************************
// @brief Count the number of set bits in the given bitmasks
//
// @param pui32bitmask
// @param i32numbits
// @return uint32_t
//*****************************************************************************
static uint32_t
popcount(uint32_t *pui32bitmask, int32_t i32numbits)
{
    uint32_t uCnt = 0;
    uint32_t ui32bm;

    // Count the number of set bits in the given bitmasks.
    uCnt = 0;
    while ( i32numbits > 0 )
    {
        ui32bm  = *pui32bitmask++;
        ui32bm &= (((uint32_t)1 << i32numbits) - 1);

        //
        // This loop will efficiently determine the number of set bits in ui32bm
        //
        while ( ui32bm )
        {
            ui32bm &= (ui32bm - 1);
            uCnt++;
        }
        i32numbits -= 32;
    }
    return uCnt;
} // popcount()

//*****************************************************************************
//
// @brief Configure an Apollo3 pin.
//
// @param ui32Pin    - pin number to be configured.
// @param bfGpioCfg - Contains multiple descriptor fields.
//
// This function configures a pin according to the parameters in ui32Config.
// All parameters are validated, and the given pin is configured according
// to the designated parameters.
//
// @return Status.
//
//*****************************************************************************
uint32_t
am_hal_gpio_pinconfig(uint32_t ui32Pin, am_hal_gpio_pincfg_t bfGpioCfg)

{
    uint32_t ui32Padreg, ui32AltPadCfg, ui32GPCfg;
    uint32_t ui32Funcsel, ui32PowerSw;
    bool bClearEnable = false;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( ui32Pin >= AM_HAL_GPIO_MAX_PADS )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Initialize the PADREG accumulator variables.
    //
    ui32GPCfg = ui32Padreg = ui32AltPadCfg = 0;

    //
    // Get the requested function and/or power switch.
    //
    ui32Funcsel = bfGpioCfg.uFuncSel;
    ui32PowerSw = bfGpioCfg.ePowerSw;

    ui32Padreg |= ui32Funcsel << PADREG_FLD_FNSEL_S;

    //
    // Check for invalid configuration requests.
    //
    if ( bfGpioCfg.ePullup != AM_HAL_GPIO_PIN_PULLUP_NONE )
    {
        //
        // This setting is needed for all pullup settings including
        // AM_HAL_GPIO_PIN_PULLUP_WEAK and AM_HAL_GPIO_PIN_PULLDOWN.
        //
        ui32Padreg |= (0x1 << PADREG_FLD_PULLUP_S);

        //
        // Check for specific pullup or pulldown settings.
        //
        if ( (bfGpioCfg.ePullup >= AM_HAL_GPIO_PIN_PULLUP_1_5K) &&
             (bfGpioCfg.ePullup <= AM_HAL_GPIO_PIN_PULLUP_24K) )
        {
            ui32Padreg |= ((bfGpioCfg.ePullup - AM_HAL_GPIO_PIN_PULLUP_1_5K) <<
                           PADREG_FLD_76_S);
#ifndef AM_HAL_DISABLE_API_VALIDATION
            if ( !(g_ui8Bit76Capabilities[ui32Pin] & CAP_PUP) )
            {
                return AM_HAL_GPIO_ERR_PULLUP;
            }
        }
        else if ( bfGpioCfg.ePullup == AM_HAL_GPIO_PIN_PULLDOWN )
        {
            if ( ui32Pin != 20 )
            {
                return AM_HAL_GPIO_ERR_PULLDOWN;
            }
        }
        else if ( bfGpioCfg.ePullup == AM_HAL_GPIO_PIN_PULLUP_WEAK )
        {
            //
            // All pads except 20 support a weak pullup, for which we only need
            // to set PADnPULL and clear 7:6 (already done at this point).
            //
            if ( ui32Pin == 20 )
            {
                return AM_HAL_GPIO_ERR_PULLUP;
            }
#endif // AM_HAL_DISABLE_API_VALIDATION
        }
    }

    //
    // Check if requesting a power switch pin
    //
    if ( ui32PowerSw != AM_HAL_GPIO_PIN_POWERSW_NONE )
    {
        if ( (ui32PowerSw == AM_HAL_GPIO_PIN_POWERSW_VDD)  &&
             (g_ui8Bit76Capabilities[ui32Pin] & CAP_VDD) )
        {
            ui32Padreg |= 0x1 << PADREG_FLD_76_S;
        }
        else if ( (ui32PowerSw == AM_HAL_GPIO_PIN_POWERSW_VSS)  &&
                  (g_ui8Bit76Capabilities[ui32Pin] & CAP_VSS) )
        {
            ui32Padreg |= 0x2 << PADREG_FLD_76_S;
        }
        else
        {
            return AM_HAL_GPIO_ERR_PWRSW;
        }
    }

    //
    // Depending on the selected pin and FNSEL, determine if INPEN needs to be set.
    //
    ui32Padreg |= (g_ui8Inpen[ui32Pin] & (1 << ui32Funcsel)) ? (1 << PADREG_FLD_INPEN_S) : 0;

    //
    // Configure ui32GpCfg based on whether nCE requested.
    //
    if ( g_ui8nCEpins[ui32Pin] == ui32Funcsel )
    {
        uint32_t ui32Outcfg;
        uint8_t ui8CEtbl;

        //
        // User is configuring a nCE. Verify the requested settings and set the
        // polarity and OUTCFG values (INCFG is not used here and should be 0).
        // Valid uNCE values are 0-3 (uNCE is a 2-bit field).
        // Valid uIOMnum are 0-5 (0-5 for IOMs, 0-2 for MSPI).
        //
#ifndef AM_HAL_DISABLE_API_VALIDATION
        //
        // Optionally validate the settings.
        //
        if ( bfGpioCfg.uNCE >= 4 )
        {
            return AM_HAL_GPIO_ERR_INVCE;   // Invalid CE specified
        }

        if ( (bfGpioCfg.bIomMSPIn)  &&
             (bfGpioCfg.uIOMnum >= AM_REG_IOM_NUM_MODULES) )
        {
            return AM_HAL_GPIO_ERR_INVCE;   // Invalid CE specified
        }
        else if ( (!bfGpioCfg.bIomMSPIn)  &&
                  (bfGpioCfg.uIOMnum >= AM_REG_MSPI_NUM_MODULES) )
        {
            return AM_HAL_GPIO_ERR_INVCE;   // Invalid CE specified
        }
#endif // AM_HAL_DISABLE_API_VALIDATION

        //
        // Construct the entry we expect to find in the table. We can determine
        // the OUTCFG value by looking for that value in the pin row.
        //
        ui8CEtbl = NCETBL(bfGpioCfg.bIomMSPIn, bfGpioCfg.uIOMnum, bfGpioCfg.uNCE);

        //
        // Now search the NCE table for a match.
        //
        for ( ui32Outcfg = 0; ui32Outcfg < 4; ui32Outcfg++ )
        {
            if ( g_ui8NCEtable[ui32Pin][ui32Outcfg] == ui8CEtbl )
            {
                break;
            }
        }

#ifndef AM_HAL_DISABLE_API_VALIDATION
        if ( ui32Outcfg >= 4 )
        {
            return AM_HAL_GPIO_ERR_INVCEPIN;
        }
#endif // AM_HAL_DISABLE_API_VALIDATION

        ui32GPCfg |= (ui32Outcfg       << GPIOCFG_FLD_OUTCFG_S) |
                     (bfGpioCfg.eCEpol << GPIOCFG_FLD_INTD_S)   |
                     (0                << GPIOCFG_FLD_INCFG_S);
    }
    else
    {
        //
        // It's not nCE, it's one of the other funcsels.
        // Start by setting the value of the requested GPIO input.
        //
        ui32Padreg |= (bfGpioCfg.eGPInput << PADREG_FLD_INPEN_S);

        //
        // Map the requested interrupt direction settings into the Apollo3
        //  GPIOCFG register field, which is a 4-bit field:
        //  [INTD(1):OUTCFG(2):INCFG(1)].
        // Bit0 of eIntDir maps to GPIOCFG.INTD  (b3).
        // Bit1 of eIntDir maps to GPIOCFG.INCFG (b0).
        //
        ui32GPCfg |= (bfGpioCfg.eGPOutcfg << GPIOCFG_FLD_OUTCFG_S)              |
                     (((bfGpioCfg.eIntDir >> 0) & 0x1) << GPIOCFG_FLD_INTD_S)   |
                     (((bfGpioCfg.eIntDir >> 1) & 0x1) << GPIOCFG_FLD_INCFG_S);

        if ( (bfGpioCfg.eGPOutcfg == AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL) ||
             pincfg_equ(&bfGpioCfg, (void*)&g_AM_HAL_GPIO_DISABLE) )
        {
            //
            // For pushpull configurations, we must be sure to clear the ENABLE
            // bit.  In pushpull, these bits turn on FAST GPIO.  For regular
            // GPIO, they must be clear.
            //
            bClearEnable = true;
        }

        //
        // There is some overlap between eGPRdZero and eIntDir as both settings
        //  utilize the overloaded INCFG bit.
        // Therefore the two fields should be used in a mutually exclusive
        //  manner. For flexibility however they are not disallowed because
        //  their functionality is dependent on FUNCSEL and whether interrupts
        //  are used.
        //
        // In the vein of mutual exclusion, eGPRdZero is primarily intended for
        //  use when GPIO interrupts are not in use and can be used when no
        //  eIntDir setting is provided.
        // If eIntDir is provided, eGPRdZero is ignored and can only be
        //  achieved via the AM_HAL_GPIO_PIN_INTDIR_NONE setting.
        //
        if ( bfGpioCfg.eIntDir == 0 )
        {
            ui32GPCfg &= ~(1 << GPIOCFG_FLD_INCFG_S);
            ui32GPCfg |= (bfGpioCfg.eGPRdZero << GPIOCFG_FLD_INCFG_S);
        }
    }

    switch ( bfGpioCfg.eDriveStrength )
    {
        // DRIVESTRENGTH is a 2-bit field.
        //  bit0 maps to bit2 of a PADREG field.
        //  bit1 maps to bit0 of an ALTPADCFG field.
        case AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA:
            ui32Padreg    |= (0 << PADREG_FLD_DRVSTR_S);
            ui32AltPadCfg |= (0 << 0);
            break;
        case AM_HAL_GPIO_PIN_DRIVESTRENGTH_4MA:
            ui32Padreg    |= (1 << PADREG_FLD_DRVSTR_S);
            ui32AltPadCfg |= (0 << 0);
            break;
        case AM_HAL_GPIO_PIN_DRIVESTRENGTH_8MA:
            ui32Padreg    |= (0 << PADREG_FLD_DRVSTR_S);
            ui32AltPadCfg |= (1 << 0);
            break;
        case AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA:
            ui32Padreg    |= (1 << PADREG_FLD_DRVSTR_S);
            ui32AltPadCfg |= (1 << 0);
            break;
    }

    //
    // At this point, the 3 configuration variables, ui32GPCfg, ui32Padreg,
    //  and ui32AltPadCfg values are set (at bit position 0) and ready to write
    //  to their respective register bitfields.
    //
    uint32_t ui32GPCfgAddr, ui32PadregAddr, ui32AltpadAddr;
    uint32_t ui32GPCfgClearMask, ui32PadClearMask;
    uint32_t ui32GPCfgShft, ui32PadShft;

    ui32GPCfgAddr       = AM_REGADDR(GPIO, CFGA)       + ((ui32Pin >> 1) & ~0x3);
    ui32PadregAddr      = AM_REGADDR(GPIO, PADREGA)    + (ui32Pin & ~0x3);
    ui32AltpadAddr      = AM_REGADDR(GPIO, ALTPADCFGA) + (ui32Pin & ~0x3);

    ui32GPCfgShft       = ((ui32Pin & 0x7) << 2);
    ui32PadShft         = ((ui32Pin & 0x3) << 3);
    ui32GPCfgClearMask  = ~((uint32_t)0xF  << ui32GPCfgShft);
    ui32PadClearMask    = ~((uint32_t)0xFF << ui32PadShft);

    //
    // Get the new values into their rightful bit positions.
    //
    ui32Padreg    <<= ui32PadShft;
    ui32AltPadCfg <<= ui32PadShft;
    ui32GPCfg     <<= ui32GPCfgShft;

    AM_CRITICAL_BEGIN

    if ( bClearEnable )
    {
        //
        // We're configuring a mode that requires clearing the Enable bit.
        //
        am_hal_gpio_output_tristate_disable(ui32Pin);
    }

    GPIO->PADKEY = GPIO_PADKEY_PADKEY_Key;

    AM_REGVAL(ui32PadregAddr)  = (AM_REGVAL(ui32PadregAddr) & ui32PadClearMask)   | ui32Padreg;
    AM_REGVAL(ui32GPCfgAddr)   = (AM_REGVAL(ui32GPCfgAddr)  & ui32GPCfgClearMask) | ui32GPCfg;
    AM_REGVAL(ui32AltpadAddr)  = (AM_REGVAL(ui32AltpadAddr) & ui32PadClearMask)   | ui32AltPadCfg;

    GPIO->PADKEY = 0;

    AM_CRITICAL_END

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_pinconfig()

//*****************************************************************************
//
// @brief Configure specified pins for FAST GPIO operation.
//
// @param psPinMask - a mask specifying up to 8 pins to be configured and
//               used for FAST GPIO (only bits 0-49 are valid).
// @param bfGpioCfg   - The GPIO configuration (same as am_hal_gpio_pinconfig()).
//               All of the pins specified by ui64PinMask will be set to this
//               configuration.
// @param ui32Masks   - If provided, an array to receive 2 32-bit values of the
//               SET and CLEAR masks that are used for the BBSETCLEAR reg.
//               Two 32-bit wds are placed for each pin indicated by the mask.
//               The 2 32-bit values will be placed at incremental indexes.
//               For example, say pin numbers 5 and 19 are indicated in the
//               mask, and an array pointer is provided in ui32Masks.  This
//               array must be allocated by the caller to be at least 4 words.
//               ui32Masks[0] = the set   mask used for pin 5.
//               ui32Masks[1] = the clear mask used for pin 5.
//               ui32Masks[2] = the set   mask used for pin 19.
//               ui32Masks[3] = the clear mask used for pin 19.
//               It is recommended that this array be allocated to 16 uint32_t.
//
// @return       Standard System Status
//
//*****************************************************************************
uint32_t
am_hal_gpio_fast_pinconfig(am_hal_gpio_mask_t *psPinMask,
                           am_hal_gpio_pincfg_t bfGpioCfg,
                           uint32_t ui32Masks[])
{
    uint32_t ux, ui32pinnum, ui32retval, ui32Mask, ui32PinMask;
    uint32_t ui32WdIdx;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( psPinMask->U.Msk[AM_HAL_GPIO_NUMWORDS - 1] &
         ~(((uint32_t)1 << (AM_HAL_GPIO_MAX_PADS - 64)) - 1)                    ||
         (popcount((uint32_t*)&psPinMask->U.Msk[0], AM_HAL_GPIO_MAX_PADS) > 8)  ||
         (bfGpioCfg.eGPOutcfg == AM_HAL_GPIO_PIN_OUTCFG_TRISTATE) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Roll through the pin mask and configure any designated pins per the
    // bfGpioCfg parameter, and enable for Fast GPIO.
    //
    ux = 0;
    ui32pinnum = 0;
    ui32WdIdx = 0;
    while ( ui32pinnum < AM_HAL_GPIO_MAX_PADS )
    {
        ui32PinMask = psPinMask->U.Msk[ui32WdIdx++];
        ui32Mask = 0;

        while ( ui32PinMask )
        {
            if ( ui32PinMask & 0x1 )
            {
                //
                // It is assumed that the caller will have disabled Fast GPIO and
                // initialized the pin value before calling this function. Therefore
                // no value initialization is done before the pin configuration, nor
                // is the am_hal_gpio_fastgpio_disable() called here.
                //
                // Configure the pin.
                //
                ui32retval = am_hal_gpio_pinconfig(ui32pinnum, bfGpioCfg);
                if ( ui32retval )
                {
                    return ui32retval;
                }

                ui32Mask |= 1 << (ui32pinnum & 0x7);

                //
                // Enable the FAST GPIO for this pin
                //
                am_hal_gpio_fastgpio_enable(ui32pinnum);

                if ( ui32Masks )
                {
                    ui32Masks[ux + 0] = _VAL2FLD(APBDMA_BBSETCLEAR_SET,   ui32Mask);
                    ui32Masks[ux + 1] = _VAL2FLD(APBDMA_BBSETCLEAR_CLEAR, ui32Mask);
                }
                ux += 2;    // Get next indexes
            }
            ui32pinnum++;
            ui32PinMask >>= 1;
        }
        ui32pinnum &= ~31;
        ui32pinnum += 32;
    }

    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_fast_pinconfig()

//*****************************************************************************
//
// @brief Read GPIO.
//
// @param ui32Pin    - pin number to be read.
// @param eReadType  - State type to read.  One of:
//     AM_HAL_GPIO_INPUT_READ
//     AM_HAL_GPIO_OUTPUT_READ
//     AM_HAL_GPIO_ENABLE_READ
// @param pui32ReadState - Pointer to the value to contain the read state.
//        When reading the value of a bit, will be either 0 or 1.
//
// This function reads a pin state as given by ui32Type.
//
// @return Status.
//
// @note This function is intended for use only when the pin is configured as GPIO.
//
//*****************************************************************************
uint32_t
am_hal_gpio_state_read(uint32_t ui32Pin,
                       am_hal_gpio_read_type_e eReadType,
                       uint32_t *pui32ReadState)
{
    uint32_t ui32ReadValue = 0xFFFFFFFF;
    uint32_t ui32BaseAddr, ui32Shift;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( pui32ReadState == NULL )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if ( ui32Pin >= AM_HAL_GPIO_MAX_PADS )
    {
        *pui32ReadState = ui32ReadValue;
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

#if 0   // By default, disable this additional validation check as it is very time consuming.
    //
    // Validate that the pin is configured for GPIO. Return error if not.
    //
    uint32_t ui32Regval, ui32PadShft, ui32FncselMsk;
    ui32Regval    = AM_REGVAL( AM_REGADDR(GPIO, PADREGA) + (ui32Pin & ~0x3) );
    ui32PadShft   = ((ui32Pin & 0x3) << 3);
    ui32FncselMsk = (uint32_t)0x38 << ui32PadShft;
    if ( (ui32Regval & ui32FncselMsk) != (0x3 << (ui32PadShft + 3)) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Compute base address + offset of 0 or 4.
    //
    ui32BaseAddr = ((ui32Pin & 0x60) >> 3);   // 0, 4, or 8
    ui32Shift    = ui32Pin & 0x1F;

    switch ( eReadType )
    {
        case AM_HAL_GPIO_INPUT_READ:
            //
            // Assumes eIntDir != AM_HAL_GPIO_PIN_INTDIR_NONE   &&
            //         eIntDir != AM_HAL_GPIO_PIN_INTDIR_BOTH
            // If either of those configs are set, returns 0.
            //
            ui32ReadValue = AM_REGVAL(AM_REGADDR(GPIO, RDA) + ui32BaseAddr);
            ui32ReadValue = (ui32ReadValue >> ui32Shift) & 0x01;
            break;
        case AM_HAL_GPIO_OUTPUT_READ:
            ui32ReadValue = AM_REGVAL(AM_REGADDR(GPIO, WTA) + ui32BaseAddr);
            ui32ReadValue = (ui32ReadValue >> ui32Shift) & 0x01;
            break;
        case AM_HAL_GPIO_ENABLE_READ:
            ui32ReadValue = AM_REGVAL(AM_REGADDR(GPIO, ENA) + ui32BaseAddr);
            ui32ReadValue = (ui32ReadValue >> ui32Shift) & 0x01;
            break;
        default:
            return AM_HAL_STATUS_INVALID_ARG;
    }

    *pui32ReadState = ui32ReadValue;

    return AM_HAL_STATUS_SUCCESS;
} // am_hal_gpio_state_read()

//*****************************************************************************
//
// @brief Write GPIO.
//
// @param ui32Pin    - pin number to be written.
//
// @param eWriteType   - State type to write.  One of:
//     AM_HAL_GPIO_OUTPUT_SET              - Write a one to a GPIO.
//     AM_HAL_GPIO_OUTPUT_CLEAR            - Write a zero to a GPIO.
//     AM_HAL_GPIO_OUTPUT_TOGGLE           - Toggle the GPIO value.
//     The following two apply when output is set for TriState (OUTCFG==3).
//     AM_HAL_GPIO_OUTPUT_TRISTATE_ENABLE  - Enable  a tri-state GPIO.
//     AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE - Disable a tri-state GPIO.
//
// This function writes a GPIO value.
//
// @return Status.
//
// This function is intended for use only when the pin is configured as GPIO.
//
//*****************************************************************************
uint32_t
am_hal_gpio_state_write(uint32_t ui32Pin, am_hal_gpio_write_type_e eWriteType)
{
    uint32_t ui32Mask, ui32Off;
    uint32_t ui32Return = AM_HAL_STATUS_SUCCESS;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( ui32Pin >= AM_HAL_GPIO_MAX_PADS )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    if ( eWriteType > AM_HAL_GPIO_OUTPUT_TRISTATE_TOGGLE )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

#if 0   // By default, disable this additional validation check as it is very time consuming.
    //
    // Validate that the pin is configured for GPIO. Return error if not.
    //
    uint32_t ui32Regval, ui32PadShft, ui32FncselMsk;
    ui32Regval    = AM_REGVAL( AM_REGADDR(GPIO, PADREGA) + (ui32Pin & ~0x3) );
    ui32PadShft   = ((ui32Pin & 0x3) << 3);
    ui32FncselMsk = (uint32_t)0x38 << ui32PadShft;
    if ( (ui32Regval & ui32FncselMsk) != (0x3 << (ui32PadShft + 3)) )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif
#endif // AM_HAL_DISABLE_API_VALIDATION

    ui32Mask = (uint32_t)0x1 << (ui32Pin % 32);
    ui32Off  = (ui32Pin & 0x60) >> 3;   // 0, 4 or 8

    AM_CRITICAL_BEGIN;
    switch ( eWriteType )
    {
        case AM_HAL_GPIO_OUTPUT_SET:                // Write a one to a GPIO.
            AM_REGVAL(AM_REGADDR(GPIO, WTSA) + ui32Off) = ui32Mask;
            break;
        case AM_HAL_GPIO_OUTPUT_CLEAR:              // Write a zero to a GPIO.
            AM_REGVAL(AM_REGADDR(GPIO, WTCA) + ui32Off) = ui32Mask;
            break;
        case AM_HAL_GPIO_OUTPUT_TOGGLE:             // Toggle the GPIO value.
            AM_REGVAL(AM_REGADDR(GPIO, WTA) + ui32Off) ^= ui32Mask;
            break;
        case AM_HAL_GPIO_OUTPUT_TRISTATE_ENABLE:    // Enable  a tri-state GPIO.
            AM_REGVAL(AM_REGADDR(GPIO, ENSA) + ui32Off) = ui32Mask;
            break;
        case AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE:   // Disable a tri-state GPIO.
            AM_REGVAL(AM_REGADDR(GPIO, ENCA) + ui32Off) = ui32Mask;
            break;
        case AM_HAL_GPIO_OUTPUT_TRISTATE_TOGGLE:   // Toggle a tri-state GPIO.
            AM_REGVAL(AM_REGADDR(GPIO, ENCA) + ui32Off) ^= ui32Mask;
            break;
        default:
            // Type values were validated on entry.
            // We can't return from here because we're in a critical section.
            ui32Return = AM_HAL_STATUS_INVALID_ARG;
            break;
    }

    AM_CRITICAL_END;

    return ui32Return;
} // am_hal_gpio_state_write()

//*****************************************************************************
//
// Enable GPIO interrupts.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_enable(am_hal_gpio_mask_t *pGpioIntMask)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( pGpioIntMask->U.Msk[AM_HAL_GPIO_MAX_PADS / 32] &
         ~(((uint32_t)1 << (AM_HAL_GPIO_MAX_PADS % 32)) - 1) )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Enable the interrupts.
    //
    AM_CRITICAL_BEGIN

    DIAG_SUPPRESS_VOLATILE_ORDER()
    GPIO->INT0EN |= pGpioIntMask->U.Msk[0];
    GPIO->INT1EN |= pGpioIntMask->U.Msk[1];
    GPIO->INT2EN |= pGpioIntMask->U.Msk[2];
    DIAG_DEFAULT_VOLATILE_ORDER()

    AM_CRITICAL_END

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_interrupt_enable()

//*****************************************************************************
//
// Disable GPIO interrupts.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_disable(am_hal_gpio_mask_t *pGpioIntMask)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION

    if ( !pGpioIntMask )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if ( pGpioIntMask->U.Msk[AM_HAL_GPIO_MAX_PADS / 32] &
         ~(((uint32_t)1 << (AM_HAL_GPIO_MAX_PADS % 32)) - 1) )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Disable the interrupts.
    //
    AM_CRITICAL_BEGIN

    DIAG_SUPPRESS_VOLATILE_ORDER()
    GPIO->INT0EN &= ~pGpioIntMask->U.Msk[0];
    GPIO->INT1EN &= ~pGpioIntMask->U.Msk[1];
    GPIO->INT2EN &= ~pGpioIntMask->U.Msk[2];
    DIAG_DEFAULT_VOLATILE_ORDER()

    AM_CRITICAL_END

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_interrupt_disable()

//*****************************************************************************
//
// Clear GPIO interrupts.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_clear(am_hal_gpio_mask_t *pGpioIntMask)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( !pGpioIntMask )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if ( pGpioIntMask->U.Msk[AM_HAL_GPIO_MAX_PADS / 32] &
         ~(((uint32_t)1 << (AM_HAL_GPIO_MAX_PADS % 32)) - 1) )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Clear the interrupts.
    //
    AM_CRITICAL_BEGIN

    DIAG_SUPPRESS_VOLATILE_ORDER()
    GPIO->INT0CLR |= pGpioIntMask->U.Msk[0];
    GPIO->INT1CLR |= pGpioIntMask->U.Msk[1];
    GPIO->INT2CLR |= pGpioIntMask->U.Msk[2];
    DIAG_DEFAULT_VOLATILE_ORDER()

    AM_CRITICAL_END

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_interrupt_clear()

//*****************************************************************************
//
// Get GPIO interrupt status.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_status_get(bool bEnabledOnly,
                                 am_hal_gpio_mask_t *pGpioIntMask)
{
    volatile uint32_t ui32Mask[3];

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( !pGpioIntMask )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if ( pGpioIntMask->U.Msk[AM_HAL_GPIO_MAX_PADS / 32] &
         ~(((uint32_t)1 << (AM_HAL_GPIO_MAX_PADS % 32)) - 1) )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Initialize mask variable outside critical section
    //
    ui32Mask[0] = 0xFFFFFFFF;
    ui32Mask[1] = 0xFFFFFFFF;
    ui32Mask[2] = 0xFFFFFFFF;

    //
    // Combine upper or lower GPIO words into one 64 bit return value.
    //
    AM_CRITICAL_BEGIN

    DIAG_SUPPRESS_VOLATILE_ORDER()
    if ( bEnabledOnly )
    {
        ui32Mask[0] = GPIO->INT0EN;
        ui32Mask[1] = GPIO->INT1EN;
        ui32Mask[2] = GPIO->INT2EN;
    }

    pGpioIntMask->U.Msk[0] = GPIO->INT0STAT & ui32Mask[0];
    pGpioIntMask->U.Msk[1] = GPIO->INT1STAT & ui32Mask[1];
    pGpioIntMask->U.Msk[2] = GPIO->INT2STAT & ui32Mask[2];
    DIAG_DEFAULT_VOLATILE_ORDER()

    AM_CRITICAL_END

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_interrupt_status_get()

//*****************************************************************************
//
// GPIO interrupt service routine registration.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_register(uint32_t ui32GPIONumber,
                               am_hal_gpio_handler_t pfnHandler)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check parameters
    //
    if ( ui32GPIONumber >= AM_HAL_GPIO_MAX_PADS )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    if ( pfnHandler == NULL )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Store the handler function pointer.
    //
    gpio_ppfnHandlers[ui32GPIONumber] = pfnHandler;

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_interrupt_register()

//*****************************************************************************
//
// @brief Advanced GPIO interrupt service routine registration.
//
// @param ui32GPIONumber - GPIO number (0-73) to be registered.
// @param pfnHandler - Function pointer to the callback.
// @param pCtxt      - context for the callback.
//
// @return Status.
//         Fails if pfnHandler is NULL or ui32GPIONumber > 73.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_register_adv(uint32_t ui32GPIONumber,
                               am_hal_gpio_handler_adv_t pfnHandler, void *pCtxt)
{
#ifndef AM_HAL_DISABLE_API_VALIDATION
    //
    // Check parameters
    //
    if ( ui32GPIONumber >= AM_HAL_GPIO_MAX_PADS )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    if ( pfnHandler == NULL )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Store the handler function pointer.
    //
    gpio_ppfnHandlers[ui32GPIONumber] = (am_hal_gpio_handler_t)((uint32_t)pfnHandler & ~0x1);
    gpio_pHandlerCtxt[ui32GPIONumber] = pCtxt;

    //
    // Return the status.
    //
    return AM_HAL_STATUS_SUCCESS;

} // am_hal_gpio_interrupt_register_adv()

//*****************************************************************************
//
// GPIO interrupt service routine.
//
//*****************************************************************************
uint32_t
am_hal_gpio_interrupt_service(am_hal_gpio_mask_t *pGpioIntMaskStatus)
{
    uint32_t ui32RetStatus = AM_HAL_STATUS_SUCCESS;
    uint32_t ui32Status, ui32FFS, ui32Idx;
    am_hal_gpio_handler_t pfnHandler;
    uint32_t *pui32Status;

#ifndef AM_HAL_DISABLE_API_VALIDATION
    if ( !pGpioIntMaskStatus )
    {
        return AM_HAL_STATUS_INVALID_ARG;
    }

    if ( pGpioIntMaskStatus->U.Msk[AM_HAL_GPIO_MAX_PADS / 32] &
         ~(((uint32_t)1 << (AM_HAL_GPIO_MAX_PADS % 32)) - 1) )
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
#endif // AM_HAL_DISABLE_API_VALIDATION

    //
    // Convert the mask status structure to an uint32_t pointer.
    //
    pui32Status = (uint32_t*)pGpioIntMaskStatus;

    //
    // Handle interrupts.
    //
    uint32_t ui32regcnt = AM_HAL_GPIO_NUMWORDS;
    ui32Idx = 0;
    while ( ui32regcnt )
    {
        //
        // Get next status word from the caller.
        //
        ui32Status = *pui32Status++;

        while ( ui32Status )
        {
            //
            // We need to FFS (Find First Set).  We can easily zero-base FFS
            // since we know that at least 1 bit is set in ui32Status.
            // FFS(x) = 31 - clz(x & -x).       // Zero-based version of FFS.
            //
            ui32FFS = ui32Status & (uint32_t)(-(int32_t)ui32Status);
            ui32FFS = 31 - AM_ASM_CLZ(ui32FFS);

            //
            // Turn off the bit we picked in the working copy
            //
            ui32Status &= ~(0x00000001 << ui32FFS);

            //
            // Check the bit handler table to see if there is an interrupt handler
            // registered for this particular bit.
            //
            pfnHandler = gpio_ppfnHandlers[ui32Idx + ui32FFS];
            if ( pfnHandler )
            {
                //
                // If we found an interrupt handler routine, call it now.
                //
                if ((uint32_t)pfnHandler & 0x1)
                {
                    pfnHandler();
                }
                else
                {
                    am_hal_gpio_handler_adv_t padvHandler = (am_hal_gpio_handler_adv_t)((uint32_t)pfnHandler | 0x1);
                    padvHandler(gpio_pHandlerCtxt[ui32Idx + ui32FFS]);
                }
            }
            else
            {
                //
                // No handler was registered for the GPIO that interrupted.
                // Return an error.
                //
                ui32RetStatus = AM_HAL_STATUS_INVALID_OPERATION;
            }
        }
        --ui32regcnt;
        ui32Idx += 32;
    }

    //
    // Return the status.
    //
    return ui32RetStatus;

} // am_hal_gpio_interrupt_service()

//*****************************************************************************
//
//  am_hal_gpio_isinput()
//
//  Determine whether a pad is configured with input enable.
//  Returns true if input enable is set, false otherwise.
//
//*****************************************************************************
bool am_hal_gpio_isinput(uint32_t ui32Pin)
{
    uint32_t ui32Regval, ui32PadShft, ui32InpenMsk;
    ui32Regval    = AM_REGVAL( AM_REGADDR(GPIO, PADREGA) + (ui32Pin & ~0x3) );
    ui32PadShft   = ((ui32Pin & 0x3) << 3);
    ui32InpenMsk = (uint32_t)0x02 << ui32PadShft;

    return (ui32Regval & ui32InpenMsk) ? true : false;
} // am_hal_gpio_isinput()

//*****************************************************************************
//  am_hal_gpio_isgpio()
//
//  Determine whether the GPIO is configured as input or output.
//
//  Return values:
//      0: Pin is not configured as GPIO.
//      1: Pin is configured as GPIO input.
//      2: Pin is configured as GPIO output.
//*****************************************************************************
uint32_t am_hal_gpio_isgpio(uint32_t ui32Pin)
{
    uint32_t ui32Padval, ui32Cfgval;

    //
    // Check PADREGx field for this pin.
    // [5:3] is FNCSEL (must be 0x18 for GPIO), [1:1] is INPEN.
    //
    ui32Padval   = AM_REGVAL( AM_REGADDR(GPIO, PADREGA) + (ui32Pin & ~0x3) );
    ui32Padval >>= ((ui32Pin & 0x3) << 3);
    if ( (ui32Padval & 0x38) != 0x18 )
    {
        //
        // Not configured as GPIO
        //
        return 0;
    }

    //
    // Determine if an input or an output.
    //
    ui32Cfgval = AM_REGVAL( AM_REGADDR(GPIO, CFGA) + ((ui32Pin >> 1) & ~0x3) );
    ui32Cfgval >>= ((ui32Pin & 0x7) * 4);
    if ( (ui32Padval & 0x02) )
    {
        //
        // INPEN is set, so probably a GPIO input.
        // However, GPIO outputs can also be set to read the pin state,
        // which requires INPEN be set.
        //
        return (ui32Cfgval & 0x6) == 0 ? 1 : 2;
    }
    else
    {
        return 2;
    }
} // am_hal_gpio_isgpio()

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
