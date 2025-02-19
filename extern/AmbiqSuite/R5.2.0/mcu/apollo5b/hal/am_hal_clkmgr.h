//*****************************************************************************
//
//! @file am_hal_clkmgr.h
//!
//! @brief Clock manager functions that manage system clocks and minimize
//!        power consumption by powering down clocks when possible.
//!
//! @addtogroup clkmgr5b CLKMGR - Clock Manager
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
#ifndef AM_HAL_CLKMGR_H
#define AM_HAL_CLKMGR_H

#ifdef __cplusplus
extern "C"
{
#endif
//
//! Capability switch on whether Clock Manager should manage XTAL_LS as well.
//! Clock manager will still continue to support clock request and release API
//! for XTAL_LS regardless of whether this switch is defined. However, it will
//! only attempt to enable/disable according to clock request if this switch is
//! defined. This switch is disabled by default for now.
//! Note: calls to am_hal_mcuctrl_control() with following eControl value
//!       be removed from BSP/Application before enabling this switch, as clock
//!       manager will take over the management of XTAL_LS:
//!       - AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_ENABLE
//!       - AM_HAL_MCUCTRL_CONTROL_EXTCLK32K_DISABLE
//
// #define AM_HAL_CLKMGR_MANAGE_XTAL_LS

//
//! Default Clock Manager Configuration
//
#define AM_HAL_CLKMGR_DEFAULT_XTAL_HS_MODE          (AM_HAL_CLKMGR_XTAL_HS_MODE_XTAL)
#define AM_HAL_CLKMGR_DEFAULT_XTAL_HS_FREQ_HZ       (24000000)
#define AM_HAL_CLKMGR_DEFAULT_XTAL_LS_MODE          (AM_HAL_CLKMGR_XTAL_LS_MODE_EXT)
#define AM_HAL_CLKMGR_DEFAULT_XTAL_LS_FREQ_HZ       (32768)
#define AM_HAL_CLKMGR_DEFAULT_EXTREF_CLK_FREQ_HZ    (0)

//
//! Identification for clock user
//
typedef enum
{
    AM_HAL_CLKMGR_USER_ID_IOS0,
    AM_HAL_CLKMGR_USER_ID_IOSFD0,
    AM_HAL_CLKMGR_USER_ID_IOSFD1,
    AM_HAL_CLKMGR_USER_ID_IOM0,
    AM_HAL_CLKMGR_USER_ID_IOM1,
    AM_HAL_CLKMGR_USER_ID_IOM2,
    AM_HAL_CLKMGR_USER_ID_IOM3,
    AM_HAL_CLKMGR_USER_ID_IOM4,
    AM_HAL_CLKMGR_USER_ID_IOM5,
    AM_HAL_CLKMGR_USER_ID_IOM6,
    AM_HAL_CLKMGR_USER_ID_IOM7,
    AM_HAL_CLKMGR_USER_ID_UART0,
    AM_HAL_CLKMGR_USER_ID_UART1,
    AM_HAL_CLKMGR_USER_ID_UART2,
    AM_HAL_CLKMGR_USER_ID_UART3,
    AM_HAL_CLKMGR_USER_ID_ADC,
    AM_HAL_CLKMGR_USER_ID_MSPI0,
    AM_HAL_CLKMGR_USER_ID_MSPI1,
    AM_HAL_CLKMGR_USER_ID_MSPI2,
    AM_HAL_CLKMGR_USER_ID_MSPI3,
    AM_HAL_CLKMGR_USER_ID_GFX,
    AM_HAL_CLKMGR_USER_ID_DC,
    AM_HAL_CLKMGR_USER_ID_DSI,
    AM_HAL_CLKMGR_USER_ID_CRYPTO,
    AM_HAL_CLKMGR_USER_ID_SDIO0,
    AM_HAL_CLKMGR_USER_ID_SDIO1,
    AM_HAL_CLKMGR_USER_ID_USB,
    AM_HAL_CLKMGR_USER_ID_OTP,
    AM_HAL_CLKMGR_USER_ID_PDM0,
    AM_HAL_CLKMGR_USER_ID_I2S0,
    AM_HAL_CLKMGR_USER_ID_I2S1,
    AM_HAL_CLKMGR_USER_ID_AUDADC,
    AM_HAL_CLKMGR_USER_ID_WDT,
    AM_HAL_CLKMGR_USER_ID_RTC,
    AM_HAL_CLKMGR_USER_ID_TIMER0,
    AM_HAL_CLKMGR_USER_ID_TIMER1,
    AM_HAL_CLKMGR_USER_ID_TIMER2,
    AM_HAL_CLKMGR_USER_ID_TIMER3,
    AM_HAL_CLKMGR_USER_ID_TIMER4,
    AM_HAL_CLKMGR_USER_ID_TIMER5,
    AM_HAL_CLKMGR_USER_ID_TIMER6,
    AM_HAL_CLKMGR_USER_ID_TIMER7,
    AM_HAL_CLKMGR_USER_ID_TIMER8,
    AM_HAL_CLKMGR_USER_ID_TIMER9,
    AM_HAL_CLKMGR_USER_ID_TIMER10,
    AM_HAL_CLKMGR_USER_ID_TIMER11,
    AM_HAL_CLKMGR_USER_ID_TIMER12,
    AM_HAL_CLKMGR_USER_ID_TIMER13,
    AM_HAL_CLKMGR_USER_ID_TIMER14,
    AM_HAL_CLKMGR_USER_ID_TIMER15,
    AM_HAL_CLKMGR_USER_ID_STIMER,
    AM_HAL_CLKMGR_USER_ID_CLKOUT,
    AM_HAL_CLKMGR_USER_ID_CLKOUT_XTAL,
    AM_HAL_CLKMGR_USER_ID_SYSPLL,
    AM_HAL_CLKMGR_USER_ID_HFRC2,
    AM_HAL_CLKMGR_USER_ID_RESV0,
    AM_HAL_CLKMGR_USER_ID_RESV1,
    AM_HAL_CLKMGR_USER_ID_MAX
} am_hal_clkmgr_user_id_e;

//
//! Identification for clocks to be managed
//
typedef enum
{
    AM_HAL_CLKMGR_CLK_ID_LFRC,
    AM_HAL_CLKMGR_CLK_ID_XTAL_LS,
    AM_HAL_CLKMGR_CLK_ID_XTAL_HS,
    AM_HAL_CLKMGR_CLK_ID_EXTREF_CLK,
    AM_HAL_CLKMGR_CLK_ID_HFRC,
    AM_HAL_CLKMGR_CLK_ID_HFRC2,
    AM_HAL_CLKMGR_CLK_ID_SYSPLL,
    AM_HAL_CLKMGR_CLK_ID_MAX,
} am_hal_clkmgr_clock_id_e;

//
//! XTAL_HS Modes available
//
typedef enum
{
    AM_HAL_CLKMGR_XTAL_HS_MODE_XTAL,
    AM_HAL_CLKMGR_XTAL_HS_MODE_EXT,
} am_hal_clkmgr_xtalhs_mode_e;

//
//! XTAL_LS Modes available
//
typedef enum
{
    AM_HAL_CLKMGR_XTAL_LS_MODE_XTAL,
    AM_HAL_CLKMGR_XTAL_LS_MODE_EXT,
} am_hal_clkmgr_xtalls_mode_e;

//
//! HFRC clock supported clock rates
//
#define AM_HAL_CLKMGR_HFRC_FREQ_FREE_RUN_APPROX_48MHZ   (0)
#define AM_HAL_CLKMGR_HFRC_FREQ_ADJ_48MHZ               (48000000)

//
//! HFRC2 Clock supported clock rates
//
#define AM_HAL_CLKMGR_HFRC2_FREQ_FREE_RUN_APPROX_250MHZ  (0)
#define AM_HAL_CLKMGR_HFRC2_FREQ_ADJ_250MHZ              (250000000)
#define AM_HAL_CLKMGR_HFRC2_FREQ_ADJ_196P608MHZ          (196608000)


//
//! Struct for clock manager board info for XTAL_HS
//
typedef struct
{
    am_hal_clkmgr_xtalhs_mode_e     eXtalHsMode;
    uint32_t                        ui32XtalHsFreq;
} am_hal_clkmgr_board_info_xtalhs_t;

//
//! Struct for clock manager board info for XTAL_LS
//
typedef struct
{
    am_hal_clkmgr_xtalls_mode_e     eXtalLsMode;
    uint32_t                        ui32XtalLsFreq;
} am_hal_clkmgr_board_info_xtalls_t;

//
//! Struct for Clock Manager Configurations
//
typedef struct
{
    am_hal_clkmgr_board_info_xtalhs_t   sXtalHs;
    am_hal_clkmgr_board_info_xtalls_t   sXtalLs;
    uint32_t                            ui32ExtRefClkFreq;
} am_hal_clkmgr_board_info_t;

//
//! Struct for clock configuration. This struct is used to configure variable
//! clock via am_hal_clkmgr_clock_config()
//
typedef union
{
    // For HFRC clock adjust.
    am_hal_clkgen_hfrcadj_t hfrc;

    // For HFRC2 clock adjust
    am_hal_clkgen_hfrc2adj_t hfrc2;

    // For SystemPLL configuration
    am_hal_syspll_config_t syspll;
} am_hal_clkmgr_clkcfg_t;


//*****************************************************************************
//
//! @brief Set Clock Configuration for a specific clock
//!
//! @param eClockID - am_hal_clkmgr_clock_id_e value that indicates which clock
//!                   to be configured
//! @param ui32RequestedClk - Frequency the clock specified is to be configured
//! @param psClockConfig - [Optional] clock config structure to be used to
//!                        configure the clock specified. set to NULL if
//!                        auto-generation is desired.
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_clock_config(am_hal_clkmgr_clock_id_e eClockId, uint32_t ui32RequestedClk, am_hal_clkmgr_clkcfg_t *psClockConfig);

//*****************************************************************************
//
//! @brief Get Clock Configuration for a specific clock
//!
//! @param eClockID - am_hal_clkmgr_clock_id_e value that indicates which clock
//!                   the config is to be queried
//! @param pui32RequestedClk - pointer to uint32_t variable to store the
//!                            returned value of frequency configred for the
//!                            clock
//! @param psClockCfg - [Optional] pointer to clock config struct for the
//!                     result of the query to be stored. Set to NULL if Clock
//!                     config is not to be queried.
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_clock_config_get(am_hal_clkmgr_clock_id_e eClockId, uint32_t *pui32RequestedClk, am_hal_clkmgr_clkcfg_t *psClockConfig);


//*****************************************************************************
//
//! @brief Request for a clock
//!
//! @param eClockID - am_hal_clkmgr_clock_id_e value that indicates which clock
//!                   the user is requesting
//! @param eUserID - am_hal_clkmgr_user_id_e value that indicates which user is
//!                  requesting the clock
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_clock_request(am_hal_clkmgr_clock_id_e eClockId, am_hal_clkmgr_user_id_e eUserId);

//*****************************************************************************
//
//! @brief Release clock specified for the user specified
//!
//! @param eClockID - am_hal_clkmgr_clock_id_e value that indicates which clock
//!                   the user is releasing
//! @param eUserID - am_hal_clkmgr_user_id_e value that indicates which user is
//!                  releasing the clock
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_clock_release(am_hal_clkmgr_clock_id_e eClockId, am_hal_clkmgr_user_id_e eUserId);

//*****************************************************************************
//
//! @brief Release all clock requested for the user specified
//!
//! @param eUserID - am_hal_clkmgr_user_id_e value that which user is
//!                  releasing the clock
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_clock_release_all(am_hal_clkmgr_user_id_e eUserId);

//*****************************************************************************
//
//! @brief Get clock status
//!
//! @param eClockID - am_hal_clkmgr_clock_id_e value that indicates which clock
//!                   the user is releasing
//! @param pui32UserCount - Pointer to uint32 variable to store the number of
//!                         users currently requested for the clock.
//!                         This parameter is mandatory, caller must pass a
//!                         valid pointer.
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_clock_status_get(am_hal_clkmgr_clock_id_e eClockId, uint32_t *pui32UserCount);

//*****************************************************************************
//
//! @brief Set Clock Manager Configuration
//!
//! @param psBoardInfo - pointer to am_hal_clkmgr_board_info_t struct that
//!                      contains clock manager configuration to be used
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//!
//! psClkMgrCfg consists of 3 configuration parameters, which is epxected to be
//! set in during low_power_init sequence, to provide clock manager with board
//! specific configurations.
//! - sXtalHS: Specifies Frequency in which XTAL_HS (XO32M) is operating at,
//!            as well as XTAL_HS mode which is either XTAL or EXT.
//! - sXtalLS: Specifies Frequency in which XTAL_LS (XO) is operating at,
//!            as well as XTAL_LS mode which is either XTAL or EXT.
//! - ui32ExtRefClkFreq: Frequency of clock source available at GPIO15
//!                      (EXTREF_CLK). Set this value to 0 if EXTREF_CLK is
//!                      not used for the board.
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_board_info_set(am_hal_clkmgr_board_info_t *psBoardInfo);

//*****************************************************************************
//
//! @brief Get Clock Manager Configuration
//!
//! @param psClkMgrCfg - pointer to am_hal_clkmgr_board_info_t struct to return
//!                      clock manager config
//!
//! @return status - Status for the operation, as defined in am_hal_status_e
//
//*****************************************************************************
extern uint32_t am_hal_clkmgr_board_info_get(am_hal_clkmgr_board_info_t *psClkMgrCfg);

#ifdef __cplusplus
}
#endif

#endif // AM_HAL_CLKMGR_H

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************


