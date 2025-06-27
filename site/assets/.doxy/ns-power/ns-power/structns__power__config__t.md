

# Struct ns\_power\_config\_t



[**ClassList**](annotated.md) **>** [**ns\_power\_config\_t**](structns__power__config__t.md)



_Power Mode Definitino._ 

* `#include <ns_peripherals_power.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  bool | [**b128kTCM**](#variable-b128ktcm)  <br>_Only enable 128k when true, 384k otherwise._  |
|  bool | [**bEnableTempCo**](#variable-benabletempco)  <br>_Enable Temperature Compensation._  |
|  bool | [**bNeedAlternativeUART**](#variable-bneedalternativeuart)  <br>_for EEMBC Power Control Module and similar_  |
|  bool | [**bNeedAudAdc**](#variable-bneedaudadc)  <br>_Prevents AUDADC from being powered off._  |
|  bool | [**bNeedBluetooth**](#variable-bneedbluetooth)  <br>_Prevents BLE from being powered off._  |
|  bool | [**bNeedCrypto**](#variable-bneedcrypto)  <br>_Prevents Crypto from being powered off._  |
|  bool | [**bNeedIOM**](#variable-bneediom)  <br>_Prevents IOMx from being powered off._  |
|  bool | [**bNeedITM**](#variable-bneeditm)  <br>_Enable Temperature Compensation._  |
|  bool | [**bNeedSharedSRAM**](#variable-bneedsharedsram)  <br>_Prevents SSRAM from being powered off._  |
|  bool | [**bNeedUSB**](#variable-bneedusb)  <br>_Prevents USB from being powered off._  |
|  bool | [**bNeedXtal**](#variable-bneedxtal)  <br>_Enable XTAL._  |
|  [**ns\_power\_mode\_e**](ns__peripherals__power_8h.md#enum-ns_power_mode_e) | [**eAIPowerMode**](#variable-eaipowermode)  <br>_CPU power mode (controls clock speed, etc)_  |












































## Public Attributes Documentation




### variable api 

_API prefix._ 
```C++
const ns_core_api_t* ns_power_config_t::api;
```




<hr>



### variable b128kTCM 

_Only enable 128k when true, 384k otherwise._ 
```C++
bool ns_power_config_t::b128kTCM;
```




<hr>



### variable bEnableTempCo 

_Enable Temperature Compensation._ 
```C++
bool ns_power_config_t::bEnableTempCo;
```




<hr>



### variable bNeedAlternativeUART 

_for EEMBC Power Control Module and similar_ 
```C++
bool ns_power_config_t::bNeedAlternativeUART;
```




<hr>



### variable bNeedAudAdc 

_Prevents AUDADC from being powered off._ 
```C++
bool ns_power_config_t::bNeedAudAdc;
```




<hr>



### variable bNeedBluetooth 

_Prevents BLE from being powered off._ 
```C++
bool ns_power_config_t::bNeedBluetooth;
```




<hr>



### variable bNeedCrypto 

_Prevents Crypto from being powered off._ 
```C++
bool ns_power_config_t::bNeedCrypto;
```




<hr>



### variable bNeedIOM 

_Prevents IOMx from being powered off._ 
```C++
bool ns_power_config_t::bNeedIOM;
```




<hr>



### variable bNeedITM 

_Enable Temperature Compensation._ 
```C++
bool ns_power_config_t::bNeedITM;
```




<hr>



### variable bNeedSharedSRAM 

_Prevents SSRAM from being powered off._ 
```C++
bool ns_power_config_t::bNeedSharedSRAM;
```




<hr>



### variable bNeedUSB 

_Prevents USB from being powered off._ 
```C++
bool ns_power_config_t::bNeedUSB;
```




<hr>



### variable bNeedXtal 

_Enable XTAL._ 
```C++
bool ns_power_config_t::bNeedXtal;
```




<hr>



### variable eAIPowerMode 

_CPU power mode (controls clock speed, etc)_ 
```C++
ns_power_mode_e ns_power_config_t::eAIPowerMode;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/includes-api/ns_peripherals_power.h`

