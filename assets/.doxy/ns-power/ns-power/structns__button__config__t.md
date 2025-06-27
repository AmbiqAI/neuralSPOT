

# Struct ns\_button\_config\_t



[**ClassList**](annotated.md) **>** [**ns\_button\_config\_t**](structns__button__config__t.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_core\_api\_t const  \* | [**api**](#variable-api)  <br>_API prefix._  |
|  bool | [**button\_0\_enable**](#variable-button_0_enable)  <br>_Enable button 0 monitoring._  |
|  int volatile \* | [**button\_0\_flag**](#variable-button_0_flag)  <br>_Flag to set when button 0 is pressed._  |
|  bool | [**button\_1\_enable**](#variable-button_1_enable)  <br>_Enable button 1 monitoring._  |
|  int volatile \* | [**button\_1\_flag**](#variable-button_1_flag)  <br>_Flag to set when button 1 is pressed._  |
|  bool | [**joulescope\_trigger\_enable**](#variable-joulescope_trigger_enable)  <br>_Enable joulescope trigger._  |
|  int volatile \* | [**joulescope\_trigger\_flag**](#variable-joulescope_trigger_flag)  <br>_Flag to set when joulescope trigger is pressed._  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
ns_core_api_t const* ns_button_config_t::api;
```




<hr>



### variable button\_0\_enable 

_Enable button 0 monitoring._ 
```C++
bool ns_button_config_t::button_0_enable;
```




<hr>



### variable button\_0\_flag 

_Flag to set when button 0 is pressed._ 
```C++
int volatile* ns_button_config_t::button_0_flag;
```




<hr>



### variable button\_1\_enable 

_Enable button 1 monitoring._ 
```C++
bool ns_button_config_t::button_1_enable;
```




<hr>



### variable button\_1\_flag 

_Flag to set when button 1 is pressed._ 
```C++
int volatile* ns_button_config_t::button_1_flag;
```




<hr>



### variable joulescope\_trigger\_enable 

_Enable joulescope trigger._ 
```C++
bool ns_button_config_t::joulescope_trigger_enable;
```




<hr>



### variable joulescope\_trigger\_flag 

_Flag to set when joulescope trigger is pressed._ 
```C++
int volatile* ns_button_config_t::joulescope_trigger_flag;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/includes-api/ns_peripherals_button.h`

