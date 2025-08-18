#include <stdint.h>
#include <string.h>

#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"
#include "ns_debug_log.h"

#include "tflm_validator.h"              // ns_incoming_config_t / ns_outgoing_stats_t
#include "mut_model_metadata.h"          // buffer sizes & locations, NS_AD_AOT
#include "validator_mem.h"               // scratch + model/arena helpers

// RPC handlers implemented in validator_rpc.c
status decodeIncomingSendblock(const dataBlock *in);
status decodeIncomingFetchblock(dataBlock *ret);
status infer(const dataBlock *in, dataBlock *res);

// ------------------------- Globals required by RPC --------------------------
NS_SRAM_BSS ns_incoming_config_t mut_cfg;
NS_SRAM_BSS ns_outgoing_stats_t  mut_stats;

// Application-allocated RPC FIFOs
NS_SRAM_BSS uint8_t aot_v_cdc_rx_ff_buf[TFLM_VALIDATOR_RX_BUFSIZE] __attribute__((aligned(4)));
NS_SRAM_BSS uint8_t aot_v_cdc_tx_ff_buf[TFLM_VALIDATOR_TX_BUFSIZE] __attribute__((aligned(4)));

// Provide a stats hook implementation expected by validator_rpc.c
void vrpc_on_after_invoke(void) {
  // For AOT we currently do not collect TFLM profiler events.
  mut_stats.stats.captured_events = 0;
  mut_stats.stats.pmu_count = 0;
  memset(mut_stats.stats.stat_buffer, 0, sizeof(mut_stats.stats.stat_buffer));
  memset(mut_stats.stats.csv_header,  0, sizeof(mut_stats.stats.csv_header));
  // computed_stat_per_event_size set to a benign default
  mut_stats.stats.computed_stat_per_event_size = sizeof(uint32_t) * 16u;
}

int main(void) {
  ns_core_config_t core_cfg = {.api = &ns_core_V1_0_0};
  NS_TRY(ns_core_init(&core_cfg), "Core init failed
");
  NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed
");
  ns_itm_printf_enable();

  ns_mem_init_defaults();

#if (TFLM_MODEL_LOCATION == NS_AD_PSRAM) || (TFLM_ARENA_LOCATION == NS_AD_PSRAM)
  // Initialize PSRAM and inform memory provider
  ns_psram_config_t psram_cfg = {
    .api = &ns_psram_V0_0_1,
    .psram_enable = true,
    .psram_type = PSRAM_TYPE_HEX,
  #ifdef apollo510_evb
    .psram_block = 0,
  #else
    .psram_block = 3,
  #endif
    .psram_size = 0,
    .psram_base_address = 0,
  };
  NS_TRY(ns_psram_init(&psram_cfg), "PSRAM Init Failed");
  ns_mem_set_psram_base((uint8_t*)psram_cfg.psram_base_address);
#endif

  ns_interrupt_master_enable();

#if (NS_VALIDATOR_RPC_TRANSPORT == NS_AD_RPC_TRANSPORT_UART)
  ns_uart_config_t rpcUARTHandle = {
      .api = &ns_uart_V0_0_1,
      .uart_config = NULL,
      .rx_cb = NULL,
      .tx_cb = NULL,
      .tx_blocking = true,
      .rx_blocking = true };
#endif

  ns_rpc_config_t rpc_cfg = {
    .api = &ns_rpc_gdo_V1_1_0,
    .mode = NS_RPC_GENERICDATA_SERVER,
    .rx_buf = aot_v_cdc_rx_ff_buf,
    .rx_bufLength = TFLM_VALIDATOR_RX_BUFSIZE,
    .tx_buf = aot_v_cdc_tx_ff_buf,
    .tx_bufLength = TFLM_VALIDATOR_TX_BUFSIZE,
#if (NS_VALIDATOR_RPC_TRANSPORT == NS_AD_RPC_TRANSPORT_UART)
    .uartHandle = (ns_uart_handle_t)&rpcUARTHandle,
    .transport = NS_RPC_TRANSPORT_UART,
#else
    .uartHandle = NULL,
    .transport = NS_RPC_TRANSPORT_USB,
#endif
    .sendBlockToEVB_cb   = decodeIncomingSendblock,
    .fetchBlockFromEVB_cb= decodeIncomingFetchblock,
    .computeOnEVB_cb     = infer,
  };
  NS_TRY(ns_rpc_genericDataOperations_init(&rpc_cfg), "RPC Init Failed");

  ns_lp_printf("[AOT] Ready to receive RPC Calls");
  while (1) {
    ns_rpc_genericDataOperations_pollServer(&rpc_cfg);
    ns_delay_us(1000);
  }
}