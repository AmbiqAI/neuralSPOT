

# Struct ns\_ble\_connection\_t



[**ClassList**](annotated.md) **>** [**ns\_ble\_connection\_t**](structns__ble__connection__t.md)



_BLE Connection control block._ [More...](#detailed-description)

* `#include <ns_ble.h>`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  bdAddr\_t | [**addr**](#variable-addr)  <br> |
|  uint8\_t | [**addrType**](#variable-addrtype)  <br> |
|  appDbHdl\_t | [**dbHdl**](#variable-dbhdl)  <br> |
|  bool\_t | [**doConnect**](#variable-doconnect)  <br> |












































## Detailed Description


Application message type 


    
## Public Attributes Documentation




### variable addr 

```C++
bdAddr_t ns_ble_connection_t::addr;
```



Type of address of device to connect to 


        

<hr>



### variable addrType 

```C++
uint8_t ns_ble_connection_t::addrType;
```



Device database record handle type 


        

<hr>



### variable dbHdl 

```C++
appDbHdl_t ns_ble_connection_t::dbHdl;
```




<hr>



### variable doConnect 

```C++
bool_t ns_ble_connection_t::doConnect;
```



Address of device to connect to 


        

<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-ble/includes-api/ns_ble.h`

