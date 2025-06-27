

# Group NeuralSPOT-spi



[**Modules**](modules.md) **>** [**NeuralSPOT-spi**](group___neural_s_p_o_t-spi.md)




















## Classes

| Type | Name |
| ---: | :--- |
| struct | [**ns\_spi\_cfg**](structns__spi__cfg.md) <br> |


## Public Types

| Type | Name |
| ---: | :--- |
| typedef void(\* | [**ns\_spi\_cb**](#typedef-ns_spi_cb)  <br> |
| typedef struct [**ns\_spi\_cfg**](structns__spi__cfg.md) | [**ns\_spi\_config\_t**](#typedef-ns_spi_config_t)  <br> |
| enum  | [**ns\_spi\_status\_e**](#enum-ns_spi_status_e)  <br> |




## Public Attributes

| Type | Name |
| ---: | :--- |
|  ns\_spi\_cb | [**ns\_spi\_cfg::cb**](#variable-cb)  <br> |
|  int8\_t | [**ns\_spi\_cfg::iom**](#variable-iom)  <br> |
|  void \* | [**ns\_spi\_cfg::iomHandle**](#variable-iomhandle)  <br> |
|  am\_hal\_iom\_config\_t | [**ns\_spi\_cfg::sIomCfg**](#variable-siomcfg)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_spi\_handle\_iom\_isr**](#function-ns_spi_handle_iom_isr) (void) <br> |
|  uint32\_t | [**ns\_spi\_interface\_init**](#function-ns_spi_interface_init) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, uint32\_t speed, am\_hal\_iom\_spi\_mode\_e mode) <br>_Initialize the SPI interface._  |
|  uint32\_t | [**ns\_spi\_read**](#function-ns_spi_read) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br>_Read from a SPI device. The SPI device is selected by the csPin parameter._  |
|  uint32\_t | [**ns\_spi\_read\_dma**](#function-ns_spi_read_dma) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br>_Issure DMA read, the cfg-&gt;callback will be called when the transfer is complete._  |
|  uint32\_t | [**ns\_spi\_transfer**](#function-ns_spi_transfer) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* txBuf, const void \* rxBuf, uint32\_t size, uint32\_t csPin) <br>_Transfer data to/from a SPI device. The SPI device is selected by the csPin parameter._  |
|  uint32\_t | [**ns\_spi\_write**](#function-ns_spi_write) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br>_Write to a SPI device. The SPI device is selected by the csPin parameter._  |




























## Public Types Documentation




### typedef ns\_spi\_cb 

```C++
typedef void(* ns_spi_cb) (struct ns_spi_cfg *);
```




<hr>



### typedef ns\_spi\_config\_t 

```C++
typedef struct ns_spi_cfg ns_spi_config_t;
```




<hr>



### enum ns\_spi\_status\_e 

```C++
enum ns_spi_status_e {
    NS_SPI_STATUS_SUCCESS = 0,
    NS_SPI_STATUS_ERROR = 1
};
```




<hr>
## Public Attributes Documentation




### variable cb 

```C++
ns_spi_cb ns_spi_cfg::cb;
```




<hr>



### variable iom 

```C++
int8_t ns_spi_cfg::iom;
```




<hr>



### variable iomHandle 

```C++
void* ns_spi_cfg::iomHandle;
```




<hr>



### variable sIomCfg 

```C++
am_hal_iom_config_t ns_spi_cfg::sIomCfg;
```




<hr>
## Public Functions Documentation




### function ns\_spi\_handle\_iom\_isr 

```C++
void ns_spi_handle_iom_isr (
    void
) 
```



IOM ISRs Take over correct IOM ISR. 


        

<hr>



### function ns\_spi\_interface\_init 

_Initialize the SPI interface._ 
```C++
uint32_t ns_spi_interface_init (
    ns_spi_config_t * cfg,
    uint32_t speed,
    am_hal_iom_spi_mode_e mode
) 
```





**Parameters:**


* `cfg` SPI Configuration 
* `speed` Bus speed in Hz 
* `mode` SPI mode (CPOL, CPHA) 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_spi\_read 

_Read from a SPI device. The SPI device is selected by the csPin parameter._ 
```C++
uint32_t ns_spi_read (
    ns_spi_config_t * cfg,
    const void * buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
) 
```





**Parameters:**


* `cfg` SPI Configuration 
* `buf` Buffer to read into 
* `bufLen` Buffer length 
* `reg` Register to read from 
* `regLen` Register length 
* `csPin` CS pin number 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_spi\_read\_dma 

_Issure DMA read, the cfg-&gt;callback will be called when the transfer is complete._ 
```C++
uint32_t ns_spi_read_dma (
    ns_spi_config_t * cfg,
    const void * buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
) 
```





**Parameters:**


* `cfg` 
* `buf` 
* `bufLen` 
* `reg` 
* `regLen` 
* `csPin` 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_spi\_transfer 

_Transfer data to/from a SPI device. The SPI device is selected by the csPin parameter._ 
```C++
uint32_t ns_spi_transfer (
    ns_spi_config_t * cfg,
    const void * txBuf,
    const void * rxBuf,
    uint32_t size,
    uint32_t csPin
) 
```





**Parameters:**


* `cfg` SPI Configuration 
* `txBuf` Transmit buffer 
* `rxBuf` Receive buffer 
* `size` Size of the transfer 
* `csPin` CS pin number 



**Returns:**

uint32\_t 





        

<hr>



### function ns\_spi\_write 

_Write to a SPI device. The SPI device is selected by the csPin parameter._ 
```C++
uint32_t ns_spi_write (
    ns_spi_config_t * cfg,
    const void * buf,
    uint32_t bufLen,
    uint64_t reg,
    uint32_t regLen,
    uint32_t csPin
) 
```





**Parameters:**


* `cfg` SPI Configuration 
* `buf` Buffer to write from 
* `bufLen` Buffer length 
* `reg` Register to write to 
* `regLen` Register length 
* `csPin` CS pin number 



**Returns:**

uint32\_t 





        

<hr>

------------------------------


