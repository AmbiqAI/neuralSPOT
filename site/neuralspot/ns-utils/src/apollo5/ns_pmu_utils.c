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
uint32_t g_ns_pmu_map_length;

const ns_pmu_map_t ns_pmu_map[] = {
    {0x0000,"ARM_PMU_SW_INCR","Software update to the PMU_SWINC register, architecturally executed and condition code check pass"},
    {0x0001,"ARM_PMU_L1I_CACHE_REFILL","L1 I-Cache refill"},
    {0x0003,"ARM_PMU_L1D_CACHE_REFILL","L1 D-Cache refill"},
    {0x0004,"ARM_PMU_L1D_CACHE","L1 D-Cache access"},
    {0x0006,"ARM_PMU_LD_RETIRED","Memory-reading instruction architecturally executed and condition code check pass"},
    {0x0007,"ARM_PMU_ST_RETIRED","Memory-writing instruction architecturally executed and condition code check pass"},
    {0x0008,"ARM_PMU_INST_RETIRED","Instruction architecturally executed"},
    {0x0009,"ARM_PMU_EXC_TAKEN","Exception entry"},
    {0x000A,"ARM_PMU_EXC_RETURN","Exception return instruction architecturally executed and the condition code check pass"},
    {0x000C,"ARM_PMU_PC_WRITE_RETIRED","Software change to the Program Counter (PC). Instruction is architecturally executed and condition code check pass"},
    {0x000D,"ARM_PMU_BR_IMMED_RETIRED","Immediate branch architecturally executed"},
    {0x000E,"ARM_PMU_BR_RETURN_RETIRED","Function return instruction architecturally executed and the condition code check pass"},
    {0x000F,"ARM_PMU_UNALIGNED_LDST_RETIRED","Unaligned memory memory-reading or memory-writing instruction architecturally executed and condition code check pass"},
    // {0x0010,"ARM_PMU_BR_MIS_PRED","Mispredicted or not predicted branch speculatively executed"},
    {0x0011,"ARM_PMU_CPU_CYCLES","Cycle"},
    // {0x0012,"ARM_PMU_BR_PRED","Predictable branch speculatively executed"},
    {0x0013,"ARM_PMU_MEM_ACCESS","Data memory access"},
    {0x0014,"ARM_PMU_L1I_CACHE","Level 1 instruction cache access"},
    {0x0015,"ARM_PMU_L1D_CACHE_WB","Level 1 data cache write-back"},
    // {0x0016,"ARM_PMU_L2D_CACHE","Level 2 data cache access"},
    // {0x0017,"ARM_PMU_L2D_CACHE_REFILL","Level 2 data cache refill"},
    // {0x0018,"ARM_PMU_L2D_CACHE_WB","Level 2 data cache write-back"},
    {0x0019,"ARM_PMU_BUS_ACCESS","Bus access"},
    {0x001A,"ARM_PMU_MEMORY_ERROR","Local memory error"},
    // {0x001B,"ARM_PMU_INST_SPEC","Instruction speculatively executed"},
    {0x001D,"ARM_PMU_BUS_CYCLES","Bus cycles"},
    {0x001E,"ARM_PMU_CHAIN","For an odd numbered counter, increment when an overflow occurs on the preceding even-numbered counter on the same PE"},
    {0x001F,"ARM_PMU_L1D_CACHE_ALLOCATE","Level 1 data cache allocation without refill"},
    // {0x0020,"ARM_PMU_L2D_CACHE_ALLOCATE","Level 2 data cache allocation without refill"},
    {0x0021,"ARM_PMU_BR_RETIRED","Branch instruction architecturally executed"},
    {0x0022,"ARM_PMU_BR_MIS_PRED_RETIRED","Mispredicted branch instruction architecturally executed"},
    {0x0023,"ARM_PMU_STALL_FRONTEND","No operation issued because of the frontend"},
    {0x0024,"ARM_PMU_STALL_BACKEND","No operation issued because of the backend"},
    // {0x0027,"ARM_PMU_L2I_CACHE","Level 2 instruction cache access"},
    // {0x0028,"ARM_PMU_L2I_CACHE_REFILL","Level 2 instruction cache refill"},
    // {0x0029,"ARM_PMU_L3D_CACHE_ALLOCATE","Level 3 data cache allocation without refill"},
    // {0x002A,"ARM_PMU_L3D_CACHE_REFILL","Level 3 data cache refill"},
    // {0x002B,"ARM_PMU_L3D_CACHE","Level 3 data cache access"},
    // {0x002C,"ARM_PMU_L3D_CACHE_WB","Level 3 data cache write-back"},
    {0x0036,"ARM_PMU_LL_CACHE_RD","Last level data cache read"}, // maybe
    {0x0037,"ARM_PMU_LL_CACHE_MISS_RD","Last level data cache read miss"}, // maybe, ll cache = l1, so not needed?
    {0x0039,"ARM_PMU_L1D_CACHE_MISS_RD","Level 1 data cache read miss"},
    // {0x003A,"ARM_PMU_OP_COMPLETE","Operation retired"},
    // {0x003B,"ARM_PMU_OP_SPEC","Operation speculatively executed"},
    {0x003C,"ARM_PMU_STALL","Stall cycle for instruction or operation not sent for execution"},
    // {0x003D,"ARM_PMU_STALL_OP_BACKEND","Stall cycle for instruction or operation not sent for execution due to pipeline backend"},
    // {0x003E,"ARM_PMU_STALL_OP_FRONTEND","Stall cycle for instruction or operation not sent for execution due to pipeline frontend"},
    // {0x003F,"ARM_PMU_STALL_OP","Instruction or operation slots not occupied each cycle"},
    {0x0040,"ARM_PMU_L1D_CACHE_RD","Level 1 data cache read"},
    {0x0100,"ARM_PMU_LE_RETIRED","Loop end instruction executed"},
    // {0x0101,"ARM_PMU_LE_SPEC","Loop end instruction speculatively executed"},
    // {0x0104,"ARM_PMU_BF_RETIRED","Branch future instruction architecturally executed and condition code check pass"},
    // {0x0105,"ARM_PMU_BF_SPEC","Branch future instruction speculatively executed and condition code check pass"},
    {0x0108,"ARM_PMU_LE_CANCEL","Loop end instruction not taken"},
    // {0x0109,"ARM_PMU_BF_CANCEL","Branch future instruction not taken"},
    {0x0114,"ARM_PMU_SE_CALL_S","Call to secure function, resulting in Security state change"},
    {0x0115,"ARM_PMU_SE_CALL_NS","Call to non-secure function, resulting in Security state change"},
    // {0x0118,"ARM_PMU_DWT_CMPMATCH0","DWT comparator 0 match"},
    // {0x0119,"ARM_PMU_DWT_CMPMATCH1","DWT comparator 1 match"},
    // {0x011A,"ARM_PMU_DWT_CMPMATCH2","DWT comparator 2 match"},
    // {0x011B,"ARM_PMU_DWT_CMPMATCH3","DWT comparator 3 match"},
    {0x0200,"ARM_PMU_MVE_INST_RETIRED","MVE instruction architecturally executed"},
    // {0x0201,"ARM_PMU_MVE_INST_SPEC","MVE instruction speculatively executed"},
    {0x0204,"ARM_PMU_MVE_FP_RETIRED","MVE floating-point instruction architecturally executed"},
    // {0x0205,"ARM_PMU_MVE_FP_SPEC","MVE floating-point instruction speculatively executed"},
    {0x0208,"ARM_PMU_MVE_FP_HP_RETIRED","MVE half-precision floating-point instruction architecturally executed"},
    // {0x0209,"ARM_PMU_MVE_FP_HP_SPEC","MVE half-precision floating-point instruction speculatively executed"},
    {0x020C,"ARM_PMU_MVE_FP_SP_RETIRED","MVE single-precision floating-point instruction architecturally executed"},
    // {0x020D,"ARM_PMU_MVE_FP_SP_SPEC","MVE single-precision floating-point instruction speculatively executed"},
    {0x0214,"ARM_PMU_MVE_FP_MAC_RETIRED","MVE floating-point multiply or multiply-accumulate instruction architecturally executed"},
    // {0x0215,"ARM_PMU_MVE_FP_MAC_SPEC","MVE floating-point multiply or multiply-accumulate instruction speculatively executed"},
    {0x0224,"ARM_PMU_MVE_INT_RETIRED","MVE integer instruction architecturally executed"},
    // {0x0225,"ARM_PMU_MVE_INT_SPEC","MVE integer instruction speculatively executed"},
    {0x0228,"ARM_PMU_MVE_INT_MAC_RETIRED","MVE multiply or multiply-accumulate instruction architecturally executed"},
    // {0x0229,"ARM_PMU_MVE_INT_MAC_SPEC","MVE multiply or multiply-accumulate instruction speculatively executed"},
    {0x0238,"ARM_PMU_MVE_LDST_RETIRED","MVE load or store instruction architecturally executed"},
    // {0x0239,"ARM_PMU_MVE_LDST_SPEC","MVE load or store instruction speculatively executed"},
    {0x023C,"ARM_PMU_MVE_LD_RETIRED","MVE load instruction architecturally executed"},
    // {0x023D,"ARM_PMU_MVE_LD_SPEC","MVE load instruction speculatively executed"},
    {0x0240,"ARM_PMU_MVE_ST_RETIRED","MVE store instruction architecturally executed"},
    // {0x0241,"ARM_PMU_MVE_ST_SPEC","MVE store instruction speculatively executed"},
    {0x0244,"ARM_PMU_MVE_LDST_CONTIG_RETIRED","MVE contiguous load or store instruction architecturally executed"},
    // {0x0245,"ARM_PMU_MVE_LDST_CONTIG_SPEC","MVE contiguous load or store instruction speculatively executed"},
    {0x0248,"ARM_PMU_MVE_LD_CONTIG_RETIRED","MVE contiguous load instruction architecturally executed"},
    // {0x0249,"ARM_PMU_MVE_LD_CONTIG_SPEC","MVE contiguous load instruction speculatively executed"},
    {0x024C,"ARM_PMU_MVE_ST_CONTIG_RETIRED","MVE contiguous store instruction architecturally executed"},
    // {0x024D,"ARM_PMU_MVE_ST_CONTIG_SPEC","MVE contiguous store instruction speculatively executed"},
    {0x0250,"ARM_PMU_MVE_LDST_NONCONTIG_RETIRED","MVE non-contiguous load or store instruction architecturally executed"},
    // {0x0251,"ARM_PMU_MVE_LDST_NONCONTIG_SPEC","MVE non-contiguous load or store instruction speculatively executed"},
    {0x0254,"ARM_PMU_MVE_LD_NONCONTIG_RETIRED","MVE non-contiguous load instruction architecturally executed"},
    // {0x0255,"ARM_PMU_MVE_LD_NONCONTIG_SPEC","MVE non-contiguous load instruction speculatively executed"},
    {0x0258,"ARM_PMU_MVE_ST_NONCONTIG_RETIRED","MVE non-contiguous store instruction architecturally executed"},
    // {0x0259,"ARM_PMU_MVE_ST_NONCONTIG_SPEC","MVE non-contiguous store instruction speculatively executed"},
    {0x025C,"ARM_PMU_MVE_LDST_MULTI_RETIRED","MVE memory instruction targeting multiple registers architecturally executed"},
    // {0x025D,"ARM_PMU_MVE_LDST_MULTI_SPEC","MVE memory instruction targeting multiple registers speculatively executed"},
    {0x0260,"ARM_PMU_MVE_LD_MULTI_RETIRED","MVE memory load instruction targeting multiple registers architecturally executed"},
    // {0x0261,"ARM_PMU_MVE_LD_MULTI_SPEC","MVE memory load instruction targeting multiple registers speculatively executed"},
    {0x0261,"ARM_PMU_MVE_ST_MULTI_RETIRED","MVE memory store instruction targeting multiple registers architecturally executed"},
    // {0x0265,"ARM_PMU_MVE_ST_MULTI_SPEC","MVE memory store instruction targeting multiple registers speculatively executed"},
    {0x028C,"ARM_PMU_MVE_LDST_UNALIGNED_RETIRED","MVE unaligned memory load or store instruction architecturally executed"},
    // {0x028D,"ARM_PMU_MVE_LDST_UNALIGNED_SPEC","MVE unaligned memory load or store instruction speculatively executed"},
    {0x0290,"ARM_PMU_MVE_LD_UNALIGNED_RETIRED","MVE unaligned load instruction architecturally executed"},
    // {0x0291,"ARM_PMU_MVE_LD_UNALIGNED_SPEC","MVE unaligned load instruction speculatively executed"},
    {0x0294,"ARM_PMU_MVE_ST_UNALIGNED_RETIRED","MVE unaligned store instruction architecturally executed"},
    // {0x0295,"ARM_PMU_MVE_ST_UNALIGNED_SPEC","MVE unaligned store instruction speculatively executed"},
    {0x0298,"ARM_PMU_MVE_LDST_UNALIGNED_NONCONTIG_RETIRED","MVE unaligned noncontiguous load or store instruction architecturally executed"},
    // {0x0299,"ARM_PMU_MVE_LDST_UNALIGNED_NONCONTIG_SPEC","MVE unaligned noncontiguous load or store instruction speculatively executed"},
    {0x02A0,"ARM_PMU_MVE_VREDUCE_RETIRED","MVE vector reduction instruction architecturally executed"},
    // {0x02A1,"ARM_PMU_MVE_VREDUCE_SPEC","MVE vector reduction instruction speculatively executed"},
    {0x02A4,"ARM_PMU_MVE_VREDUCE_FP_RETIRED ","MVE floating-point vector reduction instruction architecturally executed"},
    // {0x02A5,"ARM_PMU_MVE_VREDUCE_FP_SPEC","MVE floating-point vector reduction instruction speculatively executed"},
    {0x02A8,"ARM_PMU_MVE_VREDUCE_INT_RETIRED","MVE integer vector reduction instruction architecturally executed"},
    // {0x02A9,"ARM_PMU_MVE_VREDUCE_INT_SPEC","MVE integer vector reduction instruction speculatively executed"},
    {0x02B8,"ARM_PMU_MVE_PRED","Cycles where one or more predicated beats architecturally executed"},
    {0x02CC,"ARM_PMU_MVE_STALL","Stall cycles caused by an MVE instruction"},
    {0x02CD,"ARM_PMU_MVE_STALL_RESOURCE","Stall cycles caused by an MVE instruction because of resource conflicts"},
    {0x02CE,"ARM_PMU_MVE_STALL_RESOURCE_MEM ","Stall cycles caused by an MVE instruction because of memory resource conflicts"},
    {0x02CF,"ARM_PMU_MVE_STALL_RESOURCE_FP","Stall cycles caused by an MVE instruction because of floating-point resource conflicts"},
    {0x02D0,"ARM_PMU_MVE_STALL_RESOURCE_INT ","Stall cycles caused by an MVE instruction because of integer resource conflicts"},
    {0x02D3,"ARM_PMU_MVE_STALL_BREAK","Stall cycles caused by an MVE chain break"},
    {0x02D4,"ARM_PMU_MVE_STALL_DEPENDENCY","Stall cycles caused by MVE register dependency"},
    {0x4007,"ARM_PMU_ITCM_ACCESS","Instruction TCM access"},
    {0x4008,"ARM_PMU_DTCM_ACCESS","Data TCM access"},
    // {0x4010,"ARM_PMU_TRCEXTOUT0","ETM external output 0"},
    // {0x4011,"ARM_PMU_TRCEXTOUT1","ETM external output 1"},
    // {0x4012,"ARM_PMU_TRCEXTOUT2","ETM external output 2"},
    // {0x4013,"ARM_PMU_TRCEXTOUT3","ETM external output 3"},
    // {0x4018,"ARM_PMU_CTI_TRIGOUT4","Cross-trigger Interface output trigger 4"},
    // {0x4019,"ARM_PMU_CTI_TRIGOUT5","Cross-trigger Interface output trigger 5"},
    // {0x401A,"ARM_PMU_CTI_TRIGOUT6","Cross-trigger Interface output trigger 6"},
    // {0x401B,"ARM_PMU_CTI_TRIGOUT7","Cross-trigger Interface output trigger 7"}
};

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

uint32_t ns_pmu_get_map_length() {
    return sizeof(ns_pmu_map);
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
    g_ns_pmu_map_length = sizeof(ns_pmu_map);
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
    am_util_pmu_enable();
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

