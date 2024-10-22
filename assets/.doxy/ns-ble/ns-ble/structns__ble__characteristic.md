

# Struct ns\_ble\_characteristic



[**ClassList**](annotated.md) **>** [**ns\_ble\_characteristic**](structns__ble__characteristic.md)


























## Public Attributes

| Type | Name |
| ---: | :--- |
|  void \* | [**applicationValue**](#variable-applicationvalue)  <br> |
|  attsAttr\_t | [**ccc**](#variable-ccc)  <br> |
|  uint8\_t | [**cccArray**](#variable-cccarray)  <br> |
|  uint16\_t | [**cccHandle**](#variable-ccchandle)  <br> |
|  uint16\_t | [**cccIndex**](#variable-cccindex)  <br> |
|  uint16\_t | [**cccIndicationHandle**](#variable-cccindicationhandle)  <br> |
|  uint16\_t | [**cccLen**](#variable-ccclen)  <br> |
|  attsAttr\_t | [**declaration**](#variable-declaration)  <br> |
|  uint16\_t | [**declarationHandle**](#variable-declarationhandle)  <br> |
|  uint16\_t | [**declarationLen**](#variable-declarationlen)  <br> |
|  uint8\_t | [**declarationProperties**](#variable-declarationproperties)  <br> |
|  uint16\_t | [**handleId**](#variable-handleid)  <br>_TRUE if indication is asynchronous._  |
|  uint8\_t | [**indicationIsAsynchronous**](#variable-indicationisasynchronous)  <br>_periodic measurement period in ms_  |
|  uint32\_t | [**indicationPeriod**](#variable-indicationperiod)  <br>_periodic measurement timer_  |
|  wsfTimer\_t | [**indicationTimer**](#variable-indicationtimer)  <br> |
|  ns\_ble\_characteristic\_notify\_handler\_t | [**notifyHandlerCb**](#variable-notifyhandlercb)  <br> |
|  uint8\_t \* | [**pValue**](#variable-pvalue)  <br> |
|  ns\_ble\_characteristic\_read\_handler\_t | [**readHandlerCb**](#variable-readhandlercb)  <br> |
|  [**ns\_ble\_uuid128\_t**](structns__ble__uuid128__t.md) | [**uuid128**](#variable-uuid128)  <br> |
|  attsAttr\_t | [**value**](#variable-value)  <br> |
|  uint16\_t | [**valueHandle**](#variable-valuehandle)  <br> |
|  uint16\_t | [**valueLen**](#variable-valuelen)  <br> |
|  ns\_ble\_characteristic\_write\_handler\_t | [**writeHandlerCb**](#variable-writehandlercb)  <br> |












































## Public Attributes Documentation




### variable applicationValue 

```C++
void* ns_ble_characteristic::applicationValue;
```






### variable ccc 

```C++
attsAttr_t ns_ble_characteristic::ccc;
```






### variable cccArray 

```C++
uint8_t ns_ble_characteristic::cccArray[2];
```






### variable cccHandle 

```C++
uint16_t ns_ble_characteristic::cccHandle;
```






### variable cccIndex 

```C++
uint16_t ns_ble_characteristic::cccIndex;
```






### variable cccIndicationHandle 

```C++
uint16_t ns_ble_characteristic::cccIndicationHandle;
```






### variable cccLen 

```C++
uint16_t ns_ble_characteristic::cccLen;
```






### variable declaration 

```C++
attsAttr_t ns_ble_characteristic::declaration;
```






### variable declarationHandle 

```C++
uint16_t ns_ble_characteristic::declarationHandle;
```






### variable declarationLen 

```C++
uint16_t ns_ble_characteristic::declarationLen;
```






### variable declarationProperties 

```C++
uint8_t ns_ble_characteristic::declarationProperties[19];
```






### variable handleId 

```C++
uint16_t ns_ble_characteristic::handleId;
```






### variable indicationIsAsynchronous 

```C++
uint8_t ns_ble_characteristic::indicationIsAsynchronous;
```






### variable indicationPeriod 

```C++
uint32_t ns_ble_characteristic::indicationPeriod;
```






### variable indicationTimer 

```C++
wsfTimer_t ns_ble_characteristic::indicationTimer;
```






### variable notifyHandlerCb 

```C++
ns_ble_characteristic_notify_handler_t ns_ble_characteristic::notifyHandlerCb;
```






### variable pValue 

```C++
uint8_t* ns_ble_characteristic::pValue;
```






### variable readHandlerCb 

```C++
ns_ble_characteristic_read_handler_t ns_ble_characteristic::readHandlerCb;
```






### variable uuid128 

```C++
ns_ble_uuid128_t ns_ble_characteristic::uuid128;
```






### variable value 

```C++
attsAttr_t ns_ble_characteristic::value;
```






### variable valueHandle 

```C++
uint16_t ns_ble_characteristic::valueHandle;
```






### variable valueLen 

```C++
uint16_t ns_ble_characteristic::valueLen;
```






### variable writeHandlerCb 

```C++
ns_ble_characteristic_write_handler_t ns_ble_characteristic::writeHandlerCb;
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/includes-api/ns_ble.h`

