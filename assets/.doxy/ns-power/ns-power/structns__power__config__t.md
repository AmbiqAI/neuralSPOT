

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
|  [**ns\_power\_mode\_e**](ns__peripherals__power_8h.md#enum-ns_power_mode_e) | [**eAIPowerMode**](#variable-eaipowermode)  <br>_CPU power mode (controls clock speed, etc)_  |












































## Public Attributes Documentation




### variable api 

```C++
const ns_core_api_t* ns_power_config_t::api;
```






### variable b128kTCM 

```C++
bool ns_power_config_t::b128kTCM;
```






### variable bEnableTempCo 

```C++
bool ns_power_config_t::bEnableTempCo;
```






### variable bNeedAlternativeUART 

```C++
bool ns_power_config_t::bNeedAlternativeUART;
```






### variable bNeedAudAdc 

```C++
bool ns_power_config_t::bNeedAudAdc;
```






### variable bNeedBluetooth 

```C++
bool ns_power_config_t::bNeedBluetooth;
```






### variable bNeedCrypto 

```C++
bool ns_power_config_t::bNeedCrypto;
```






### variable bNeedIOM 

```C++
bool ns_power_config_t::bNeedIOM;
```






### variable bNeedITM 

```C++
bool ns_power_config_t::bNeedITM;
```






### variable bNeedSharedSRAM 

```C++
bool ns_power_config_t::bNeedSharedSRAM;
```






### variable bNeedUSB 

```C++
bool ns_power_config_t::bNeedUSB;
```






### variable eAIPowerMode 

```C++
ns_power_mode_e ns_power_config_t::eAIPowerMode;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/includes-api/ns_peripherals_power.h`

