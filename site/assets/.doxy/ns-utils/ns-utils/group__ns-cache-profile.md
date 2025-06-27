

# Group ns-cache-profile



[**Modules**](modules.md) **>** [**ns-cache-profile**](group__ns-cache-profile.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_cache\_config\_t**](structns__cache__config__t.md) <br>_Struct to configure the cache profiler._  |
| struct | [**ns\_cache\_dump\_t**](structns__cache__dump__t.md) <br>_Struct to capture a snapshot of the cache performance counters._  |
| struct | [**ns\_perf\_counters\_t**](structns__perf__counters__t.md) <br> |






















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint8\_t | [**ns\_cache\_profiler\_init**](#function-ns_cache_profiler_init) ([**ns\_cache\_config\_t**](structns__cache__config__t.md) \* cfg) <br>_Initialize the cache profiler._  |
|  void | [**ns\_capture\_cache\_stats**](#function-ns_capture_cache_stats) ([**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* dump) <br>_Capture the current value of the cache performance counters in the provided struct._  |
|  void | [**ns\_capture\_perf\_profiler**](#function-ns_capture_perf_profiler) ([**ns\_perf\_counters\_t**](structns__perf__counters__t.md) \* c) <br>_Capture the current value of the HW performance counters in the provided struct._  |
|  void | [**ns\_delta\_cache**](#function-ns_delta_cache) ([**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* s, [**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* e, [**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* d) <br>_Capture the delta between two cache performance counter snapshots._  |
|  void | [**ns\_delta\_perf**](#function-ns_delta_perf) ([**ns\_perf\_counters\_t**](structns__perf__counters__t.md) \* s, [**ns\_perf\_counters\_t**](structns__perf__counters__t.md) \* e, [**ns\_perf\_counters\_t**](structns__perf__counters__t.md) \* d) <br>_Calculate the delta between two HW performance snapshots._  |
|  void | [**ns\_init\_perf\_profiler**](#function-ns_init_perf_profiler) (void) <br>_Configure the HW performance capture unit._  |
|  void | [**ns\_print\_cache\_stats**](#function-ns_print_cache_stats) ([**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* dump) <br>_Print the cache performance counters._  |
|  void | [**ns\_print\_cache\_stats\_delta**](#function-ns_print_cache_stats_delta) ([**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* start, [**ns\_cache\_dump\_t**](structns__cache__dump__t.md) \* end) <br>_Print the delta between two cache performance counter snapshots._  |
|  void | [**ns\_print\_perf\_profile**](#function-ns_print_perf_profile) ([**ns\_perf\_counters\_t**](structns__perf__counters__t.md) \* c) <br>_Print a HW performance counter snapshot._  |
|  void | [**ns\_reset\_perf\_counters**](#function-ns_reset_perf_counters) (void) <br>_Reset the HW performance counters._  |
|  void | [**ns\_start\_perf\_profiler**](#function-ns_start_perf_profiler) (void) <br>_Start the HW performance capture unit._  |
|  void | [**ns\_stop\_perf\_profiler**](#function-ns_stop_perf_profiler) (void) <br>_Stop the HW performance capture unit._  |




























## Public Functions Documentation




### function ns\_cache\_profiler\_init 

_Initialize the cache profiler._ 
```C++
uint8_t ns_cache_profiler_init (
    ns_cache_config_t * cfg
) 
```





**Parameters:**


* `cfg` Configuration struct 



**Returns:**

uint8\_t


Initialize the cache profiler.


Enables cache performance counters (uses slightly more power)




**Parameters:**


* `cfg` 



**Returns:**

uint8\_t 





        

<hr>



### function ns\_capture\_cache\_stats 

_Capture the current value of the cache performance counters in the provided struct._ 
```C++
void ns_capture_cache_stats (
    ns_cache_dump_t * dump
) 
```





**Parameters:**


* `dump` 

Capture the current value of the cache performance counters in the provided struct.




**Parameters:**


* `dump` 




        

<hr>



### function ns\_capture\_perf\_profiler 

_Capture the current value of the HW performance counters in the provided struct._ 
```C++
void ns_capture_perf_profiler (
    ns_perf_counters_t * c
) 
```





**Parameters:**


* `c` 

Capture the current value of the HW performance counters in the provided struct.




**Parameters:**


* `c` 




        

<hr>



### function ns\_delta\_cache 

_Capture the delta between two cache performance counter snapshots._ 
```C++
void ns_delta_cache (
    ns_cache_dump_t * s,
    ns_cache_dump_t * e,
    ns_cache_dump_t * d
) 
```





**Parameters:**


* `s` start snapshot 
* `e` end snapshot 
* `d` struct where delta will be stored 




        

<hr>



### function ns\_delta\_perf 

_Calculate the delta between two HW performance snapshots._ 
```C++
void ns_delta_perf (
    ns_perf_counters_t * s,
    ns_perf_counters_t * e,
    ns_perf_counters_t * d
) 
```





**Parameters:**


* `s` starting snapshot 
* `e` ending snapshot 
* `d` Calculated delta 




        

<hr>



### function ns\_init\_perf\_profiler 

_Configure the HW performance capture unit._ 
```C++
void ns_init_perf_profiler (
    void
) 
```



Configure the HW performance capture unit. 


        

<hr>



### function ns\_print\_cache\_stats 

_Print the cache performance counters._ 
```C++
void ns_print_cache_stats (
    ns_cache_dump_t * dump
) 
```





**Parameters:**


* `dump` 

Print the cache performance counters.




**Parameters:**


* `dump` 




        

<hr>



### function ns\_print\_cache\_stats\_delta 

_Print the delta between two cache performance counter snapshots._ 
```C++
void ns_print_cache_stats_delta (
    ns_cache_dump_t * start,
    ns_cache_dump_t * end
) 
```





**Parameters:**


* `start` 
* `end` 

Print the delta between two cache performance counter snapshots.




**Parameters:**


* `start` 
* `end` 




        

<hr>



### function ns\_print\_perf\_profile 

_Print a HW performance counter snapshot._ 
```C++
void ns_print_perf_profile (
    ns_perf_counters_t * c
) 
```





**Parameters:**


* `c` 

Print a HW performance counter snapshot.




**Parameters:**


* `c` 




        

<hr>



### function ns\_reset\_perf\_counters 

_Reset the HW performance counters._ 
```C++
void ns_reset_perf_counters (
    void
) 
```



Reset the HW performance counters. 


        

<hr>



### function ns\_start\_perf\_profiler 

_Start the HW performance capture unit._ 
```C++
void ns_start_perf_profiler (
    void
) 
```



Start the HW performance capture unit. 


        

<hr>



### function ns\_stop\_perf\_profiler 

_Stop the HW performance capture unit._ 
```C++
void ns_stop_perf_profiler (
    void
) 
```



Stop the HW performance capture unit. 


        

<hr>

------------------------------


