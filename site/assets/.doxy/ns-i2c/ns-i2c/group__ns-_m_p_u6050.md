

# Group ns-MPU6050



[**Modules**](modules.md) **>** [**ns-MPU6050**](group__ns-_m_p_u6050.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**mpu6050\_config\_t**](structmpu6050__config__t.md) <br> |
| struct | [**mpu6050\_fifo\_config\_t**](structmpu6050__fifo__config__t.md) <br> |
| struct | [**mpu6050\_int\_config\_t**](structmpu6050__int__config__t.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**mpu6050\_accel\_fs\_t**](#enum-mpu6050_accel_fs_t)  <br> |
| enum  | [**mpu6050\_clock\_src\_t**](#enum-mpu6050_clock_src_t)  <br> |
| enum  | [**mpu6050\_dlpf\_cfg\_t**](#enum-mpu6050_dlpf_cfg_t)  <br> |
| enum  | [**mpu6050\_gyro\_fs\_t**](#enum-mpu6050_gyro_fs_t)  <br> |
| enum  | [**mpu6050\_status\_e**](#enum-mpu6050_status_e)  <br> |




















## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**mpu6050\_accel\_fsr\_value**](#function-mpu6050_accel_fsr_value) ([**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) range) <br>_Acceleration scale in units of G._  |
|  float | [**mpu6050\_accel\_resolution**](#function-mpu6050_accel_resolution) ([**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) range) <br>_Acceleration scale LSB resolution._  |
|  float | [**mpu6050\_accel\_to\_gravity**](#function-mpu6050_accel_to_gravity) (int16\_t val, [**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) range) <br>_Convert acceleration value to G._  |
|  uint32\_t | [**mpu6050\_calibrate**](#function-mpu6050_calibrate) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Calibrate device offsets. Device must be still on a flat surface._  |
|  uint32\_t | [**mpu6050\_configure\_fifo**](#function-mpu6050_configure_fifo) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_fifo\_config\_t**](structmpu6050__fifo__config__t.md) \* fifoConfig) <br>_Configure FIFO._  |
|  uint32\_t | [**mpu6050\_configure\_interrupt**](#function-mpu6050_configure_interrupt) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_int\_config\_t**](structmpu6050__int__config__t.md) \* intConfig) <br>_Configure interrupts._  |
|  uint32\_t | [**mpu6050\_device\_reset**](#function-mpu6050_device_reset) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Hard reset device._  |
|  uint32\_t | [**mpu6050\_fifo\_pop**](#function-mpu6050_fifo_pop) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int16\_t \* value) <br>_Pop next value from FIFO._  |
|  uint32\_t | [**mpu6050\_get\_accel\_values**](#function-mpu6050_get_accel_values) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int16\_t \* x, int16\_t \* y, int16\_t \* z) <br>_Get current acceleration values._  |
|  uint32\_t | [**mpu6050\_get\_fifo\_count**](#function-mpu6050_get_fifo_count) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint16\_t \* count) <br>_Get FIFO count._  |
|  uint32\_t | [**mpu6050\_get\_gyro\_values**](#function-mpu6050_get_gyro_values) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int16\_t \* x, int16\_t \* y, int16\_t \* z) <br>_Get current gyro values._  |
|  uint32\_t | [**mpu6050\_get\_interrupt\_status**](#function-mpu6050_get_interrupt_status) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t \* status) <br>_Get interrupt status register._  |
|  uint32\_t | [**mpu6050\_get\_temperature**](#function-mpu6050_get_temperature) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int16\_t \* t) <br>_Get current temperature value._  |
|  uint32\_t | [**mpu6050\_gyro\_fsr\_value**](#function-mpu6050_gyro_fsr_value) (const [**mpu6050\_gyro\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_gyro_fs_t) range) <br>_Gyroscope scale in units dps._  |
|  float | [**mpu6050\_gyro\_resolution**](#function-mpu6050_gyro_resolution) (const [**mpu6050\_gyro\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_gyro_fs_t) range) <br>_Gyroscope scale LSB resolution._  |
|  float | [**mpu6050\_gyro\_to\_deg\_per\_sec**](#function-mpu6050_gyro_to_deg_per_sec) (int val, [**mpu6050\_gyro\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_gyro_fs_t) range) <br>_Convert gyroscope value to degrees/second._  |
|  uint32\_t | [**mpu6050\_init**](#function-mpu6050_init) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, [**mpu6050\_config\_t**](structmpu6050__config__t.md) \* c, uint32\_t devAddr) <br> |
|  uint32\_t | [**mpu6050\_mean\_sensors**](#function-mpu6050_mean_sensors) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int \* meanAX, int \* meanAY, int \* meanAZ, int \* meanGX, int \* meanGY, int \* meanGZ) <br>_Get average sensor values._  |
|  uint32\_t | [**mpu6050\_read\_sensors**](#function-mpu6050_read_sensors) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t \* buffer) <br>_Reads MPU sensor values (3-axis accel, 1 temp, 3-axis gyro)_  |
|  uint32\_t | [**mpu6050\_reset\_fifo**](#function-mpu6050_reset_fifo) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Reset FIFO._  |
|  uint32\_t | [**mpu6050\_reset\_signal\_conds**](#function-mpu6050_reset_signal_conds) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Reset signal conditions._  |
|  uint32\_t | [**mpu6050\_reset\_signal\_paths**](#function-mpu6050_reset_signal_paths) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Reset signal paths._  |
|  uint32\_t | [**mpu6050\_set\_accel\_full\_scale**](#function-mpu6050_set_accel_full_scale) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) fsr) <br>_Set accel full scale range._  |
|  uint32\_t | [**mpu6050\_set\_accel\_offset**](#function-mpu6050_set_accel_offset) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t axis, int offset) <br>_Set acceleration offset for axis._  |
|  uint32\_t | [**mpu6050\_set\_clock\_source**](#function-mpu6050_set_clock_source) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_clock\_src\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_clock_src_t) sel) <br>_Set clock source._  |
|  uint32\_t | [**mpu6050\_set\_fifo\_enable**](#function-mpu6050_set_fifo_enable) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t enable) <br>_Enable FIFO._  |
|  uint32\_t | [**mpu6050\_set\_gyro\_full\_scale**](#function-mpu6050_set_gyro_full_scale) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_gyro\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_gyro_fs_t) fsr) <br>_Set gyro full scale range._  |
|  uint32\_t | [**mpu6050\_set\_gyro\_offset**](#function-mpu6050_set_gyro_offset) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t axis, int offset) <br>_Set gyroscope offset for axis._  |
|  uint32\_t | [**mpu6050\_set\_interrupt\_enable**](#function-mpu6050_set_interrupt_enable) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t overflowEnable, uint8\_t dataReadyEnable) <br>_Enable/disable interrupts._  |
|  uint32\_t | [**mpu6050\_set\_lowpass\_filter**](#function-mpu6050_set_lowpass_filter) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, mpu6050\_dlpf\_cfg\_t value) <br>_Set digital lowpass filter._  |
|  uint32\_t | [**mpu6050\_set\_lowpower\_accel\_mode**](#function-mpu6050_set_lowpower_accel_mode) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t wakeFreq) <br>_Put device into low-power acceleration-only mode._  |
|  uint32\_t | [**mpu6050\_set\_sample\_rate**](#function-mpu6050_set_sample_rate) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint16\_t rate) <br>_Set target sample rate._  |
|  uint32\_t | [**mpu6050\_set\_sample\_rate\_divider**](#function-mpu6050_set_sample_rate_divider) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t divider) <br>_Set sample rate divider._  |
|  uint32\_t | [**mpu6050\_set\_sleep**](#function-mpu6050_set_sleep) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t sleep) <br>_Enable/disable sleep mode._  |
|  uint32\_t | [**mpu6050\_set\_temperature\_disable**](#function-mpu6050_set_temperature_disable) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t disable) <br>_Disable temperature readings._  |
|  float | [**mpu6050\_temperature\_to\_celsius**](#function-mpu6050_temperature_to_celsius) (int16\_t val) <br>_Convert temperature value to Celsius._  |
|  uint32\_t | [**mpu6050\_test\_connection**](#function-mpu6050_test_connection) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Verify device by reading WHO\_AM\_I register._  |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**MPU\_I2CADDRESS\_AD0\_HIGH**](ns__mpu6050__i2c__driver_8h.md#define-mpu_i2caddress_ad0_high)  `0x69`<br> |
| define  | [**MPU\_I2CADDRESS\_AD0\_LOW**](ns__mpu6050__i2c__driver_8h.md#define-mpu_i2caddress_ad0_low)  `0x68`<br> |

## Public Types Documentation




### enum mpu6050\_accel\_fs\_t 

```C++
enum mpu6050_accel_fs_t {
    ACCEL_FS_2G = 0,
    ACCEL_FS_4G = 1,
    ACCEL_FS_8G = 2,
    ACCEL_FS_16G = 3
};
```



Accel full-scale range 


        

<hr>



### enum mpu6050\_clock\_src\_t 

```C++
enum mpu6050_clock_src_t {
    CLOCK_INTERNAL = 0,
    CLOCK_GX_PLL = 1,
    CLOCK_GY_PLL = 2,
    CLOCK_GZ_PLL = 3,
    CLOCK_32_REF = 4,
    CLOCK_19_REF = 5,
    CLOCK_KEEP_RESET = 7
};
```



Clock selection 


        

<hr>



### enum mpu6050\_dlpf\_cfg\_t 

```C++
enum mpu6050_dlpf_cfg_t {
    DLPF_260HZ = 0,
    DLPF_184HZ = 1,
    DLPF_094HZ = 2,
    DLPF_044HZ = 3,
    DLPF_021HZ = 4,
    DLPF_010HZ = 5,
    DLPF_005HZ = 6
};
```




<hr>



### enum mpu6050\_gyro\_fs\_t 

```C++
enum mpu6050_gyro_fs_t {
    GYRO_FS_250DPS = 0,
    GYRO_FS_500DPS = 1,
    GYRO_FS_1000DPS = 2,
    GYRO_FS_2000DPS = 3
};
```



Gyroscope full-scale range 


        

<hr>



### enum mpu6050\_status\_e 

```C++
enum mpu6050_status_e {
    MPU6050_STATUS_SUCCESS = 0,
    MPU6050_STATUS_ERROR = 1
};
```




<hr>
## Public Functions Documentation




### function mpu6050\_accel\_fsr\_value 

_Acceleration scale in units of G._ 
```C++
uint32_t mpu6050_accel_fsr_value (
    mpu6050_accel_fs_t range
) 
```





**Parameters:**


* `range` Full scale range selection 



**Returns:**

uint32\_t Scale in G 





        

<hr>



### function mpu6050\_accel\_resolution 

_Acceleration scale LSB resolution._ 
```C++
float mpu6050_accel_resolution (
    mpu6050_accel_fs_t range
) 
```





**Parameters:**


* `range` Full scale range selection 



**Returns:**

float Resolution (G) 





        

<hr>



### function mpu6050\_accel\_to\_gravity 

_Convert acceleration value to G._ 
```C++
float mpu6050_accel_to_gravity (
    int16_t val,
    mpu6050_accel_fs_t range
) 
```





**Parameters:**


* `val` Accel register value 
* `range` Full-scale range used 



**Returns:**

Acceleration (G) 





        

<hr>



### function mpu6050\_calibrate 

_Calibrate device offsets. Device must be still on a flat surface._ 
```C++
uint32_t mpu6050_calibrate (
    ns_i2c_config_t * cfg,
    uint32_t devAddr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t status


Calibrate device offsets. Device must be still on a flat surface.




**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_configure\_fifo 

_Configure FIFO._ 
```C++
uint32_t mpu6050_configure_fifo (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    mpu6050_fifo_config_t * fifoConfig
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `fifoConfig` FIFO configuration 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_configure\_interrupt 

_Configure interrupts._ 
```C++
uint32_t mpu6050_configure_interrupt (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    mpu6050_int_config_t * intConfig
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `intConfig` Interrupt configuration 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_device\_reset 

_Hard reset device._ 
```C++
uint32_t mpu6050_device_reset (
    ns_i2c_config_t * cfg,
    uint32_t devAddr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_fifo\_pop 

_Pop next value from FIFO._ 
```C++
uint32_t mpu6050_fifo_pop (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    int16_t * value
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `value` Pointer to FIFO value 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_get\_accel\_values 

_Get current acceleration values._ 
```C++
uint32_t mpu6050_get_accel_values (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    int16_t * x,
    int16_t * y,
    int16_t * z
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `x` Pointer to store x axis 
* `y` Pointer to store y axis 
* `z` Pointer to store z axis 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_get\_fifo\_count 

_Get FIFO count._ 
```C++
uint32_t mpu6050_get_fifo_count (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint16_t * count
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `count` Pointer to store count 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_get\_gyro\_values 

_Get current gyro values._ 
```C++
uint32_t mpu6050_get_gyro_values (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    int16_t * x,
    int16_t * y,
    int16_t * z
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `x` Pointer to store x axis 
* `y` Pointer to store y axis 
* `z` Pointer to store z axis 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_get\_interrupt\_status 

_Get interrupt status register._ 
```C++
uint32_t mpu6050_get_interrupt_status (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t * status
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `status` Pointer to store status register 



**Returns:**

uint32\_t 





        

<hr>



### function mpu6050\_get\_temperature 

_Get current temperature value._ 
```C++
uint32_t mpu6050_get_temperature (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    int16_t * t
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `t` Pointer to store Temperature 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_gyro\_fsr\_value 

_Gyroscope scale in units dps._ 
```C++
uint32_t mpu6050_gyro_fsr_value (
    const mpu6050_gyro_fs_t range
) 
```





**Parameters:**


* `range` Full scale range selection 



**Returns:**

uint32\_t Scale (°/s) 





        

<hr>



### function mpu6050\_gyro\_resolution 

_Gyroscope scale LSB resolution._ 
```C++
float mpu6050_gyro_resolution (
    const mpu6050_gyro_fs_t range
) 
```





**Parameters:**


* `range` Full scale range selection 



**Returns:**

float Resolution (°/s) 





        

<hr>



### function mpu6050\_gyro\_to\_deg\_per\_sec 

_Convert gyroscope value to degrees/second._ 
```C++
float mpu6050_gyro_to_deg_per_sec (
    int val,
    mpu6050_gyro_fs_t range
) 
```





**Parameters:**


* `val` Gyroscope register value 
* `range` Full-scale range used 



**Returns:**

float Gyroscope (°/s) 





        

<hr>



### function mpu6050\_init 

```C++
uint32_t mpu6050_init (
    ns_i2c_config_t * cfg,
    mpu6050_config_t * c,
    uint32_t devAddr
) 
```




<hr>



### function mpu6050\_mean\_sensors 

_Get average sensor values._ 
```C++
uint32_t mpu6050_mean_sensors (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    int * meanAX,
    int * meanAY,
    int * meanAZ,
    int * meanGX,
    int * meanGY,
    int * meanGZ
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `meanAX` Pointer to store mean X accel 
* `meanAY` Pointer to store mean Y accel 
* `meanAZ` Pointer to store mean Z accel 
* `meanGX` Pointer to store mean X gyro 
* `meanGY` Pointer to store mean Y gyro 
* `meanGZ` Pointer to store mean Z gyro 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_read\_sensors 

_Reads MPU sensor values (3-axis accel, 1 temp, 3-axis gyro)_ 
```C++
uint32_t mpu6050_read_sensors (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t * buffer
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `buffer` 14-byte buffer to store values 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_reset\_fifo 

_Reset FIFO._ 
```C++
uint32_t mpu6050_reset_fifo (
    ns_i2c_config_t * cfg,
    uint32_t devAddr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t 





        

<hr>



### function mpu6050\_reset\_signal\_conds 

_Reset signal conditions._ 
```C++
uint32_t mpu6050_reset_signal_conds (
    ns_i2c_config_t * cfg,
    uint32_t devAddr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_reset\_signal\_paths 

_Reset signal paths._ 
```C++
uint32_t mpu6050_reset_signal_paths (
    ns_i2c_config_t * cfg,
    uint32_t devAddr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_accel\_full\_scale 

_Set accel full scale range._ 
```C++
uint32_t mpu6050_set_accel_full_scale (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    mpu6050_accel_fs_t fsr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `fsr` Full scale range selection 



**Returns:**

uint32\_t 





        

<hr>



### function mpu6050\_set\_accel\_offset 

_Set acceleration offset for axis._ 
```C++
uint32_t mpu6050_set_accel_offset (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t axis,
    int offset
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `axis` x=0, y=1, z=2 
* `offset` Offset value 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_clock\_source 

_Set clock source._ 
```C++
uint32_t mpu6050_set_clock_source (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    mpu6050_clock_src_t sel
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `sel` Clock selection 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_fifo\_enable 

_Enable FIFO._ 
```C++
uint32_t mpu6050_set_fifo_enable (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t enable
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `enable` Enable FIFO flag 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_gyro\_full\_scale 

_Set gyro full scale range._ 
```C++
uint32_t mpu6050_set_gyro_full_scale (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    mpu6050_gyro_fs_t fsr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `fsr` Full scale range selection 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_gyro\_offset 

_Set gyroscope offset for axis._ 
```C++
uint32_t mpu6050_set_gyro_offset (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t axis,
    int offset
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `axis` x=0, y=1, z=2 
* `offset` Offset value 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_interrupt\_enable 

_Enable/disable interrupts._ 
```C++
uint32_t mpu6050_set_interrupt_enable (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t overflowEnable,
    uint8_t dataReadyEnable
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `overflowEnable` Overflow triggers interrupt 
* `dataReadyEnable` Data ready triggers interrupt 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_lowpass\_filter 

_Set digital lowpass filter._ 
```C++
uint32_t mpu6050_set_lowpass_filter (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    mpu6050_dlpf_cfg_t value
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `value` Filter selection 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_lowpower\_accel\_mode 

_Put device into low-power acceleration-only mode._ 
```C++
uint32_t mpu6050_set_lowpower_accel_mode (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t wakeFreq
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `wakeFreq` Wake-up frequency 0=1.25Hz, 1=5Hz 2=20Hz 3=40Hz 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_sample\_rate 

_Set target sample rate._ 
```C++
uint32_t mpu6050_set_sample_rate (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint16_t rate
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `rate` Target rate in Hz 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_sample\_rate\_divider 

_Set sample rate divider._ 
```C++
uint32_t mpu6050_set_sample_rate_divider (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t divider
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `divider` Sample rate divider 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_sleep 

_Enable/disable sleep mode._ 
```C++
uint32_t mpu6050_set_sleep (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t sleep
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `sleep` Sleep flag 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_set\_temperature\_disable 

_Disable temperature readings._ 
```C++
uint32_t mpu6050_set_temperature_disable (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t disable
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 
* `disable` Disable temperature flag 



**Returns:**

uint32\_t status 





        

<hr>



### function mpu6050\_temperature\_to\_celsius 

_Convert temperature value to Celsius._ 
```C++
float mpu6050_temperature_to_celsius (
    int16_t val
) 
```





**Parameters:**


* `val` Temperature register 



**Returns:**

float Temperature (°C) 





        

<hr>



### function mpu6050\_test\_connection 

_Verify device by reading WHO\_AM\_I register._ 
```C++
uint32_t mpu6050_test_connection (
    ns_i2c_config_t * cfg,
    uint32_t devAddr
) 
```





**Parameters:**


* `cfg` I2C configuration 
* `devAddr` Device I2C address 



**Returns:**

uint32\_t status 





        

<hr>
## Macro Definition Documentation





### define MPU\_I2CADDRESS\_AD0\_HIGH 

```C++
#define MPU_I2CADDRESS_AD0_HIGH `0x69`
```




<hr>



### define MPU\_I2CADDRESS\_AD0\_LOW 

```C++
#define MPU_I2CADDRESS_AD0_LOW `0x68`
```




<hr>

------------------------------


