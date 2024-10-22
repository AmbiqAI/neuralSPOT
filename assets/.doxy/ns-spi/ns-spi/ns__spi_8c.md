

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
|  const IRQn\_Type | [**gc\_iomIrq**](#variable-gc_iomirq)   = = (IRQn\_Type)(1 + IOMSTR0\_IRQn)<br> |
|  [**ns\_spi\_config\_t**](structns__spi__cfg.md) | [**ns\_spi\_config**](#variable-ns_spi_config)  <br> |
|  uint32\_t | [**ns\_spi\_tcb\_command\_buffer**](#variable-ns_spi_tcb_command_buffer)  <br> |
















## Public Functions

| Type | Name |
| ---: | :--- |
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
| define  | [**NS\_SPI\_DMA\_MAX\_XFER\_SIZE**](ns__spi_8c.md#define-ns_spi_dma_max_xfer_size)  4095<br> |
| define  | [**am\_iom\_isr**](ns__spi_8c.md#define-am_iom_isr) (n) am\_iomaster##n##\_isr<br> |
| define  | [**am\_iom\_isrx**](ns__spi_8c.md#define-am_iom_isrx) (n) am\_iom\_isr(n)<br> |
| define  | [**iom\_isr**](ns__spi_8c.md#define-iom_isr) (void) am\_iom\_isrx(1)<br>_iom isr prototype_  |

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




### variable gc\_iomIrq 

```C++
const IRQn_Type gc_iomIrq;
```






### variable ns\_spi\_config 

```C++
ns_spi_config_t ns_spi_config;
```






### variable ns\_spi\_tcb\_command\_buffer 

```C++
uint32_t ns_spi_tcb_command_buffer[NS_SPI_DMA_MAX_XFER_SIZE+1];
```



## Public Functions Documentation




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





        
## Public Static Functions Documentation




### function ns\_spi\_dma\_read\_complete\_cb 

```C++
static void ns_spi_dma_read_complete_cb (
    void * pCallbackCtxt,
    uint32_t ui32TransactionStatus
) 
```



## Macro Definition Documentation





### define NS\_SPI\_DMA\_MAX\_XFER\_SIZE 

```C++
#define NS_SPI_DMA_MAX_XFER_SIZE 4095
```






### define am\_iom\_isr 

```C++
#define am_iom_isr (
    n
) am_iomaster##n##_isr
```






### define am\_iom\_isrx 

```C++
#define am_iom_isrx (
    n
) am_iom_isr(n)
```






### define iom\_isr 

_iom isr prototype_ 
```C++
#define iom_isr (
    void
) am_iom_isrx(1)
```



Take over the interrupt handler for whichever IOM is used.


IOM ISRs Take over correct IOM ISR. 


        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-spi/src/ns_spi.c`

