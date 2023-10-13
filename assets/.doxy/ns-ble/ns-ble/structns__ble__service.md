

# Struct ns\_ble\_service



[**ClassList**](annotated.md) **>** [**ns\_ble\_service**](structns__ble__service.md)



_BLE Service Configuration._ 

* `#include <ns_ble.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t \* | [**advData**](#variable-advdata)  <br> |
|  uint8\_t | [**advDataLen**](#variable-advdatalen)  <br> |
|  attsAttr\_t \* | [**attributes**](#variable-attributes)  <br> |
|  uint16\_t | [**baseHandle**](#variable-basehandle)  <br>_length of name string_  |
|  attsCccSet\_t \* | [**cccSet**](#variable-cccset)  <br> |
|  [**ns\_ble\_characteristic\_t**](structns__ble__characteristic.md) \*\* | [**characteristics**](#variable-characteristics)  <br> |
|  [**ns\_ble\_service\_control\_t**](ns__ble_8h.md#typedef-ns_ble_service_control_t) \* | [**control**](#variable-control)  <br> |
|  attsGroup\_t | [**group**](#variable-group)  <br> |
|  uint16\_t | [**handleId**](#variable-handleid)  <br>_pointer to WSF pool configuration struct_  |
|  uint16\_t | [**maxHandle**](#variable-maxhandle)  <br> |
|  char | [**name**](#variable-name)  <br>_char array of 128b UUID_  |
|  attsAttr\_t | [**nameAttribute**](#variable-nameattribute)  <br> |
|  uint8\_t | [**nameLen**](#variable-namelen)  <br>_string name of service_  |
|  uint16\_t | [**nextAttributeIndex**](#variable-nextattributeindex)  <br> |
|  uint16\_t | [**nextCccIndex**](#variable-nextcccindex)  <br> |
|  uint16\_t | [**nextCccIndicationHandle**](#variable-nextcccindicationhandle)  <br> |
|  uint16\_t | [**nextCharacteristicIndex**](#variable-nextcharacteristicindex)  <br> |
|  uint16\_t | [**nextHandleId**](#variable-nexthandleid)  <br> |
|  uint16\_t | [**numAttributes**](#variable-numattributes)  <br>_initialize to first handle of service_  |
|  uint16\_t | [**numCharacteristics**](#variable-numcharacteristics)  <br>_Number of attributes in service (initialize to 0)_  |
|  [**ns\_ble\_pool\_config\_t**](structns__ble__pool__config__t.md) \* | [**poolConfig**](#variable-poolconfig)  <br>_Number of characteristics in service._  |
|  attsAttr\_t | [**primaryAttribute**](#variable-primaryattribute)  <br> |
|  uint16\_t | [**primaryAttributeLen**](#variable-primaryattributelen)  <br> |
|  attsReadCback\_t | [**readCback**](#variable-readcback)  <br> |
|  uint8\_t \* | [**scanData**](#variable-scandata)  <br> |
|  uint8\_t | [**scanDataLen**](#variable-scandatalen)  <br> |
|  [**ns\_ble\_uuid128\_t**](structns__ble__uuid128__t.md) | [**uuid128**](#variable-uuid128)  <br> |
|  attsAttr\_t | [**versionAttribute**](#variable-versionattribute)  <br> |
|  attsWriteCback\_t | [**writeCback**](#variable-writecback)  <br> |












































## Public Attributes Documentation




### variable advData 

```C++
uint8_t* ns_ble_service::advData;
```






### variable advDataLen 

```C++
uint8_t ns_ble_service::advDataLen;
```






### variable attributes 

```C++
attsAttr_t* ns_ble_service::attributes;
```






### variable baseHandle 

```C++
uint16_t ns_ble_service::baseHandle;
```






### variable cccSet 

```C++
attsCccSet_t* ns_ble_service::cccSet;
```






### variable characteristics 

```C++
ns_ble_characteristic_t** ns_ble_service::characteristics;
```






### variable control 

```C++
ns_ble_service_control_t* ns_ble_service::control;
```






### variable group 

```C++
attsGroup_t ns_ble_service::group;
```






### variable handleId 

```C++
uint16_t ns_ble_service::handleId;
```






### variable maxHandle 

```C++
uint16_t ns_ble_service::maxHandle;
```






### variable name 

```C++
char ns_ble_service::name[31];
```






### variable nameAttribute 

```C++
attsAttr_t ns_ble_service::nameAttribute;
```






### variable nameLen 

```C++
uint8_t ns_ble_service::nameLen;
```






### variable nextAttributeIndex 

```C++
uint16_t ns_ble_service::nextAttributeIndex;
```






### variable nextCccIndex 

```C++
uint16_t ns_ble_service::nextCccIndex;
```






### variable nextCccIndicationHandle 

```C++
uint16_t ns_ble_service::nextCccIndicationHandle;
```






### variable nextCharacteristicIndex 

```C++
uint16_t ns_ble_service::nextCharacteristicIndex;
```






### variable nextHandleId 

```C++
uint16_t ns_ble_service::nextHandleId;
```






### variable numAttributes 

```C++
uint16_t ns_ble_service::numAttributes;
```






### variable numCharacteristics 

```C++
uint16_t ns_ble_service::numCharacteristics;
```






### variable poolConfig 

```C++
ns_ble_pool_config_t* ns_ble_service::poolConfig;
```






### variable primaryAttribute 

```C++
attsAttr_t ns_ble_service::primaryAttribute;
```






### variable primaryAttributeLen 

```C++
uint16_t ns_ble_service::primaryAttributeLen;
```






### variable readCback 

```C++
attsReadCback_t ns_ble_service::readCback;
```






### variable scanData 

```C++
uint8_t* ns_ble_service::scanData;
```






### variable scanDataLen 

```C++
uint8_t ns_ble_service::scanDataLen;
```






### variable uuid128 

```C++
ns_ble_uuid128_t ns_ble_service::uuid128;
```






### variable versionAttribute 

```C++
attsAttr_t ns_ble_service::versionAttribute;
```






### variable writeCback 

```C++
attsWriteCback_t ns_ble_service::writeCback;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/includes-api/ns_ble.h`

