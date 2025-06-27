

# Struct ns\_profiler\_sidecar\_t



[**ClassList**](annotated.md) **>** [**ns\_profiler\_sidecar\_t**](structns__profiler__sidecar__t.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_cache\_dump\_t | [**cache\_snapshot**](#variable-cache_snapshot)  <br> |
|  uint32\_t | [**captured\_event\_num**](#variable-captured_event_num)  <br>_How many events have been captured so far._  |
|  uint32\_t | [**estimated\_mac\_count**](#variable-estimated_mac_count)  <br> |
|  bool | [**has\_estimated\_macs**](#variable-has_estimated_macs)  <br> |
|  const [**ns\_perf\_mac\_count\_t**](structns__perf__mac__count__t.md) \* | [**m**](#variable-m)  <br> |
|  uint32\_t \* | [**mac\_count\_map**](#variable-mac_count_map)  <br> |
|  int | [**number\_of\_layers**](#variable-number_of_layers)  <br>_Number of layers for which we have mac estimates._  |
|  ns\_perf\_counters\_t | [**perf\_snapshot**](#variable-perf_snapshot)  <br> |












































## Public Attributes Documentation




### variable cache\_snapshot 

```C++
ns_cache_dump_t ns_profiler_sidecar_t::cache_snapshot[NS_PROFILER_MAX_EVENTS];
```




<hr>



### variable captured\_event\_num 

_How many events have been captured so far._ 
```C++
uint32_t ns_profiler_sidecar_t::captured_event_num;
```




<hr>



### variable estimated\_mac\_count 

```C++
uint32_t ns_profiler_sidecar_t::estimated_mac_count[NS_PROFILER_MAX_EVENTS];
```




<hr>



### variable has\_estimated\_macs 

```C++
bool ns_profiler_sidecar_t::has_estimated_macs;
```




<hr>



### variable m 

```C++
const ns_perf_mac_count_t* ns_profiler_sidecar_t::m;
```




<hr>



### variable mac\_count\_map 

```C++
uint32_t* ns_profiler_sidecar_t::mac_count_map;
```




<hr>



### variable number\_of\_layers 

_Number of layers for which we have mac estimates._ 
```C++
int ns_profiler_sidecar_t::number_of_layers;
```




<hr>



### variable perf\_snapshot 

```C++
ns_perf_counters_t ns_profiler_sidecar_t::perf_snapshot[NS_PROFILER_MAX_EVENTS];
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-harness/includes-api/ns_debug_log.h`

