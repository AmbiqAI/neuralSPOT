

# File ns\_spi.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-spi**](dir_83ef0682a07896930595a928a537e437.md) **>** [**src**](dir_ec178003752deb789134573b3c734db0.md) **>** [**ns\_spi.c**](ns__spi_8c.md)

[Go to the source code of this file](ns__spi_8c_source.md)

_Generic SPI driver._ [More...](#detailed-description)

* `#include <string.h>`
* `#include "ns_spi.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_ambiqsuite_harness.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  am\_hal\_gpio\_pincfg\_t | [**NS\_AM\_BSP\_GPIO\_IOM1\_CS**](#variable-ns_am_bsp_gpio_iom1_cs)   = `/* multi line expression */`<br> |
|  am\_hal\_gpio\_pincfg\_t | [**NS\_AM\_BSP\_GPIO\_IOM1\_MISO**](#variable-ns_am_bsp_gpio_iom1_miso)   = `/* multi line expression */`<br> |
|  am\_hal\_gpio\_pincfg\_t | [**NS\_AM\_BSP\_GPIO\_IOM1\_MOSI**](#variable-ns_am_bsp_gpio_iom1_mosi)   = `/* multi line expression */`<br> |
|  am\_hal\_gpio\_pincfg\_t | [**NS\_AM\_BSP\_GPIO\_IOM1\_SCK**](#variable-ns_am_bsp_gpio_iom1_sck)   = `/* multi line expression */`<br> |
|  [**ns\_spi\_config\_t**](structns__spi__cfg.md) | [**ns\_spi\_config**](#variable-ns_spi_config)  <br> |
|  uint32\_t | [**ns\_spi\_tcb\_command\_buffer**](#variable-ns_spi_tcb_command_buffer)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_high\_drive\_pins\_enable**](#function-ns_high_drive_pins_enable) () <br> |
|  void | [**ns\_spi\_handle\_iom\_isr**](#function-ns_spi_handle_iom_isr) (void) <br> |
|  uint32\_t | [**ns\_spi\_interface\_init**](#function-ns_spi_interface_init) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, uint32\_t speed, am\_hal\_iom\_spi\_mode\_e mode) <br>_Initialize the SPI interface._  |
|  uint32\_t | [**ns\_spi\_read**](#function-ns_spi_read) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br>_Read from a SPI device. The SPI device is selected by the csPin parameter._  |
|  uint32\_t | [**ns\_spi\_read\_dma**](#function-ns_spi_read_dma) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br>_Issure DMA read, the cfg-&gt;callback will be called when the transfer is complete._  |
|  uint32\_t | [**ns\_spi\_transfer**](#function-ns_spi_transfer) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* txBuf, const void \* rxBuf, uint32\_t size, uint32\_t csPin) <br>_Transfer data to/from a SPI device. The SPI device is selected by the csPin parameter._  |
|  uint32\_t | [**ns\_spi\_write**](#function-ns_spi_write) ([**ns\_spi\_config\_t**](structns__spi__cfg.md) \* cfg, const void \* buf, uint32\_t bufLen, uint64\_t reg, uint32\_t regLen, uint32\_t csPin) <br>_Write to a SPI device. The SPI device is selected by the csPin parameter._  |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**ns\_spi\_dma\_read\_complete\_cb**](#function-ns_spi_dma_read_complete_cb) (void \* pCallbackCtxt, uint32\_t ui32TransactionStatus) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**NS\_SPI\_DMA\_MAX\_XFER\_SIZE**](ns__spi_8c.md#define-ns_spi_dma_max_xfer_size)  `4095`<br> |

## Detailed Description




**Author:**

Adam Page 




**Version:**

0.1 




**Date:**

2022-08-26




**Copyright:**

Copyright (c) 2022 





    
## Public Attributes Documentation




### variable NS\_AM\_BSP\_GPIO\_IOM1\_CS 

```C++
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_CS;
```




<hr>



### variable NS\_AM\_BSP\_GPIO\_IOM1\_MISO 

```C++
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_MISO;
```




<hr>



### variable NS\_AM\_BSP\_GPIO\_IOM1\_MOSI 

```C++
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_MOSI;
```




<hr>



### variable NS\_AM\_BSP\_GPIO\_IOM1\_SCK 

```C++
am_hal_gpio_pincfg_t NS_AM_BSP_GPIO_IOM1_SCK;
```




<hr>



### variable ns\_spi\_config 

```C++
ns_spi_config_t ns_spi_config;
```




<hr>



### variable ns\_spi\_tcb\_command\_buffer 

```C++
uint32_t ns_spi_tcb_command_buffer[NS_SPI_DMA_MAX_XFER_SIZE+1];
```




<hr>
## Public Functions Documentation




### function ns\_high\_drive\_pins\_enable 

```C++
void ns_high_drive_pins_enable () 
```




<hr>



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
## Public Static Functions Documentation




### function ns\_spi\_dma\_read\_complete\_cb 

```C++
static void ns_spi_dma_read_complete_cb (
    void * pCallbackCtxt,
    uint32_t ui32TransactionStatus
) 
```




<hr>
## Macro Definition Documentation





### define NS\_SPI\_DMA\_MAX\_XFER\_SIZE 

```C++
#define NS_SPI_DMA_MAX_XFER_SIZE `4095`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-spi/src/ns_spi.c`

