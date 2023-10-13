

# Struct ns\_profiler\_sidecar\_t



[**ClassList**](annotated.md) **>** [**ns\_profiler\_sidecar\_t**](structns__profiler__sidecar__t.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_cache\_dump\_t | [**cache\_end**](#variable-cache_end)  <br> |
|  ns\_cache\_dump\_t | [**cache\_start**](#variable-cache_start)  <br> |
|  uint32\_t | [**captured\_event\_num**](#variable-captured_event_num)  <br>_How many events have been captured so far._  |
|  uint32\_t | [**estimated\_mac\_count**](#variable-estimated_mac_count)  <br> |
|  bool | [**has\_estimated\_macs**](#variable-has_estimated_macs)  <br> |
|  uint32\_t \* | [**mac\_count\_map**](#variable-mac_count_map)  <br> |
|  int | [**number\_of\_layers**](#variable-number_of_layers)  <br>_Number of layers for which we have mac estimates._  |
|  ns\_perf\_counters\_t | [**perf\_end**](#variable-perf_end)  <br> |
|  ns\_perf\_counters\_t | [**perf\_start**](#variable-perf_start)  <br> |












































## Public Attributes Documentation




### variable cache\_end 

```C++
ns_cache_dump_t ns_profiler_sidecar_t::cache_end[NS_PROFILER_MAX_EVENTS];
```






### variable cache\_start 

```C++
ns_cache_dump_t ns_profiler_sidecar_t::cache_start[NS_PROFILER_MAX_EVENTS];
```






### variable captured\_event\_num 

```C++
uint32_t ns_profiler_sidecar_t::captured_event_num;
```






### variable estimated\_mac\_count 

```C++
uint32_t ns_profiler_sidecar_t::estimated_mac_count[NS_PROFILER_MAX_EVENTS];
```






### variable has\_estimated\_macs 

```C++
bool ns_profiler_sidecar_t::has_estimated_macs;
```






### variable mac\_count\_map 

```C++
uint32_t* ns_profiler_sidecar_t::mac_count_map;
```






### variable number\_of\_layers 

```C++
int ns_profiler_sidecar_t::number_of_layers;
```






### variable perf\_end 

```C++
ns_perf_counters_t ns_profiler_sidecar_t::perf_end[NS_PROFILER_MAX_EVENTS];
```






### variable perf\_start 

```C++
ns_perf_counters_t ns_profiler_sidecar_t::perf_start[NS_PROFILER_MAX_EVENTS];
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-harness/includes-api/ns_debug_log.h`

