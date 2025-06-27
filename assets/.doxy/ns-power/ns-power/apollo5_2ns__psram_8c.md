

# File ns\_psram.c



[**FileList**](files.md) **>** [**apollo5**](dir_d179b330ef1d2a1dad64ceb5f2d1a96e.md) **>** [**ns\_psram.c**](apollo5_2ns__psram_8c.md)

[Go to the source code of this file](apollo5_2ns__psram_8c_source.md)

_Utility for configuring and accessing PSRAM._ [More...](#detailed-description)

* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "ns_core.h"`
* `#include "ns_ambiqsuite_harness.h"`
* `#include "ns_peripherals_psram.h"`
* `#include "am_devices_mspi_psram_aps25616n.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  AM\_SHARED\_RW uint32\_t | [**DMATCBBuffer**](#variable-dmatcbbuffer)  <br> |
|  am\_devices\_mspi\_psram\_ddr\_timing\_config\_t | [**MSPIDdrTimingConfig**](#variable-mspiddrtimingconfig)  <br> |
|  am\_devices\_mspi\_psram\_config\_t | [**MSPI\_PSRAM\_HexCE0MSPIConfig**](#variable-mspi_psram_hexce0mspiconfig)   = `/* multi line expression */`<br> |
|  am\_devices\_mspi\_psram\_config\_t | [**MSPI\_PSRAM\_HexCE1MSPIConfig**](#variable-mspi_psram_hexce1mspiconfig)   = `/* multi line expression */`<br> |
|  am\_devices\_mspi\_psram\_config\_t | [**MSPI\_PSRAM\_OctalCE0MSPIConfig**](#variable-mspi_psram_octalce0mspiconfig)   = `/* multi line expression */`<br> |
|  am\_devices\_mspi\_psram\_config\_t | [**MSPI\_PSRAM\_OctalCE1MSPIConfig**](#variable-mspi_psram_octalce1mspiconfig)   = `/* multi line expression */`<br> |
|  void \* | [**g\_pDevHandle**](#variable-g_pdevhandle)  <br> |
|  void \* | [**g\_pHandle**](#variable-g_phandle)  <br> |
|  am\_hal\_mpu\_attr\_t | [**sMPUAttr**](#variable-smpuattr)   = `/* multi line expression */`<br> |
|  am\_hal\_mpu\_region\_config\_t | [**sMPUCfg**](#variable-smpucfg)   = `/* multi line expression */`<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  const IRQn\_Type | [**mspi\_interrupts**](#variable-mspi_interrupts)   = `/* multi line expression */`<br>_MSPI interrupts._  |














## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**ns\_psram\_platform\_init**](#function-ns_psram_platform_init) ([**ns\_psram\_config\_t**](structns__psram__config__t.md) \* cfg) <br> |



























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**MSPI\_PSRAM\_OCTAL\_CONFIG**](apollo5_2ns__psram_8c.md#define-mspi_psram_octal_config)  `MSPI\_PSRAM\_OctalCE0MSPIConfig`<br> |
| define  | [**MSPI\_PSRAM\_OCTAL\_CONFIG**](apollo5_2ns__psram_8c.md#define-mspi_psram_octal_config)  `MSPI\_PSRAM\_OctalCE0MSPIConfig`<br> |
| define  | [**MSPI\_TEST\_MODULE**](apollo5_2ns__psram_8c.md#define-mspi_test_module)  `3`<br> |
| define  | [**MSPI\_XIP\_BASE\_ADDRESS**](apollo5_2ns__psram_8c.md#define-mspi_xip_base_address)  `MSPI3\_APERTURE\_START\_ADDR`<br> |
| define  | [**am\_mspi\_isr**](apollo5_2ns__psram_8c.md#define-am_mspi_isr) (n) `am\_mspi ## n ## \_isr`<br> |
| define  | [**am\_mspi\_isr1**](apollo5_2ns__psram_8c.md#define-am_mspi_isr1) (n) `am\_mspi\_isr(n)`<br> |
| define  | [**apply\_psram\_timing**](apollo5_2ns__psram_8c.md#define-apply_psram_timing)  `am\_devices\_mspi\_psram\_aps25616n\_apply\_ddr\_timing`<br> |
| define  | [**enable\_psram\_xip**](apollo5_2ns__psram_8c.md#define-enable_psram_xip)  `am\_devices\_mspi\_psram\_aps25616n\_ddr\_enable\_xip`<br> |
| define  | [**init\_psram**](apollo5_2ns__psram_8c.md#define-init_psram)  `am\_devices\_mspi\_psram\_aps25616n\_ddr\_init`<br> |
| define  | [**init\_psram\_timing\_check**](apollo5_2ns__psram_8c.md#define-init_psram_timing_check)  `am\_devices\_mspi\_psram\_aps25616n\_ddr\_init\_timing\_check`<br> |
| define  | [**psram\_mspi\_isr**](apollo5_2ns__psram_8c.md#define-psram_mspi_isr)  `am\_mspi\_isr1(MSPI\_TEST\_MODULE)`<br> |

## Detailed Description




**Author:**

Ambiq 




**Version:**

0.1 




**Date:**

2024-04-19




**Copyright:**

Copyright (c) 2024 





    
## Public Attributes Documentation




### variable DMATCBBuffer 

```C++
AM_SHARED_RW uint32_t DMATCBBuffer[2560];
```




<hr>



### variable MSPIDdrTimingConfig 

```C++
am_devices_mspi_psram_ddr_timing_config_t MSPIDdrTimingConfig;
```




<hr>



### variable MSPI\_PSRAM\_HexCE0MSPIConfig 

```C++
am_devices_mspi_psram_config_t MSPI_PSRAM_HexCE0MSPIConfig;
```




<hr>



### variable MSPI\_PSRAM\_HexCE1MSPIConfig 

```C++
am_devices_mspi_psram_config_t MSPI_PSRAM_HexCE1MSPIConfig;
```




<hr>



### variable MSPI\_PSRAM\_OctalCE0MSPIConfig 

```C++
am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE0MSPIConfig;
```




<hr>



### variable MSPI\_PSRAM\_OctalCE1MSPIConfig 

```C++
am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE1MSPIConfig;
```




<hr>



### variable g\_pDevHandle 

```C++
void* g_pDevHandle;
```




<hr>



### variable g\_pHandle 

```C++
void* g_pHandle;
```




<hr>



### variable sMPUAttr 

```C++
am_hal_mpu_attr_t sMPUAttr;
```




<hr>



### variable sMPUCfg 

```C++
am_hal_mpu_region_config_t sMPUCfg;
```




<hr>
## Public Static Attributes Documentation




### variable mspi\_interrupts 

_MSPI interrupts._ 
```C++
const IRQn_Type mspi_interrupts[];
```




<hr>
## Public Functions Documentation




### function ns\_psram\_platform\_init 

```C++
uint32_t ns_psram_platform_init (
    ns_psram_config_t * cfg
) 
```




<hr>
## Macro Definition Documentation





### define MSPI\_PSRAM\_OCTAL\_CONFIG 

```C++
#define MSPI_PSRAM_OCTAL_CONFIG `MSPI_PSRAM_OctalCE0MSPIConfig`
```




<hr>



### define MSPI\_PSRAM\_OCTAL\_CONFIG 

```C++
#define MSPI_PSRAM_OCTAL_CONFIG `MSPI_PSRAM_OctalCE0MSPIConfig`
```




<hr>



### define MSPI\_TEST\_MODULE 

```C++
#define MSPI_TEST_MODULE `3`
```




<hr>



### define MSPI\_XIP\_BASE\_ADDRESS 

```C++
#define MSPI_XIP_BASE_ADDRESS `MSPI3_APERTURE_START_ADDR`
```




<hr>



### define am\_mspi\_isr 

```C++
#define am_mspi_isr (
    n
) `am_mspi ## n ## _isr`
```




<hr>



### define am\_mspi\_isr1 

```C++
#define am_mspi_isr1 (
    n
) `am_mspi_isr(n)`
```




<hr>



### define apply\_psram\_timing 

```C++
#define apply_psram_timing `am_devices_mspi_psram_aps25616n_apply_ddr_timing`
```




<hr>



### define enable\_psram\_xip 

```C++
#define enable_psram_xip `am_devices_mspi_psram_aps25616n_ddr_enable_xip`
```




<hr>



### define init\_psram 

```C++
#define init_psram `am_devices_mspi_psram_aps25616n_ddr_init`
```




<hr>



### define init\_psram\_timing\_check 

```C++
#define init_psram_timing_check `am_devices_mspi_psram_aps25616n_ddr_init_timing_check`
```




<hr>



### define psram\_mspi\_isr 

```C++
#define psram_mspi_isr `am_mspi_isr1(MSPI_TEST_MODULE)`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-peripherals/src/apollo5/ns_psram.c`

