

# File ns\_i2c\_register\_driver.h



[**FileList**](files.md) **>** [**includes-api**](dir_ead24bed0d4d5fb1b23b4266f13c1ec5.md) **>** [**ns\_i2c\_register\_driver.h**](ns__i2c__register__driver_8h.md)

[Go to the source code of this file](ns__i2c__register__driver_8h_source.md)

_Generic i2c driver for register-based i2c devices._ [More...](#detailed-description)

* `#include "ns_i2c.h"`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_i2c\_read\_reg**](#function-ns_i2c_read_reg) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t regAddr, uint8\_t \* value, uint8\_t mask) <br>_Read 8-bit register over I2C._  |
|  uint32\_t | [**ns\_i2c\_read\_sequential\_regs**](#function-ns_i2c_read_sequential_regs) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint32\_t regAddr, void \* buf, uint32\_t size) <br>_Read sequential 8-bit registers over I2C._  |
|  uint32\_t | [**ns\_i2c\_write\_reg**](#function-ns_i2c_write_reg) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t regAddr, uint8\_t value, uint8\_t mask) <br>_Write 8-bit register over I2C._  |
|  uint32\_t | [**ns\_i2c\_write\_sequential\_regs**](#function-ns_i2c_write_sequential_regs) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint32\_t regAddr, void \* buf, uint32\_t size) <br>_Write sequential 8-bit registers over I2C._  |




























# Detailed Description




**Author:**

Carlos Morales 




**Version:**

0.1 




**Date:**

2022-08-26




**Copyright:**

Copyright (c) 2022 





    
## Public Functions Documentation




### function ns\_i2c\_read\_reg 

_Read 8-bit register over I2C._ 
```C++
uint32_t ns_i2c_read_reg (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t regAddr,
    uint8_t * value,
    uint8_t mask
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device address 
* `regAddr` Register address 
* `value` Register Value 
* `mask` Read mask 



**Returns:**

uint32\_t status




**Parameters:**


* `cfg` Handle obtained from ns\_i2c\_interface\_init 
* `devAddr` Device address 
* `regAddr` 8-bit register address 
* `value` Register value 
* `mask` Read mask 



**Returns:**

uint32\_t status 





        



### function ns\_i2c\_read\_sequential\_regs 

_Read sequential 8-bit registers over I2C._ 
```C++
uint32_t ns_i2c_read_sequential_regs (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint32_t regAddr,
    void * buf,
    uint32_t size
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device address 
* `regAddr` First register address 
* `buf` Buffer to store register values 
* `size` Number of registers to read 



**Returns:**

uint32\_t status




**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device address 
* `regAddr` First 8-bit register address 
* `buf` Buffer to store register values 
* `size` Number of registers to read 



**Returns:**

uint32\_t status 





        



### function ns\_i2c\_write\_reg 

_Write 8-bit register over I2C._ 
```C++
uint32_t ns_i2c_write_reg (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t regAddr,
    uint8_t value,
    uint8_t mask
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device address 
* `regAddr` Register address 
* `value` Register Value 
* `mask` Write mask 



**Returns:**

uint32\_t status




**Parameters:**


* `cfg` Handle obtained from ns\_i2c\_interface\_init 
* `devAddr` Device address 
* `regAddr` 8-bit register address 
* `value` Register value 
* `mask` Write mask 



**Returns:**

uint32\_t status 





        



### function ns\_i2c\_write\_sequential\_regs 

_Write sequential 8-bit registers over I2C._ 
```C++
uint32_t ns_i2c_write_sequential_regs (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint32_t regAddr,
    void * buf,
    uint32_t size
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device address 
* `regAddr` First register address 
* `buf` Pointer to buffer of values to be written 
* `size` Number of bytes to write 



**Returns:**

uint32\_t status




**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device address 
* `regAddr` First 8-bit register address 
* `buf` Pointer to buffer of values to be written 
* `size` Number of bytes to write 



**Returns:**

uint32\_t status 





        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-i2c/includes-api/ns_i2c_register_driver.h`

