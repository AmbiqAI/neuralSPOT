

# File ns\_debug\_log.h



[**FileList**](files.md) **>** [**includes-api**](dir_5a7ca3359baeaf8be53d9a3d1e900244.md) **>** [**ns\_debug\_log.h**](ns__debug__log_8h.md)

[Go to the source code of this file](ns__debug__log_8h_source.md)



* `#include "ns_perf_profile.h"`
* `#include "ns_timer.h"`















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_perf\_mac\_count\_t**](structns__perf__mac__count__t.md) <br> |
| struct | [**ns\_profiler\_event\_stats\_t**](structns__profiler__event__stats__t.md) <br>_Used by RPC validation functionality._  |
| struct | [**ns\_profiler\_sidecar\_t**](structns__profiler__sidecar__t.md) <br> |






















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_TFDebugLogInit**](#function-ns_tfdebugloginit) (ns\_timer\_config\_t \* t, [**ns\_perf\_mac\_count\_t**](structns__perf__mac__count__t.md) \* m) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_PROFILER\_MAX\_EVENTS**](ns__debug__log_8h.md#define-ns_profiler_max_events)  1024<br> |
| define  | [**NS\_PROFILER\_TAG\_SIZE**](ns__debug__log_8h.md#define-ns_profiler_tag_size)  12<br> |

## Public Functions Documentation




### function ns\_TFDebugLogInit 

```C++
void ns_TFDebugLogInit (
    ns_timer_config_t * t,
    ns_perf_mac_count_t * m
) 
```



## Macro Definition Documentation





### define NS\_PROFILER\_MAX\_EVENTS 

```C++
#define NS_PROFILER_MAX_EVENTS 1024
```






### define NS\_PROFILER\_TAG\_SIZE 

```C++
#define NS_PROFILER_TAG_SIZE 12
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-harness/includes-api/ns_debug_log.h`
