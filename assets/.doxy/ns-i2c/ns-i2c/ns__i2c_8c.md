

# File ns\_i2c.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-i2c**](dir_cc1f492d5d9f73ec0b0ac0581cc456e0.md) **>** [**src**](dir_d6c56226b0c7cd8d6baff594b5865597.md) **>** [**ns\_i2c.c**](ns__i2c_8c.md)

[Go to the source code of this file](ns__i2c_8c_source.md)

_Generic i2c driver._ [More...](#detailed-description)

* `#include "ns_i2c.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  const ns\_core\_api\_t | [**ns\_i2c\_V0\_0\_1**](#variable-ns_i2c_v0_0_1)  <br> |
|  const ns\_core\_api\_t | [**ns\_i2c\_V1\_0\_0**](#variable-ns_i2c_v1_0_0)  <br> |
|  const ns\_core\_api\_t | [**ns\_i2c\_current\_version**](#variable-ns_i2c_current_version)  <br> |
|  const ns\_core\_api\_t | [**ns\_i2c\_oldest\_supported\_version**](#variable-ns_i2c_oldest_supported_version)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_i2c\_interface\_init**](#function-ns_i2c_interface_init) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t speed) <br>_Initialize I2C on one of the IOM (IO managers)_  |
|  uint32\_t | [**ns\_i2c\_read**](#function-ns_i2c_read) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, const void \* buf, uint32\_t size, uint16\_t addr) <br>_Perform low-level I2C read using IOM transfer._  |
|  uint32\_t | [**ns\_i2c\_transfer**](#function-ns_i2c_transfer) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, [**ns\_i2c\_msg\_t**](structns__i2c__msg__t.md) \* msgs, size\_t numMsgs) <br>_Perform sequence of low-level I2C transfers (similar to Linux)_  |
|  uint32\_t | [**ns\_i2c\_write**](#function-ns_i2c_write) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, const void \* buf, uint32\_t size, uint16\_t addr) <br>_Perform low-level I2C write using IOM transfer._  |
|  uint32\_t | [**ns\_i2c\_write\_read**](#function-ns_i2c_write_read) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint16\_t addr, const void \* writeBuf, size\_t numWrite, void \* readBuf, size\_t numRead) <br>_Perform low-level I2C write followed by immediate read._  |




























# Detailed Description




**Author:**

Adam Page 




**Version:**

0.1 




**Date:**

2022-08-26




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable ns\_i2c\_V0\_0\_1 

```C++
const ns_core_api_t ns_i2c_V0_0_1;
```






### variable ns\_i2c\_V1\_0\_0 

```C++
const ns_core_api_t ns_i2c_V1_0_0;
```






### variable ns\_i2c\_current\_version 

```C++
const ns_core_api_t ns_i2c_current_version;
```






### variable ns\_i2c\_oldest\_supported\_version 

```C++
const ns_core_api_t ns_i2c_oldest_supported_version;
```



## Public Functions Documentation




### function ns\_i2c\_interface\_init 

_Initialize I2C on one of the IOM (IO managers)_ 
```C++
uint32_t ns_i2c_interface_init (
    ns_i2c_config_t * cfg,
    uint32_t speed
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `speed` I2C speed in Hz 



**Returns:**

uint32\_t status 





        



### function ns\_i2c\_read 

_Perform low-level I2C read using IOM transfer._ 
```C++
uint32_t ns_i2c_read (
    ns_i2c_config_t * cfg,
    const void * buf,
    uint32_t size,
    uint16_t addr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `buf` Buffer to store read bytes 
* `size` Number of bytes to read 
* `addr` I2C device address 




        



### function ns\_i2c\_transfer 

_Perform sequence of low-level I2C transfers (similar to Linux)_ 
```C++
uint32_t ns_i2c_transfer (
    ns_i2c_config_t * cfg,
    ns_i2c_msg_t * msgs,
    size_t numMsgs
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `msgs` I2C messages to transfer 
* `numMsgs` Number of I2C messsages 




        



### function ns\_i2c\_write 

_Perform low-level I2C write using IOM transfer._ 
```C++
uint32_t ns_i2c_write (
    ns_i2c_config_t * cfg,
    const void * buf,
    uint32_t size,
    uint16_t addr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `buf` Buffer of bytes to write 
* `size` Number of bytes to write 
* `addr` I2C device address 




        



### function ns\_i2c\_write\_read 

_Perform low-level I2C write followed by immediate read._ 
```C++
uint32_t ns_i2c_write_read (
    ns_i2c_config_t * cfg,
    uint16_t addr,
    const void * writeBuf,
    size_t numWrite,
    void * readBuf,
    size_t numRead
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `writeBuf` Write buffer 
* `numWrite` Number of bytes to write 
* `readBuf` Read buffer 
* `numRead` Number of bytes to read 
* `addr` I2C device address 




        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-i2c/src/ns_i2c.c`

