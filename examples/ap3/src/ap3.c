#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    ns_core_init(&ns_core_cfg);
    ns_itm_printf_enable();
    ns_lp_printf("Hello World!\n");
    while (1)
        ;
}
