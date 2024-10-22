

# File ns\_audadc.c



[**FileList**](files.md) **>** [**apollo4**](dir_9e4df1ce7893b775eb3c7fcb555505ab.md) **>** [**ns\_audadc.c**](ns__audadc_8c.md)

[Go to the source code of this file](ns__audadc_8c_source.md)



* `#include "../ns_audadc.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_audio.h"`
* `#include "ns_core.h"`





















## Public Attributes

| Type | Name |
| ---: | :--- |
|  AUDADC\_Type \* | [**g\_adc**](#variable-g_adc)  <br> |
|  MCUCTRL\_Type \* | [**g\_mcuctrl**](#variable-g_mcuctrl)  <br> |
|  am\_hal\_audadc\_dma\_config\_t | [**g\_sAUDADCDMAConfig**](#variable-g_saudadcdmaconfig)  <br> |
|  am\_hal\_audadc\_gain\_config\_t | [**g\_sAudadcGainConfig**](#variable-g_saudadcgainconfig)   = = {
    .ui32LGA = 0,      
    .ui32HGADELTA = 0, 
    .ui32LGB = 0,      
    .ui32HGBDELTA = 0, 
    .eUpdateMode = AM\_HAL\_AUDADC\_GAIN\_UPDATE\_IMME,
}<br> |
|  [**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) | [**ns\_audadc\_default**](#variable-ns_audadc_default)   = = {
    
    
    .clock = NS\_CLKSEL\_HFRC2\_ADJ,
    .low\_power\_mode = true,
    .repeating\_trigger\_mode = true,
    .dcmp\_enable = false,
}<br> |
|  [**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) | [**ns\_audadc\_vos\_default**](#variable-ns_audadc_vos_default)   = = {
    .clock = NS\_CLKSEL\_HFRC2\_ADJ,
    .low\_power\_mode = true,
    .repeating\_trigger\_mode = false,
    .dcmp\_enable = false,
}<br> |


## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**axiScratchBuf**](#variable-axiscratchbuf)  <br> |
|  void \* | [**g\_AUDADCHandle**](#variable-g_audadchandle)  <br> |
|  volatile bool | [**g\_bAUDADCDMAError**](#variable-g_baudadcdmaerror)  <br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**am\_audadc0\_isr**](#function-am_audadc0_isr) (void) <br> |
|  uint32\_t | [**audadc\_config**](#function-audadc_config) ([**ns\_audadc\_cfg\_t**](structns__audadc__cfg__t.md) \* cfg) <br>_Configure and enable the AUDADC._  |
|  void | [**audadc\_deinit**](#function-audadc_deinit) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |
|  uint32\_t | [**audadc\_init**](#function-audadc_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |
|  uint32\_t | [**audadc\_slot\_config**](#function-audadc_slot_config) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  void | [**audadc\_config\_dma**](#function-audadc_config_dma) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |
|  void | [**audadc\_pga\_init**](#function-audadc_pga_init) ([**ns\_audio\_config\_t**](ns__audio_8h.md#typedef-ns_audio_config_t) \* cfg) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**CH\_A0\_GAIN\_DB**](ns__audadc_8c.md#define-ch_a0_gain_db)  18<br> |
| define  | [**CH\_A1\_GAIN\_DB**](ns__audadc_8c.md#define-ch_a1_gain_db)  18<br> |
| define  | [**CH\_B0\_GAIN\_DB**](ns__audadc_8c.md#define-ch_b0_gain_db)  12<br> |
| define  | [**CH\_B1\_GAIN\_DB**](ns__audadc_8c.md#define-ch_b1_gain_db)  12<br> |
| define  | [**CLK\_SRC**](ns__audadc_8c.md#define-clk_src)  HFRC<br> |
| define  | [**HFRC**](ns__audadc_8c.md#define-hfrc)  1<br> |
| define  | [**HFRC2**](ns__audadc_8c.md#define-hfrc2)  2<br> |
| define  | [**HFRC2\_ADJ**](ns__audadc_8c.md#define-hfrc2_adj)  3<br> |
| define  | [**PREAMP\_FULL\_GAIN**](ns__audadc_8c.md#define-preamp_full_gain)  24<br> |
| define  | [**XTHS**](ns__audadc_8c.md#define-xths)  0<br> |

## Public Attributes Documentation




### variable g\_adc 

```C++
AUDADC_Type* g_adc;
```






### variable g\_mcuctrl 

```C++
MCUCTRL_Type* g_mcuctrl;
```






### variable g\_sAUDADCDMAConfig 

```C++
am_hal_audadc_dma_config_t g_sAUDADCDMAConfig;
```






### variable g\_sAudadcGainConfig 

```C++
am_hal_audadc_gain_config_t g_sAudadcGainConfig;
```






### variable ns\_audadc\_default 

```C++
ns_audadc_cfg_t ns_audadc_default;
```






### variable ns\_audadc\_vos\_default 

```C++
ns_audadc_cfg_t ns_audadc_vos_default;
```



## Public Static Attributes Documentation




### variable axiScratchBuf 

```C++
uint32_t axiScratchBuf[20];
```






### variable g\_AUDADCHandle 

```C++
void* g_AUDADCHandle;
```






### variable g\_bAUDADCDMAError 

```C++
volatile bool g_bAUDADCDMAError;
```



## Public Functions Documentation




### function am\_audadc0\_isr 

```C++
void am_audadc0_isr (
    void
) 
```






### function audadc\_config 

_Configure and enable the AUDADC._ 
```C++
uint32_t audadc_config (
    ns_audadc_cfg_t * cfg
) 
```





**Parameters:**


* `cfg` - AUDADC-specific configuration 



**Returns:**

uint32\_t status 





        



### function audadc\_deinit 

```C++
void audadc_deinit (
    ns_audio_config_t * cfg
) 
```






### function audadc\_init 

```C++
uint32_t audadc_init (
    ns_audio_config_t * cfg
) 
```






### function audadc\_slot\_config 

```C++
uint32_t audadc_slot_config (
    ns_audio_config_t * cfg
) 
```



## Public Static Functions Documentation




### function audadc\_config\_dma 

```C++
static void audadc_config_dma (
    ns_audio_config_t * cfg
) 
```






### function audadc\_pga\_init 

```C++
static void audadc_pga_init (
    ns_audio_config_t * cfg
) 
```



## Macro Definition Documentation





### define CH\_A0\_GAIN\_DB 

```C++
#define CH_A0_GAIN_DB 18
```






### define CH\_A1\_GAIN\_DB 

```C++
#define CH_A1_GAIN_DB 18
```






### define CH\_B0\_GAIN\_DB 

```C++
#define CH_B0_GAIN_DB 12
```






### define CH\_B1\_GAIN\_DB 

```C++
#define CH_B1_GAIN_DB 12
```






### define CLK\_SRC 

```C++
#define CLK_SRC HFRC
```






### define HFRC 

```C++
#define HFRC 1
```






### define HFRC2 

```C++
#define HFRC2 2
```






### define HFRC2\_ADJ 

```C++
#define HFRC2_ADJ 3
```






### define PREAMP\_FULL\_GAIN 

```C++
#define PREAMP_FULL_GAIN 24
```






### define XTHS 

```C++
#define XTHS 0
```




------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-audio/src/apollo4/ns_audadc.c`

