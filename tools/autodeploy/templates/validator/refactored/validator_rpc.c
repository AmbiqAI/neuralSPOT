#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "validator_runtime_iface.h"
#include "validator_chunk.h"

// Ambiq / neuralSPOT includes
#include "ns_ambiqsuite_harness.h"
#include "ns_rpc_generic_data.h"   // dataBlock, enums
#include "ns_debug_log.h"
#include "ns_malloc.h"
#include "../tflm_validator.h"        // ns_incoming_config_t, ns_outgoing_stats_t, tensor detail unions

// -----------------------------------------------------------------------------
// Configuration macros
// -----------------------------------------------------------------------------
#ifndef VRPC_TX_MARGIN
#define VRPC_TX_MARGIN (1000u)   // safety margin under TX buffer size
#endif

#ifndef NS_OUTPUT_TENSOR_BUFFER_SIZE
  #ifdef AM_PART_APOLLO5B
    #define NS_OUTPUT_TENSOR_BUFFER_SIZE 200000u
  #elif defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
    #define NS_OUTPUT_TENSOR_BUFFER_SIZE 20000u
  #else
    #define NS_OUTPUT_TENSOR_BUFFER_SIZE 50000u
  #endif
#endif

// -----------------------------------------------------------------------------
// Weak hooks for memory/scratch access. Provide strong definitions elsewhere
// (e.g., validator_mem.c) to avoid dynamic allocation.
// -----------------------------------------------------------------------------
__attribute__((weak)) uint8_t* vrpc_tx_scratch(void){
  static uint8_t s_tx[NS_OUTPUT_TENSOR_BUFFER_SIZE];
  return s_tx;
}
__attribute__((weak)) uint32_t vrpc_tx_scratch_size(void){ return NS_OUTPUT_TENSOR_BUFFER_SIZE; }

__attribute__((weak)) uint8_t* vrpc_out_hold_buf(void){
  static uint8_t s_hold[NS_OUTPUT_TENSOR_BUFFER_SIZE];
  return s_hold;  // used only when output > tx payload
}
__attribute__((weak)) int vrpc_model_write(uint32_t offset, const void* data, uint32_t len){
  (void)offset; (void)data; (void)len; return -1;  // not supported by default
}

__attribute__((weak)) void vrpc_on_after_invoke(void) {}

static inline uint32_t vrpc_tx_payload_max(void){
  uint32_t sz = vrpc_tx_scratch_size();
  return (sz > VRPC_TX_MARGIN) ? (sz - VRPC_TX_MARGIN) : (sz/2);
}

extern ns_timer_config_t s_tickTimer;

// -----------------------------------------------------------------------------
// Local state
// -----------------------------------------------------------------------------
static ns_chunk_t g_in_chunk;     // input tensor receive (if chunked)
static ns_chunk_t g_out_chunk;    // output tensor send (if chunked)
static bool       g_input_chunked = false;
static bool       g_output_chunked = false;
static uint32_t   g_warmups_seen = 0;
static uint32_t   g_out_total_size = 0;   // total bytes across all outputs

// Details arrays filled at configure time
static ns_incoming_tensor_details_u g_in_details[NS_MAX_INPUT_TENSORS];
static ns_incoming_tensor_details_u g_out_details[NS_MAX_OUTPUT_TENSORS];

// Input staging offset (when chunking directly into mapped input buffer)
static uint32_t g_input_offset = 0;

// PMU full-layer mode book-keeping (delegated to stats path; kept here for API parity)
static uint32_t g_pmu_events_per_layer = 0;  // 0 => normal stats
static uint32_t g_pmu_layer_iter = 0;

// Runtime binding
static const ns_validator_rt_api_t* g_rt = NULL;

// Externs from the existing firmware (declared in header)
extern ns_incoming_config_t mut_cfg;
extern ns_outgoing_stats_t  mut_stats;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
static inline void vrpc_fill_block(dataBlock* blk,
                                   command cmd,
                                   void* payload,
                                   uint32_t len,
                                   const char* desc) {
  blk->cmd = cmd;
  blk->dType = uint8_e;
  blk->length = len;

  // RPC bug workaround: for strings and payload binary structs, eRPC will attempt to free() the memory - this is kind
  // of an ERPC bug IMHO. Nevertheless, strings & structs must be malloc'd using ns_malloc.

  blk->description = (char*)ns_malloc(sizeof(char) * 30);
  memcpy(blk->description, desc, sizeof(char) * 30);
  uint8_t* payload_copy = (uint8_t*)ns_malloc(len);
  memcpy(payload_copy, payload, len);
  binary_t b = { .data = (uint8_t*)payload_copy, .dataLength = len };
  blk->buffer = b;
}

static uint32_t vrpc_sum_output_bytes(void){
  uint32_t total = 0;
  ns_lp_printf("num_output_tensors %d\n", mut_cfg.config.num_output_tensors);
  for (uint32_t t=0; t<mut_cfg.config.num_output_tensors; ++t){
    ns_lp_printf("g_out_details[%d].details.tensorSizeBytes %d\n", t, g_out_details[t].details.tensorSizeBytes);
    total += g_out_details[t].details.tensorSizeBytes;
  }
  return total;
}

static void vrpc_reset_session_state(void){
  ns_chunk_reset(&g_in_chunk);
  ns_chunk_reset(&g_out_chunk);
  g_input_chunked = false;
  g_output_chunked = false;
  g_input_offset = 0;
  g_out_total_size = 0;
  g_pmu_events_per_layer = 0;
  g_pmu_layer_iter = 0;
  g_warmups_seen = 0;
}

// -----------------------------------------------------------------------------
// Configuration path (sendBlock with cmd == generic_cmd)
// -----------------------------------------------------------------------------
static status vrpc_configure_model(const dataBlock* in){
  // Basic size sanity: header + variable-length arrays
  if (in->buffer.dataLength < sizeof(mut_cfg)) {
    ns_lp_printf("[ERROR] Config too small. exp>=%d got=%d\n", (int)sizeof(mut_cfg), in->buffer.dataLength);
    return ns_rpc_data_failure;
  }
  // Copy fixed preamble
  memcpy(&mut_cfg, in->buffer.data, sizeof(mut_cfg));

  uint32_t need = sizeof(mut_cfg) + 4u*(mut_cfg.config.num_input_tensors + mut_cfg.config.num_output_tensors);
  if (in->buffer.dataLength != need){
    ns_lp_printf("[ERROR] Config size mismatch. exp=%u got=%u\n", (unsigned)need, (unsigned)in->buffer.dataLength);
    return ns_rpc_data_failure;
  }

  // Copy variable arrays (sizes only)
  memcpy(g_in_details,  in->buffer.data + sizeof(mut_cfg), 4u * mut_cfg.config.num_input_tensors);
  memcpy(g_out_details, in->buffer.data + sizeof(mut_cfg) + 4u * mut_cfg.config.num_input_tensors, 4u * mut_cfg.config.num_output_tensors);

  // Bind runtime
  g_rt = ns_get_runtime_api();
  if (!g_rt) { ns_lp_printf("[ERROR] Runtime API not linked\n"); return ns_rpc_data_failure; }

  // Initialize runtime
  if (g_rt->init(mut_cfg.config.num_input_tensors,
                 mut_cfg.config.num_output_tensors,
                 mut_cfg.config.profile_mut,
                 mut_cfg.config.profile_warmup,
                 &s_tickTimer) != 0){
    ns_lp_printf("[ERROR] runtime init failed\n");
    return ns_rpc_data_failure;
  }

  // Precompute output total and reset chunk state
  g_out_total_size = vrpc_sum_output_bytes();
  ns_chunk_reset(&g_in_chunk);
  ns_chunk_reset(&g_out_chunk);
  g_input_chunked = false;
  g_output_chunked = false;
  g_input_offset = 0;
  g_warmups_seen = 0;

  // Update arena usage (TFLM) — AOT may report 0
  mut_stats.stats.computed_arena_size = g_rt->arena_used_bytes ? g_rt->arena_used_bytes() : 0;

  // Platform field filled later in getStatistics()
  return ns_rpc_data_success;
}

// -----------------------------------------------------------------------------
// Large tensor input chunking (sendBlock with cmd == write_cmd)
// -----------------------------------------------------------------------------
static status vrpc_incoming_tensor_chunk(const dataBlock* in){
  // Map input 0 writeable and append chunk in place
  uint32_t cap = 0;
  uint8_t* inbuf = (uint8_t*) (g_rt->map_input_writable ? g_rt->map_input_writable(0, &cap) : NULL);
  if (!inbuf || cap < (g_input_offset + in->buffer.dataLength)){
    ns_lp_printf("[ERROR] Runtime input mapping unsupported or too small (cap=%u, need>=%u)\n",
                 (unsigned)cap, (unsigned)(g_input_offset + in->buffer.dataLength));
    return ns_rpc_data_failure;
  }
  memcpy(inbuf + g_input_offset, in->buffer.data, in->buffer.dataLength);
  g_input_offset += in->buffer.dataLength;
  g_input_chunked = true;
  return ns_rpc_data_success;
}

// -----------------------------------------------------------------------------
// Optional PSRAM model chunking (sendBlock with other cmd)
// -----------------------------------------------------------------------------
static status vrpc_incoming_model_chunk(const dataBlock* in){
  int rc = vrpc_model_write(g_input_offset, in->buffer.data, in->buffer.dataLength);
  if (rc != 0){ ns_lp_printf("[ERROR] Model write not supported or failed\n"); return ns_rpc_data_failure; }
  g_input_offset += in->buffer.dataLength;  // reuse offset for model
  return ns_rpc_data_success;
}

// -----------------------------------------------------------------------------
// Public: sendBlock handler (drop-in replacement name)
// -----------------------------------------------------------------------------
status decodeIncomingSendblock(const dataBlock* in){
  ns_lp_printf("decodeIncomingSendblock cmd %d\n", in->cmd);
  if (in->cmd == generic_cmd) return vrpc_configure_model(in);
  if (in->cmd == write_cmd)   return vrpc_incoming_tensor_chunk(in);
  return vrpc_incoming_model_chunk(in);
}

// -----------------------------------------------------------------------------
// fetchBlock handler → serve stats or stats chunk. PMU-per-layer handled by
// higher-level stats module; here we just stream mut_stats.bytes in chunks.
// -----------------------------------------------------------------------------
static status vrpc_get_stats_full(dataBlock* out){
  // Compute maximum payload we will send now
  uint32_t txmax = vrpc_tx_payload_max();
  uint32_t statSize = (uint32_t)sizeof(mut_stats.bytes);
  uint32_t to_send = (statSize <= txmax) ? statSize : txmax;

  uint8_t* payload = vrpc_tx_scratch();
  memcpy(payload, mut_stats.bytes, to_send);

  // Fill out block
  vrpc_fill_block(out, generic_cmd, payload, to_send, (to_send == statSize) ? "FullStats" : "PartStats");
  ns_lp_printf("fill block done\n");
  ns_lp_printf("to_send %d, statSize %d\n", to_send, statSize);
  ns_rpc_genericDataOperations_printDatablock(out);
  // If we chunked, prime chunk state for subsequent fetches
  if (to_send < statSize){
    ns_chunk_begin(&g_out_chunk, statSize, txmax);
    // We just sent first chunk
    ns_chunk_advance(&g_out_chunk, to_send);
  }
  return ns_rpc_data_success;
}

static status vrpc_get_stats_chunk(dataBlock* out){
  uint32_t txmax = vrpc_tx_payload_max();
  uint32_t n = ns_chunk_next(&g_out_chunk);
  uint8_t* payload = vrpc_tx_scratch();
  memcpy(payload, mut_stats.bytes + g_out_chunk.progressed, n);
  vrpc_fill_block(out, write_cmd, payload, n, ns_chunk_done(&g_out_chunk) ? "LastStats" : "PartStats");
  ns_chunk_advance(&g_out_chunk, n);
  return ns_rpc_data_success;
}

status decodeIncomingFetchblock(dataBlock* out){
  ns_lp_printf("decodeIncomingFetchblock\n");
  // Stamp platform & computed sizes each time (keeps parity with original code)
#if defined(AM_PART_APOLLO3) || defined(AM_PART_APOLLO3P)
  mut_stats.stats.platform = NS_MUT_STATS_PLATFORM_AP3;
#elif defined(AM_PART_APOLLO4P) || defined(AM_PART_APOLLO4L)
  mut_stats.stats.platform = NS_MUT_STATS_PLATFORM_AP4;
#elif defined(AM_PART_APOLLO5B)
  mut_stats.stats.platform = NS_MUT_STATS_PLATFORM_AP5;
#else
  mut_stats.stats.platform = 0;
#endif
  mut_stats.stats.computed_stat_buffer_size = sizeof(mut_stats.bytes);

  if (!g_out_chunk.active) {
    ns_lp_printf("get stats full\n");
    status rc = vrpc_get_stats_full(out);
    ns_lp_printf("get stats full done %d\n", rc);
    return rc;
  }
  ns_lp_printf("get stats chunk, g_out_chunk.active %d\n", g_out_chunk.active);
  status rc = vrpc_get_stats_chunk(out);
  ns_lp_printf("get stats chunk done\n");
  return rc;
}

// -----------------------------------------------------------------------------
// computeOnEVB handler (invoke path + output chunking)
// -----------------------------------------------------------------------------
status infer(const dataBlock* in, dataBlock* out){
  ns_lp_printf("infer\n");
  // If host asks for next output chunk (by calling compute with write_cmd)
  if (in->cmd == write_cmd && g_output_chunked && g_out_chunk.active){
    uint32_t n = ns_chunk_next(&g_out_chunk);
    uint8_t* payload = vrpc_tx_scratch();
    memcpy(payload, vrpc_out_hold_buf() + g_out_chunk.progressed, n);
    vrpc_fill_block(out, write_cmd, payload, n, ns_chunk_done(&g_out_chunk) ? "LastTensor" : "PartTensor");
    ns_chunk_advance(&g_out_chunk, n);
    return ns_rpc_data_success;
  }

  // Normal invoke path -------------------------------------------------
  // If input wasn’t chunked, copy entire input now
  if (!g_input_chunked){
    if (g_rt->set_input(0, in->buffer.data, in->buffer.dataLength) != 0){
      ns_lp_printf("[ERROR] set_input failed\n");
      return ns_rpc_data_failure;
    }
  }


  if (g_rt->invoke() != 0){
    static const char err[] = "Invoke failed";
    ns_lp_printf("Invoke failed\n");
    vrpc_fill_block(out, generic_cmd, (void*)err, (uint32_t)sizeof(err), "Invoke failed");
    return ns_rpc_data_failure;
  }
  // Optionally let runtime snapshot stats prior to invoke
  if (g_rt->get_stats_hook && (mut_cfg.config.profile_mut == 1) && (g_warmups_seen == mut_cfg.config.profile_warmup)){
    ns_lp_printf("get_stats_hook\n");
    g_rt->get_stats_hook();
  }
  ns_lp_printf("get_stats_hook done\n");
  ns_lp_printf("Invoke done\n");
  // Prepare output payload ---------------------------------------------
  vrpc_on_after_invoke();
  g_out_total_size = vrpc_sum_output_bytes();
  uint32_t txmax = vrpc_tx_payload_max();

  if (g_out_total_size <= txmax){
    // Pack directly into TX scratch
    uint8_t* payload = vrpc_tx_scratch();
    uint32_t off = 0;
    for (uint32_t t=0; t<mut_cfg.config.num_output_tensors; ++t){
      uint32_t len = g_out_details[t].details.tensorSizeBytes;
      // copy via map if available for speed, else via get_output
      uint32_t map_sz = 0; const void* src = g_rt->map_output_readonly ? g_rt->map_output_readonly(t, &map_sz) : NULL;
      if (src && map_sz >= len) memcpy(payload + off, src, len);
      else if (g_rt->get_output(t, payload + off, len) != 0){ ns_lp_printf("[ERROR] get_output failed\n"); return ns_rpc_data_failure; }
      off += len;
    }
    vrpc_fill_block(out, generic_cmd, payload, g_out_total_size, "FullTensor");
    g_output_chunked = false;
  } else {
    // Stage into hold buffer once, then chunk out
    uint8_t* hold = vrpc_out_hold_buf();
    uint32_t off = 0;
    for (uint32_t t=0; t<mut_cfg.config.num_output_tensors; ++t){
      uint32_t len = g_out_details[t].details.tensorSizeBytes;
      uint32_t map_sz = 0; const void* src = g_rt->map_output_readonly ? g_rt->map_output_readonly(t, &map_sz) : NULL;
      if (src && map_sz >= len) memcpy(hold + off, src, len);
      else if (g_rt->get_output(t, hold + off, len) != 0){ ns_lp_printf("[ERROR] get_output failed\n"); return ns_rpc_data_failure; }
      off += len;
    }
    ns_chunk_begin(&g_out_chunk, g_out_total_size, txmax);
    uint32_t n = ns_chunk_next(&g_out_chunk);
    uint8_t* payload = vrpc_tx_scratch();
    memcpy(payload, hold, n);
    vrpc_fill_block(out, generic_cmd, payload, n, "PartTensor");
    ns_chunk_advance(&g_out_chunk, n);
    g_output_chunked = true;
  }

  g_warmups_seen++;
  g_input_chunked = false;  // reset for next call
  g_input_offset = 0;
  return ns_rpc_data_success;
}