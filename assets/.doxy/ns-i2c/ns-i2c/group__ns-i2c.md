

# Group ns-i2c



[**Modules**](modules.md) **>** [**ns-i2c**](group__ns-i2c.md)
















## Modules

| Type | Name |
| ---: | :--- |
| module | [**Ns-MAX86150**](group__ns-_m_a_x86150.md) <br> |
| module | [**Ns-MPU6050**](group__ns-_m_p_u6050.md) <br> |




## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_i2c\_config\_t**](structns__i2c__config__t.md) <br>_i2c configuration_  |
| struct | [**ns\_i2c\_msg\_t**](structns__i2c__msg__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**ns\_i2c\_status\_e**](#enum-ns_i2c_status_e)  <br> |
| enum  | [**ns\_i2c\_xfer\_flag\_e**](#enum-ns_i2c_xfer_flag_e)  <br> |




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
|  uint32\_t | [**ns\_i2c\_read\_reg**](#function-ns_i2c_read_reg) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t regAddr, uint8\_t \* value, uint8\_t mask) <br>_Read 8-bit register over I2C._  |
|  uint32\_t | [**ns\_i2c\_read\_sequential\_regs**](#function-ns_i2c_read_sequential_regs) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint32\_t regAddr, void \* buf, uint32\_t size) <br>_Read sequential 8-bit registers over I2C._  |
|  uint32\_t | [**ns\_i2c\_transfer**](#function-ns_i2c_transfer) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, [**ns\_i2c\_msg\_t**](structns__i2c__msg__t.md) \* msgs, size\_t numMsgs) <br>_Perform sequence of low-level I2C transfers (similar to Linux)_  |
|  uint32\_t | [**ns\_i2c\_write**](#function-ns_i2c_write) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, const void \* buf, uint32\_t size, uint16\_t addr) <br>_Perform low-level I2C write using IOM transfer._  |
|  uint32\_t | [**ns\_i2c\_write\_read**](#function-ns_i2c_write_read) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint16\_t addr, const void \* writeBuf, size\_t numWrite, void \* readBuf, size\_t numRead) <br>_Perform low-level I2C write followed by immediate read._  |
|  uint32\_t | [**ns\_i2c\_write\_reg**](#function-ns_i2c_write_reg) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t regAddr, uint8\_t value, uint8\_t mask) <br>_Write 8-bit register over I2C._  |
|  uint32\_t | [**ns\_i2c\_write\_sequential\_regs**](#function-ns_i2c_write_sequential_regs) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint32\_t regAddr, void \* buf, uint32\_t size) <br>_Write sequential 8-bit registers over I2C._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_I2C\_API\_ID**](ns__i2c_8h.md#define-ns_i2c_api_id)  0xCA0004<br> |
| define  | [**NS\_I2C\_CURRENT\_VERSION**](ns__i2c_8h.md#define-ns_i2c_current_version)  NS\_I2C\_V1\_0\_0<br> |
| define  | [**NS\_I2C\_OLDEST\_SUPPORTED\_VERSION**](ns__i2c_8h.md#define-ns_i2c_oldest_supported_version)  NS\_I2C\_V0\_0\_1<br> |
| define  | [**NS\_I2C\_V0\_0\_1**](ns__i2c_8h.md#define-ns_i2c_v0_0_1)          { .major = 0, .minor = 0, .revision = 1 }<br> |
| define  | [**NS\_I2C\_V1\_0\_0**](ns__i2c_8h.md#define-ns_i2c_v1_0_0)          { .major = 1, .minor = 0, .revision = 0 }<br> |

## Public Types Documentation




### enum ns\_i2c\_status\_e 

```C++
enum ns_i2c_status_e {
    NS_I2C_STATUS_SUCCESS = 0,
    NS_I2C_STATUS_ERROR = 1
};
```






### enum ns\_i2c\_xfer\_flag\_e 

```C++
enum ns_i2c_xfer_flag_e {
    NS_I2C_XFER_WR = 0,
    NS_I2C_XFER_RD = (1u << 0)
};
```



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





        
## Macro Definition Documentation





### define NS\_I2C\_API\_ID 

```C++
#define NS_I2C_API_ID 0xCA0004
```






### define NS\_I2C\_CURRENT\_VERSION 

```C++
#define NS_I2C_CURRENT_VERSION NS_I2C_V1_0_0
```






### define NS\_I2C\_OLDEST\_SUPPORTED\_VERSION 

```C++
#define NS_I2C_OLDEST_SUPPORTED_VERSION NS_I2C_V0_0_1
```






### define NS\_I2C\_V0\_0\_1 

```C++
#define NS_I2C_V0_0_1 { .major = 0, .minor = 0, .revision = 1 }
```






### define NS\_I2C\_V1\_0\_0 

```C++
#define NS_I2C_V1_0_0 { .major = 1, .minor = 0, .revision = 0 }
```




------------------------------


