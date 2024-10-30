/**
 * @file ns_core.h
 * @author Carlos Morales
 * @brief Code common to all neuralSPOT
 * @version 0.1
 * @date 2022-11-03
 *
 * @copyright Copyright (c) 2022
 *
 * \addtogroup ns-core
 * @{
 *
 */
#ifndef NS_CORE
    #define NS_CORE

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "am_bsp.h"
    #include "am_mcu_apollo.h"
    #include "am_util.h"

typedef struct {
    uint16_t major;
    uint16_t minor;
    uint16_t revision;
} ns_semver_t;

typedef struct {
    uint32_t apiId;
    ns_semver_t version;
} ns_core_api_t;

    #include "ns_ambiqsuite_harness.h"

    #define NS_CORE_V0_0_1                                                                         \
        { .major = 0, .minor = 0, .revision = 1 }
    #define NS_CORE_V1_0_0                                                                         \
        { .major = 1, .minor = 0, .revision = 0 }
    #define NS_CORE_OLDEST_SUPPORTED_VERSION NS_CORE_V0_0_1
    #define NS_CORE_CURRENT_VERSION NS_CORE_V1_0_0

extern const ns_core_api_t ns_core_V0_0_1;
extern const ns_core_api_t ns_core_V1_0_0;
extern const ns_core_api_t ns_core_oldest_supported_version;
extern const ns_core_api_t ns_core_current_version;

    // #define NS_DISABLE_API_VALIDATION
    #define NS_STATUS_SUCCESS 0
    #define NS_STATUS_FAILURE -1
    #define NS_STATUS_INVALID_HANDLE 1
    #define NS_STATUS_INVALID_VERSION 2
    #define NS_STATUS_INVALID_CONFIG 3
    #define NS_STATUS_INIT_FAILED 4

    #define NS_TRY(func, msg)                                                                      \
        if (func) {                                                                                \
            ns_lp_printf(msg);                                                                     \
            ns_core_fail_loop();                                                                   \
        }

    #define NS_TRY_ABORT(func, msg)                                                                \
        if (func) {                                                                                \
            ns_lp_printf(msg);                                                                     \
            ns_lp_printf("\n");                                                                    \
            return NS_STATUS_FAILURE;                                                              \
        }

/**
 * @brief Core config struct
 *
 */
typedef struct {
    const ns_core_api_t *api; ///< Core API version
} ns_core_config_t;

typedef struct {
    ns_core_api_t const *api; // API prefix
    bool initialized;
    bool itmPrintWantsToBeEnabled;  // Desired state for ITM printing
    bool itmPrintCurrentlyEnabled;  // Current state for ITM printing
    bool tempcoWantsToBeEnabled;    // Desired state for TempCo
    bool tempcoCurrentlyEnabled;    // Current state for TempCo
    bool cryptoWantsToBeEnabled;    // Desired state for Crypto
    bool cryptoCurrentlyEnabled;    // Current state for Crypto
    bool uartPrintWantsToBeEnabled; // Desired state for ITM printing
    bool uartPrintCurrentlyEnabled; // Current state for ITM printing
    bool ap3BurstModeInitialized;   // Current state for Apollo3 Burst Mode
} ns_desired_state_t;

/**
 * @brief Collection of ns-wide states and desired states
 *
 */
extern ns_desired_state_t g_ns_state;

extern bool ns_core_initialized();

/**
 * @brief Initialize core state
 *
 * @param c ns_core_config_t indicating the API version
 * @return uint32_t
 */
extern uint32_t ns_core_init(ns_core_config_t *c);

extern uint32_t ns_core_check_api(
    const ns_core_api_t *submitted, const ns_core_api_t *oldest, const ns_core_api_t *newest);

extern void ns_core_fail_loop();

    #ifdef __cplusplus
}
    #endif
#endif
/** @}*/
