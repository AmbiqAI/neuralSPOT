//*****************************************************************************
//
//! @file am_hal_mcuctrl.h
//!
//! @brief Functions for interfacing with the MCUCTRL.
//!
//! @addtogroup mcuctrl4_ap510L MCUCTRL - MCU Control
//! @ingroup apollo330P_hal
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
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
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

#define APOLLO330P_A0                                                     \
  ((MCUCTRL->CHIPREV  &                                                   \
    (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) ==         \
      (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_A) |       \
        _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))

#define APOLLO330P_GE_A0                                                  \
  ((MCUCTRL->CHIPREV  &                                                   \
    (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >=         \
      (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_A) |       \
        _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))

#define APOLLO330P_GT_A0                                                  \
  ((MCUCTRL->CHIPREV  &                                                   \
    (MCUCTRL_CHIPREV_REVMAJ_Msk | MCUCTRL_CHIPREV_REVMIN_Msk)) >          \
      (_VAL2FLD(MCUCTRL_CHIPREV_REVMAJ, MCUCTRL_CHIPREV_REVMAJ_A) |       \
        _VAL2FLD(MCUCTRL_CHIPREV_REVMIN, MCUCTRL_CHIPREV_REVMIN_REV0)))

#define A0_TRIMVER_1  1
#define A0_TRIMVER_2  2

#define APOLLO330P_A0_TRIMVER1    (APOLLO330P_A0 && (g_ui32TrimVer == A0_TRIMVER_1))
#define APOLLO330P_A0_GE_TRIMVER1 (APOLLO330P_A0 && (g_ui32TrimVer >= A0_TRIMVER_1))
#define APOLLO330P_A0_LE_TRIMVER1 (APOLLO330P_A0 && (g_ui32TrimVer <= A0_TRIMVER_1))

#define APOLLO330P_A0_TRIMVER2    (APOLLO330P_A0 && (g_ui32TrimVer == A0_TRIMVER_2))
#define APOLLO330P_A0_GE_TRIMVER2 (APOLLO330P_A0 && (g_ui32TrimVer >= A0_TRIMVER_2))
#define APOLLO330P_A0_LE_TRIMVER2 (APOLLO330P_A0 && (g_ui32TrimVer <= A0_TRIMVER_2))

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
} am_hal_mcuctrl_control_e;


//**************************************
//! MCUCTRL EXT32K status enumerations
//**************************************
typedef enum
{
    AM_HAL_MCUCTRL_EXT32K_STATUS_OFF,
    AM_HAL_MCUCTRL_EXT32K_STATUS_XTAL,
    AM_HAL_MCUCTRL_EXT32K_STATUS_EXT_CLK,
}am_hal_mcuctrl_ext32k_status_e;

//**************************************
//
//! MCUCTRL info get
//
//**************************************
typedef enum
{
    AM_HAL_MCUCTRL_INFO_FEATURES_AVAIL,
    AM_HAL_MCUCTRL_INFO_DEVICEID
} am_hal_mcuctrl_infoget_e;

//*****************************************************************************
//
//! MCUCTRL SKU/Feature Enums
//
//*****************************************************************************

typedef enum
{
    AM_HAL_MCUCTRL_DTCM_256K
} am_hal_mcuctrl_dtcm_e;

typedef enum
{
    AM_HAL_MCUCTRL_SSRAM_1M,
    AM_HAL_MCUCTRL_SSRAM_1P75M,
} am_hal_mcuctrl_ssram_e;
#define AM_HAL_MCUCTRL_SSRAM_MAX    AM_HAL_MCUCTRL_SSRAM_1P75M

//
//! MRAM Size Setting
//
typedef enum
{
    AM_HAL_MCUCTRL_MRAM_1M,
    AM_HAL_MCUCTRL_MRAM_2M,
} am_hal_mcuctrl_mram_e;

#define AM_HAL_MCUCTRL_MRAM_MAX     AM_HAL_MCUCTRL_MRAM_2M

//
//! MVE Configuration.
//
typedef enum
{
    AM_HAL_MCUCTRL_NO_MVE,
    AM_HAL_MCUCTRL_INT_MVE,
    AM_HAL_MCUCTRL_FLT_MVE
} am_hal_mcuctrl_mve_e;

//*****************************************************************************
//
// MCUCTRL data structures
//
//*****************************************************************************
//**************************************
//
//! MCUCTRL device structure
//
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
    //! SKU.
    //
    uint32_t ui32SKU;

    //
    //! Qualified chip.
    //
    uint32_t ui32Qualified;

    //
    //! NVM Size.
    //
    uint32_t ui32FlashSize;

    //
    //! ITCM Size
    //
    uint32_t ui32ITCMSize;

    //
    //! DTCM Size
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
//
//! MCUCTRL status structure
//
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
//
//! MCUCTRL features available structure
//
//**************************************
typedef struct
{
    am_hal_mcuctrl_dtcm_e       eDTCMSize;          // DTCM Size
    am_hal_mcuctrl_ssram_e      eSharedSRAMSize;    // SSRAM Size
    am_hal_mcuctrl_mram_e       eMRAMSize;          // MRAM Size
    bool                        bDISP;              // DISP
    bool                        bSupportHPMode;     // HP Mode
    bool                        bMIPIDSI;           // MIPI/DSI
    bool                        bGPU;               // GPU
    bool                        bUSB;               // USB
    bool                        bSecBootFeature;    // Secure Boot
    bool                        bRadioDisable;      // SKURADIODISABLE
    bool                        bRadioBTBLE;        // SKURADIOBTBLE
    bool                        bRadioBLE;          // SKURADIOMM
    bool                        bCM4DEBUG;          // CM4 SWD
    bool                        bFPU;               // FPU
    am_hal_mcuctrl_mve_e        eMVECfg;            // MVE
    union                                           // Trim version info
    {
        uint32_t ui32trimver;
        struct
        {
            uint32_t ui8TrimVerMin  : 8;    // [7:0]
            uint32_t ui8TrimVerMaj  : 8;    // [15:8]
            uint32_t bTrimVerValid  : 1;    // [16:16] 1 if version data valid
            uint32_t bTrimVerPCM    : 1;    // [17:17] 1 if PCM numbering
            uint32_t bTrimVerRsvd18 : 6;    // [23:18]
            uint32_t bTrimVerRsvd24 : 8;    // [31:24]
        } trimver_b;
    };
} am_hal_mcuctrl_feature_t;

// ****************************************************************************
//
//! @brief Apply various specific commands/controls on the MCUCTRL module.
//!
//! This function is used to apply various controls to MCUCTRL.
//!
//! @param eControl - One of the following:
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_ENABLE
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_DISABLE
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK48M_KICK_START
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK48M_NORMAL
//!     AM_HAL_MCUCTRL_CONTROL_EXTCLK48M_DISABLE
//! @param pArgs - Pointer to arguments for Control Switch Case
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_control(am_hal_mcuctrl_control_e eControl,
                                       void *pArgs);

// ****************************************************************************
//
//! @brief Get current status of EXTCLK32K.
//!
//! @param peStatus - pointer to enum variable for query result. It will be
//!                   written to one of the followings:
//!                   - AM_HAL_MCUCTRL_EXT32K_STATUS_OFF: EXTCLK32K not enabled by SW
//!                   - AM_HAL_MCUCTRL_EXT32K_STATUS_XTAL: EXTCLK32K enabled in XTAL Mode by SW
//!                   - AM_HAL_MCUCTRL_EXT32K_STATUS_EXT_CLK: EXTCLK32K enabled in EXT_CLK mode by SW
//!
//! @return status      - generic or interface specific status.
//
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_extclk32k_status_get(am_hal_mcuctrl_ext32k_status_e *peStatus);

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
//!
//! @note IMPORTANT - This function requires access to INFO0 and/or INFO1 data.
//! Therefore, if either INFO0 or INFO1 is in OTP, OTP must be powered up prior
//! to calling this function. For more information, see the description for
//! am_hal_info0_read().
//
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_info_get(am_hal_mcuctrl_infoget_e eInfoGet,
                                        void *pInfo);

//*****************************************************************************
//
//! @brief Get the count of Radio Fine Counter
//!
//! @return - the count of Radio Fine Counter with 0.5us precision
//
//*****************************************************************************
extern uint32_t am_hal_mcuctrl_finecnt_get(void);

//*****************************************************************************
//
//! @brief Get the count of Radio CLKN Counter
//!
//! @return - the count of Radio CLKN Counter with 312.5us precision
//
//*****************************************************************************
extern uint32_t am_hal_mcuctrl_clkncnt_get(void);

//*****************************************************************************
//
//! @brief USB PHY LDO (0.9V) Switch Control
//!
//! @param bEnable -  TRUE: Enable  USB PHY LDO (0.9V) power-on
//!                  FALSE: Disable USB PHY LDO (0.9V) power-off.
//!
//
//*****************************************************************************
extern void am_hal_mcuctrl_usb_phy_ldo0p9_enable(bool bEnable);

//*****************************************************************************
//
//! @brief I3C PHY pull down switch
//!
//! @param bEnable -  TRUE: Enable  I3C PHY pull down.
//!                  FALSE: Disable I3C PHY pull down.
//!
//
//*****************************************************************************
extern void am_hal_mcuctrl_i3c_phy_pulldown_enable(bool bEnable);

// ****************************************************************************
//
//! @brief Set Radio subsystem code base address
//!
//! @param ui32Codebase_addr - Radio subsystem code base address from 0x400000
//!                            to 0x5FFFFF(2MB),16Bytes align
//!
//! @return status      - generic or interface specific status.
// ****************************************************************************
extern uint32_t am_hal_mcuctrl_rss_codebase_set(uint32_t ui32Codebase_addr);

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
