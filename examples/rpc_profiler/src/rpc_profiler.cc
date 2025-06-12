/**
 * @file tflm_validator.cc
 * @author Carlos Morales
 * @brief Shell application for instantiating a TFLM model and allowing a RPC
 * client to set input tensors, invoke() it, and collect output tensors
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstdlib>
#include <cstring>


#include "tflm_validator.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_core.h"
#include "ns_malloc.h"
#include "ns_peripherals_power.h"
#include "ns_uart.h"
#include "ns_rpc_generic_data.h"
#ifdef AM_PART_APOLLO5B
#include "ns_pmu_utils.h"
#include "ns_pmu_map.h"
#endif


#define NS_AD_RPC_TRANSPORT_UART 0
#define NS_AD_RPC_TRANSPORT_USB 1


#define NS_AD_RPC_BUFSIZE 4096
// default RPC buffer sizes
#define TFLM_VALIDATOR_RX_BUFSIZE NS_AD_RPC_BUFSIZE
#define TFLM_VALIDATOR_TX_BUFSIZE NS_AD_RPC_BUFSIZE



#define NS_AD_TRANSPORT NS_AD_RPC_TRANSPORT_USB

#define NS_VALIDATOR_RPC_TRANSPORT NS_AD_TRANSPORT

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = (NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024;
uint8_t ucHeap[(NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024] __attribute__((aligned(4)));
#endif





// Validator Stuff
AM_SHARED_RW ns_incoming_config_t mut_cfg;
AM_SHARED_RW ns_outgoing_stats_t mut_stats;

bool stats_is_chunked = false;
uint32_t stats_offset = 0;
uint32_t stats_remaining = 0;
uint32_t pmu_events_per_layer = 0;

extern ns_pmu_config_t ns_microProfilerPMU;



#ifdef NS_MLPROFILE
// Timer is used for TF profiling
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

#ifdef AM_PART_APOLLO5B
ns_pmu_config_t pmu_cfg;
#endif

#endif


status decodeIncomingSendblock(const dataBlock *in) {
    return ns_rpc_data_success;
}


// Handler for fetchBlockFromEVB, invoked by PC
/**
 * @brief Get the statistics about the last Invoke()
 *
 * @param block - serialized tflm_stats_struct
 * @return status
 */
status getStatistics(dataBlock *res) {

    uint32_t statSize = sizeof(mut_stats.bytes) * sizeof(uint8_t);
    uint32_t bufSize = (statSize <= (TFLM_VALIDATOR_RX_BUFSIZE - 1000))
                           ? statSize
                           : (TFLM_VALIDATOR_RX_BUFSIZE - 1000);



    uint8_t *resultBuffer = (uint8_t *)ns_malloc(bufSize);
    char *msg_store = (char *)ns_malloc(sizeof(char) * 30);
    res->length = bufSize;
    res->dType = uint8_e;
    res->description = msg_store;
    res->cmd = generic_cmd;
    binary_t binaryBlock = {.data = (uint8_t *)resultBuffer, .dataLength = bufSize};
    res->buffer = binaryBlock;

    #if defined(AM_PART_APOLLO3) or defined(AM_PART_APOLLO3P)
    mut_stats.stats.platform = NS_MUT_STATS_PLATFORM_AP3;
    #elif defined(AM_PART_APOLLO4P) or defined (AM_PART_APOLLO4L)
    mut_stats.stats.platform = NS_MUT_STATS_PLATFORM_AP4;
    #elif defined(AM_PART_APOLLO5B)
    mut_stats.stats.platform = NS_MUT_STATS_PLATFORM_AP5;
    #else
    mut_stats.stats.platform = 0;
    #error "Unknown platform"
    #endif

    mut_stats.stats.pmu_count = NS_PMU_MAP_SIZE;

    mut_stats.stats.computed_stat_buffer_size = sizeof(mut_stats.bytes);

    // ns_lp_printf("MUT csv header: %s\n", mut_stats.stats.csv_header);
    
    init_load();
    ns_pmu_characterize_function(&run_load, &ns_microProfilerPMU, mut_stats.stats.csv_header, mut_stats.stats.pmu_event_counters);

    memcpy(resultBuffer, mut_stats.bytes, bufSize);
    char msg[] = "NSProfileTest\0";
    memcpy(msg_store, msg, sizeof(msg));

    return ns_rpc_data_success;
}

status decodeIncomingFetchblock(dataBlock *ret) {
    return getStatistics(ret);
}


bool full_characterization_done = false;
uint32_t totalSize = 0;
binary_t binaryBlock;

status run_inference(const dataBlock *in, dataBlock *res) {
    const char *msg_store = "this is a no-op";

    // Check the command value and if it is 'write' then send an output tensor chunk
    msg_store = (char *)ns_malloc(sizeof(char) * 30);

    if (in->cmd == write_cmd) {
        return ns_rpc_data_success;
    }

    //ns_pmu_characterize_function(&run_load, &ns_microProfilerPMU, mut_stats.stats.csv_header, mut_stats.stats.pmu_event_counters);

    return ns_rpc_data_success;
}

void ns_preAction(void) { ns_lp_printf("Starting action\n"); }

void ns_postAction(void) { ns_lp_printf("Stopping action\n"); }

uint8_t tflm_v_cdc_rx_ff_buf[TFLM_VALIDATOR_RX_BUFSIZE] __attribute__((aligned(4)));
uint8_t tlfm_v_cdc_tx_ff_buf[TFLM_VALIDATOR_TX_BUFSIZE] __attribute__((aligned(4)));

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};

    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    const ns_power_config_t ns_hp_mode = {
        .api = &ns_power_V1_0_0,
        .eAIPowerMode = NS_MAXIMUM_PERF,
        .bNeedAudAdc = true,
        .bNeedSharedSRAM = true,
        .bNeedCrypto = false,
        .bNeedBluetooth = true,
        .bNeedUSB = true,
        .bNeedIOM = true,
        .bNeedAlternativeUART = true,
        .b128kTCM = false,
        .bEnableTempCo = false,
        .bNeedITM = true,
        .bNeedXtal = true};
    const ns_power_config_t ns_lp_mode = {
        .api = &ns_power_V1_0_0,
        .eAIPowerMode = NS_MINIMUM_PERF,
        .bNeedAudAdc = true,
        .bNeedSharedSRAM = true,
        .bNeedCrypto = false,
        .bNeedBluetooth = true,
        .bNeedUSB = true,
        .bNeedIOM = true,
        .bNeedAlternativeUART = true,
        .b128kTCM = false,
        .bEnableTempCo = false,
        .bNeedITM = true,
        .bNeedXtal = true};
    NS_TRY(ns_power_config(&ns_hp_mode), "Power Init Failed\n");
    ns_itm_printf_enable();

    

#ifdef NS_MLPROFILE
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
#endif

    // ns_lp_printf("Model Address: 0x%x, arena 0x%x\n", mut_model, tensor_arena);
    

    ns_interrupt_master_enable();

    #if (NS_VALIDATOR_RPC_TRANSPORT == NS_AD_RPC_TRANSPORT_UART)
    ns_uart_config_t rpcGenericUARTHandle = {
        .api = &ns_uart_V0_0_1,
        .uart_config = NULL,
        .rx_cb = NULL,
        .tx_cb = NULL,
        .tx_blocking = true,
        .rx_blocking = true};
    #endif

    // Add callbacks to handle incoming requests
    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_1_0,
        .mode = NS_RPC_GENERICDATA_SERVER, // Puts EVB in RPC server mode
        .rx_buf = tflm_v_cdc_rx_ff_buf,
        .rx_bufLength = TFLM_VALIDATOR_RX_BUFSIZE,
        .tx_buf = tlfm_v_cdc_tx_ff_buf,
        .tx_bufLength = TFLM_VALIDATOR_TX_BUFSIZE,
    #if (NS_VALIDATOR_RPC_TRANSPORT == NS_AD_RPC_TRANSPORT_UART)
        nouart
        .uartHandle = (ns_uart_handle_t)&rpcGenericUARTHandle,
    #else
        .uartHandle = NULL,
    #endif
        .sendBlockToEVB_cb = decodeIncomingSendblock,
        .fetchBlockFromEVB_cb = decodeIncomingFetchblock,
        .computeOnEVB_cb = run_inference,
    #if (NS_VALIDATOR_RPC_TRANSPORT == NS_AD_RPC_TRANSPORT_UART)
        .transport = NS_RPC_TRANSPORT_UART
    #else
        .transport = NS_RPC_TRANSPORT_USB
    #endif
    };
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n");


    while (1) {
        ns_rpc_genericDataOperations_pollServer(&rpcConfig);
        ns_delay_us(1000);
        // ns_deep_sleep();
    }
}
