

# File ns\_energy\_monitor.h



[**FileList**](files.md) **>** [**includes-api**](dir_0f796f8be3b51b94a477512418b4fa0e.md) **>** [**ns\_energy\_monitor.h**](ns__energy__monitor_8h.md)

[Go to the source code of this file](ns__energy__monitor_8h_source.md)



* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`
* `#include "ns_ambiqsuite_harness.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_init\_power\_monitor\_state**](#function-ns_init_power_monitor_state) (void) <br>_Configure GPIO, then initialize to NS\_IDLE._  |
|  void | [**ns\_set\_power\_monitor\_state**](#function-ns_set_power_monitor_state) (uint8\_t state) <br>_Set GPIOs to indicate current state._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_DATA\_COLLECTION**](ns__energy__monitor_8h.md#define-ns_data_collection)  `1`<br> |
| define  | [**NS\_FEATURE\_EXTRACTION**](ns__energy__monitor_8h.md#define-ns_feature_extraction)  `2`<br> |
| define  | [**NS\_IDLE**](ns__energy__monitor_8h.md#define-ns_idle)  `0`<br> |
| define  | [**NS\_INFERING**](ns__energy__monitor_8h.md#define-ns_infering)  `3`<br> |

## Public Functions Documentation




### function ns\_init\_power\_monitor\_state 

_Configure GPIO, then initialize to NS\_IDLE._ 
```C++
void ns_init_power_monitor_state (
    void
) 
```




<hr>



### function ns\_set\_power\_monitor\_state 

_Set GPIOs to indicate current state._ 
```C++
void ns_set_power_monitor_state (
    uint8_t state
) 
```





**Parameters:**


* `state` 




        

<hr>
## Macro Definition Documentation





### define NS\_DATA\_COLLECTION 

```C++
#define NS_DATA_COLLECTION `1`
```




<hr>



### define NS\_FEATURE\_EXTRACTION 

```C++
#define NS_FEATURE_EXTRACTION `2`
```




<hr>



### define NS\_IDLE 

```C++
#define NS_IDLE `0`
```




<hr>



### define NS\_INFERING 

```C++
#define NS_INFERING `3`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/includes-api/ns_energy_monitor.h`

