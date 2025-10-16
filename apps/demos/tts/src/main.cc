#include "tflm_ns_model.h"

static inline uint8_t* tensor_data_u8(TfLiteTensor* t) {
    return reinterpret_cast<uint8_t*>(t->data.raw);
}
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_peripherals_power.h"



#include "tflm_ns_model.h"

// #include "phone2fuse_init.h"
#include "fuse2mel_init.h"

#define INPUT_SIZE_P2F 7936
#define INPUT_SIZE_F2M 76800
#define OUTPUT_SIZE 16
// #include "ref_p2f.h"
#include "ref_f2m.h"


int main(void) {
    ns_core_config_t core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&core_cfg), "Core init failed");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed");
    ns_itm_printf_enable();
    

    ns_model_state_t model_state_f2m;
    ns_model_state_t *pt_f2m = &model_state_f2m;

    // Initialize model

    if (fuse2mel_init(pt_f2m) != 0) {
        ns_lp_printf("Model f2m initialization failed\\n");
        return 1;
    }

    int16_t *ptr = (int16_t*) pt_f2m->interpreter->input(0)->data.i16;

    memcpy(
        ptr, ref_f2m_input, 76800
    );

    ptr = (int16_t*) pt_f2m->interpreter->input(0)->data.i16;
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("input[%d] = %d\n", i, ptr[i]);
    }
    // Run inference
    TfLiteStatus status_f2m = pt_f2m->interpreter->Invoke();
    if (status_f2m != kTfLiteOk) { ns_lp_printf("Inference failed\\n"); return 1; }

    // Get output
    ptr = (int16_t*) pt_f2m->interpreter->output(0)->data.i16;
    ns_lp_printf("f2m output:\n");
    for (int i = 0; i < 10; i++) {
        ns_lp_printf("%d, (out,ref)=%d,%d\n",
            i, ptr[i], ref_f2m_output[i]);
    }
    
    return 0;
}