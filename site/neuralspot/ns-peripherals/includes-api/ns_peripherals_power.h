/**
 * @file ns_peripherals_power.h
 * @author Carlos Morales
 * @brief NeuralSPOT Power Management Library
 * @version 0.1
 * @date 2022-09-02
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-power
 *  @{
 */

//*****************************************************************************
//
// Copyright (c) 2021, Ambiq Micro, Inc.
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
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef NS_POWER
    #define NS_POWER

    #ifdef __cplusplus
extern "C" {
    #endif
    #include "ns_core.h"
    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"

    #define NS_POWER_V0_0_1                                                                        \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_POWER_V1_0_0                                                                        \
        { .major = 1, .minor = 0, .revision = 0 }

    #define NS_POWER_OLDEST_SUPPORTED_VERSION NS_POWER_V0_0_1
    #define NS_POWER_CURRENT_VERSION NS_POWER_V1_0_0
    #define NS_POWER_API_ID 0xCA0007

extern const ns_core_api_t ns_power_V0_0_1;
extern const ns_core_api_t ns_power_V1_0_0;
extern const ns_core_api_t ns_power_oldest_supported_version;
extern const ns_core_api_t ns_power_current_version;

typedef enum {
    NS_MINIMUM_PERF = 0, ///< slowest clock (96Mhz)
    NS_MEDIUM_PERF = 1,  ///< 96Mhz clock
    NS_MAXIMUM_PERF = 2  ///< 192Mhz clock
} ns_power_mode_e;

/// Power Mode Definitino
typedef struct {
    const ns_core_api_t *api;     ///< API prefix
    ns_power_mode_e eAIPowerMode; ///< CPU power mode (controls clock speed, etc)
    bool bNeedAudAdc;             ///< Prevents AUDADC from being powered off
    bool bNeedSharedSRAM;         ///< Prevents SSRAM from being powered off
    bool bNeedCrypto;             ///< Prevents Crypto from being powered off
    bool bNeedBluetooth;          ///< Prevents BLE from being powered off
    bool bNeedUSB;                ///< Prevents USB from being powered off
    bool bNeedIOM;                ///< Prevents IOMx from being powered off
    bool bNeedAlternativeUART;    ///< for EEMBC Power Control Module and similar
    bool b128kTCM;                ///< Only enable 128k when true, 384k otherwise
    bool bEnableTempCo;           ///< Enable Temperature Compensation
    bool bNeedITM;                ///< Enable Temperature Compensation
    bool bNeedXtal;               ///< Enable XTAL
} ns_power_config_t;

extern const ns_power_config_t ns_development_default; ///< Enables most things
extern const ns_power_config_t ns_debug_default;       ///< Enables all things
extern const ns_power_config_t ns_good_default;  ///< Reasonable settings for more applications
extern const ns_power_config_t ns_mlperf_mode1;  ///< Good power/perf setting
extern const ns_power_config_t ns_mlperf_mode2;  ///< Good power/perf setting
extern const ns_power_config_t ns_mlperf_mode3;  ///< Good power/perf setting
extern const ns_power_config_t ns_audio_default; ///< Good for AI that uses audio peripherals

/**
 * @brief Set SOC Power Mode
 *
 * @param ns_power_config_t Desired power mode
 * @return uint32_t success/failure
 */
extern uint32_t ns_power_config(const ns_power_config_t *);

/**
 * @brief neuralSPOT-aware deep_sleep - turns off certain systems off before sleeping
 * and turns them back upon waking.
 *
 */
extern void ns_deep_sleep(void);

/**
 * @brief Sets CPU frequency to one of the ns_power_modes
 *
 * @param eAIPowerMode
 * @return uint32_t status
 */
uint32_t ns_set_performance_mode(ns_power_mode_e eAIPowerMode);

    #ifdef __cplusplus
}
    #endif
#endif // NS_POWER
/** @}*/
