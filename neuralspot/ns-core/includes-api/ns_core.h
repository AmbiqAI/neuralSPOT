/**
 * @file ns_core.h
 * @author Carlos Morales
 * @brief Code common to all neuralSPOT
 * @version 0.1
 * @date 2022-11-03
 *
 * @copyright Copyright (c) 2022
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
#include "ns_ambiqsuite_harness.h"

#define NEURALSPOT_VERSION "0.0.1"

typedef struct {
    bool itmPrintWantsToBeEnabled;  ///< Desired state for ITM printing
    bool itmPrintCurrentlyEnabled;  ///< Current state for ITM printing
    bool tempcoWantsToBeEnabled;    ///< Desired state for TempCo
    bool tempcoCurrentlyEnabled;    ///< Current state for TempCo
    bool cryptoWantsToBeEnabled;    ///< Desired state for Crypto
    bool cryptoCurrentlyEnabled;    ///< Current state for Crypto
    bool uartPrintWantsToBeEnabled; ///< Desired state for ITM printing
    bool uartPrintCurrentlyEnabled; ///< Current state for ITM printing
} ns_desired_state_t;

/**
 * @brief Collection of ns-wide states and desired states
 *
 */
extern ns_desired_state_t g_ns_state;

/**
 * @brief Initializes various ns state variables
 *
 */
extern void
ns_core_init();

#ifdef __cplusplus
}
#endif
#endif
