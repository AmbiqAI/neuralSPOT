

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




<hr>



### variable ccc 

```C++
attsAttr_t ns_ble_characteristic::ccc;
```




<hr>



### variable cccArray 

```C++
uint8_t ns_ble_characteristic::cccArray[2];
```




<hr>



### variable cccHandle 

```C++
uint16_t ns_ble_characteristic::cccHandle;
```




<hr>



### variable cccIndex 

```C++
uint16_t ns_ble_characteristic::cccIndex;
```




<hr>



### variable cccIndicationHandle 

```C++
uint16_t ns_ble_characteristic::cccIndicationHandle;
```




<hr>



### variable cccLen 

```C++
uint16_t ns_ble_characteristic::cccLen;
```




<hr>



### variable declaration 

```C++
attsAttr_t ns_ble_characteristic::declaration;
```




<hr>



### variable declarationHandle 

```C++
uint16_t ns_ble_characteristic::declarationHandle;
```




<hr>



### variable declarationLen 

```C++
uint16_t ns_ble_characteristic::declarationLen;
```




<hr>



### variable declarationProperties 

```C++
uint8_t ns_ble_characteristic::declarationProperties[19];
```




<hr>



### variable handleId 

_TRUE if indication is asynchronous._ 
```C++
uint16_t ns_ble_characteristic::handleId;
```




<hr>



### variable indicationIsAsynchronous 

_periodic measurement period in ms_ 
```C++
uint8_t ns_ble_characteristic::indicationIsAsynchronous;
```




<hr>



### variable indicationPeriod 

_periodic measurement timer_ 
```C++
uint32_t ns_ble_characteristic::indicationPeriod;
```




<hr>



### variable indicationTimer 

```C++
wsfTimer_t ns_ble_characteristic::indicationTimer;
```




<hr>



### variable notifyHandlerCb 

```C++
ns_ble_characteristic_notify_handler_t ns_ble_characteristic::notifyHandlerCb;
```




<hr>



### variable pValue 

```C++
uint8_t* ns_ble_characteristic::pValue;
```




<hr>



### variable readHandlerCb 

```C++
ns_ble_characteristic_read_handler_t ns_ble_characteristic::readHandlerCb;
```




<hr>



### variable uuid128 

```C++
ns_ble_uuid128_t ns_ble_characteristic::uuid128;
```




<hr>



### variable value 

```C++
attsAttr_t ns_ble_characteristic::value;
```




<hr>



### variable valueHandle 

```C++
uint16_t ns_ble_characteristic::valueHandle;
```




<hr>



### variable valueLen 

```C++
uint16_t ns_ble_characteristic::valueLen;
```




<hr>



### variable writeHandlerCb 

```C++
ns_ble_characteristic_write_handler_t ns_ble_characteristic::writeHandlerCb;
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/includes-api/ns_ble.h`

