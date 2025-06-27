

# File ns\_energy\_monitor.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-utils**](dir_8caed56d1b8d43fb57ec0577c38aa59e.md) **>** [**src**](dir_5922fa0bec7bd191dd0e3ff5da447491.md) **>** [**ns\_energy\_monitor.c**](ns__energy__monitor_8c.md)

[Go to the source code of this file](ns__energy__monitor_8c_source.md)



* `#include "ns_energy_monitor.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_ambiqsuite_harness.h"`























## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  bool | [**g\_ns\_power\_monitor\_enabled**](#variable-g_ns_power_monitor_enabled)   = `false`<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_init\_power\_monitor\_state**](#function-ns_init_power_monitor_state) (void) <br>_Configure GPIO, then initialize to NS\_IDLE._  |
|  void | [**ns\_set\_power\_monitor\_state**](#function-ns_set_power_monitor_state) (uint8\_t state) <br>_Set GPIOs to indicate current state._  |




























## Public Static Attributes Documentation




### variable g\_ns\_power\_monitor\_enabled 

```C++
bool g_ns_power_monitor_enabled;
```




<hr>
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

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/src/ns_energy_monitor.c`

