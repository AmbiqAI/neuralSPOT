#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_peripherals_power.h"
#include "tts.h"
#include "fuse2mel_init.h"
#include "phone2fuse_init.h"
#include "tflm_ns_model.h"

// #include "phone2fuse_init.h"

#include "ns_ambiqsuite_harness.h"
#define NUM_TOKENS 31
#define TIMESTEPS 300
#define DIM_FUSE 128
#define BYTES_INPUT_SIZE_F2M 76800
// static ns_model_state_t model_state_p2f;

int main(void) {
    ns_core_config_t core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&core_cfg), "Core init failed");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed");
    ns_itm_printf_enable();

    tts_init();

    // ns_lp_printf("phone2fuse\n");
    // unitTest_phone2fuse();

    // ns_lp_printf("fuse2mel\n");
    // unitTest_fuse2mel();
    
    tts();
    return 0;   
}