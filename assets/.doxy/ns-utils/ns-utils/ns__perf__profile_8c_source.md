

# File ns\_perf\_profile.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_perf\_profile.c**](ns__perf__profile_8c.md)

[Go to the documentation of this file](ns__perf__profile_8c.md)

```C++


// #if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
// // AP3TODO
// #else
#include "ns_perf_profile.h"
#include "ns_ambiqsuite_harness.h"

uint8_t ns_cache_profiler_init(ns_cache_config_t *cfg) {
    uint8_t status = AM_HAL_STATUS_SUCCESS;
    char dummy = 0;

    if (cfg->enable) {
        am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_MONITOR_ENABLE, (void *)&dummy);
    } else {
        am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_MONITOR_DISABLE, (void *)&dummy);
    }

    return status;
}

void ns_capture_cache_stats(ns_cache_dump_t *dump) {
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    dump->daccess = CACHECTRL->DMON0;
    dump->dtaglookup = CACHECTRL->DMON1;
    dump->dhitslookup = CACHECTRL->DMON2;
    dump->dhitsline = CACHECTRL->DMON3;
    dump->iaccess = CACHECTRL->IMON0;
    dump->itaglookup = CACHECTRL->IMON1;
    dump->ihitslookup = CACHECTRL->IMON2;
    dump->ihitsline = CACHECTRL->IMON3;
#else

    dump->daccess = CPU->DMON0;
    dump->dtaglookup = CPU->DMON1;
    dump->dhitslookup = CPU->DMON2;
    dump->dhitsline = CPU->DMON3;
    dump->iaccess = CPU->IMON0;
    dump->itaglookup = CPU->IMON1;
    dump->ihitslookup = CPU->IMON2;
    dump->ihitsline = CPU->IMON3;
#endif
}

void ns_delta_cache(ns_cache_dump_t *s, ns_cache_dump_t *e, ns_cache_dump_t *d) {
    d->daccess = e->daccess - s->daccess;
    d->dtaglookup = e->dtaglookup - s->dtaglookup;
    d->dhitslookup = e->dhitslookup - s->dhitslookup;
    d->dhitsline = e->dhitsline - s->dhitsline;
    d->iaccess = e->iaccess - s->iaccess;
    d->itaglookup = e->itaglookup - s->itaglookup;
    d->ihitslookup = e->ihitslookup - s->ihitslookup;
    d->ihitsline = e->ihitsline - s->ihitsline;
}

void ns_print_cache_stats(ns_cache_dump_t *dump) {
    ns_lp_printf("****** Dcache Accesses :         %d\r\n", dump->daccess);
    ns_lp_printf("****** Dcache Tag Lookups :      %d\r\n", dump->dtaglookup);
    ns_lp_printf("****** Dcache hits for lookups : %d\r\n", dump->dhitslookup);
    ns_lp_printf("****** Dcache hits for lines :   %d\r\n", dump->dhitsline);
    ns_lp_printf("****** Icache Accesses :         %d\r\n", dump->iaccess);
    ns_lp_printf("****** Icache Tag Lookups :      %d\r\n", dump->itaglookup);
    ns_lp_printf("****** Icache hits for lookups : %d\r\n", dump->ihitslookup);
    ns_lp_printf("****** Icache hits for lines :   %d\r\n", dump->ihitsline);
}

void ns_print_cache_stats_delta(ns_cache_dump_t *start, ns_cache_dump_t *end) {
    ns_lp_printf("****** Delta Dcache Accesses :         %d\r\n", end->daccess - start->daccess);
    ns_lp_printf(
        "****** Delta Dcache Tag Lookups :      %d\r\n", end->dtaglookup - start->dtaglookup);
    ns_lp_printf(
        "****** Delta Dcache hits for lookups : %d\r\n", end->dhitslookup - start->dhitslookup);
    ns_lp_printf(
        "****** Delta Dcache hits for lines :   %d\r\n", end->dhitsline - start->dhitsline);
    ns_lp_printf("****** Delta Icache Accesses :         %d\r\n", end->iaccess - start->iaccess);
    ns_lp_printf(
        "****** Delta Icache Tag Lookups :      %d\r\n", end->itaglookup - start->itaglookup);
    ns_lp_printf(
        "****** Delta Icache hits for lookups : %d\r\n", end->ihitslookup - start->ihitslookup);
    ns_lp_printf(
        "****** Delta Icache hits for lines :   %d\r\n", end->ihitsline - start->ihitsline);
}

void ns_reset_perf_counters(void) {

    DWT->CYCCNT = 0;
    DWT->CPICNT = 0;
    DWT->EXCCNT = 0;
    DWT->SLEEPCNT = 0;
    DWT->LSUCNT = 0;
    DWT->FOLDCNT = 0;
}

void ns_init_perf_profiler(void) {
    DWT->CTRL = 0;
    ns_reset_perf_counters();
}

void ns_start_perf_profiler(void) {
    am_hal_itm_enable();
    // DWT->CTRL = 1;
    ns_delay_us(10000);
    DWT->CTRL = _VAL2FLD(DWT_CTRL_CYCCNTENA, 1) | _VAL2FLD(DWT_CTRL_CPIEVTENA, 1) |
                _VAL2FLD(DWT_CTRL_EXCEVTENA, 1) | _VAL2FLD(DWT_CTRL_SLEEPEVTENA, 1) |
                _VAL2FLD(DWT_CTRL_LSUEVTENA, 1) | _VAL2FLD(DWT_CTRL_FOLDEVTENA, 1) |
                _VAL2FLD(DWT_CTRL_CYCEVTENA, 1);
}

void ns_stop_perf_profiler(void) { DWT->CTRL = 0; }

void ns_capture_perf_profiler(ns_perf_counters_t *c) {
    c->cyccnt = DWT->CYCCNT;
    c->cpicnt = DWT->CPICNT;
    c->exccnt = DWT->EXCCNT;
    c->sleepcnt = DWT->SLEEPCNT;
    c->lsucnt = DWT->LSUCNT;
    c->foldcnt = DWT->FOLDCNT;
}

static uint32_t ns_delta_byte_counter_wrap(uint32_t e, uint32_t s) {
    uint32_t retval = (e < s) ? (e + 256 - s) : e - s;
    return retval;
}

void ns_delta_perf(ns_perf_counters_t *s, ns_perf_counters_t *e, ns_perf_counters_t *d) {
    d->cyccnt = e->cyccnt - s->cyccnt; // 32 bits, probably won't wrap
    d->cpicnt = ns_delta_byte_counter_wrap(e->cpicnt, s->cpicnt);
    d->exccnt = ns_delta_byte_counter_wrap(e->exccnt, s->exccnt);
    d->sleepcnt = ns_delta_byte_counter_wrap(e->sleepcnt, s->sleepcnt);
    d->lsucnt = ns_delta_byte_counter_wrap(e->lsucnt, s->lsucnt);
    d->foldcnt = ns_delta_byte_counter_wrap(e->foldcnt, s->foldcnt);
}

void ns_print_perf_profile(ns_perf_counters_t *c) {
    uint32_t instruction_count;

    instruction_count = c->cyccnt - c->cpicnt - c->exccnt - c->sleepcnt - c->lsucnt + c->foldcnt;

    ns_lp_printf("Summary: %d cycles, %d instructions\n", c->cyccnt, instruction_count);
    ns_lp_printf("Details\n");
    ns_lp_printf("Cycle Count: %d\n", c->cyccnt);
    ns_lp_printf("CPI Count: %d\n", c->cpicnt);
    ns_lp_printf("Exception Entry/Exits Count: %d\n", c->exccnt);
    ns_lp_printf("Sleep Cycles Count: %d\n", c->sleepcnt);
    ns_lp_printf("Load/Store Wait Count: %d\n", c->lsucnt);
    ns_lp_printf("Folded (cycles saved by zero-cycle instructions) Count: %d\n", c->foldcnt);
}
// #endif

```

