/**
 * @file ns_profile.c
 * @author Carlos Morales
 * @brief Collection of performance and power profiling utilities
 * @version 0.1
 * @date 2022-10-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "ns_cache_profile.h"
#include "ns_ambiqsuite_harness.h"

uint8_t
ns_cache_profiler_init(ns_cache_config_t *cfg) {
    uint8_t status = AM_HAL_STATUS_SUCCESS;
    char dummy = 0;

    if (cfg->enable) {
        am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_MONITOR_ENABLE, (void *)&dummy);
    } else {
        am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_MONITOR_DISABLE, (void *)&dummy);
    }

    return status;
}

void
ns_capture_cache_stats(ns_cache_dump_t *dump) {
    dump->daccess = CPU->DMON0;
    dump->dtaglookup = CPU->DMON1;
    dump->dhitslookup = CPU->DMON2;
    dump->dhitsline = CPU->DMON3;
    dump->iaccess = CPU->IMON0;
    dump->itaglookup = CPU->IMON1;
    dump->ihitslookup = CPU->IMON2;
    dump->ihitsline = CPU->IMON3;
}

void
ns_print_cache_stats(ns_cache_dump_t *dump) {
    ns_printf("****** Dcache Accesses :         %d\r\n", dump->daccess);
    ns_printf("****** Dcache Tag Lookups :      %d\r\n", dump->dtaglookup);
    ns_printf("****** Dcache hits for lookups : %d\r\n", dump->dhitslookup);
    ns_printf("****** Dcache hits for lines :   %d\r\n", dump->dhitsline);
    ns_printf("****** Icache Accesses :         %d\r\n", dump->iaccess);
    ns_printf("****** Icache Tag Lookups :      %d\r\n", dump->itaglookup);
    ns_printf("****** Icache hits for lookups : %d\r\n", dump->ihitslookup);
    ns_printf("****** Icache hits for lines :   %d\r\n", dump->ihitsline);
}

void
ns_print_cache_stats_delta(ns_cache_dump_t *start, ns_cache_dump_t *end) {
    ns_printf("****** Delta Dcache Accesses :         %d\r\n", end->daccess - start->daccess);
    ns_printf("****** Delta Dcache Tag Lookups :      %d\r\n", end->dtaglookup - start->dtaglookup);
    ns_printf("****** Delta Dcache hits for lookups : %d\r\n",
              end->dhitslookup - start->dhitslookup);
    ns_printf("****** Delta Dcache hits for lines :   %d\r\n", end->dhitsline - start->dhitsline);
    ns_printf("****** Delta Icache Accesses :         %d\r\n", end->iaccess - start->iaccess);
    ns_printf("****** Delta Icache Tag Lookups :      %d\r\n", end->itaglookup - start->itaglookup);
    ns_printf("****** Delta Icache hits for lookups : %d\r\n",
              end->ihitslookup - start->ihitslookup);
    ns_printf("****** Delta Icache hits for lines :   %d\r\n", end->ihitsline - start->ihitsline);
}
