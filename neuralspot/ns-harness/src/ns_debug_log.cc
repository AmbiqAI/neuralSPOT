/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "ns_debug_log.h"
#include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"

#include "ns_ambiqsuite_harness.h"
#include "ns_pmu_map.h"
#include "ns_pmu_utils.h"
#include <cstring>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void
ns_TFDebugLog(const char *s) {
#if defined(NS_MLDEBUG) || defined(NS_MLPROFILE)
    ns_lp_printf("%s", s);
#endif
}

#ifdef NS_MLPROFILE
ns_timer_config_t *ns_microProfilerTimer;
ns_cache_config_t ns_microProfiler_cache_config;
#ifdef AM_PART_APOLLO5B
ns_pmu_config_t ns_microProfilerPMU;
AM_SHARED_RW char ns_profiler_pmu_header[2048];
#define NS_DEBUG_NUM_PMU_COUNTERS sizeof(ns_pmu_map)/sizeof(ns_pmu_map_t)
#endif  // AM_PART_APOLLO5B
AM_SHARED_RW ns_profiler_sidecar_t ns_microProfilerSidecar;
AM_SHARED_RW ns_profiler_event_stats_t ns_profiler_events_stats[NS_PROFILER_RPC_EVENTS_MAX];
AM_SHARED_RW char ns_profiler_csv_header[512];
#endif // NS_MLPROFILE

void
ns_TFDebugLogInit(ns_debug_log_init_t *cfg) {

#if defined(NS_MLDEBUG) || defined(NS_MLPROFILE)
    RegisterDebugLogCallback(ns_TFDebugLog);
#endif

#ifdef NS_MLPROFILE
    ns_microProfilerTimer = cfg->t;
    if (cfg->m != NULL) {
        ns_microProfilerSidecar.has_estimated_macs = true;
        ns_microProfilerSidecar.number_of_layers = cfg->m->number_of_layers;
        ns_microProfilerSidecar.mac_count_map = cfg->m->mac_count_map;
        ns_microProfilerSidecar.m = cfg->m;

        // print out the mac count map
        // ns_lp_printf("est is located at 0x%x\n", ns_microProfilerSidecar.mac_count_map);
        // for (int i = 0; i < 50; i++) {
        //     ns_lp_printf("Layer %d: %d\n", i, ns_microProfilerSidecar.mac_count_map[i]);
        // }
    } else {
        ns_microProfilerSidecar.has_estimated_macs = false;
    }
    // ns_init_perf_profiler();
    // ns_start_perf_profiler();
    #ifdef AM_PART_APOLLO5B
    // The micro_profiler needs access to a global, so copy it in
    if (cfg->pmu != NULL) {
        memcpy(&ns_microProfilerPMU, cfg->pmu, sizeof(ns_pmu_config_t));
        am_util_pmu_enable();
    }
    #else
    ns_microProfiler_cache_config.enable = true;
    ns_cache_profiler_init(&ns_microProfiler_cache_config);
    #endif

#endif
}

/**
 * @brief Given a model, characterize it by running it repeatedly and capturing PMU events.
 * 
 * 
 * @return uint32_t 
 */
uint32_t ns_characterize_model(invoke_fp func) {
    // Repeatedly run the model, capturing different PMU every time.
#ifdef NS_MLPROFILE
#ifdef AM_PART_APOLLO5B
    uint32_t map_index = 0;
    ns_lp_printf("Starting model characterization, capturing %d (%d/%d) PMU events per layer\n", sizeof(ns_pmu_map)/sizeof(ns_pmu_map_t), sizeof(ns_pmu_map), sizeof(ns_pmu_map_t));
    for (map_index = 0; map_index < sizeof(ns_pmu_map)/sizeof(ns_pmu_map_t); map_index = map_index + 4) {
        // Init the pmu capture
        ns_pmu_reset_config(&ns_microProfilerPMU);
        
        for (uint32_t i = 0; i < 4; i++) {
            if (map_index + i >= sizeof(ns_pmu_map)/sizeof(ns_pmu_map_t)) {
                // We've run out of event types, init with event 0
                // ns_lp_printf("Ran out of PMU events at %d, initializing event %d with event 0\n", map_index, i);
                ns_pmu_event_create(&(ns_microProfilerPMU.events[i]), ns_pmu_map[0].eventId, NS_PMU_EVENT_COUNTER_SIZE_32);
            } else {
                // ns_lp_printf("Creating event %d at %d\n", map_index, i);
                ns_pmu_event_create(&(ns_microProfilerPMU.events[i]), ns_pmu_map[map_index + i].eventId, NS_PMU_EVENT_COUNTER_SIZE_32);
            }
        }
        // ns_lp_printf("PMU event %d: %s, %s, %s, %s\n", map_index, ns_pmu_map[map_index].regname, ns_pmu_map[map_index + 1].regname, ns_pmu_map[map_index + 2].regname, ns_pmu_map[map_index + 3].regname);
        ns_pmu_init(&ns_microProfilerPMU);
        // ns_lp_printf("PMU initialized\n");
        // Run the model
        func();
        // ns_lp_printf("Model run complete\n");
        ns_lp_printf(".");
    }
#endif
#endif
    return NS_STATUS_SUCCESS;
}

/**
 * @brief  Searches for the "CALL_ONCE" layer in the global profiler stats array.
 *
 * This function scans through all layers to see if any layer is tagged "CALL_ONCE".
 * If found, the layer index is returned. If not, returns -1.
 *
 * @param[in] num_layers  The total number of layers in the model or pipeline.
 *
 * @return Index of the CALL_ONCE layer if found, otherwise -1.
 */
#ifdef NS_MLPROFILE
#ifdef AM_PART_APOLLO5B 
static int32_t
find_call_once_layer(uint32_t num_layers)
{
   
    for (uint32_t i = 0; i < num_layers; i++)
    {
        if (strcmp(ns_profiler_events_stats[i].tag, "CALL_ONCE") == 0)
        {
            return (int32_t)i;
        }
    }
    return -1;
}
#endif // AM_PART_APOLLO5B
#endif // NS_MLPROFILE

uint32_t ns_set_pmu_header(void) {
    // Set ns_profiler_csv_header to the PMU header

#ifdef NS_MLPROFILE
#ifdef AM_PART_APOLLO5B
    char name[50];
    snprintf(ns_profiler_pmu_header, 2048, "Event,Tag,uSeconds,EST_MAC,MAC_EQ,OUTPUT_MAG,OUTPUT_SHAPE,FILTER_SHAPE,STRIDE_H,STRIDE_W,DILATION_H,DILATION_W");
    // ns_lp_printf("Event,\"Tag\",\"uSeconds\",\"Est MACs\",\"MAC Eq\",\"Output Mag\",\"Output Shape\",\"Filter Shape\", \"Stride H\", \"Stride W\", \"Dilation H\", \"Dilation W\""); 
    for (int map_index = 0; map_index < sizeof(ns_pmu_map)/sizeof(ns_pmu_map_t); map_index++) {
        strcpy(name, ns_pmu_map[map_index].regname);
        snprintf(ns_profiler_pmu_header + strlen(ns_profiler_pmu_header), 2048 - strlen(ns_profiler_pmu_header),
                 ",%s", name);
        // ns_lp_printf(",\"%s\"", name); 
    };
    // ns_lp_printf("\n");
#endif // AM_PART_APOLLO5B
#endif // NS_MLPROFILE

    return NS_STATUS_SUCCESS;
}


/**
 * @brief  Retrieves PMU counters for a given layer.
 *
 * This function determines if the 
 * layer is within a "skip" range for PMU counters (related to CALL_ONCE), then 
 * either returns zeros or calculates the counters based on the "non-obvious" index 
 * formula:
 *
 * \code
 *   index = (num_layers + source_layer) + (source_layer_count) * (map_index / 4);
 *   counterValue = ns_microProfilerSidecar.pmu_snapshot[index].counterValue[ map_index % 4 ];
 * \endcode
 *
 * @param[in]  layer         The layer index for which PMU values are requested.
 * @param[in]  num_layers    Total layers in the pipeline.
 * @param[in]  rv            Number of "resource variables" used to offset certain layers.
 * @param[out] out_counters  Pointer to an array in which to store the retrieved counter values.
 * @param[in]  max_counters  Size of the out_counters array (e.g. number of PMU registers).
 *
 * @return NS_STATUS_SUCCESS on success. Could return an error code if desired.
 */
uint32_t
ns_get_layer_counters(uint32_t layer,
                      uint32_t num_layers,
                      uint32_t rv,
                      uint32_t *out_counters)
{
#ifdef NS_MLPROFILE
#ifdef AM_PART_APOLLO5B
    // 1. Find if a CALL_ONCE layer exists
    int32_t call_once_layer = find_call_once_layer(num_layers);
    // ns_lp_printf("[INFO] NS_GET_LAYER_COUNTERS: CALL_ONCE %d, layer %d num_layers %d, rv %d\n", call_once_layer, layer, num_layers, rv);

    // 2. If found, figure out if the requested layer is in the skip region
    //    The skip region is: call_once_layer < layer <= (call_once_layer + rv*2)
    //    as per the snippet logic
    if (call_once_layer != -1 &&
        (layer > (uint32_t)call_once_layer) &&
        (layer <= (uint32_t)(call_once_layer + rv * 2)))
    {
        //  Return all zeros for these "skipped" layers
        for (uint32_t i = 0; i < NS_DEBUG_NUM_PMU_COUNTERS; i++)
        {
            out_counters[i] = 0;
        }
        // ns_lp_printf("Layer %d is in the skip region, returning zeros\n", layer);
        return NS_STATUS_SUCCESS;
    }

    // 3. Otherwise, proceed with the "source layer" calculation
    //    We also define source_layer_count = num_layers - rv*2
    uint32_t source_layer_count = num_layers - rv * 2;

    // 4. Determine the actual "source_layer" for reading PMU counters
    //    (some layers are offset by rv*2 if after the skip region)
    uint32_t source_layer = layer;
    if (call_once_layer != -1 && layer > (uint32_t)(call_once_layer + rv * 2))
    {
        source_layer = layer - rv * 2;
    }
    // ns_lp_printf("Layer %d, source layer %d, source layer count %d\n", layer, source_layer, source_layer_count);

    // 5. For each PMU counter index (map_index), compute the "non-obvious" index
    //    and extract the PMU counter from the sidecar
    for (uint32_t map_index = 0; map_index < NS_DEBUG_NUM_PMU_COUNTERS; map_index++)
    {
        // index formula from the snippet
        uint32_t index = (num_layers + source_layer) + source_layer_count * (map_index / 4);
        // retrieve the PMU snapshot (mocked by g_ns_microProfilerSidecar)
        uint32_t counterValue = ns_microProfilerSidecar.pmu_snapshot[index].counterValue[map_index % 4];
        // ns_lp_printf("Layer %d, map_index %d, index %d, counterValue %d\n", layer, map_index, index, counterValue);
        out_counters[map_index] = counterValue;
    }
#endif
#endif

    return NS_STATUS_SUCCESS;
}

/**
 * @brief     Parses and prints performance monitoring unit (PMU) statistics for each layer.
 *
 * This function retrieves PMU counter values from a global profiler sidecar and prints them
 * in a CSV-like format for external analysis. It supports the concept of a "CALL_ONCE" layer,
 * whereby certain layers are "skipped" on subsequent runs (and thus display zero counters).
 *
 * The high-level logic:
 *  1) Search for a layer tagged as "CALL_ONCE". If found, record its index.
 *  2) Print CSV headers, including names of PMU registers.
 *  3) For each layer:
 *     - Print basic profiling data (layer index, tag, time, estimated MACs).
 *     - Print detailed sidecar information (e.g., output shape, filter shape, strides).
 *     - Determine if the layer is in the "skipped" region. If so, print zeros.
 *       Otherwise, compute a non-obvious index for accessing PMU counters and print them.
 *
 * The non-obvious indexing formula for the PMU counters follows:
 * @code
 *     // source_layer is either the current layer or offset by rv*2 if we are beyond the skip region
 *     uint32_t index = (num_layers + source_layer) + (num_layers - rv * 2) * (map_index / 4);
 *     uint32_t counterValue = ns_microProfilerSidecar.pmu_snapshot[index].counterValue[ map_index % 4 ];
 * @endcode
 *
 * Skipped region logic:
 *   - If a CALL_ONCE layer is present at index C, then any layer in the range
 *     (C, C + rv*2] prints zero counters.
 *
 * @param[in]  num_layers  The total number of layers (including any special layers).
 * @param[in]  rv          The number of resource variables, used to determine how many
 *                         layers get skipped after the CALL_ONCE layer.
 *
 * @return     NS_STATUS_SUCCESS on success.
 */
uint32_t ns_parse_pmu_stats(uint32_t num_layers, uint32_t rv) {
#ifdef NS_MLPROFILE
#ifdef AM_PART_APOLLO5B
    char name[50];
    uint32_t map_index = 0;
    int call_once_layer = -1;
    uint32_t source_layer = 0; // The source of the PMU data starts from the second run, so we need to offset the layer index
                               // it increments by (num_layers-28) for each pmu block
    // uint32_t rv = model.rv_count;
    uint32_t source_layer_count = num_layers - rv*2;
    // Once PMU data is collected using ns_characterize_model, parse it and print it out in CSV format
    
    // Adjust for CALL_ONCE layers. The heuristic is this (IMPORTANT: not widely tested):
    // If a CALL_ONCE layer is present:
    //   - All layers before it are run normally, as is the CALL_ONCE layer
    //   - RV*2 layers are skipped (RV - number of resource variables) on subsequent runs
    //   - The number of layers captured per run is num_layers minus RV*2

    // Search layer tags for the CALL_ONCE layer
    for (uint32_t i = 0; i < num_layers; i++) {
        if (strcmp(ns_profiler_events_stats[i].tag, "CALL_ONCE") == 0) {
            call_once_layer = i;
            break;
        }
    }
    if (call_once_layer != -1) {
        ns_lp_printf("CALL_ONCE layer found at layer %d\n", call_once_layer);
    } 


    ns_lp_printf("Number of layers: %d, RV: %d, Source layer count: %d\n", num_layers, rv, source_layer_count);
    ns_lp_printf("\n");
    ns_delay_us(10000);

    // Start with the header
    ns_lp_printf("\"Event\",\"Tag\",\"uSeconds\",\"Est MACs\",\"MAC Eq\",\"Output Mag\",\"Output Shape\",\"Filter Shape\", \"Stride H\", \"Stride W\", \"Dilation H\", \"Dilation W\""); 
    ns_delay_us(10000);
    for (map_index = 0; map_index < sizeof(ns_pmu_map)/sizeof(ns_pmu_map_t); map_index++) {
        strcpy(name, ns_pmu_map[map_index].regname);
        ns_lp_printf(",\"%s\"", name); 
        ns_delay_us(10000);
    };
    ns_lp_printf("\n");

    // Now the data
    for (int print_layer = 0; print_layer < num_layers; print_layer++) {
        
        // Print event number, tag, time, and estimated MACs
        ns_lp_printf("%d, %s, %d, %d, ",
            print_layer,
            ns_profiler_events_stats[print_layer].tag,
            ns_profiler_events_stats[print_layer].elapsed_us,
            ns_profiler_events_stats[print_layer].estimated_macs
        );
        ns_lp_printf("\"%s\", %d, \"%s\", \"%s\", %d, %d, %d, %d", ns_microProfilerSidecar.m->mac_compute_string[print_layer],
                    ns_microProfilerSidecar.m->output_magnitudes[print_layer], ns_microProfilerSidecar.m->output_shapes[print_layer],
                    ns_microProfilerSidecar.m->filter_shapes[print_layer], ns_microProfilerSidecar.m->stride_h[print_layer],
                    ns_microProfilerSidecar.m->stride_w[print_layer], ns_microProfilerSidecar.m->dilation_h[print_layer],
                    ns_microProfilerSidecar.m->dilation_w[print_layer]);               

        // Print the PMU counters if not the call-once layers
        if ((call_once_layer != -1) &&  (print_layer > call_once_layer) && (print_layer<=(rv*2 + call_once_layer))) {
            // print zeros for all pmu values
            for (map_index = 0; map_index < NS_DEBUG_NUM_PMU_COUNTERS; map_index++) {
                ns_lp_printf(", 0");
            }
            // ns_lp_printf(", no pmu values");
        } 
        else {
            for (map_index = 0; map_index < NS_DEBUG_NUM_PMU_COUNTERS; map_index++) {
                // Fetch all the PMU counters for event i, spread out in the ns_profiler_events_stats[index].pmu_delta.counterValue array
                // The index is  num_layers * (4/map_index), and counterValue index is map_index%4
                if (call_once_layer == -1) {
                    source_layer = print_layer;
                } else if (print_layer > (rv*2 + call_once_layer)) {
                    source_layer = print_layer - rv*2;
                } else {
                    source_layer = print_layer;
                }
                uint32_t index = (num_layers + source_layer) + source_layer_count * (map_index/4);
                uint32_t counterValue = ns_microProfilerSidecar.pmu_snapshot[index].counterValue[map_index%4];
                // ns_lp_printf(", (%d, %d, %d,)%u", index, map_index, source_layer, counterValue);
                ns_lp_printf(", %u", counterValue);
            // source_layer++;
            }
        }
        // ns_delay_us(10000);
        
        ns_lp_printf("\n");
    }   
#endif
#endif
    return NS_STATUS_SUCCESS;
}



#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
