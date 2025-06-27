

# Struct ns\_timer\_config



[**ClassList**](annotated.md) **>** [**ns\_timer\_config**](structns__timer__config.md)



_ns-timer configuration struct_ 

* `#include <ns_timer.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  ns\_timer\_callback\_cb | [**callback**](#variable-callback)  <br>_Invoked when timer ticks._  |
|  bool | [**enableInterrupt**](#variable-enableinterrupt)  <br>_Will generate interrupts, needs callback._  |
|  uint32\_t | [**periodInMicroseconds**](#variable-periodinmicroseconds)  <br>_For interrupts._  |
|  [**ns\_timers\_e**](ns__timer_8h.md#enum-ns_timers_e) | [**timer**](#variable-timer)  <br>_NS\_TIMER\_COUNTER, NS\_TIMER\_INTERRUPT, or NS\_TIMER\_USB._  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
const ns_core_api_t* ns_timer_config::api;
```




<hr>



### variable callback 

_Invoked when timer ticks._ 
```C++
ns_timer_callback_cb ns_timer_config::callback;
```




<hr>



### variable enableInterrupt 

_Will generate interrupts, needs callback._ 
```C++
bool ns_timer_config::enableInterrupt;
```




<hr>



### variable periodInMicroseconds 

_For interrupts._ 
```C++
uint32_t ns_timer_config::periodInMicroseconds;
```




<hr>



### variable timer 

_NS\_TIMER\_COUNTER, NS\_TIMER\_INTERRUPT, or NS\_TIMER\_USB._ 
```C++
ns_timers_e ns_timer_config::timer;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-utils/includes-api/ns_timer.h`

