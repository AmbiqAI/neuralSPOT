

# Struct ns\_ble\_control



[**ClassList**](annotated.md) **>** [**ns\_ble\_control**](structns__ble__control.md)



_BLE Control block._ 

* `#include <ns_ble.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  appAdvCfg\_t \* | [**advCfg**](#variable-advcfg)  <br> |
|  const ns\_core\_api\_t \* | [**api**](#variable-api)  <br>_API prefix._  |
|  appCfg\_t \* | [**appCfg**](#variable-appcfg)  <br> |
|  appDiscCfg\_t \* | [**appDiscCfg**](#variable-appdisccfg)  <br> |
|  bool\_t | [**autoConnect**](#variable-autoconnect)  <br> |
|  hciConnSpec\_t \* | [**connCfg**](#variable-conncfg)  <br> |
|  uint8\_t | [**discState**](#variable-discstate)  <br> |
|  wsfHandlerId\_t | [**handlerId**](#variable-handlerid)  <br> |
|  uint8\_t | [**hdlListLen**](#variable-hdllistlen)  <br> |
|  uint16\_t | [**nextHandleId**](#variable-nexthandleid)  <br> |
|  uint16\_t | [**numServices**](#variable-numservices)  <br> |
|  bool\_t | [**scanning**](#variable-scanning)  <br> |
|  appSecCfg\_t \* | [**secCfg**](#variable-seccfg)  <br> |
|  bool | [**secureConnections**](#variable-secureconnections)  <br> |
|  [**ns\_ble\_service\_control\_t**](ns__ble_8h.md#typedef-ns_ble_service_control_t) \* | [**service\_config**](#variable-service_config)  <br> |
|  [**ns\_ble\_service\_t**](ns__ble_8h.md#typedef-ns_ble_service_t) \* | [**services**](#variable-services)  <br> |
|  appSlaveCfg\_t \* | [**slaveCfg**](#variable-slavecfg)  <br> |
|  smpCfg\_t \* | [**smpCfg**](#variable-smpcfg)  <br> |
|  appUpdateCfg\_t \* | [**updateCfg**](#variable-updatecfg)  <br> |












































## Public Attributes Documentation




### variable advCfg 

```C++
appAdvCfg_t* ns_ble_control::advCfg;
```



TRUE to use secure connections 


        

<hr>



### variable api 

_API prefix._ 
```C++
const ns_core_api_t* ns_ble_control::api;
```




<hr>



### variable appCfg 

```C++
appCfg_t* ns_ble_control::appCfg;
```



Discovery configuration 


        

<hr>



### variable appDiscCfg 

```C++
appDiscCfg_t* ns_ble_control::appDiscCfg;
```



Connection parameters 


        

<hr>



### variable autoConnect 

```C++
bool_t ns_ble_control::autoConnect;
```



TRUE if scanning 


        

<hr>



### variable connCfg 

```C++
hciConnSpec_t* ns_ble_control::connCfg;
```



SMP configuration 


        

<hr>



### variable discState 

```C++
uint8_t ns_ble_control::discState;
```



TRUE if auto-connecting 


        

<hr>



### variable handlerId 

```C++
wsfHandlerId_t ns_ble_control::handlerId;
```




<hr>



### variable hdlListLen 

```C++
uint8_t ns_ble_control::hdlListLen;
```



Service discovery state 


        

<hr>



### variable nextHandleId 

```C++
uint16_t ns_ble_control::nextHandleId;
```



Cached handle list length 


        

<hr>



### variable numServices 

```C++
uint16_t ns_ble_control::numServices;
```



Array of services 


        

<hr>



### variable scanning 

```C++
bool_t ns_ble_control::scanning;
```



WSF hander ID 


        

<hr>



### variable secCfg 

```C++
appSecCfg_t* ns_ble_control::secCfg;
```



Master configuration 


        

<hr>



### variable secureConnections 

```C++
bool ns_ble_control::secureConnections;
```



Next handle ID to allocate 


        

<hr>



### variable service\_config 

```C++
ns_ble_service_control_t* ns_ble_control::service_config;
```



Application configuration 


        

<hr>



### variable services 

```C++
ns_ble_service_t* ns_ble_control::services[NS_BLE_MAX_SERVICES];
```



Service configuration 


        

<hr>



### variable slaveCfg 

```C++
appSlaveCfg_t* ns_ble_control::slaveCfg;
```



Advertising configuration 


        

<hr>



### variable smpCfg 

```C++
smpCfg_t* ns_ble_control::smpCfg;
```



Connection update configuration 


        

<hr>



### variable updateCfg 

```C++
appUpdateCfg_t* ns_ble_control::updateCfg;
```



Security configuration 


        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/includes-api/ns_ble.h`

