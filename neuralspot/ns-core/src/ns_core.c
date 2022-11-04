/**
 * @file ns_core.c
 * @author Carlos Morales
 * @brief Code common to all neuralSPOT
 * @version 0.1
 * @date 2022-11-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ns_core.h"

ns_desired_state_t g_ns_state;

void ns_core_init() {
    g_ns_state.itmPrintWantsToBeEnabled = false;
    g_ns_state.itmPrintCurrentlyEnabled = false;
    g_ns_state.tempcoWantsToBeEnabled = false;
    g_ns_state.tempcoCurrentlyEnabled = false;
    g_ns_state.cryptoWantsToBeEnabled = true; // reset state of Crypto is 'on'
    g_ns_state.cryptoCurrentlyEnabled = true;
}