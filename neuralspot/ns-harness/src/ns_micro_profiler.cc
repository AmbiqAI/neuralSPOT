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

    #include "tensorflow/lite/kernels/internal/compatibility.h"
#ifdef NS_TFSTRUCTURE_RECENT
        #include "tensorflow/lite/micro/micro_log.h"
    #else
        #include "tensorflow/lite/micro/micro_error_reporter.h"
    #endif
    #include "tensorflow/lite/micro/micro_time.h"

namespace tflite {

uint32_t
MicroProfiler::BeginEvent(const char *tag) {
    if (num_events_ == kMaxEvents) {
        num_events_ = 0;
    }

    tags_[num_events_] = tag;
    start_ticks_[num_events_] = ns_us_ticker_read(ns_microProfilerTimer);
    ns_reset_perf_counters();
    ns_capture_cache_stats(&(ns_microProfilerSidecar.cache_start[num_events_]));
    ns_capture_perf_profiler(&(ns_microProfilerSidecar.perf_start[num_events_]));
    if (ns_microProfilerSidecar.has_estimated_macs) {
        ns_microProfilerSidecar.estimated_mac_count[num_events_] =
            ns_microProfilerSidecar
                .mac_count_map[num_events_ % ns_microProfilerSidecar.number_of_layers];
    }
    end_ticks_[num_events_] = start_ticks_[num_events_] - 1;
    // ns_lp_printf("START: %s handle %d: %d\n", tag, num_events_,
    // ns_microProfilerSidecar.perf_start[num_events_].lsucnt);
    // ns_print_perf_profile(&(ns_microProfilerSidecar.perf_start[num_events_]));
    return num_events_++;
}

void
MicroProfiler::EndEvent(uint32_t event_handle) {
    TFLITE_DCHECK(event_handle < kMaxEvents);
    end_ticks_[event_handle] = ns_us_ticker_read(ns_microProfilerTimer);
    ns_capture_cache_stats(&(ns_microProfilerSidecar.cache_end[event_handle]));
    ns_capture_perf_profiler(&(ns_microProfilerSidecar.perf_end[event_handle]));
    // ns_lp_printf("END: %d: %d\n", event_handle,
    // ns_microProfilerSidecar.perf_end[event_handle].lsucnt);
    // ns_print_perf_profile(&(ns_microProfilerSidecar.perf_end[num_events_]));
}

uint32_t
MicroProfiler::GetTotalTicks() const {
    int32_t ticks = 0;
    for (int i = 0; i < num_events_; ++i) {
        ticks += end_ticks_[i] - start_ticks_[i];
    }
    return ticks;
}

void
MicroProfiler::Log() const {
    ns_perf_counters_t d;
    for (int i = 0; i < num_events_; ++i) {
        uint32_t ticks = end_ticks_[i] - start_ticks_[i];
        ns_delta_perf(&(ns_microProfilerSidecar.perf_start[i]),
                      &(ns_microProfilerSidecar.perf_end[i]), &d);
        MicroPrintf("%s took %d us (%d cycles).", tags_[i], ticks, d.cyccnt);
    }
}

void
MicroProfiler::LogCsv() const {
    ns_perf_counters_t p;
    ns_cache_dump_t c;
    uint32_t macs;

    ns_microProfilerSidecar.captured_event_num = num_events_;
    MicroPrintf(
        "\"Event\",\"Tag\",\"uSeconds\",\"Est MACs\",\"cycles\",\"cpi\",\"exc\",\"sleep\",\"lsu\","
        "\"fold\",\"daccess\",\"dtaglookup\",\"dhitslookup\",\"dhitsline\",\"iaccess\","
        "\"itaglookup\",\"ihitslookup\",\"ihitsline\"");
    for (int i = 0; i < num_events_; ++i) {
        uint32_t ticks = end_ticks_[i] - start_ticks_[i];
        ns_delta_perf(&(ns_microProfilerSidecar.perf_start[i]),
                      &(ns_microProfilerSidecar.perf_end[i]), &p);
        ns_delta_cache(&(ns_microProfilerSidecar.cache_start[i]),
                       &(ns_microProfilerSidecar.cache_end[i]), &c);
        if (ns_microProfilerSidecar.has_estimated_macs) {
            macs = ns_microProfilerSidecar
                       .estimated_mac_count[i % ns_microProfilerSidecar.number_of_layers];
        } else {
            macs = 0;
        }

        MicroPrintf("%d,%s,%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", i,
                    tags_[i], ticks, macs, p.cyccnt, p.cpicnt, p.exccnt, p.sleepcnt, p.lsucnt,
                    p.foldcnt, c.daccess, c.dtaglookup, c.dhitslookup, c.dhitsline, c.iaccess,
                    c.itaglookup, c.ihitslookup, c.ihitsline);

    // Capture statistics for Validator if enabled
    #ifdef NS_TFLM_VALIDATOR
        if (i < NS_PROFILER_RPC_EVENTS_MAX) {
            memcpy(&ns_profiler_events_stats[i].cache_delta, &c, sizeof(ns_cache_dump_t));
            memcpy(&ns_profiler_events_stats[i].perf_delta, &p, sizeof(ns_perf_counters_t));
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
        total_ticks_per_tag[position].tag = tags_[i];
        total_ticks_per_tag[position].ticks = total_ticks_per_tag[position].ticks + ticks;
        total_ticks += ticks;
    }

    for (int i = 0; i < num_events_; ++i) {
        TicksPerTag each_tag_entry = total_ticks_per_tag[i];
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
        TicksPerTag each_tag_entry = total_ticks_per_tag[pos];
        if (each_tag_entry.tag == nullptr || strcmp(each_tag_entry.tag, tag_name) == 0) {
            return pos;
        }
    }
    return pos < num_events_ ? pos : -1;
}
} // namespace tflite
#endif
