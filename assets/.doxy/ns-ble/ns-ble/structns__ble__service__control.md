

# Struct ns\_ble\_service\_control



[**ClassList**](annotated.md) **>** [**ns\_ble\_service\_control**](structns__ble__service__control.md)



_BLE Service control block._ 

* `#include <ns_ble.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  uint8\_t \* | [**advData**](#variable-advdata)  <br> |
|  uint8\_t | [**advDataLen**](#variable-advdatalen)  <br> |
|  bool\_t | [**autoConnect**](#variable-autoconnect)  <br> |
|  wsfBufPoolDesc\_t \* | [**bufferDescriptors**](#variable-bufferdescriptors)  <br>_Pointer to WSF buffer descriptors (allocated by caller)_  |
|  uint32\_t | [**bufferDescriptorsSize**](#variable-bufferdescriptorssize)  <br>_in bytes_  |
|  uint32\_t \* | [**bufferPool**](#variable-bufferpool)  <br>_Pointer to WSF buffer pool (allocated by caller)_  |
|  uint32\_t | [**bufferPoolSize**](#variable-bufferpoolsize)  <br>_in bytes_  |
|  uint32\_t | [**cccCount**](#variable-ccccount)  <br> |
|  attsCccSet\_t \* | [**cccSet**](#variable-cccset)  <br> |
|  [**ns\_ble\_connection\_t**](structns__ble__connection__t.md) \* | [**connection**](#variable-connection)  <br> |
|  uint8\_t | [**discState**](#variable-discstate)  <br> |
|  attsGroup\_t \* | [**group**](#variable-group)  <br> |
|  wsfHandlerId\_t | [**handlerId**](#variable-handlerid)  <br> |
|  ns\_ble\_service\_specific\_handler\_cb | [**handler\_cb**](#variable-handler_cb)  <br> |
|  ns\_ble\_service\_specific\_handler\_init\_cb | [**handler\_init\_cb**](#variable-handler_init_cb)  <br> |
|  uint8\_t | [**hdlListLen**](#variable-hdllistlen)  <br> |
|  attsAttr\_t \* | [**list**](#variable-list)  <br> |
|  ns\_ble\_service\_specific\_procMsg\_cb | [**procMsg\_cb**](#variable-procmsg_cb)  <br> |
|  uint8\_t \* | [**scanData**](#variable-scandata)  <br> |
|  uint8\_t | [**scanDataLen**](#variable-scandatalen)  <br> |
|  bool\_t | [**scanning**](#variable-scanning)  <br> |
|  ns\_ble\_service\_specific\_init\_cb | [**service\_init\_cb**](#variable-service_init_cb)  <br> |
|  uint8\_t | [**wsfBufCount**](#variable-wsfbufcount)  <br>_Number of WSF buffers._  |












































## Public Attributes Documentation




### variable advData 


```C++
uint8_t* ns_ble_service_control::advData;
```



Number of CCC descriptors 


        



### variable advDataLen 


```C++
uint8_t ns_ble_service_control::advDataLen;
```



Advertising data 


        



### variable autoConnect 


```C++
bool_t ns_ble_service_control::autoConnect;
```



TRUE if scanning 


        



### variable bufferDescriptors 

```C++
wsfBufPoolDesc_t* ns_ble_service_control::bufferDescriptors;
```






### variable bufferDescriptorsSize 

```C++
uint32_t ns_ble_service_control::bufferDescriptorsSize;
```






### variable bufferPool 

```C++
uint32_t* ns_ble_service_control::bufferPool;
```






### variable bufferPoolSize 

```C++
uint32_t ns_ble_service_control::bufferPoolSize;
```






### variable cccCount 


```C++
uint32_t ns_ble_service_control::cccCount;
```



CCC descriptor list 


        



### variable cccSet 


```C++
attsCccSet_t* ns_ble_service_control::cccSet;
```



Attribute list 


        



### variable connection 


```C++
ns_ble_connection_t* ns_ble_service_control::connection;
```



Cached handle list length 


        



### variable discState 


```C++
uint8_t ns_ble_service_control::discState;
```



TRUE if auto-connecting 


        



### variable group 


```C++
attsGroup_t* ns_ble_service_control::group;
```



Connection control block 


        



### variable handlerId 

```C++
wsfHandlerId_t ns_ble_service_control::handlerId;
```






### variable handler\_cb 


```C++
ns_ble_service_specific_handler_cb ns_ble_service_control::handler_cb;
```



Scan data length 


        



### variable handler\_init\_cb 


```C++
ns_ble_service_specific_handler_init_cb ns_ble_service_control::handler_init_cb;
```



Called after generic event handling 


        



### variable hdlListLen 


```C++
uint8_t ns_ble_service_control::hdlListLen;
```



Service discovery state 


        



### variable list 


```C++
attsAttr_t* ns_ble_service_control::list;
```



Attribute group 


        



### variable procMsg\_cb 


```C++
ns_ble_service_specific_procMsg_cb ns_ble_service_control::procMsg_cb;
```



Service-specific init 


        



### variable scanData 


```C++
uint8_t* ns_ble_service_control::scanData;
```



Advertising data length 


        



### variable scanDataLen 


```C++
uint8_t ns_ble_service_control::scanDataLen;
```



Scan data 


        



### variable scanning 


```C++
bool_t ns_ble_service_control::scanning;
```



WSF handler ID 


        



### variable service\_init\_cb 


```C++
ns_ble_service_specific_init_cb ns_ble_service_control::service_init_cb;
```



Service-specific handler init 


        



### variable wsfBufCount 

_Number of WSF buffers._ 
```C++
uint8_t ns_ble_service_control::wsfBufCount;
```



Service-specific message handler 


        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/includes-api/ns_ble.h`

