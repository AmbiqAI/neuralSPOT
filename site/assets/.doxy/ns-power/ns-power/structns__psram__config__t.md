

# Struct ns\_psram\_config\_t



[**ClassList**](annotated.md) **>** [**ns\_psram\_config\_t**](structns__psram__config__t.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_core\_api\_t const  \* | [**api**](#variable-api)  <br>_API prefix._  |
|  uint32\_t | [**psram\_base\_address**](#variable-psram_base_address)  <br>_PSRAM base address, filled in by init._  |
|  uint8\_t | [**psram\_block**](#variable-psram_block)  <br>_PSRAM block._  |
|  bool | [**psram\_enable**](#variable-psram_enable)  <br>_Enable PSRAM._  |
|  uint32\_t | [**psram\_size**](#variable-psram_size)  <br>_PSRAM size, filled in by init._  |
|  psram\_type\_e | [**psram\_type**](#variable-psram_type)  <br>_PSRAM type._  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
ns_core_api_t const* ns_psram_config_t::api;
```




<hr>



### variable psram\_base\_address 

_PSRAM base address, filled in by init._ 
```C++
uint32_t ns_psram_config_t::psram_base_address;
```




<hr>



### variable psram\_block 

_PSRAM block._ 
```C++
uint8_t ns_psram_config_t::psram_block;
```




<hr>



### variable psram\_enable 

_Enable PSRAM._ 
```C++
bool ns_psram_config_t::psram_enable;
```




<hr>



### variable psram\_size 

_PSRAM size, filled in by init._ 
```C++
uint32_t ns_psram_config_t::psram_size;
```




<hr>



### variable psram\_type 

_PSRAM type._ 
```C++
psram_type_e ns_psram_config_t::psram_type;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/includes-api/ns_peripherals_psram.h`

