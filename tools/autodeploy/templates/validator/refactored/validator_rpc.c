/*
 * ============================================================================
 * validator_rpc.c — Call flow overview & interaction with validator.py
 * ============================================================================
 *
 * This file implements the EVB-side RPC glue used by the Python tool
 * tools/autodeploy/validator.py. The Python script drives the EVB using three
 * RPCs exposed by the Generic Data Operations service:
 *
 *   - sendBlockToEVB(dataBlock in)         → decodeIncomingSendblock()
 *   - fetchBlockFromEVB(out dataBlock ret) → decodeIncomingFetchblock()
 *   - computeOnEVB(dataBlock in, out dataBlock ret) → infer()
 *
 * The wire object is a dataBlock with:
 *   cmd      : semantic opcode (generic_cmd, write_cmd, read)
 *   dType    : element type (we treat payload as bytes)
 *   length   : payload length
 *   buffer   : payload
 *   description : short ASCII tag (e.g., "FullTensor", "PartStats", …)
 *
 * ------------------------- High-level sequence (TFLM/AOT) -------------------
 * 1) (optional) Model Streaming (PSRAM builds only)
 *    validator.py → send_model_to_evb()      → cmd=read
 *       sendBlockToEVB(ModelChunk)           → decodeIncomingSendblock()
 *       -> vrpc_incoming_model_chunk(): writes PSRAM via vrpc_model_write()
 *       Repeats until full model transferred.
 *
 * 2) Configure runtime
 *    validator.py → configModel()            → cmd=generic_cmd
 *       sendBlockToEVB(Config)
 *       → decodeIncomingSendblock()
 *         -> vrpc_configure_model():
 *            - Copies mut_cfg and the variable-length input/output size arrays
 *              into g_in_details[] / g_out_details[].
 *            - Binds the runtime via ns_get_runtime_api() (TFLM or AOT).
 *            - Calls rt->init(num_inputs, num_outputs, profile, warmup, &s_tickTimer).
 *            - Precomputes total output bytes and resets all chunk state.
 *            - Updates mut_stats.stats.computed_arena_size (TFLM) via rt->arena_used_bytes().
 *
 * 3) Inference
 *    validator.py → validateModel()
 *      3a) Input handling
 *          - If the input tensor fits in a single block:
 *              computeOnEVB(cmd=generic_cmd, buffer=<full input>)
 *          - If the input tensor exceeds the RPC payload size:
 *              sendLongInputTensor(): multiple sendBlockToEVB() with cmd=write_cmd
 *                → decodeIncomingSendblock() → vrpc_incoming_tensor_chunk()
 *                  Uses rt->map_input_writable(0, &cap) and appends in place.
 *              Then computeOnEVB(cmd=generic_cmd, buffer=empty) to trigger invoke.
 *
 *      3b) Invoke path (infer())
 *          infer(in, out):
 *            - If in->cmd == write_cmd and an output chunk is in progress,
 *              return the next output chunk (PartTensor/LastTensor).
 *            - Else (normal invoke):
 *                • If the input was not pre-chunked, call rt->set_input(0, …).
 *                • Call rt->invoke().
 *                • If profiling enabled and warmups reached, call rt->get_stats_hook()
 *                  (TFLM: flushes profiler CSV and stops PMU snapshot).
 *                • Call vrpc_on_after_invoke() to translate profiler buffers into
 *                  mut_stats (captured_events, csv_header, etc.).
 *                • Pack outputs:
 *                    – If total output bytes ≤ TX capacity, assemble into TX scratch
 *                      and return a single "FullTensor".
 *                    – Otherwise, stage into a hold buffer and return first "PartTensor",
 *                      initializing g_out_chunk for subsequent chunk reads.
 *
 * 4) Statistics fetch
 *    validator.py → getModelStats()
 *      fetchBlockFromEVB(ret):
 *        → decodeIncomingFetchblock():
 *            - If no stats chunk is active:
 *                vrpc_get_stats_full(): copies mut_stats.bytes into TX scratch.
 *                Returns "FullStats" if it fits, else "PartStats" and primes
 *                g_out_chunk to continue on subsequent fetches.
 *              When FullStats fit in a single transfer and Full-PMU was requested,
 *              AP5 builds prime the per-layer PMU stream here by setting
 *              mut_stats.stats.pmu_events_per_layer and internal iterators.
 *            - If a stats chunk is active: vrpc_get_stats_chunk() returns the next
 *              "PartStats"/"LastStats".
 *
 * 5) (AP5 only, optional) Full PMU per-layer stream
 *    validator.py → getPMUStats()  (called after FullStats when full_pmu_capture is set)
 *      Repeated fetchBlockFromEVB() calls retrieve per-layer PMU snapshots.
 *      This file primes the PMU stream after a single-packet FullStats response;
 *      platform-specific code (profiler/PMU modules) populates the PMU payload
 *      and tags the response "FullPMUStats".
 *
 * ------------------------------ Chunking notes ------------------------------
 *  - ns_chunk_t tracks progress for both output tensors and stats. The RX/TX
 *    payload ceiling is derived from the scratch buffer size with a safety
 *    margin (VRPC_TX_MARGIN).
 *  - Weak scratch providers (vrpc_tx_scratch / vrpc_out_hold_buf) can be
 *    overridden by validator_mem.c to avoid dynamic allocation and to place
 *    buffers in fast memory.
 *
 * ------------------------------ Runtime binding -----------------------------
 *  - This RPC layer is runtime-agnostic. The concrete runtime (TFLM or AOT)
 *    provides its ns_validator_rt_api_t implementation:
 *       TFLM → validator_runtime_tflm.cc
 *       AOT  → validator_runtime_aot.c
 *    vrpc_configure_model() obtains the vtable via ns_get_runtime_api().
 *
 * Any changes to command semantics or chunking thresholds must stay in sync
 * with validator.py’s calls:
 *   - send_model_to_evb()          ↔ vrpc_incoming_model_chunk()
 *   - configModel()                ↔ vrpc_configure_model()
 *   - sendLongInputTensor()        ↔ vrpc_incoming_tensor_chunk()
 *   - validateModel() (invoke)     ↔ infer()
 *   - getModelStats()              ↔ decodeIncomingFetchblock() / stats helpers
 *   - getPMUStats() (AP5 Full PMU) ↔ PMU priming here; payload filled by PMU code
 */

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
#include "../mut_model_metadata.h"

// PMU helpers (AP5 only)
#ifdef AM_PART_APOLLO5B
#include "ns_pmu_utils.h"
#include "ns_pmu_map.h"
#endif

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

#if NS_AD_AOT == 1
#define VALIDATOR_LAYER_COUNT AOT_NUM_LAYERS
#else
#define VALIDATOR_LAYER_COUNT TFLM_VALIDATOR_MAC_ESTIMATE_COUNT
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


// PMU full-layer mode book-keeping
//  - g_pmu_events_per_layer mirrors mut_stats.stats.pmu_events_per_layer when active
//  - g_pmu_layer_iter is the layer currently being served in FullPMUStats responses
static uint32_t g_pmu_events_per_layer = 0;  // 0 => normal stats mode
static uint32_t g_pmu_layer_iter = 0;

// Details arrays filled at configure time
static ns_incoming_tensor_details_u g_in_details[NS_MAX_INPUT_TENSORS];
static ns_incoming_tensor_details_u g_out_details[NS_MAX_OUTPUT_TENSORS];

// Input staging offset (when chunking directly into mapped input buffer)
static uint32_t g_input_offset = 0;

// Runtime binding
static const ns_validator_rt_api_t* g_rt = NULL;

#ifdef AM_PART_APOLLO5B
// One-time full characterization (ns_characterize_model) after warmups
static bool g_full_characterization_done = false;
static int vrpc_invoke_cb(void){ return (g_rt && g_rt->invoke) ? g_rt->invoke() : -1; }
#endif

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
  for (uint32_t t=0; t<mut_cfg.config.num_output_tensors; ++t){
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
  ns_lp_printf("runtime init done\n");
  // Precompute output total and reset chunk state
  g_out_total_size = vrpc_sum_output_bytes();
  ns_chunk_reset(&g_in_chunk);
  ns_chunk_reset(&g_out_chunk);
  g_input_chunked = false;
  g_output_chunked = false;
  g_input_offset = 0;
  g_warmups_seen = 0;
  ns_lp_printf("reset session state done\n");
  // Update arena usage (TFLM) — AOT may report 0
  mut_stats.stats.computed_arena_size = g_rt->arena_used_bytes ? g_rt->arena_used_bytes() : 0;
  ns_lp_printf("arena used bytes %d\n", mut_stats.stats.computed_arena_size);
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

//   ns_rpc_genericDataOperations_printDatablock(out);
  // If we chunked, prime chunk state for subsequent fetches
  if (to_send < statSize){
    ns_chunk_begin(&g_out_chunk, statSize, txmax);
    // We just sent first chunk
    ns_chunk_advance(&g_out_chunk, to_send);
  } else {
#ifdef AM_PART_APOLLO5B
    // Full stats fit in one block; if Full PMU requested, prime PMU stream now.
    if (mut_cfg.config.full_pmu_stats == 1) {
      mut_stats.stats.pmu_events_per_layer = NS_NUM_PMU_MAP_SIZE;
      g_pmu_events_per_layer = mut_stats.stats.pmu_events_per_layer;
      g_pmu_layer_iter = 0;
    }
#endif
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
  // If that was the final stats chunk and Full PMU is requested, switch to PMU layer stream
#ifdef AM_PART_APOLLO5B
  if (!g_out_chunk.active && (mut_cfg.config.full_pmu_stats == 1)) {
    // Inform host how many PMU counters per layer and prime first layer
    mut_stats.stats.pmu_events_per_layer = NS_NUM_PMU_MAP_SIZE;
    g_pmu_events_per_layer = mut_stats.stats.pmu_events_per_layer;
    g_pmu_layer_iter = 0;
  }
#endif
  return ns_rpc_data_success;
}

// Serve a single layer's PMU counters as "FullPMUStats" (AP5 only)
#ifdef AM_PART_APOLLO5B
static status vrpc_get_pmu_layer(dataBlock* out){
  // Prepare union view for pmu payload
  // Populate CSV header and layer index, then fetch counters
  ns_set_pmu_header();
  memcpy(mut_stats.pmu_stats.csv_header, ns_profiler_pmu_header, sizeof(mut_stats.pmu_stats.csv_header));
  mut_stats.pmu_stats.layer = g_pmu_layer_iter;
  ns_get_layer_counters(
      g_pmu_layer_iter,
      VALIDATOR_LAYER_COUNT,
      TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES,
      mut_stats.pmu_stats.pmu_event_counters);

  // Marshal just the PMU struct portion of the union
  uint32_t pmu_size = (uint32_t)sizeof(mut_stats.pmu_stats);
  uint8_t* payload = vrpc_tx_scratch();
  if (pmu_size > vrpc_tx_scratch_size()) { return ns_rpc_data_failure; }
  memcpy(payload, mut_stats.bytes, pmu_size);
  vrpc_fill_block(out, generic_cmd, payload, pmu_size, "FullPMUStats");

  // Advance layer; stop PMU mode after last layer
  g_pmu_layer_iter++;
  if (g_pmu_layer_iter >= VALIDATOR_LAYER_COUNT){
    g_pmu_layer_iter = 0;
    g_pmu_events_per_layer = 0;
  }
  return ns_rpc_data_success;
}
#endif


status decodeIncomingFetchblock(dataBlock* out){
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

#ifdef AM_PART_APOLLO5B
  // If Full PMU mode is active, serve PMU stats one layer per fetch
  if (g_pmu_events_per_layer != 0){
    // computed_stat_per_event_size already reflects profiler events;
    // pmu_events_per_layer is communicated in the stats payload.
    return vrpc_get_pmu_layer(out);
  }
#endif
  
  if (!g_out_chunk.active) {
    ns_lp_printf("get stats full\n");
    status rc = vrpc_get_stats_full(out);
    return rc;
  }
  status rc = vrpc_get_stats_chunk(out);
  return rc;
}

// -----------------------------------------------------------------------------
// computeOnEVB handler (invoke path + output chunking)
// -----------------------------------------------------------------------------
status infer(const dataBlock* in, dataBlock* out){
//   ns_lp_printf("infer\n");
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

#ifdef AM_PART_APOLLO5B
  // On AP5, if Full PMU is requested, run a one-time full characterization
  // (mirrors ns_characterize_model(tf_invoke) in the original template).
  // Do this after the warmups when profiling is enabled so per-layer data exists.
  if (!g_full_characterization_done &&
      (mut_cfg.config.full_pmu_stats == 1) &&
      (mut_cfg.config.profile_mut == 1) &&
      (g_warmups_seen == mut_cfg.config.profile_warmup)) {
    ns_lp_printf("Running full PMU characterization\n");
    (void)ns_characterize_model(vrpc_invoke_cb);
    g_full_characterization_done = true;
  }
#endif

//   ns_lp_printf("get_stats_hook done\n");
//   ns_lp_printf("Invoke done\n");
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