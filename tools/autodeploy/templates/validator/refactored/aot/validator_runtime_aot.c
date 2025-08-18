// C glue for Helios-AOT runtime. Implements ns_validator_rt_api_t.
#include <stdint.h>
#include <string.h>
#include "validator_runtime_iface.h"
#include "../mut_model_metadata.h"
#include "ns_timer.h"

#if NS_AD_AOT != 1
#warning "validator_runtime_aot.c compiled while NS_AD_AOT != 1"
#endif

// AOT generated headers
#include "NS_AD_NAME_model.h"            // model types/macros
// Some builds also provide an explicit API header; guard its availability
#ifdef __has_include
#  if __has_include("NS_AD_NAME_AOT_api.h")
#    include "NS_AD_NAME_AOT_api.h"
#  endif
#endif

#define AOT_MAX_LAYERS  (TFLM_VALIDATOR_MAC_ESTIMATE_COUNT)

#ifdef AM_PART_APOLLO5B
// PMU accumulator support for per-op callbacks
#include "ns_pmu_accumulator.h"
#include "ns_pmu_map.h"
#define NS_PMU_MAP_ENTRIES NS_PMU_MAP_SIZE
#endif

// Input/output backing storage (single IO tensors per current generator)
static NS_AD_INPUT_TENSOR_TYPE  s_in_buf[NS_AD_INPUT_TENSOR_LEN];
static NS_AD_OUTPUT_TENSOR_TYPE s_out_buf[NS_AD_OUTPUT_TENSOR_LEN];

static NS_AD_NAME_model_context_t s_ctx = {
  .input_data  = { s_in_buf },
  .output_data = { s_out_buf },
  .callback    = NULL,
  .user_data   = NULL,
};

// Tick timer passed from the app
static ns_timer_config_t* s_tick = NULL;
// Per-layer timing (microseconds) for the last inference
static uint32_t s_layer_start_us[AOT_MAX_LAYERS];
static uint32_t s_layer_elapsed_us[AOT_MAX_LAYERS];


#ifdef AM_PART_APOLLO5B
// Accumulator matrix for per-op PMU characterization
static uint32_t       s_accm_store[TFLM_VALIDATOR_MAC_ESTIMATE_COUNT * NS_PMU_MAP_ENTRIES];
static ns_pmu_accm_t  s_accm;
#endif

// // Model runtime callback: marks op boundaries for the accumulator
// static void aot_profiler_cb(int32_t op,
//                             NS_AD_NAME_operator_state_e state,
//                             int32_t status,
//                             void *user_data) {
//   (void)status; (void)user_data;
//   if (state == NS_AD_NAME_model_state_started) {
//     ns_pmu_accm_op_begin(s_accm, op);
//   } else if (state == NS_AD_NAME_model_state_finished) {
//     ns_pmu_accm_op_end(s_accm, op);
//   }
// }
// #endif


// Runtime callback: time per layer and (on AP5) PMU op boundaries
static void aot_profiler_cb(int32_t op,
                            NS_AD_NAME_operator_state_e state,
                            int32_t status,
                            void *user_data) {
  (void)status; (void)user_data;
  if ((op < 0) || (op >= (int32_t)AOT_MAX_LAYERS) || (s_tick == NULL)) return;
  if (state == NS_AD_NAME_model_state_started) {
    s_layer_start_us[op] = ns_us_ticker_read(s_tick);
#ifdef AM_PART_APOLLO5B
    ns_pmu_accm_op_begin(s_accm, op);
#endif
  } else if (state == NS_AD_NAME_model_state_finished) {
    uint32_t now = ns_us_ticker_read(s_tick);
    s_layer_elapsed_us[op] += (now - s_layer_start_us[op]);
    ns_lp_printf("layer %d, elapsed %d\n", op, s_layer_elapsed_us[op]);
#ifdef AM_PART_APOLLO5B
    ns_pmu_accm_op_end(s_accm, op);
#endif
  }
}

static int aot_init(uint32_t num_inputs, uint32_t num_outputs, uint32_t profile, uint32_t warmup, ns_timer_config_t *tickTimer){
  (void)num_inputs; (void)num_outputs; (void)profile; (void)warmup;
  (void)num_inputs; (void)num_outputs; (void)profile; (void)warmup;
  s_tick = tickTimer;
  memset(s_layer_elapsed_us, 0, sizeof(s_layer_elapsed_us));
  memset(s_layer_start_us,   0, sizeof(s_layer_start_us));
  // Install the callback before init so the runtime can call it immediately
  s_ctx.callback = aot_profiler_cb;
  int rc = NS_AD_NAME_model_init(&s_ctx);
#ifdef AM_PART_APOLLO5B
  // Create an accumulator matrix and enable callback-based op stamping
  s_accm = ns_pmu_accm_create(TFLM_VALIDATOR_MAC_ESTIMATE_COUNT, NS_PMU_MAP_ENTRIES, s_accm_store);
#endif
  return (rc == 0) ? 0 : -1;
}

static int aot_set_input(uint32_t idx, const void* data, uint32_t len){
  if (idx != 0) return -1; // current AOT glue supports single input
  memcpy((void*)s_in_buf, data, len);
  return 0;
}

static void* aot_map_input(uint32_t idx, uint32_t* cap_out){
  if (idx != 0) return NULL;
  if (cap_out) *cap_out = (uint32_t)(sizeof(s_in_buf));
  return (void*)s_in_buf;
}

static int aot_invoke(void){
  ns_lp_printf("aot_invoke\n");
  // Reset per-layer timing before each run
  memset(s_layer_elapsed_us, 0, sizeof(s_layer_elapsed_us));
#ifdef AM_PART_APOLLO5B
  // Wrap the inference so the accumulator can aggregate properly
  ns_pmu_accm_inference_begin(s_accm);
#endif
  int rc = (NS_AD_NAME_model_run(&s_ctx) == 0) ? 0 : -1;
#ifdef AM_PART_APOLLO5B
  ns_pmu_accm_inference_end(s_accm);
#endif
  return rc;
}

static int aot_get_output(uint32_t idx, void* dst, uint32_t len){
  if (idx != 0) return -1;
  memcpy(dst, (const void*)s_out_buf, len);
  return 0;
}

static const void* aot_map_output(uint32_t idx, uint32_t* size_out){
  if (idx != 0) return NULL;
  if (size_out) *size_out = (uint32_t)sizeof(s_out_buf);
  return (const void*)s_out_buf;
}

static uint32_t aot_arena_used_bytes(void){ return 0; }

static void aot_get_stats_hook(void){
  // For AOT we gather PMU via callback boundaries; nothing to flush here.
  // Leaving this non-NULL tells the upper layer that a stats phase exists.
}

// Accessors used by the AOT validator main to marshal timing results
uint32_t ns_aot_layer_count(void){ return (uint32_t)AOT_MAX_LAYERS; }
const uint32_t* ns_aot_layer_elapsed_us(void){ return s_layer_elapsed_us; }

static const ns_validator_rt_api_t kAPI_AOT = {
  aot_init,
  aot_set_input,
  aot_map_input,
  aot_invoke,
  aot_get_output,
  aot_map_output,
  aot_get_stats_hook,
  aot_arena_used_bytes
};

const ns_validator_rt_api_t* ns_get_runtime_api(void){ return &kAPI_AOT; }