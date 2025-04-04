//*****************************************************************************
//
//! @file am_hal_mcuctrl.h
//!
//! @brief Functions for accessing and configuring the MCUCTRL.
//!
//! @addtogroup mcuctrl4 MCU Control (MCUCTRL)
//! @ingroup apollo4hal
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_2_0-0a31245a64 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef AM_HAL_MCUCTRL_H
#define AM_HAL_MCUCTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

#define MCUCTRL_CHIPPN_PARTNUM_PN_M             0xFF000000
#define MCUCTRL_CHIPPN_PARTNUM_PN_S             24

//**********************************************************
//! MCUCTRL XTALHSCAP defaults for Cooper
//! Refer to App Note Apollo4 Blue 32MHz Crystal Calibration
//**********************************************************
#define XTALHSCAP2TRIM_DEFAULT  44
#define XTALHSCAPTRIM_DEFAULT   4

//
// Designate this peripheral.
//
#define AM_APOLLO3_MCUCTRL  1

#define APOLLO4_A                                                      \
  ((MCUCTRL->CHIPREV & MCUCTRL_CHIPREV_REVMAJ_Msk) ==                  \
      _VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_A))

#define APOLLO4_B0                                                     \
  ((MCUCTRL->CHIPREV  &                                                \
    (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) ==      \
      (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
        _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))

#define APOLLO4_B1                                                     \
  ((MCUCTRL->CHIPREV  &                                                \
    (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) ==      \
      (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
        _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV1)))

#define APOLLO4_B2                                                     \
  ((MCUCTRL->CHIPREV  &                                                \
    (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) ==      \
      (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
        _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV2)))

//
// Determine if >= a given revision level.
//
#define APOLLO4_GE_B0                                                       \
        ((MCUCTRL->CHIPREV  &                                               \
           (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >=    \
           (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
            _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))

#define APOLLO4_GE_B1                                                       \
        ((MCUCTRL->CHIPREV  &                                               \
           (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >=    \
           (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
            _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV1)))

#define APOLLO4_GE_B2                                                       \
        ((MCUCTRL->CHIPREV  &                                               \
           (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >=    \
           (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
            _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV2)))

#define APOLLO4_GT_B2                                                       \
        ((MCUCTRL->CHIPREV  &                                               \
           (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >     \
           (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_B) |    \
            _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV2)))

#define APOLLO4_GE_C0                                                       \
        ((MCUCTRL->CHIPREV  &                                               \
           (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >=    \
           (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_C) |    \
            _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))

#define APOLLO4_GT_C0                                                       \
        ((MCUCTRL->CHIPREV  &                                               \
           (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >     \
           (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_C) |    \
            _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))


//*****************************************************************************
//
// MCUCTRL specific definitions.
//
//*****************************************************************************
// Define the size of fields derived from the PARTNUM register
#define AM_HAL_MCUCTRL_CHIPPN_NV_SIZE_N     (_FLD2VAL(MCUCTRL_CHIPPN_MRAMSIZE, 0xFFFFFFFF) + 1)
#define AM_HAL_MCUCTRL_CHIPPN_SRAM_SIZE_N   (_FLD2VAL(MCUCTRL_CHIPPN_SRAMSIZE, 0xFFFFFFFF) + 1)

// Define the size of fields derived from the SKU register
#define AM_HAL_MCUCTRL_SKU_SSRAM_SIZE_N     (_FLD2VAL(MCUCTRL_SKU_SKUSRAMSIZE, 0xFFFFFFFF) + 1)
#define AM_HAL_MCUCTRL_SKU_MRAM_SIZE_N      (_FLD2VAL(MCUCTRL_SKU_SKUMRAMSIZE, 0xFFFFFFFF) + 1)

#define AM_HAL_MCUCTRL_CRYSTAL_IBST_DURATION    500

//*****************************************************************************
//
// MCUCTRL enumerations
//
//*****************************************************************************
//**************************************
//! MCUCTRL control operations
//**************************************
typedef enum
{
    AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_ENABLE,
    AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_DISABLE,
    AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_KICK_START,
    AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_NORMAL,
    AM_HAL_MCUCTRL_CONTROL_EXTCLK32M_DISABLE
} am_hal_mcuctrl_control_e;

//**************************************
//! MCUCTRL info get
//**************************************
typedef enum
{
    AM_HAL_MCUCTRL_INFO_FEATURES_AVAIL,
    AM_HAL_MCUCTRL_INFO_DEVICEID
} am_hal_mcuctrl_infoget_e;

//*****************************************************************************
//
// MCUCTRL SKU/Feature Enums
//
//*****************************************************************************
typedef enum
{
    AM_HAL_MCUCTRL_DTCM_384K
} am_hal_mcuctrl_dtcm_e;

//
// SKU SSRAM size: 0=1MB,   1=2MB, 2=1MB,   3=2MB
// SKU MRAM  size: 0=0.5MB, 1=1MB, 2=1.5MB, 3=2MB,
//
typedef enum
{
    AM_HAL_MCUCTRL_SSRAM_1M_PLUS_EXT0,  // 1024KB SSRAM + 384KB Ext
    AM_HAL_MCUCTRL_SSRAM_2M_PLUS_EXT1,  // 2048KB SSRAM + 384KB Ext
    AM_HAL_MCUCTRL_SSRAM_1M_PLUS_EXT,   // 1024KB SSRAM + 384KB Ext
    AM_HAL_MCUCTRL_SSRAM_2M_PLUS_EXT,   // 2048KB SSRAM + 384KB Ext
} am_hal_mcuctrl_ssram_e;
#define AM_HAL_MCUCTRL_SSRAM_MAX    AM_HAL_MCUCTRL_SSRAM_1M_PLUS_DSP

typedef enum
{
    AM_HAL_MCUCTRL_MRAM_0P5M,
    AM_HAL_MCUCTRL_MRAM_1M,
    AM_HAL_MCUCTRL_MRAM_1P5M,
    AM_HAL_MCUCTRL_MRAM_2M,
} am_hal_mcuctrl_mram_e;
#define AM_HAL_MCUCTRL_MRAM_MAX     AM_HAL_MCUCTRL_MRAM_2M

typedef enum
{
    AM_HAL_MCUCTRL_CM4F_ONLY,
    AM_HAL_MCUCTRL_CM4F_GPDSP,
    AM_HAL_MCUCTRL_CM4F_GPDSP_LLDSP
} am_hal_mcuctrl_dsp_e;

//*****************************************************************************
//
// MCUCTRL data structures
//
//*****************************************************************************
//**************************************
//! MCUCTRL device structure
//**************************************
typedef struct
{
    //
    //! Device part number. (BCD format)
    //
    uint32_t ui32ChipPN;

    //
    //! Unique Chip ID 0.
    //
    uint32_t ui32ChipID0;

    //
    //! Unique Chip ID 1.
    //
    uint32_t ui32ChipID1;

    //
    //! Chip Revision.
    //
    uint32_t ui32ChipRev;

    //
    //! Vendor ID.
    //
    uint32_t ui32VendorID;

    //
    //! SKU (Apollo3).
    //
    uint32_t ui32SKU;

    //
    //! Qualified chip.
    //
    uint32_t ui32Qualified;

    //
    //! Flash Size.
    //
    uint32_t ui32FlashSize;

    //
    //! SRAM Size.
    //! Note: Total onboard SRAM is computed as ui32DTCMSize + ui32SSRAMSize.
    //
    uint32_t ui32DTCMSize;

    //
    //! SSRAM Size.
    //
    uint32_t ui32SSRAMSize;

    //
    //! MRAM Size.
    //
    uint32_t ui32MRAMSize;

    //
    // JEDEC chip info
    //
    uint32_t ui32JedecPN;
    uint32_t ui32JedecJEPID;
    uint32_t ui32JedecCHIPREV;
    uint32_t ui32JedecCID;
} am_hal_mcuctrl_device_t;

//**************************************
//! MCUCTRL status structure
//**************************************
typedef struct
{
    bool        bDebuggerLockout;   // DEBUGGER
    bool        bADCcalibrated;     // ADCCAL
    bool        bBattLoadEnabled;   // ADCBATTLOAD
    uint8_t     bSecBootOnWarmRst;  // BOOTLOADER
    uint8_t     bSecBootOnColdRst;  // BOOTLOADER
} am_hal_mcuctrl_status_t;

//**************************************
//! MCUCTRL features available structure
//**************************************
typedef struct
{
    am_hal_mcuctrl_dtcm_e       eDTCMSize;
    am_hal_mcuctrl_ssram_e      eSharedSRAMSize;
    am_hal_mcuctrl_mram_e       eMRAMSize;
    bool                        bTurboSpot;
    bool                        bDisplayCtrl;
    bool                        bGPU;
    bool                        bUSB;
    bool                        bSecBootFeature;
} am_hal_mcuctrl_feature_t;

//**********************************************************
//! MCUCTRL XTALHSCAP Globals for Cooper Device
//! Refer to App Note Apollo4 Blue 32MHz Crystal Calibration
//**********************************************************
extern uint32_t g_ui32xtalhscap2trim;
extern uint32_t g_ui32xtalhscaptrim;

// ****************************************************************************
//
//! @brief Apply various specific commands/controls on the MCUCTRL module.
//!
//! This function is used to apply various controls to MCUCTRL.
//!
//! @param eControl - One of the following:
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_ENABLE
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_DISABLE
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_control(am_hal_mcuctrl_control_e eControl,
                                       void *pArgs);

// ****************************************************************************
//
//! @brief MCUCTRL status function
//!
//! This function returns current status of the MCUCTRL as obtained from
//! various registers of the MCUCTRL block.
//!
//! @param psStatus - ptr to a status structure to receive the current statuses.
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_status_get(am_hal_mcuctrl_status_t *psStatus);

// ****************************************************************************
//
//! @brief Get information of the given MCUCTRL item.
//!
//! This function returns a data structure of information regarding the given
//! MCUCTRL parameter.
//!
//! @param eInfoGet - One of the following:         Return structure type:
//!     AM_HAL_MCUCTRL_INFO_DEVICEID,               psDevice
//!     AM_HAL_MCUCTRL_INFO_FAULT_STATUS            psFault
//!
//! @param pInfo - A pointer to a structure to receive the return data,
//! the type of which is dependent on the eInfo parameter.
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_info_get(am_hal_mcuctrl_infoget_e eInfoGet,
                                        void *pInfo);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_MCUCTRL_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
