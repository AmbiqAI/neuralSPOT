/**
 * @file ns_pmu_utils.c
 * @author Ambiq
 * @brief A collection of functions to collect and analyze performance data
 * @version 0.1
 * @date 2024-08-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ns_core.h"
#include "ns_pmu_map.h"
#include "ns_pmu_utils.h"
#include <string.h>

const ns_core_api_t ns_pmu_V0_0_1 = {.apiId = NS_PMU_API_ID, .version = NS_PMU_V0_0_1};
const ns_core_api_t ns_pmu_V1_0_0 = {.apiId = NS_PMU_API_ID, .version = NS_PMU_V1_0_0};
const ns_core_api_t ns_pmu_oldest_supported_version = {
    .apiId = NS_PMU_API_ID, .version = NS_PMU_V0_0_1};

const ns_core_api_t ns_pmu_current_version = {
    .apiId = NS_PMU_API_ID, .version = NS_PMU_V1_0_0};

static am_util_pmu_config_t ns_am_pmu_config;
// static am_util_pmu_profiling_t ns_am_pmu_profiling;
static bool ns_pmu_initialized = false;
static bool ns_pmu_profiling = false;
static uint32_t ns_pmu_config_index[8];

//*****************************************************************************
//
// Enable Cycle Counter and Event Counters
//
//*****************************************************************************
static uint32_t
cntr_enable(uint32_t ui32CountersEnable)
{
    if (ui32CountersEnable & (~VALID_PMU_COUNTERS))
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }
    //
    // PMU counters increment if the appropriate bit in PMU_CNTENSET register is set.
    //
    ARM_PMU_CNTR_Enable(ui32CountersEnable);
    return AM_HAL_STATUS_SUCCESS;
}
//*****************************************************************************
//
// Disable Cycle Counter and Event Counters
//
//*****************************************************************************
static uint32_t
cntr_disable(uint32_t ui32CountersDisable)
{
    if (ui32CountersDisable & (~VALID_PMU_COUNTERS))
    {
        return AM_HAL_STATUS_OUT_OF_RANGE;
    }

    ARM_PMU_CNTR_Disable(ui32CountersDisable);
    return AM_HAL_STATUS_SUCCESS;
}

void ns_pmu_reset_counters() {
    ARM_PMU_CYCCNT_Reset();
    ARM_PMU_EVCNTR_ALL_Reset();
    ARM_PMU_Set_CNTR_OVS(VALID_PMU_OVSCLRS);
}


int ns_pmu_get_map_index(uint32_t eventId) {
    // iterate over the length of ns_pmu_map until we find the eventId
    for (int i = 0; i<sizeof(ns_pmu_map); i++) {
        // ns_lp_printf("Checking %d for 0x%x match\n", i, eventId);
        if (ns_pmu_map[i].eventId == eventId) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Initialize and start the PMU profiling syste
 * 
 * @param cfg 
 * @return uint32_t status
 */

uint32_t ns_pmu_init(ns_pmu_config_t *cfg) {
    uint8_t totalCounters = 0;
    uint32_t counterMask = PMU_CNTENSET_CCNTR_ENABLE_Msk;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        ns_lp_printf("Invalid handle\n");
        return NS_STATUS_INVALID_HANDLE;
    }

    // Check API
    if (ns_core_check_api(cfg->api, &ns_pmu_oldest_supported_version, &ns_pmu_current_version) != NS_STATUS_SUCCESS) {
        ns_lp_printf("Invalid API version\n");
        return NS_STATUS_INVALID_VERSION;
    }

    // Check that the total number of counters fits in the PMU
    for (int i = 0; i < 8; i++) {
        if (cfg->events[i].enabled) {
            if (cfg->events[i].counterSize == NS_PMU_EVENT_COUNTER_SIZE_32) {
                totalCounters += 2;
            } else {
                totalCounters++;
            }
        }  
    }
    if (totalCounters > 8) {
        ns_lp_printf("Too many counters enabled tc is %d\n", totalCounters);
        return NS_STATUS_INVALID_CONFIG;
    }
#endif

    // Init counter state and ns_am_pmu_config
    for (int i = 0; i < 8; i++) {
        cfg->counter[i].counterValue = 0;
        cfg->counter[i].added = false;
        if (cfg->events[i].enabled) {
            cfg->counter[i].mapIndex = ns_pmu_get_map_index(cfg->events[i].eventId);
            if (cfg->counter[i].mapIndex == -1) {
                ns_lp_printf("Invalid event id %d\n", cfg->events[i].eventId);
                return NS_STATUS_INVALID_CONFIG;
            }
            // ns_lp_printf("%d. Adding eventid 0x%d Counter %s (%s) mapIndex: %d\n", i, cfg->events[i].eventId, ns_pmu_map[cfg->counter[i].mapIndex].regname, ns_pmu_map[cfg->counter[i].mapIndex].description, cfg->counter[i].mapIndex);
        }
        ns_am_pmu_config.ui32EventType[i] = 0xffff;
        ns_am_pmu_config.ui32Counters = 0;
    }

    // Create a am_util_pmu_config_t to pass to the am_util_pmu_init function
    // Pack the 32 bit counters to the front of the array
    totalCounters = 0;
    for (int i = 0; i < 8; i++) {
        if ((cfg->events[i].enabled) && (!cfg->counter[i].added) && (cfg->events[i].counterSize == NS_PMU_EVENT_COUNTER_SIZE_32)) {
            counterMask |= (1 << totalCounters);
            ns_pmu_config_index[totalCounters] = i; // map the pmu counter to the cf counter index
            ns_am_pmu_config.ui32EventType[totalCounters++] = cfg->events[i].eventId;
            counterMask |= (1 << totalCounters);
            ns_am_pmu_config.ui32EventType[totalCounters++] = ARM_PMU_CHAIN;
            cfg->counter[i].added = true;
            // ns_lp_printf("%d. Added 32b PMU event index %d, eventId 0x%x\n", i, totalCounters, cfg->events[i].eventId);

        }
    }

    // Now add in the 16 bit counters
    for (int i = 0; i < 8; i++) {
        if ((cfg->events[i].enabled) && (!cfg->counter[i].added) && (cfg->events[i].counterSize == NS_PMU_EVENT_COUNTER_SIZE_16)) {
            counterMask |= (1 << totalCounters);
            // cfg->counter[i].pmuIndex = totalCounters;
            ns_pmu_config_index[totalCounters] = i; // map the pmu counter to the cf counter index
            ns_am_pmu_config.ui32EventType[totalCounters++] = cfg->events[i].eventId;
            cfg->counter[i].added = true;
            // ns_lp_printf("%d. Added 16b PMU event index %d, eventId 0x%x\n", i, totalCounters, cfg->events[i].eventId);
        }
    }

    ns_am_pmu_config.ui32Counters = counterMask;

    am_util_pmu_init(&ns_am_pmu_config);
    ns_pmu_initialized = true;
    ns_pmu_profiling = true;

    return NS_STATUS_SUCCESS;
}

/**
 * @brief Get PMU counter values
 * 
 */

uint32_t ns_pmu_get_counters(ns_pmu_config_t *cfg) {
    // Adapted from am_util_pmu_get_profiling
    uint32_t value;

    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }
    if (!ns_pmu_initialized) {
        return NS_STATUS_INIT_FAILED;
    }

    cntr_disable(ns_am_pmu_config.ui32Counters);

    // Update cfg->counter for each enabled counter
    for (int pmu_index = 0; pmu_index < 8; pmu_index++) {
        if (ns_am_pmu_config.ui32Counters & (1 << pmu_index)) {
            // Handle chain counters
            uint32_t cfgIndex = ns_pmu_config_index[pmu_index];
            if (cfg->events[cfgIndex].counterSize == NS_PMU_EVENT_COUNTER_SIZE_32) {
                // value = ARM_PMU_Get_EVCNTR(pmu_index);
                uint32_t baseValue = ARM_PMU_Get_EVCNTR(pmu_index);
                uint32_t chainValue = ARM_PMU_Get_EVCNTR(pmu_index + 1);
                cfg->counter[cfgIndex].counterValue = baseValue + (chainValue << 16);
                // ns_lp_printf("%d. Reading 32 bit counter %s (%s): 0x%x\n", pmu_index, ns_pmu_map[cfg->counter[cfgIndex].mapIndex].regname, ns_pmu_map[cfg->counter[cfgIndex].mapIndex].description, cfg->counter[cfgIndex].counterValue);
                pmu_index++;
            } else {
                value = ARM_PMU_Get_EVCNTR(pmu_index);
                cfg->counter[cfgIndex].counterValue = value;
            } 
        }
    }

    ARM_PMU_CYCCNT_Reset();
    ARM_PMU_EVCNTR_ALL_Reset();
    ARM_PMU_Set_CNTR_OVS(VALID_PMU_OVSCLRS);

    cntr_enable(ns_am_pmu_config.ui32Counters);
    return NS_STATUS_SUCCESS;
}

void ns_delta_pmu(ns_pmu_counters_t *s, ns_pmu_counters_t *e, ns_pmu_counters_t *d) {
    for (int i = 0; i < 8; i++) {
        // ns_lp_printf("%d. s %d, e %d, Delta %d\n", i, s->counterValue[i], e->counterValue[i], e->counterValue[i] - s->counterValue[i]);
        d->counterValue[i] = e->counterValue[i] - s->counterValue[i];
    }
}

uint32_t ns_pmu_print_counters(ns_pmu_config_t *cfg) {
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    for (int i = 0; i < 8; i++) {
        if (cfg->events[i].enabled) {
            uint32_t mapIndex = cfg->counter[i].mapIndex;
            // ns_lp_printf("Counter %s (%s): 0x%x\n", ns_pmu_map[mapIndex].regname, ns_pmu_map[mapIndex].description, cfg->counter[i].counterValue);
            ns_lp_printf("%d, \t%s, \t \"%s\"\n", cfg->counter[i].counterValue, ns_pmu_map[mapIndex].regname, ns_pmu_map[mapIndex].description);

        }
    }
    return NS_STATUS_SUCCESS;
}

void ns_pmu_get_name(ns_pmu_config_t *cfg, uint32_t i, char *name) {
    if (cfg == NULL) {
        ns_lp_printf("Invalid handle\n");
        strcpy(name, "Invalid handle");
        return;
    }
    if (cfg->events[i].enabled) {
        uint32_t mapIndex = cfg->counter[i].mapIndex;
        // ns_lp_printf("map index %d\n", mapIndex);
        // ns_lp_printf(" Name %s\n", ns_pmu_map[mapIndex].regname);
        // ns_lp_printf("Counter %s (%s): 0x%x\n", ns_pmu_map[mapIndex].regname, ns_pmu_map[mapIndex].description, cfg->counter[i].counterValue);
        strcpy(name, ns_pmu_map[mapIndex].regname);
    } else {
        ns_lp_printf("Not enabled\n");
        strcpy(name, "Not enabled");
    }
}

void ns_pmu_reset_config(ns_pmu_config_t *cfg) {
    for (int i = 0; i < 8; i++) {
        cfg->events[i].enabled = false;
        cfg->counter[i].counterValue = 0;
        cfg->counter[i].added = false;
    }
}

void ns_pmu_event_create(ns_pmu_event_t *event, uint32_t eventId, ns_pmu_event_counter_size_e counterSize) {
    // ns_lp_printf("Creating event 0x%x\n", eventId);
    event->enabled = true;
    event->eventId = eventId;
    event->counterSize = counterSize;
}

// Characterize a function by repeatedly calling it and checking different PMU counters every time

// int map_index = 0;
void ns_pmu_characterize_function(invoke_fp func, ns_pmu_config_t *cfg) {
    am_util_pmu_enable();

    // Walk through every event counter in PMU map
    for (uint32_t map_index = 0; map_index < NS_PMU_MAP_SIZE; map_index++) {
        // We can do 4 32b counters at a time
        // ns_lp_printf("Characterizing event %d %d\n", map_index, NS_PMU_MAP_SIZE);
        ns_pmu_event_create(&(cfg->events[0]), ns_pmu_map[map_index].eventId, NS_PMU_EVENT_COUNTER_SIZE_32);
        // ns_lp_printf("Characterizing event %d %d %d\n", map_index, map_index%71, NS_PMU_MAP_SIZE);
        ns_pmu_event_create(&(cfg->events[1]), ns_pmu_map[(++map_index)%NS_PMU_MAP_SIZE].eventId, NS_PMU_EVENT_COUNTER_SIZE_32);
        // ns_lp_printf("Characterizing event %d %d\n", map_index, NS_PMU_MAP_SIZE);
        ns_pmu_event_create(&(cfg->events[2]), ns_pmu_map[(++map_index)%NS_PMU_MAP_SIZE].eventId, NS_PMU_EVENT_COUNTER_SIZE_32);
        // ns_lp_printf("Characterizing event %d %d\n", map_index, NS_PMU_MAP_SIZE);
        ns_pmu_event_create(&(cfg->events[3]), ns_pmu_map[(++map_index)%NS_PMU_MAP_SIZE].eventId, NS_PMU_EVENT_COUNTER_SIZE_32);
        // ns_lp_printf("Characterizing event %d %d\n", map_index, NS_PMU_MAP_SIZE);

        ns_pmu_init(cfg);
        // Call the function
        func();
        ns_pmu_get_counters(cfg);
        ns_pmu_print_counters(cfg);
    }
}