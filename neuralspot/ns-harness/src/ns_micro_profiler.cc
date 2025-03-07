/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

#if defined(NS_MLPROFILE)

    #include "ns_debug_log.h"
    #include "tensorflow/lite/micro/micro_profiler.h"

    #include <cinttypes>
    #include <cstdint>
    #include <cstring>
    #include <cstdio>

    #include "tensorflow/lite/kernels/internal/compatibility.h"
    #ifdef NS_TFSTRUCTURE_RECENT
        #include "tensorflow/lite/micro/micro_log.h"
    #else
        #include "tensorflow/lite/micro/micro_error_reporter.h"
    #endif
    #include "tensorflow/lite/micro/micro_time.h"

#if defined(AM_PART_APOLLO5B)
extern ns_pmu_config_t ns_microProfilerPMU;
#endif

namespace tflite {

// uint32_t real_event = 0;
#ifdef AM_PART_APOLLO5B
void capture_pmu_counters(ns_pmu_counters_t *dest) {
    ns_pmu_get_counters(&ns_microProfilerPMU);
    // ns_pmu_print_counters(&ns_microProfilerPMU);
    for (int i = 0; i < 4; i++) {
        dest->counterValue[i] = ns_microProfilerPMU.counter[i].counterValue;
    }    
}
#endif // AM_PART_APOLLO5B

uint32_t
MicroProfiler::BeginEvent(const char *tag) {
            // ns_lp_printf("Clobberdetector event %d real event %d, pointer 0x%x.\n", num_events_, real_event, ns_microProfilerSidecar.mac_count_map);
    if (num_events_ == NS_PROFILER_MAX_EVENTS) {
        // ns_lp_printf("MicroProfiler::BeginEvent: Exceeded maximum number of events %d.\n",
        //              kMaxEvents);
        // num_events_ = 0;
        // return num_events_;
        num_events_ = NS_PROFILER_MAX_EVENTS - 1;
        // TFLITE_ASSERT_FALSE;
    }

    // real_event++;
    tags_[num_events_] = tag;
    start_ticks_[num_events_] = ns_us_ticker_read(ns_microProfilerTimer);
    ns_reset_perf_counters();
    // #ifndef AM_PART_APOLLO5A
    #if defined(AM_PART_APOLLO5B)
    // ns_pmu_get_counters(&(ns_microProfilerSidecar.pmu_snapshot[num_events_])); // this implicitly resets the counters
    // ns_lp_printf("Start PMUs for %d (%s)\n", num_events_, tag);
    ns_pmu_reset_counters();
    capture_pmu_counters(&(ns_microProfilerSidecar.pmu_snapshot[num_events_])); // this implicitly resets the counters


    #endif

    #if not defined(AM_PART_APOLLO5B) && not defined(AM_PART_APOLLO5A)
    ns_capture_cache_stats(&(ns_microProfilerSidecar.cache_snapshot[num_events_]));
    ns_capture_perf_profiler(&(ns_microProfilerSidecar.perf_snapshot[num_events_]));
    #endif
    // ns_lp_printf("m - 0x%x\n", ns_microProfilerSidecar.m);
    // ns_lp_printf("m mag - 0x%x\n", ns_microProfilerSidecar.m->output_magnitudes);
    if (ns_microProfilerSidecar.has_estimated_macs) {
        ns_microProfilerSidecar.estimated_mac_count[num_events_] =
            ns_microProfilerSidecar
                .mac_count_map[num_events_ % ns_microProfilerSidecar.number_of_layers];
    }
    end_ticks_[num_events_] = start_ticks_[num_events_] - 1;
    return num_events_++;
}

void
MicroProfiler::EndEvent(uint32_t event_handle) {
    // ns_lp_printf("MicroProfiler::EndEvent: event_handle %d, num_events_ %d addr et 0x%x addr ec 0x%x.\n", event_handle,
    //              num_events_, &(end_ticks_[event_handle]), &num_events_);
    TFLITE_DCHECK(event_handle < kMaxEvents);

    end_ticks_[event_handle] = ns_us_ticker_read(ns_microProfilerTimer);
    #if not defined(AM_PART_APOLLO5B) && not defined(AM_PART_APOLLO5A)
    ns_cache_dump_t c;
    ns_capture_cache_stats(&c);
    ns_delta_cache(&(ns_microProfilerSidecar.cache_snapshot[event_handle]), &c,
                   &(ns_microProfilerSidecar.cache_snapshot[event_handle]));

    ns_perf_counters_t p;
    ns_capture_perf_profiler(&p);
    ns_delta_perf(&(ns_microProfilerSidecar.perf_snapshot[event_handle]), &p,
                  &(ns_microProfilerSidecar.perf_snapshot[event_handle]));
    // ns_capture_cache_stats(&(ns_microProfilerSidecar.cache_end[event_handle]));
    #elif defined(AM_PART_APOLLO5B)
    ns_pmu_counters_t pmu;
    // ns_lp_printf("End PMUs for %d\n", event_handle);
    capture_pmu_counters(&pmu);
    ns_delta_pmu(&(ns_microProfilerSidecar.pmu_snapshot[event_handle]), &pmu,
                 &(ns_microProfilerSidecar.pmu_snapshot[event_handle]));

    // debug - override with event handle value
    // ns_microProfilerSidecar.pmu_snapshot[event_handle].counterValue[0] = event_handle;
    // ns_microProfilerSidecar.pmu_snapshot[event_handle].counterValue[1] = event_handle;
    // ns_microProfilerSidecar.pmu_snapshot[event_handle].counterValue[2] = event_handle;
    // ns_microProfilerSidecar.pmu_snapshot[event_handle].counterValue[3] = event_handle;
    #endif

}

uint32_t
MicroProfiler::GetTotalTicks() const {
    int32_t ticks = 0;
    for (int i = 0; i < num_events_; ++i) {
        ticks += end_ticks_[i] - start_ticks_[i];
    }
    ns_lp_printf("GTT num_events_ %d, ticks %d.\n", num_events_, ticks);
    return ticks;
}

void
MicroProfiler::Log() const {
    // ns_perf_counters_t d;
    for (int i = 0; i < num_events_; ++i) {
        uint32_t ticks = end_ticks_[i] - start_ticks_[i];

        // ns_delta_perf(&(ns_microProfilerSidecar.perf_start[i]),
        //               &(ns_microProfilerSidecar.perf_end[i]), &d);
        MicroPrintf("%s took %d us (%d cycles).", tags_[i], ticks, ns_microProfilerSidecar.perf_snapshot[i].cyccnt);
    }
}

void
MicroProfiler::LogCsv() const {
    ns_perf_counters_t *p;
    uint32_t max_events = num_events_>NS_PROFILER_MAX_EVENTS?NS_PROFILER_MAX_EVENTS:num_events_;
    #ifdef AM_PART_APOLLO5B
    ns_pmu_counters_t *pmu;
    char name[50];
    #else
    ns_cache_dump_t *c;
    #endif

    uint32_t macs;
    ns_lp_printf("LogCsv %d events (num_events_ %d).\n", max_events, num_events_);
    ns_microProfilerSidecar.captured_event_num = max_events;
    #if defined(AM_PART_APOLLO5B)

    // print same header to ns_profiler_csv_header
    snprintf(ns_profiler_csv_header, 512,
             "\"Event\",\"Tag\",\"uSeconds\",\"Est MACs\"");
    ns_lp_printf("\"Event\",\"Tag\",\"uSeconds\",\"Est MACs\",\"MAC Eq\",\"Output Mag\",\"Output Shape\",\"Filter Shape\", \"Stride H\", \"Stride W\", \"Dilation H\", \"Dilation W\""); 
    for (int i = 0; i < 4; i++) {
        ns_pmu_get_name(&ns_microProfilerPMU, i, name);
        snprintf(ns_profiler_csv_header + strlen(ns_profiler_csv_header), 512 - strlen(ns_profiler_csv_header),
                 ",\"%s\"", name);
        ns_lp_printf(",\"%s\"", name);
    }
    // ns_lp_printf("%s\n", ns_profiler_csv_header);
    ns_lp_printf("\n");
    #else
    MicroPrintf(
        "\"Event\",\"Tag\",\"uSeconds\",\"Est MACs\",\"cycles\",\"cpi\",\"exc\",\"sleep\",\"lsu\","
        "\"fold\",\"daccess\",\"dtaglookup\",\"dhitslookup\",\"dhitsline\",\"iaccess\","
        "\"itaglookup\",\"ihitslookup\",\"ihitsline\"");

    // print same header to ns_profiler_csv_header
    snprintf(ns_profiler_csv_header, 512,
             "\"Event\",\"Tag\",\"uSeconds\",\"Est MACs\",\"cycles\",\"cpi\",\"exc\",\"sleep\",\"lsu\","
             "\"fold\",\"daccess\",\"dtaglookup\",\"dhitslookup\",\"dhitsline\",\"iaccess\","
             "\"itaglookup\",\"ihitslookup\",\"ihitsline\"");
    #endif



    // print the ns_profiler_csv_header to console for debug
    ns_lp_printf("ns_profiler_csv_header: %s\n", ns_profiler_csv_header);

    for (uint32_t i = 0; i < max_events; ++i) {
        uint32_t ticks = end_ticks_[i] - start_ticks_[i];
        p = &(ns_microProfilerSidecar.perf_snapshot[i]);
        #ifdef AM_PART_APOLLO5B
        pmu = &(ns_microProfilerSidecar.pmu_snapshot[i]);
        #else
        c = &(ns_microProfilerSidecar.cache_snapshot[i]);
        #endif
        
        if (ns_microProfilerSidecar.has_estimated_macs) {
            macs = ns_microProfilerSidecar
                       .estimated_mac_count[i % ns_microProfilerSidecar.number_of_layers];

        } else {
            macs = 0;
        }

        // Debug, print all the metadata from ns_perf_mac_count_t
        // ns_lp_printf("Metadata for event %d: mag = %d, sh = %d, sw = %d, dh = %d, dw = %d, mac = %s, out = %s, filter = %s\n",
        //              i, ns_microProfilerSidecar.m->output_magnitudes[i], ns_microProfilerSidecar.m->stride_h[i],
        //              ns_microProfilerSidecar.m->stride_w[i], ns_microProfilerSidecar.m->dilation_h[i],
        //              ns_microProfilerSidecar.m->dilation_w[i], ns_microProfilerSidecar.m->mac_compute_string[i],
        //              ns_microProfilerSidecar.m->output_shapes[i], ns_microProfilerSidecar.m->filter_shapes[i]);

        #ifdef AM_PART_APOLLO5B
        ns_lp_printf("%d, %s, %d, %d, ", i, tags_[i], ticks, macs);
        ns_lp_printf("\"%s\", %d, \"%s\", \"%s\", %d, %d, %d, %d", ns_microProfilerSidecar.m->mac_compute_string[i],
                    ns_microProfilerSidecar.m->output_magnitudes[i], ns_microProfilerSidecar.m->output_shapes[i],
                    ns_microProfilerSidecar.m->filter_shapes[i], ns_microProfilerSidecar.m->stride_h[i],
                    ns_microProfilerSidecar.m->stride_w[i], ns_microProfilerSidecar.m->dilation_h[i],
                    ns_microProfilerSidecar.m->dilation_w[i]);
        for (int j = 0; j < 4; j++) {
            ns_lp_printf( ", %d", pmu->counterValue[j]);
        }
        ns_lp_printf("\n");

        #else
        MicroPrintf("%d,%s,%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", i,
                    tags_[i], ticks, macs, p->cyccnt, p->cpicnt, p->exccnt, p->sleepcnt, p->lsucnt,
                    p->foldcnt, c->daccess, c->dtaglookup, c->dhitslookup, c->dhitsline, c->iaccess,
                    c->itaglookup, c->ihitslookup, c->ihitsline);
        #endif

    // Capture statistics for Validator if enabled
    #ifdef NS_TFLM_VALIDATOR
        if (i < NS_PROFILER_RPC_EVENTS_MAX) {
            #ifdef AM_PART_APOLLO5B
            memcpy(&ns_profiler_events_stats[i].pmu_delta, pmu, sizeof(ns_pmu_counters_t));
            #else
            memcpy(&ns_profiler_events_stats[i].cache_delta, c, sizeof(ns_cache_dump_t));
            #endif
            memcpy(&ns_profiler_events_stats[i].perf_delta, p, sizeof(ns_perf_counters_t));
            ns_profiler_events_stats[i].estimated_macs = macs;
            ns_profiler_events_stats[i].elapsed_us = ticks;
            strncpy(ns_profiler_events_stats[i].tag, tags_[i], NS_PROFILER_TAG_SIZE - 1);
            ns_profiler_events_stats[i].tag[NS_PROFILER_TAG_SIZE - 1] = 0;
        }
    #endif
    }
}

void
MicroProfiler::LogTicksPerTagCsv() {
    MicroPrintf("\"Unique Tag\",\"Total ticks across all events with that tag.\"");
    int total_ticks = 0;
    for (int i = 0; i < num_events_; ++i) {
        uint32_t ticks = end_ticks_[i] - start_ticks_[i];
        TFLITE_DCHECK(tags_[i] != nullptr);
        int position = FindExistingOrNextPosition(tags_[i]);
        TFLITE_DCHECK(position >= 0);
        total_ticks_per_tag_[position].tag = tags_[i];
        total_ticks_per_tag_[position].ticks = total_ticks_per_tag_[position].ticks + ticks;
        total_ticks += ticks;
    }

    for (int i = 0; i < num_events_; ++i) {
        TicksPerTag each_tag_entry = total_ticks_per_tag_[i];
        if (each_tag_entry.tag == nullptr) {
            break;
        }
        MicroPrintf("%s, %d", each_tag_entry.tag, each_tag_entry.ticks);
    }
    MicroPrintf("total number of ticks, %d", total_ticks);
}

// This method finds a particular array element in the total_ticks_per_tag array
// with the matching tag_name passed in the method. If it can find a
// matching array element that has the same tag_name, then it will return the
// position of the matching element. But if it unable to find a matching element
// with the given tag_name, it will return the next available empty position
// from the array.
int
MicroProfiler::FindExistingOrNextPosition(const char *tag_name) {
    int pos = 0;
    for (; pos < num_events_; pos++) {
        TicksPerTag each_tag_entry = total_ticks_per_tag_[pos];
        if (each_tag_entry.tag == nullptr || strcmp(each_tag_entry.tag, tag_name) == 0) {
            return pos;
        }
    }
    return pos < num_events_ ? pos : -1;
}
} // namespace tflite
#endif
