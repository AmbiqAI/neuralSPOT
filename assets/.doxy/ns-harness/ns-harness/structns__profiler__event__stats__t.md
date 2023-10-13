

# Struct ns\_profiler\_event\_stats\_t



[**ClassList**](annotated.md) **>** [**ns\_profiler\_event\_stats\_t**](structns__profiler__event__stats__t.md)



_Used by RPC validation functionality._ 

* `#include <ns_debug_log.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_cache\_dump\_t | [**cache\_delta**](#variable-cache_delta)  <br> |
|  uint32\_t | [**elapsed\_us**](#variable-elapsed_us)  <br> |
|  uint32\_t | [**estimated\_macs**](#variable-estimated_macs)  <br> |
|  ns\_perf\_counters\_t | [**perf\_delta**](#variable-perf_delta)  <br> |
|  char | [**tag**](#variable-tag)  <br>_Tag from TFLM microprofiler._  |












































## Public Attributes Documentation




### variable cache\_delta 

```C++
ns_cache_dump_t ns_profiler_event_stats_t::cache_delta;
```






### variable elapsed\_us 

```C++
uint32_t ns_profiler_event_stats_t::elapsed_us;
```






### variable estimated\_macs 

```C++
uint32_t ns_profiler_event_stats_t::estimated_macs;
```






### variable perf\_delta 

```C++
ns_perf_counters_t ns_profiler_event_stats_t::perf_delta;
```






### variable tag 

```C++
char ns_profiler_event_stats_t::tag[NS_PROFILER_TAG_SIZE];
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-harness/includes-api/ns_debug_log.h`

