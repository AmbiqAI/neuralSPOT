//*****************************************************************************
//
//! @file am_hal_gpio.h
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

#ifndef AM_HAL_GPIO_H
#define AM_HAL_GPIO_H   1

#ifdef __cplusplus
extern "C"
{
#endif

//
//! Designate this peripheral.
//
#define AM_APOLLO3_GPIO     1

//
//! Maximum number of GPIOs on this device
//
#define AM_HAL_GPIO_MAX_PADS        (74)
#define AM_HAL_GPIO_NUMWORDS        ((AM_HAL_GPIO_MAX_PADS + 31) / 32)

//
//! Macros to assist with defining a GPIO mask given a GPIO number.
//!
//! IMPORTANT: AM_HAL_GPIO_BIT(n) is DEPRECATED and is
//!            replaced with AM_HAL_GPIO_MASKBIT().
//
#define AM_HAL_GPIO_BIT(n)          DEPRECATED_USE_AM_HAL_GPIO_MASKBIT  // Force a compile error if used

//!
//! The following macros ensure forward compatibility with future SDK releases.
//! They should be used, in lieu of AM_HAL_GPIO_BIT(), when creating bitmasks
//! for GPIO interrupts.
//!     AM_HAL_GPIO_MASKCREATE()
//!     AM_HAL_GPIO_MASKBIT()
//!
#define AM_HAL_GPIO_MASKCREATE(sMaskNm)             \
        am_hal_gpio_mask_t sMaskNm;                 \
        am_hal_gpio_mask_t *p##sMaskNm = &sMaskNm;  \
        sMaskNm.U.Msk[0] = sMaskNm.U.Msk[1] = sMaskNm.U.Msk[2] = 0;
//! AM_HAL_GPIO_MASKCREATE() should be used before AM_HAL_GPIO_MASKBIT() to
//! ensure forward compatibility.  In future releases it will allocate and
//! initialize a bitmask structure used in the various GPIO interrupt functions.
//!

//  #define AM_HAL_GPIO_MASKBIT(psMaskNm, n)    inline_function_below
//! AM_HAL_GPIO_MASKBIT(psMaskNm, n)
//! Implemented as an inline function below.
//! Support macros for use with AM_HAL_GPIO_MASKBIT().
//!  AM_HAL_GPIO_MASKCREATE()
//!  AM_HAL_GPIO_MASKCLR()
//!
//! To set a single bit based on a pin number in an existing bitmask structure.
//!     AM_HAL_GPIO_MASKBIT(pGpioIntMask, n)
//! where n is the desired GPIO bit number.
//! Note - this usage is analogous to the deprecated AM_HAL_GPIO_BIT(n).
//!

#define AM_HAL_GPIO_MASKCLR(psMaskNm)   ( psMaskNm->U.Msk[0] = psMaskNm->U.Msk[1] = psMaskNm->U.Msk[2] = 0 )
//! AM_HAL_GPIO_MASKCLR()
//! Clear an existing GpioIntMask bitmask structure.
//! Note that AM_HAL_GPIO_MASKCREATE() clears the bitmask struct on creation.
//! IMPORTANT - The AM_HAL_GPIO_MASKCLR() macro does not operate on any hardware
//! or register.  It is used for initializing/clearing the memory allocated for
//! the bitmask structure.
//!
//! // Usage example for any Apollo device:
//! // Create a GPIO interrupt bitmask structure named GpioIntMask, initialize
//! // that structure, and create a ptr to that structure named pGpioIntMask.
//! // Then use that structure to pass a bitmask to the interrupt function.
//! AM_HAL_GPIO_MASKCREATE(GpioIntMask);
//! am_hal_gpio_interrupt_clear(AM_HAL_GPIO_MASKBIT(pGpioIntMask));
//!

//*****************************************************************************
//!
//! Structure for defining bitmasks used in the interrupt functions.
//!
//*****************************************************************************
typedef struct
{
    union
    {
        volatile  uint32_t Msk[AM_HAL_GPIO_NUMWORDS];

        struct
        {
            volatile uint32_t   b0:     1;
            volatile uint32_t   b1:     1;
            volatile uint32_t   b2:     1;
            volatile uint32_t   b3:     1;
            volatile uint32_t   b4:     1;
            volatile uint32_t   b5:     1;
            volatile uint32_t   b6:     1;
            volatile uint32_t   b7:     1;
            volatile uint32_t   b8:     1;
            volatile uint32_t   b9:     1;
            volatile uint32_t   b10:    1;
            volatile uint32_t   b11:    1;
            volatile uint32_t   b12:    1;
            volatile uint32_t   b13:    1;
            volatile uint32_t   b14:    1;
            volatile uint32_t   b15:    1;
            volatile uint32_t   b16:    1;
            volatile uint32_t   b17:    1;
            volatile uint32_t   b18:    1;
            volatile uint32_t   b19:    1;
            volatile uint32_t   b20:    1;
            volatile uint32_t   b21:    1;
            volatile uint32_t   b22:    1;
            volatile uint32_t   b23:    1;
            volatile uint32_t   b24:    1;
            volatile uint32_t   b25:    1;
            volatile uint32_t   b26:    1;
            volatile uint32_t   b27:    1;
            volatile uint32_t   b28:    1;
            volatile uint32_t   b29:    1;
            volatile uint32_t   b30:    1;
            volatile uint32_t   b31:    1;
            volatile uint32_t   b32:    1;
            volatile uint32_t   b33:    1;
            volatile uint32_t   b34:    1;
            volatile uint32_t   b35:    1;
            volatile uint32_t   b36:    1;
            volatile uint32_t   b37:    1;
            volatile uint32_t   b38:    1;
            volatile uint32_t   b39:    1;
            volatile uint32_t   b40:    1;
            volatile uint32_t   b41:    1;
            volatile uint32_t   b42:    1;
            volatile uint32_t   b43:    1;
            volatile uint32_t   b44:    1;
            volatile uint32_t   b45:    1;
            volatile uint32_t   b46:    1;
            volatile uint32_t   b47:    1;
            volatile uint32_t   b48:    1;
            volatile uint32_t   b49:    1;
            volatile uint32_t   b50:    1;
            volatile uint32_t   b51:    1;
            volatile uint32_t   b52:    1;
            volatile uint32_t   b53:    1;
            volatile uint32_t   b54:    1;
            volatile uint32_t   b55:    1;
            volatile uint32_t   b56:    1;
            volatile uint32_t   b57:    1;
            volatile uint32_t   b58:    1;
            volatile uint32_t   b59:    1;
            volatile uint32_t   b60:    1;
            volatile uint32_t   b61:    1;
            volatile uint32_t   b62:    1;
            volatile uint32_t   b63:    1;
            volatile uint32_t   b64:    1;
            volatile uint32_t   b65:    1;
            volatile uint32_t   b66:    1;
            volatile uint32_t   b67:    1;
            volatile uint32_t   b68:    1;
            volatile uint32_t   b69:    1;
            volatile uint32_t   b70:    1;
            volatile uint32_t   b71:    1;
            volatile uint32_t   b72:    1;
            volatile uint32_t   b73:    1;
            volatile uint32_t   brsvd: 22;  // Pad out to the next full word
        } Msk_b;
    } U;
} am_hal_gpio_mask_t;

//*****************************************************************************
//!
//! AM_HAL_GPIO_MASKBIT()
//!
//*****************************************************************************
//
// AM_HAL_GPIO_MASKBIT(psMaskNm, n)
//
// Entry:
//  psMaskNm is a pointer to the structure type, am_hal_gpio_mask_t.
//  n is the desired bitnumber.
//
// Returns ptr to the bit structure.
//
// Since the ptr is required as a return value, a macro does not work.
// Therefore an inline function is used.
//
static inline am_hal_gpio_mask_t*
AM_HAL_GPIO_MASKBIT(am_hal_gpio_mask_t*psMaskNm, uint32_t n)
{
    psMaskNm->U.Msk[n / 32] = (1 << (n % 32));
    return psMaskNm;
}

//*****************************************************************************
//!
//! AM_HAL_GPIO_MASKBITSMULT()
//!
//*****************************************************************************
//
// AM_HAL_GPIO_MASKBITSMULT(psMaskNm, n)
//
// Entry:
//  psMaskNm is a pointer to the structure type, am_hal_gpio_mask_t.
//  n is the desired bitnumber.
//
// Returns ptr to the bit structure.
//
// Since the ptr is required as a return value, a macro does not work.
// Therefore an inline function is used.
//
static inline am_hal_gpio_mask_t*
AM_HAL_GPIO_MASKBITSMULT(am_hal_gpio_mask_t*psMaskNm, uint32_t n)
{
    psMaskNm->U.Msk[n / 32] |= (1 << (n % 32));
    return psMaskNm;
}

//*****************************************************************************
//!
//! Read types for am_hal_gpio_state_read().
//!
//! - AM_HAL_GPIO_INPUT_READ\n
//!     Used for reading the value on the pad whether the pad is configured
//!     as an input or an output. Assumes that the pin is configured with
//!     AM_HAL_GPIO_PIN_INPUT_ENABLE and AM_HAL_GPIO_PIN_RDZERO_READPIN, even
//!     if configured for output (e.g. see g_AM_HAL_GPIO_OUTPUT_WITH_READ).\n
//!     Perhaps a reasonable alias would be AM_HAL_GPIO_PAD_READ.
//!
//! - AM_HAL_GPIO_OUTPUT_READ\n
//!     Used for reading the last value written to the GPIO output, e.g via
//!     am_hal_gpio_state_write(AM_HAL_GPIO_OUTPUT_SET) or even the macro
//!     am_hal_gpio_output_set(n).
//!
//! - AM_HAL_GPIO_ENABLE_READ\n
//!     Used for reading the state of the tristate enable, e.g. the state
//!     of the tristate enable after calling
//!     am_hal_gpio_state_write(AM_HAL_GPIO_OUTPUT_TRISTATE_ENABLE) or
//!     am_hal_gpio_state_write(AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE), or even
//!     the macro am_hal_gpio_output_tristate_disable(n).
//!
//*****************************************************************************
typedef enum
{
    AM_HAL_GPIO_INPUT_READ,
    AM_HAL_GPIO_OUTPUT_READ,
    AM_HAL_GPIO_ENABLE_READ
} am_hal_gpio_read_type_e;

//*****************************************************************************
//!
//! Write types for am_hal_gpio_state_write().
//!
//*****************************************************************************
typedef enum
{
    AM_HAL_GPIO_OUTPUT_CLEAR,
    AM_HAL_GPIO_OUTPUT_SET,
    AM_HAL_GPIO_OUTPUT_TOGGLE,
    AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE,
    AM_HAL_GPIO_OUTPUT_TRISTATE_ENABLE,
    AM_HAL_GPIO_OUTPUT_TRISTATE_TOGGLE
} am_hal_gpio_write_type_e;


//*****************************************************************************
//!
//! Types for ui32GpioCfg bitfields in am_hal_gpio_pinconfig().
//!
//*****************************************************************************
//!
//! @par
//! Power Switch configuration: am_hal_gpio_pincfg_t.ePowerSw enums
//!
typedef enum
{
    AM_HAL_GPIO_PIN_POWERSW_NONE,
    AM_HAL_GPIO_PIN_POWERSW_VDD,
    AM_HAL_GPIO_PIN_POWERSW_VSS,
    AM_HAL_GPIO_PIN_POWERSW_INVALID,
} am_hal_gpio_powersw_e;

//!
//! Pullup configuration: am_hal_gpio_pincfg_t.ePullup enums
//!
//! Define pullup enums.\n
//! The 1.5K - 24K pullup values are valid for select I2C enabled pads.\n
//! For Apollo3 these pins are 0-1,5-6,8-9,25,27,39-40,42-43,48-49.\n
//! The "weak" value is used for almost every other pad except pin 20.
//!
typedef enum
{
    AM_HAL_GPIO_PIN_PULLUP_NONE = 0x00,
    AM_HAL_GPIO_PIN_PULLUP_WEAK,
    AM_HAL_GPIO_PIN_PULLUP_1_5K,
    AM_HAL_GPIO_PIN_PULLUP_6K,
    AM_HAL_GPIO_PIN_PULLUP_12K,
    AM_HAL_GPIO_PIN_PULLUP_24K,
    AM_HAL_GPIO_PIN_PULLDOWN
} am_hal_gpio_pullup_e;

//!
//! Pad Drive Strength configuration: am_hal_gpio_pincfg_t.eDriveStrength enums
//!
//! DRIVESTRENGTH is a 2-bit field.\n
//!  - bit0 maps to bit2 of a PADREG field.
//!  - bit1 maps to bit0 of an ALTPADCFG field.
//!
typedef enum
{
    AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA   = 0x0,
    AM_HAL_GPIO_PIN_DRIVESTRENGTH_4MA   = 0x1,
    AM_HAL_GPIO_PIN_DRIVESTRENGTH_8MA   = 0x2,
    AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA  = 0x3
} am_hal_gpio_drivestrength_e;

//!
//! OUTCFG pad configuration: am_hal_gpio_pincfg_t.eGPOutcfg enums\n
//! Applies only to GPIO configured pins.\n
//! Ultimately maps to GPIOCFG.OUTCFG, bits [2:1].
//!
typedef enum
{
    AM_HAL_GPIO_PIN_OUTCFG_DISABLE     = 0x0,
    AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL    = 0x1,
    AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN   = 0x2,
    AM_HAL_GPIO_PIN_OUTCFG_TRISTATE    = 0x3
} am_hal_gpio_outcfg_e;

//!
//! GPIO input configuration: am_hal_gpio_pincfg_t.eGPInput enums\n
//! Applies only to GPIO configured pins!\n
//! Ultimately maps to PADREG.INPEN, bit1.
//!
typedef enum
{
    AM_HAL_GPIO_PIN_INPUT_AUTO          = 0x0,
    AM_HAL_GPIO_PIN_INPUT_NONE          = 0x0,
    AM_HAL_GPIO_PIN_INPUT_ENABLE        = 0x1
} am_hal_gpio_input_e;

//!
//! GPIO interrupt direction configuration: am_hal_gpio_pincfg_t.eIntDir enums\n
//! @note Setting INTDIR_NONE has the side-effect of disabling being able to
//!       read a pin - the pin will always read back as 0.
//!
typedef enum
{
    // Bit1 of these values maps to GPIOCFG.INCFG (b0).
    // Bit0 of these values maps to GPIOCFG.INTD  (b3).
    AM_HAL_GPIO_PIN_INTDIR_LO2HI        = 0x0,
    AM_HAL_GPIO_PIN_INTDIR_HI2LO        = 0x1,
    AM_HAL_GPIO_PIN_INTDIR_NONE         = 0x2,
    AM_HAL_GPIO_PIN_INTDIR_BOTH         = 0x3
} am_hal_gpio_intdir_e;

//!
//! am_hal_gpio_pincfg_t.eGPRdZero\n
//! For GPIO configurations (funcsel=3), the pin value can be read or 0 can be
//! forced as the read value.
//!
typedef enum
{
    AM_HAL_GPIO_PIN_RDZERO_READPIN      = 0x0,
    AM_HAL_GPIO_PIN_RDZERO_ZERO         = 0x1
} am_hal_gpio_readen_e;

//!
//! nCE polarity configuration: am_hal_gpio_pincfg_t.eCEpol enums
//!
typedef enum
{
    AM_HAL_GPIO_PIN_CEPOL_ACTIVELOW     = 0x0,
    AM_HAL_GPIO_PIN_CEPOL_ACTIVEHIGH    = 0x1
} am_hal_gpio_cepol_e;


//
// Apollo3 usage of bits [7:6] of a PADREG field:
// PULLUPs are available on pins: 0,1,5,6,8,9,25,27,39,40,42,43,48,49
// RESERVED on pins:              2,4,7,10-24,26,28-35,38,44-47
// VDD PWR on pins:               3, 36 (b7=0, b6=1)
// VSS PWR on pins:               37,41 (b7=1, b6=0)
//

//!
//! Define the am_hal_gpio_pinconfig() bitfield structure.
//!
//!    - uFuncSel:      - A value of 0-7 corresponding to the FNCSEL field of PADREG.
//!    - ePowerSw:      - Select pins can be set as a power source or sink.
//!    - ePullup:       - Select pins can enable a pullup of varying values.
//!    - eDriveStrength: - Select pins can be set for varying drive strengths.
//!    - eGPOutcfg:     - GPIO pin only, corresponds to GPIOCFG.OUTCFG field.
//!    - eGPInput:      - GPIO pin only, corresponds to PADREG.INPEN.
//!    - eGPRdZero:     - GPIO read zero.  Corresponds to GPIOCFG.INCFG.
//!    - eIntDir:       - Interrupt direction, l2h, h2l, both, none.
//!    - eGPRdZero:     - Read the pin value, or always read the pin as zero.
//!    - uIOMnum:       - nCE pin IOMnumber (0-5, or 6 for MSPI)
//!    - nNCE:          - Selects the SPI channel (CE) number (0-3)
//!    - eCEpol:        - CE polarity.
//!
typedef struct
{
    uint32_t uFuncSel       : 3;    //!< [2:0]   Function select (FUNCSEL)
    uint32_t ePowerSw       : 2;    //!< [4:3]   Pin is a power switch source (VCC) or sink (VSS)
    uint32_t ePullup        : 3;    //!< [7:5]   Pin will enable a pullup resistor
    uint32_t eDriveStrength : 2;    //!< [9:8]   Pad strength designator
    uint32_t eGPOutcfg      : 2;    //!< [11:10] OUTCFG (GPIO config only)
    uint32_t eGPInput       : 1;    //!< [12:12] GPIO Input (GPIO config only)
    uint32_t eIntDir        : 2;    //!< [14:13] Interrupt direction
    uint32_t eGPRdZero      : 1;    //!< [15:15] GPIO read as zero

    //
    //! @name chip_enable_features
    //! The following descriptors designate the chip enable features of the
    //! pin being configured.  If not a CE, these descriptors are ignored.
    //! uIOMnum is 0-5 for the IOMs, or 0-2 for MSPI.
    //! @{
    //
    uint32_t bIomMSPIn      : 1;    //!< [16:16] 1 if CE is IOM, 0 if MSPI
    uint32_t uIOMnum        : 3;    //!< [19:17] IOM number (0-5) or MSPI (0-2)
    uint32_t uNCE           : 2;    //!< [21:20] NCE number (0-3).
    uint32_t eCEpol         : 1;    //!< [22:22] NCE polarity.
    uint32_t uRsvd23        : 9;    //!< [31:23]

    //! @} // chip_enable_features

} am_hal_gpio_pincfg_t;

//#define IOMNUM_MSPI         6
//#define IOMNUM_MAX          IOMNUM_MSPI

//
//! Define shift and width values for the above bitfields.
// - C bitfields do not provide shift, width, or mask values.
// - Shift values are generally compiler specific.  However for IAR, Keil, and
//   GCC, the bitfields are all exactly as defined in the above structure.
// - These defines should be used sparingly.
//
#define UFUNCSEL_S          0
#define EPOWERSW_S          3
#define EPULLUP_S           5
#define EDRVSTR_S           8
#define EGPOUTCFG_S         10
#define EGPINPUT_S          12
#define EINTDIR_S           13
#define UIOMNUM_S           16
#define UNCE_S              19
#define ECEPOL_S            21

#define UFUNCSEL_W          3
#define EPOWERSW_W          2
#define EPULLUP_W           3
#define EDRVSTR_W           2
#define EGPOUTCFG_W         2
#define EGPINPUT_W          1
#define EINTDIR_W           2
#define UIOMNUM_W           3
#define UNCE_W              2
#define ECEPOL_W            1

//!
//! Define GPIO error codes that are returned by am_hal_gpio_pinconfig().
//!
enum am_hal_gpio_pincfgerr
{
    AM_HAL_GPIO_ERR_PULLUP      = (AM_HAL_STATUS_MODULE_SPECIFIC_START + 0x100),
    AM_HAL_GPIO_ERR_PULLDOWN,
    AM_HAL_GPIO_ERR_PWRSW,
    AM_HAL_GPIO_ERR_INVCE,
    AM_HAL_GPIO_ERR_INVCEPIN,
    AM_HAL_GPIO_ERR_PULLUPENUM
};

//*****************************************************************************
//
//! Globals
//
//*****************************************************************************
//*****************************************************************************
//!  Define some common GPIO pin configurations.
//*****************************************************************************
//! Basics
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_DISABLE;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_TRISTATE;

//! Input variations
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT;
//! Input with various pullups (weak, 1.5K, 6K, 12K, 24K)
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_1_5;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_6;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_12;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_INPUT_PULLUP_24;

//! Output variations
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_4;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_8;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_12;
extern const am_hal_gpio_pincfg_t g_AM_HAL_GPIO_OUTPUT_WITH_READ;

//*****************************************************************************
//
//! Function pointer type for GPIO interrupt handlers.
//
//*****************************************************************************
typedef void (*am_hal_gpio_handler_t)(void);
typedef void (*am_hal_gpio_handler_adv_t)(void *);

//*****************************************************************************
//
//! @brief Configure an Apollo3 pin.
//!
//! @param ui32Pin     - pin number to be configured.
//! @param bfGpioCfg   - Contains multiple descriptor fields.
//!
//! @details This function configures a pin according to the descriptor parameters as
//! passed in sPinCfg.  All parameters are validated with regard to each
//! other and according to the requested function.  Once the parameters and
//! settings have been confirmed, the pin is configured accordingly.
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_pinconfig(uint32_t ui32Pin,
                                      am_hal_gpio_pincfg_t bfGpioCfg);

//*****************************************************************************
//
//! @brief Configure specified pins for FAST GPIO operation.
//!
//! @param psPinMask
//!        - a mask specifying up to 8 pins to be configured and
//!        used for FAST GPIO (only bits 0-73 are valid).
//!
//! @param bfGpioCfg
//!         - The GPIO configuration (same as am_hal_gpio_pinconfig()).\n
//!         - All of the pins specified by psPinMask will be set to this
//!         configuration.
//! @param ui32Masks
//! @parblock
//!         - If NULL, not used.\n
//!         - Otherwise if provided, an array to
//!         receive two 32-bit values, per pin, of the SET and CLEAR
//!         masks that can be used for the BBSETCLEAR register.
//!
//!         - The two 32-bit values will be placed at incremental indexes.\n
//!         For example, say pin numbers 5 and 19 are indicated in the
//!         mask, and an array pointer is provided in ui32Masks.\n
//!         This array must be allocated by the caller to be at least 4 wds.
//!
//!                 - ui32Masks[0] = the set   mask used for pin 5.
//!                 - ui32Masks[1] = the clear mask used for pin 5.
//!                 - ui32Masks[2] = the set   mask used for pin 19.
//!                 - ui32Masks[3] = the clear mask used for pin 19.
//! @endparblock
//! @return Status.
//!
//!     Fast GPIO helper macros:
//!         am_hal_gpio_fastgpio_set(n) - Sets the value for pin number 'n'.
//!         am_hal_gpio_fastgpio_clr(n) - Clear the value for pin number 'n'.
//!
//!         am_hal_gpio_fastgpio_enable(n)  - Enable Fast GPIO on pin 'n'.
//!         am_hal_gpio_fastgpio_disable(n) - Disable Fast GPIO on pin 'n'.
//!
//! @note The enable and disable macros assume the pin has already been
//!     configured. Once disabled, the state of the pin will revert to the
//!     state of the normal GPIO configuration for that pin.
//! @par
//! @note NOTES on pin configuration:
//! - To avoid glitches on the pin, it is strongly recommended that before
//!    calling am_hal_gpio_fast_pinconfig() that am_hal_gpio_fastgpio_disable()
//!   first be called to make sure that Fast GPIO is disabled before config.
//! - If the state of the pin is important, preset the value of the pin to the
//!   desired value BEFORE calling am_hal_gpio_fast_pinconfig().  The set and
//!   clear macros shown above can be used for this purpose.
//! @par
//! @note NOTES on general use of Fast GPIO:
//! Fast GPIO input or output will not work if the pin is configured as
//! tristate.  The overloaded OUTPUT ENABLE control is used for enabling both
//! modes, so Apollo3 logic specifically disallows Fast GPIO input or output
//! when the pin is configured for tristate mode.
//! Fast GPIO input can be used for pushpull, opendrain, or disable modes.
//! @par
//! Fast GPIO pin groupings:
//! The FaST GPIO pins are grouped across a matrix of pins.  Each
//! row of pins is controlled by a single data bit.
//! @par
//! Referring to the below chart:
//!  - If pin 35 were configured for Fast GPIO output, it would be set
//!  when bit3 of BBSETCLEAR.SET was written with a 1.
//!  - It would be cleared when bit3 of BBSETCLEAR.CLEAR was written with 1.
//! @par
//!  Note that if all the pins in a row were configured for Fast GPIO output,
//!  all the pins would respond to set/clear.
//!
//!  Input works in a similar fashion.
//!
//!       BIT   PIN controlled
//!       ---  ---------------------------------------
//!        0     0   8  16  24  32  40  48  56  64  72
//!        1     1   9  17  25  33  41  49  57  65  73
//!        2     2  10  18  26  34  42  50  58  66
//!        3     3  11  19  27  35  43  51  59  67
//!        4     4  12  20  28  36  44  52  60  68
//!        5     5  13  21  29  37  45  53  61  69
//!        6     6  14  22  30  38  46  54  62  70
//!        7     7  15  23  31  39  47  55  63  71
//!
//
//*****************************************************************************
extern uint32_t am_hal_gpio_fast_pinconfig(am_hal_gpio_mask_t *psPinMask,
                                           am_hal_gpio_pincfg_t bfGpioCfg,
                                           uint32_t ui32Masks[]);

//*****************************************************************************
//
//! @brief Read GPIO.
//!
//! This function reads a pin state as given by eReadType.
//!
//! @param ui32Pin      - pin number to be read.
//! @param eReadType    - State type to read.  One of:
//!      - AM_HAL_GPIO_INPUT_READ
//!      - AM_HAL_GPIO_OUTPUT_READ
//!      - AM_HAL_GPIO_ENABLE_READ
//! @param pui32ReadState - returns pin state value here
//!
//! @return Status.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_state_read(
                       uint32_t ui32Pin,
                       am_hal_gpio_read_type_e eReadType,
                       uint32_t *pui32ReadState);

//*****************************************************************************
//
//! @brief Write GPIO.
//!
//! @param ui32Pin      - pin number to be read.
//! @param eWriteType   - State type to write.  One of:
//!       - AM_HAL_GPIO_OUTPUT_SET              - Write a one to a GPIO.
//!       - AM_HAL_GPIO_OUTPUT_CLEAR            - Write a zero to a GPIO.
//!       - AM_HAL_GPIO_OUTPUT_TOGGLE           - Toggle the GPIO value.
//!       -   ** The following two apply when output is set for TriState (OUTCFG==3).
//!       - AM_HAL_GPIO_OUTPUT_TRISTATE_ENABLE  - Enable  a tri-state GPIO.
//!       - AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE - Disable a tri-state GPIO.
//!
//! This function writes a GPIO value.
//!
//! @return Status.
//!         - Fails if the pad is not configured for GPIO (PADFNCSEL != 3).
//
//*****************************************************************************
extern uint32_t am_hal_gpio_state_write(uint32_t ui32Pin,
                                        am_hal_gpio_write_type_e eWriteType);

//*****************************************************************************
//
//! @brief Enable GPIO interrupts.
//!
//! @param pGpioIntMask - Mask of GPIO interrupts to enable.\n
//!     - Only bits 0-73 are valid in the mask.
//!
//! @return Status.
//!         - Fails if any bit above bit73 is set in pGpioIntMask.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_enable(am_hal_gpio_mask_t *pGpioIntMask);

//*****************************************************************************
//
//! @brief Disable GPIO interrupts.
//!
//! @param pGpioIntMask - Mask of GPIO interrupts to disable.\n
//!     - Only bits 0-73 are valid in the mask.
//!
//! @return Status.
//!         - Fails if any bit above bit73 is set in pGpioIntMask.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_disable(am_hal_gpio_mask_t *pGpioIntMask);

//*****************************************************************************
//
//! @brief Clear GPIO interrupts.
//!
//! @param pGpioIntMask - Mask of GPIO interrupts to be cleared.\n
//!     - Only bits 0-73 are valid in the mask.
//!
//! @return Status.
//!         - Fails if any bit above bit73 is set in pGpioIntMask.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_clear(am_hal_gpio_mask_t *pGpioIntMask);

//*****************************************************************************
//
//! @brief Get GPIO interrupt status.
//!
//! @param bEnabledOnly - Return status only for currently enabled interrupts.
//!
//! @param pGpioIntMask - Bitmask array to return a bitmask of the status
//! of the interrupts.
//!
//! @return Status.
//!         - Fails if pGpioIntMask is NULL.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_status_get(bool bEnabledOnly,
                                                 am_hal_gpio_mask_t *pGpioIntMask);

//*****************************************************************************
//
//! @brief GPIO interrupt service routine registration.
//!
//! @param ui32GPIONumber - GPIO number (0-73) to be registered.
//!
//! @param pfnHandler - Function pointer to the callback.
//!
//! @return Status.
//!         - Fails if pfnHandler is NULL or ui32GPIONumber > 73.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_register(uint32_t ui32GPIONumber,
                                               am_hal_gpio_handler_t pfnHandler);

//*****************************************************************************
//
//! @brief Advanced GPIO interrupt service routine registration.
//!
//! @param ui32GPIONumber - GPIO number (0-73) to be registered.
//!
//! @param pfnHandler - Function pointer to the callback.
//!
//! @param pCtxt      - Context for the callback.
//!
//! @return Status.
//!         - Fails if pfnHandler is NULL or ui32GPIONumber > 73.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_register_adv(uint32_t ui32GPIONumber,
                               am_hal_gpio_handler_adv_t pfnHandler, void *pCtxt);

//*****************************************************************************
//
// GPIO interrupt service routine.
//! @brief GPIO interrupt service routine registration.
//!
//! @param pGpioIntMask - Mask of the interrupt(s) to be serviced.\n This mask is
//! typically obtained via a call to am_hal_gpio_interrupt_status_get().
//!
//! The intended use is that the application first registers a handler for a
//! particular GPIO via am_hal_gpio_interrupt_register(), and to supply the
//! main ISR, am_gpio_isr().
//!
//! On a GPIO interrupt, am_gpio_isr() calls am_hal_gpio_interrupt_status_get()
//! and provides the return value to this function.
//!
//! In the event that multiple GPIO interrupts are active, the corresponding
//! interrupt handlers will be called in numerical order by GPIO number
//! starting with the lowest GPIO number.
//!
//! @return Status.
//!         - AM_HAL_STATUS_INVALID_OPERATION if no handler had been registered
//!             for any of the GPIOs that caused the interrupt.
//!         - AM_HAL_STATUS_OUT_OF_RANGE if any bit above bit73 is set.
//!         - AM_HAL_STATUS_FAIL if pGpioIntMask is 0.
//!         - AM_HAL_STATUS_SUCCESS otherwise.
//
//*****************************************************************************
extern uint32_t am_hal_gpio_interrupt_service(am_hal_gpio_mask_t *pGpioIntMask);

//*****************************************************************************
//!
//! am_hal_gpio_isinput()
//!
//! Determine whether a pad is configured with input enable.
//! Returns true if input enable is set, false otherwise.
//!
//*****************************************************************************
extern bool am_hal_gpio_isinput(uint32_t ui32Pin);

//*****************************************************************************
//!
//! am_hal_gpio_isgpio()
//!
//! Determine whether the GPIO is configured as input or output.
//!
//! Return values:
//!     0: Pin is not configured as GPIO.
//!     1: Pin is configured as GPIO input.
//!     2: Pin is configured as GPIO output.
//!
//*****************************************************************************
extern uint32_t am_hal_gpio_isgpio(uint32_t ui32Pin);

//*****************************************************************************
//
//! @brief Macros to read GPIO values in an optimized manner.
//!
//! @param n - The GPIO number to be read.
//!
//! In almost all cases, it is reasonable to use am_hal_gpio_state_read() to
//! read GPIO values with all of the inherent error checking, critical
//! sectioning, and general safety.
//!
//! However, occasionally there is a need to read a GPIO value in an optimized
//! manner.  These 3 macros will accomplish that.  Each macro will return a
//! value of 1 or 0.
//!
//! Note that the macros are named as lower-case counterparts to the
//! enumerations for the am_hal_gpio_state_read() function.  That is:
//!
//!     AM_HAL_GPIO_INPUT_READ  -> am_hal_gpio_input_read(n)
//!     AM_HAL_GPIO_OUTPUT_READ -> am_hal_gpio_output_read(n)
//!     AM_HAL_GPIO_ENABLE_READ -> am_hal_gpio_enable_read(n)
//!
//! @return Each macro will return a 1 or 0 per the value of the requested GPIO.
//!
//
//*****************************************************************************
#define am_hal_gpio_input_read(n)       (                                                                                                   \
        (AM_REGVAL( (AM_REGADDR(GPIO, RDA) + (((uint32_t)(n) & 0x60) >> 3)) ) >>                /* Read appropriate register */             \
          ((uint32_t)(n) & 0x1F) )      &                                                       /* Shift by appropriate number of bits */   \
         ((uint32_t)0x1) )                                                                      /* Mask out the LSB */

#define am_hal_gpio_output_read(n)      (                                                                                                   \
        (AM_REGVAL( (AM_REGADDR(GPIO, WTA) + (((uint32_t)(n) & 0x60) >> 3)) ) >>                /* Read appropriate register */             \
          ((uint32_t)(n) & 0x1F) )      &                                                       /* Shift by appropriate number of bits */   \
         ((uint32_t)0x1) )                                                                      /* Mask out the LSB */

#define am_hal_gpio_enable_read(n)      (                                                                                                   \
        (AM_REGVAL( (AM_REGADDR(GPIO, ENA) + (((uint32_t)(n) & 0x60) >> 3)) ) >>                /* Read appropriate register */             \
          ((uint32_t)(n) & 0x1F) )      &                                                       /* Shift by appropriate number of bits */   \
         ((uint32_t)0x1) )                                                                      /* Mask out the LSB */


//*****************************************************************************
//
//! @brief Macros to write GPIO values in an optimized manner.
//!
//! @param n - The GPIO number to be written.
//!
//! In almost all cases, it is reasonable to use am_hal_gpio_state_write() to
//! write GPIO values with all of the inherent error checking, critical
//! sectioning, and general safety.
//!
//! However, occasionally there is a need to write a GPIO value in an optimized
//! manner.  These 3 macros will accomplish that.
//!
//! Note that the macros are named as lower-case counterparts to the
//! enumerations for the am_hal_gpio_state_read() function.  That is:
//!
//!    AM_HAL_GPIO_OUTPUT_CLEAR            -> am_hal_gpio_output_clear(n)
//!    AM_HAL_GPIO_OUTPUT_SET              -> am_hal_gpio_output_set(n)
//!    AM_HAL_GPIO_OUTPUT_TOGGLE           -> am_hal_gpio_output_toggle(n)
//!    AM_HAL_GPIO_OUTPUT_TRISTATE_DISABLE -> am_hal_gpio_output_tristate_disable(n)
//!    AM_HAL_GPIO_OUTPUT_TRISTATE_ENABLE  -> am_hal_gpio_output_tristate_enable(n)
//!    AM_HAL_GPIO_OUTPUT_TRISTATE_TOGGLE  -> am_hal_gpio_output_tristate_toggle(n)
//!
//! @return None.
//!
//*****************************************************************************
//
// Note - these macros use byte-oriented addressing.
//
#define am_hal_gpio_output_clear(n)                                                 \
    ((*((volatile uint32_t *)                                                       \
    ((AM_REGADDR(GPIO, WTCA) + (((uint32_t)(n) & 0x60) >> 3))))) =                  \
     ((uint32_t) 0x1 << ((uint32_t)(n) % 32)))

#define am_hal_gpio_output_set(n)                                                   \
    ((*((volatile uint32_t *)                                                       \
    ((AM_REGADDR(GPIO, WTSA) + (((uint32_t)(n) & 0x60) >> 3))))) =                  \
     ((uint32_t) 0x1 << ((uint32_t)(n) % 32)))

#define am_hal_gpio_output_toggle(n)                                                \
    if ( 1 )                                                                        \
    {                                                                               \
        AM_CRITICAL_BEGIN                                                           \
        ((*((volatile uint32_t *)                                                   \
        ((AM_REGADDR(GPIO, WTA) + (((uint32_t)(n) & 0x60) >> 3))))) ^=              \
         ((uint32_t) 0x1 << ((uint32_t)(n) % 32)));                                 \
        AM_CRITICAL_END                                                             \
    }

#define am_hal_gpio_output_tristate_disable(n)                                      \
    ((*((volatile uint32_t *)                                                       \
     ((AM_REGADDR(GPIO, ENCA) + (((uint32_t)(n) & 0x60) >> 3))))) =                 \
      ((uint32_t) 0x1 << ((uint32_t)(n) % 32)))

#define am_hal_gpio_output_tristate_enable(n)                                       \
    ((*((volatile uint32_t *)                                                       \
     ((AM_REGADDR(GPIO, ENSA) + (((uint32_t)(n) & 0x60) >> 3))))) =                 \
      ((uint32_t) 0x1 << ((uint32_t)(n) % 32)))

#define am_hal_gpio_output_tristate_toggle(n)                                       \
    if ( 1 )                                                                        \
    {                                                                               \
        AM_CRITICAL_BEGIN                                                           \
        ((*((volatile uint32_t *)                                                   \
         ((AM_REGADDR(GPIO, ENA) + (((uint32_t)(n) & 0x60) >> 3))))) ^=             \
          ((uint32_t) 0x1 << ((uint32_t)(n) % 32)));                                \
        AM_CRITICAL_END                                                             \
    }


//*****************************************************************************
//!
//! @brief Fast GPIO helper macros.
//!
//*****************************************************************************
//
// Define Fast GPIO enable and disable.
//
#define am_hal_gpio_fastgpio_enable(n)  am_hal_gpio_output_tristate_enable(n)
#define am_hal_gpio_fastgpio_disable(n) am_hal_gpio_output_tristate_disable(n)

//
// Macros for accessing Fast GPIO: set, clear, and read.
// The 'n' parameter is the pin number.
// Note - these macros are most efficient if 'n' is a constant value, and
//        of course when compiled with -O3.
//
#define am_hal_gpio_fastgpio_read(n)    ((APBDMA->BBINPUT >> (n & 0x7)) & 0x1)
#define am_hal_gpio_fastgpio_set(n)     (APBDMA->BBSETCLEAR = _VAL2FLD(APBDMA_BBSETCLEAR_SET,   (1 << (n & 0x7))))
#define am_hal_gpio_fastgpio_clr(n)     (APBDMA->BBSETCLEAR = _VAL2FLD(APBDMA_BBSETCLEAR_CLEAR, (1 << (n & 0x7))))
#define am_hal_gpio_fastgpio_setmsk(m)  (APBDMA->BBSETCLEAR = _VAL2FLD(APBDMA_BBSETCLEAR_SET, m))
#define am_hal_gpio_fastgpio_clrmsk(m)  (APBDMA->BBSETCLEAR = _VAL2FLD(APBDMA_BBSETCLEAR_CLEAR, m))
#define am_hal_gpio_fastgpio_wrval(val) (APBDMA->BBSETCLEAR =               \
                                (_VAL2FLD(APBDMA_BBSETCLEAR_SET, val)   |   \
                                 _VAL2FLD(APBDMA_BBSETCLEAR_CLEAR, val ^ 0xFF)))


#ifdef __cplusplus
}
#endif

#endif  // AM_HAL_GPIO_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
