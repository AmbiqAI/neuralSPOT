/**
 * @file ns_tempco.h
 * @author your name (you@domain.com)
 * @brief Temperature Compensation
 * @version 0.1
 * @date 2022-11-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef NS_TEMPCO
#define NS_TEMPCO

#ifdef __cplusplus
extern "C"
{
#endif

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ns_ambiqsuite_harness.h"

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
extern uint32_t g_ns_tempco_ui32TempcoIsrRet;
extern uint32_t g_ns_tempco_ui32TempcoADCslot;
extern am_hal_adc_sample_t g_ns_tempco_sSamples[AM_HAL_TEMPCO_NUMSAMPLES];
extern void *g_ns_tempco_ADCHandle;
#define TEMPCO_ADC_TEMPERATURE_SLOT     5

extern uint32_t ns_tempco_init(void);

#ifdef __cplusplus
}
#endif
#endif