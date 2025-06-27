

# File ns\_debug\_log.cc



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-harness**](dir_e0d7b3aff6df2cba2f05a768a095730e.md) **>** [**src**](dir_8df671c8e5b7eec7f2ec532421bc80bd.md) **>** [**ns\_debug\_log.cc**](ns__debug__log_8cc.md)

[Go to the source code of this file](ns__debug__log_8cc_source.md)



* `#include "ns_debug_log.h"`
* `#include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_pmu_map.h"`
* `#include "ns_pmu_utils.h"`
* `#include <cstring>`
* `#include <cstdio>`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_TFDebugLog**](#function-ns_tfdebuglog) (const char \* s) <br> |
|  void | [**ns\_TFDebugLogInit**](#function-ns_tfdebugloginit) ([**ns\_debug\_log\_init\_t**](structns__debug__log__init__t.md) \* cfg) <br> |
|  uint32\_t | [**ns\_characterize\_model**](#function-ns_characterize_model) (invoke\_fp func) <br>_Given a model, characterize it by running it repeatedly and capturing PMU events._  |
|  uint32\_t | [**ns\_get\_layer\_counters**](#function-ns_get_layer_counters) (uint32\_t layer, uint32\_t num\_layers, uint32\_t rv, uint32\_t \* out\_counters) <br>_Retrieves PMU counters for a given layer._  |
|  uint32\_t | [**ns\_parse\_pmu\_stats**](#function-ns_parse_pmu_stats) (uint32\_t num\_layers, uint32\_t rv) <br>_Parses and prints performance monitoring unit (PMU) statistics for each layer._  |
|  uint32\_t | [**ns\_set\_pmu\_header**](#function-ns_set_pmu_header) (void) <br>_Searches for the "CALL\_ONCE" layer in the global profiler stats array._  |




























## Public Functions Documentation




### function ns\_TFDebugLog 

```C++
void ns_TFDebugLog (
    const char * s
) 
```




<hr>



### function ns\_TFDebugLogInit 

```C++
void ns_TFDebugLogInit (
    ns_debug_log_init_t * cfg
) 
```




<hr>



### function ns\_characterize\_model 

_Given a model, characterize it by running it repeatedly and capturing PMU events._ 
```C++
uint32_t ns_characterize_model (
    invoke_fp func
) 
```





**Returns:**

uint32\_t 





        

<hr>



### function ns\_get\_layer\_counters 

_Retrieves PMU counters for a given layer._ 
```C++
uint32_t ns_get_layer_counters (
    uint32_t layer,
    uint32_t num_layers,
    uint32_t rv,
    uint32_t * out_counters
) 
```



This function determines if the layer is within a "skip" range for PMU counters (related to CALL\_ONCE), then either returns zeros or calculates the counters based on the "non-obvious" index formula:



```C++
index = (num_layers + source_layer) + (source_layer_count) * (map_index / 4);
counterValue = ns_microProfilerSidecar.pmu_snapshot[index].counterValue[ map_index % 4 ];
```





**Parameters:**


* `layer` The layer index for which PMU values are requested. 
* `num_layers` Total layers in the pipeline. 
* `rv` Number of "resource variables" used to offset certain layers. 
* `out_counters` Pointer to an array in which to store the retrieved counter values. 
* `max_counters` Size of the out\_counters array (e.g. number of PMU registers).



**Returns:**

NS\_STATUS\_SUCCESS on success. Could return an error code if desired. 





        

<hr>



### function ns\_parse\_pmu\_stats 

_Parses and prints performance monitoring unit (PMU) statistics for each layer._ 
```C++
uint32_t ns_parse_pmu_stats (
    uint32_t num_layers,
    uint32_t rv
) 
```



This function retrieves PMU counter values from a global profiler sidecar and prints them in a CSV-like format for external analysis. It supports the concept of a "CALL\_ONCE" layer, whereby certain layers are "skipped" on subsequent runs (and thus display zero counters).


The high-level logic: 1) Search for a layer tagged as "CALL\_ONCE". If found, record its index. 2) Print CSV headers, including names of PMU registers. 3) For each layer:
* Print basic profiling data (layer index, tag, time, estimated MACs).
* Print detailed sidecar information (e.g., output shape, filter shape, strides).
* Determine if the layer is in the "skipped" region. If so, print zeros. Otherwise, compute a non-obvious index for accessing PMU counters and print them.




The non-obvious indexing formula for the PMU counters follows: 
```C++
// source_layer is either the current layer or offset by rv*2 if we are beyond the skip region
uint32_t index = (num_layers + source_layer) + (num_layers - rv * 2) * (map_index / 4);
uint32_t counterValue = ns_microProfilerSidecar.pmu_snapshot[index].counterValue[ map_index % 4 ];
```



Skipped region logic:
* If a CALL\_ONCE layer is present at index C, then any layer in the range (C, C + rv\*2] prints zero counters.






**Parameters:**


* `num_layers` The total number of layers (including any special layers). 
* `rv` The number of resource variables, used to determine how many layers get skipped after the CALL\_ONCE layer.



**Returns:**

NS\_STATUS\_SUCCESS on success. 





        

<hr>



### function ns\_set\_pmu\_header 

_Searches for the "CALL\_ONCE" layer in the global profiler stats array._ 
```C++
uint32_t ns_set_pmu_header (
    void
) 
```



This function scans through all layers to see if any layer is tagged "CALL\_ONCE". If found, the layer index is returned. If not, returns -1.




**Parameters:**


* `num_layers` The total number of layers in the model or pipeline.



**Returns:**

Index of the CALL\_ONCE layer if found, otherwise -1. 





        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-harness/src/ns_debug_log.cc`

