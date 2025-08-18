// C glue for Helios-AOT runtime. Implements ns_validator_rt_api_t.
#include <stdint.h>
#include <string.h>
#include "validator_runtime_iface.h"
#include "mut_model_metadata.h"

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

// Input/output backing storage (single IO tensors per current generator)
static NS_AD_INPUT_TENSOR_TYPE  s_in_buf[NS_AD_INPUT_TENSOR_LEN];
static NS_AD_OUTPUT_TENSOR_TYPE s_out_buf[NS_AD_OUTPUT_TENSOR_LEN];

static NS_AD_NAME_AOT_model_context_t s_ctx = {
  .input_data  = { s_in_buf },
  .output_data = { s_out_buf },
  .callback    = NULL,
  .user_data   = NULL,
};

static int aot_init(uint32_t num_inputs, uint32_t num_outputs, uint32_t profile, uint32_t warmup){
  (void)num_inputs; (void)num_outputs; (void)profile; (void)warmup;
  return (NS_AD_NAME_AOT_model_init(&s_ctx) == 0) ? 0 : -1;
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
  return (NS_AD_NAME_AOT_model_invoke(&s_ctx) == 0) ? 0 : -1;
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

static const ns_validator_rt_api_t kAPI_AOT = {
  aot_init,
  aot_set_input,
  aot_map_input,
  aot_invoke,
  aot_get_output,
  aot_map_output,
  NULL,                 // no stats hook for AOT today
  aot_arena_used_bytes
};

const ns_validator_rt_api_t* ns_get_runtime_api(void){ return &kAPI_AOT; }