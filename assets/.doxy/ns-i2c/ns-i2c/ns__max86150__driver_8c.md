

# File ns\_max86150\_driver.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-i2c**](dir_cc1f492d5d9f73ec0b0ac0581cc456e0.md) **>** [**src**](dir_d6c56226b0c7cd8d6baff594b5865597.md) **>** [**ns\_max86150\_driver.c**](ns__max86150__driver_8c.md)

[Go to the source code of this file](ns__max86150__driver_8c_source.md)

_Register driver for MAX86150 ECG/PPG IC._ [More...](#detailed-description)

* `#include "ns_max86150_driver.h"`
* `#include <ctype.h>`
* `#include <stdint.h>`
* `#include <string.h>`























## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  const uint8\_t | [**MAX86150\_ECG\_CONFIG1**](#variable-max86150_ecg_config1)   = = 0x3C<br> |
|  const uint8\_t | [**MAX86150\_ECG\_CONFIG3**](#variable-max86150_ecg_config3)   = = 0x3E<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_CONFIG**](#variable-max86150_fifo_config)   = = 0x08<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_CONTROL1**](#variable-max86150_fifo_control1)   = = 0x09<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_CONTROL2**](#variable-max86150_fifo_control2)   = = 0x0A<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_DATA**](#variable-max86150_fifo_data)   = = 0x07<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_OVERFLOW**](#variable-max86150_fifo_overflow)   = = 0x05<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_RD\_PTR**](#variable-max86150_fifo_rd_ptr)   = = 0x06<br> |
|  const uint8\_t | [**MAX86150\_FIFO\_WR\_PTR**](#variable-max86150_fifo_wr_ptr)   = = 0x04<br> |
|  const uint8\_t | [**MAX86150\_INT\_EN1**](#variable-max86150_int_en1)   = = 0x02<br> |
|  const uint8\_t | [**MAX86150\_INT\_EN2**](#variable-max86150_int_en2)   = = 0x03<br> |
|  const uint8\_t | [**MAX86150\_INT\_STAT1**](#variable-max86150_int_stat1)   = = 0x00<br> |
|  const uint8\_t | [**MAX86150\_INT\_STAT2**](#variable-max86150_int_stat2)   = = 0x01<br> |
|  const uint8\_t | [**MAX86150\_LED1\_PA**](#variable-max86150_led1_pa)   = = 0x11<br> |
|  const uint8\_t | [**MAX86150\_LED2\_PA**](#variable-max86150_led2_pa)   = = 0x12<br> |
|  const uint8\_t | [**MAX86150\_LEDP\_PA**](#variable-max86150_ledp_pa)   = = 0x15<br> |
|  const uint8\_t | [**MAX86150\_LED\_RANGE**](#variable-max86150_led_range)   = = 0x14<br> |
|  const uint8\_t | [**MAX86150\_PART\_ID**](#variable-max86150_part_id)   = = 0xFF<br> |
|  const uint8\_t | [**MAX86150\_PPG\_CONFIG1**](#variable-max86150_ppg_config1)   = = 0x0E<br> |
|  const uint8\_t | [**MAX86150\_PPG\_CONFIG2**](#variable-max86150_ppg_config2)   = = 0x0F<br> |
|  const uint8\_t | [**MAX86150\_PPG\_PROX\_INT\_THRESH**](#variable-max86150_ppg_prox_int_thresh)   = = 0x10<br> |
|  const uint8\_t | [**MAX86150\_SYS\_CONTROL**](#variable-max86150_sys_control)   = = 0x0D<br> |














## Public Functions

| Type | Name |
| ---: | :--- |
|  void | [**max86150\_clear\_fifo**](#function-max86150_clear_fifo) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Clear FIFO._  |
|  void | [**max86150\_disable\_slots**](#function-max86150_disable_slots) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Disables all FIFO slots data sources._  |
|  uint8\_t | [**max86150\_get\_fifo\_overflow\_counter**](#function-max86150_get_fifo_overflow_counter) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Get FIFO overflow counter._  |
|  uint8\_t | [**max86150\_get\_fifo\_rd\_pointer**](#function-max86150_get_fifo_rd_pointer) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Get FIFO write pointer._  |
|  uint8\_t | [**max86150\_get\_fifo\_wr\_pointer**](#function-max86150_get_fifo_wr_pointer) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Get FIFO write pointer._  |
|  uint8\_t | [**max86150\_get\_int1**](#function-max86150_get_int1) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Get interrupt 1 register A\_FULL[7] PPG\_RDY[6] ALC\_OVF[5] PROX\_INT[4] PWR\_RDY[0]._  |
|  uint8\_t | [**max86150\_get\_int2**](#function-max86150_get_int2) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Get interrupt 2 register VDD\_OOR[7] ECG\_RDY[2]._  |
|  uint8\_t | [**max86150\_get\_part\_id**](#function-max86150_get_part_id) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Get part ID. Should be 0x1E for this part._  |
|  uint8\_t | [**max86150\_get\_register**](#function-max86150_get_register) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t reg, uint8\_t mask) <br>_Read register field._  |
|  void | [**max86150\_powerup**](#function-max86150_powerup) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Takes chip out ofpower-save mode._  |
|  uint32\_t | [**max86150\_read\_fifo\_samples**](#function-max86150_read_fifo_samples) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint32\_t \* buffer, max86150\_slot\_type \* slots, uint8\_t numSlots) <br>_Reads all data available in FIFO._  |
|  void | [**max86150\_reset**](#function-max86150_reset) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_This performs full power-on-reset. All registers are reset._  |
|  void | [**max86150\_set\_alc\_ovf\_int\_flag**](#function-max86150_set_alc_ovf_int_flag) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set ambient light cancellation (ALC) overflow interrupt enable flag._  |
|  void | [**max86150\_set\_alm\_full\_int\_flag**](#function-max86150_set_alm_full_int_flag) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set FIFO almost full interrupt enable flag._  |
|  void | [**max86150\_set\_almost\_full\_flag\_options**](#function-max86150_set_almost_full_flag_options) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t options) <br>_Set FIFO almost full flag options._  |
|  void | [**max86150\_set\_almost\_full\_int\_options**](#function-max86150_set_almost_full_int_options) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t options) <br>_Set FIFO almost full interrupt options._  |
|  void | [**max86150\_set\_almost\_full\_rollover**](#function-max86150_set_almost_full_rollover) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set whether FIFO rolls over when full._  |
|  void | [**max86150\_set\_almost\_full\_threshold**](#function-max86150_set_almost_full_threshold) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t space) <br>_Set FIFO almost full value (i.e. how many samples till interrupt is triggered)_  |
|  void | [**max86150\_set\_ecg\_ia\_gain**](#function-max86150_set_ecg_ia_gain) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set ECG instrument amplifier gain._  |
|  void | [**max86150\_set\_ecg\_pga\_gain**](#function-max86150_set_ecg_pga_gain) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set ECG PGA gain. Gain = 2\*\*value (V/V)_  |
|  void | [**max86150\_set\_ecg\_rdy\_int\_flag**](#function-max86150_set_ecg_rdy_int_flag) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set new ECG FIFO data ready interrupt enable flag._  |
|  void | [**max86150\_set\_ecg\_sample\_rate**](#function-max86150_set_ecg_sample_rate) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br> |
|  void | [**max86150\_set\_fifo\_enable**](#function-max86150_set_fifo_enable) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set FIFO enable state._  |
|  uint8\_t | [**max86150\_set\_fifo\_overflow\_counter**](#function-max86150_set_fifo_overflow_counter) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set FIFO overflow counter._  |
|  void | [**max86150\_set\_fifo\_rd\_pointer**](#function-max86150_set_fifo_rd_pointer) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set FIFO read pointer._  |
|  void | [**max86150\_set\_fifo\_slot**](#function-max86150_set_fifo_slot) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t slot, max86150\_slot\_type type) <br>_Configure FIFO for given slot (element)_  |
|  void | [**max86150\_set\_fifo\_slots**](#function-max86150_set_fifo_slots) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, max86150\_slot\_type \* slots) <br>_Configure all FIFO slots._  |
|  void | [**max86150\_set\_fifo\_wr\_pointer**](#function-max86150_set_fifo_wr_pointer) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set FIFO write pointer._  |
|  void | [**max86150\_set\_led\_current\_range**](#function-max86150_set_led_current_range) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t led, uint8\_t value) <br>_Set LED current range._  |
|  void | [**max86150\_set\_led\_pulse\_amplitude**](#function-max86150_set_led_pulse_amplitude) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t led, uint8\_t value) <br> |
|  void | [**max86150\_set\_ppg\_adc\_range**](#function-max86150_set_ppg_adc_range) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t range) <br>_Set PPG ADC range._  |
|  void | [**max86150\_set\_ppg\_pulse\_width**](#function-max86150_set_ppg_pulse_width) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set PPG pulse width._  |
|  void | [**max86150\_set\_ppg\_rdy\_int\_flag**](#function-max86150_set_ppg_rdy_int_flag) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set new PPG FIFO data ready interrupt enable flag._  |
|  void | [**max86150\_set\_ppg\_sample\_average**](#function-max86150_set_ppg_sample_average) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set PPG sample averaging. Will average and decimate adjacent samples._  |
|  void | [**max86150\_set\_ppg\_sample\_rate**](#function-max86150_set_ppg_sample_rate) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Set PPG sample rate and pulse rate. NOTE: If rate cant be acheived, then highest available will be selected._  |
|  void | [**max86150\_set\_prox\_int\_flag**](#function-max86150_set_prox_int_flag) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set proximity interrupt enable flag._  |
|  void | [**max86150\_set\_proximity\_threshold**](#function-max86150_set_proximity_threshold) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t value) <br>_Sets the IR ADC count that triggers the beginning of the PPG mode._  |
|  int | [**max86150\_set\_register**](#function-max86150_set_register) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t reg, uint8\_t value, uint8\_t mask) <br>_Set register field._  |
|  void | [**max86150\_set\_vdd\_oor\_int\_flag**](#function-max86150_set_vdd_oor_int_flag) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx, uint8\_t enable) <br>_Set VDD Out-of-Range indicator interrupt flag._  |
|  void | [**max86150\_shutdown**](#function-max86150_shutdown) (const [**max86150\_context\_t**](structmax86150__context__t.md) \* ctx) <br>_Put chip into power-save mode. Registers retain their values._  |




























# Detailed Description




**Author:**

Adam Page ([adam.page@ambiq.com](mailto:adam.page@ambiq.com)) 




**Version:**

0.1 




**Date:**

2022-11-02




**Copyright:**

Copyright (c) 2022 





    
## Public Static Attributes Documentation




### variable MAX86150\_ECG\_CONFIG1 

```C++
const uint8_t MAX86150_ECG_CONFIG1;
```






### variable MAX86150\_ECG\_CONFIG3 

```C++
const uint8_t MAX86150_ECG_CONFIG3;
```






### variable MAX86150\_FIFO\_CONFIG 

```C++
const uint8_t MAX86150_FIFO_CONFIG;
```






### variable MAX86150\_FIFO\_CONTROL1 

```C++
const uint8_t MAX86150_FIFO_CONTROL1;
```






### variable MAX86150\_FIFO\_CONTROL2 

```C++
const uint8_t MAX86150_FIFO_CONTROL2;
```






### variable MAX86150\_FIFO\_DATA 

```C++
const uint8_t MAX86150_FIFO_DATA;
```






### variable MAX86150\_FIFO\_OVERFLOW 

```C++
const uint8_t MAX86150_FIFO_OVERFLOW;
```






### variable MAX86150\_FIFO\_RD\_PTR 

```C++
const uint8_t MAX86150_FIFO_RD_PTR;
```






### variable MAX86150\_FIFO\_WR\_PTR 

```C++
const uint8_t MAX86150_FIFO_WR_PTR;
```






### variable MAX86150\_INT\_EN1 

```C++
const uint8_t MAX86150_INT_EN1;
```






### variable MAX86150\_INT\_EN2 

```C++
const uint8_t MAX86150_INT_EN2;
```






### variable MAX86150\_INT\_STAT1 

```C++
const uint8_t MAX86150_INT_STAT1;
```






### variable MAX86150\_INT\_STAT2 

```C++
const uint8_t MAX86150_INT_STAT2;
```






### variable MAX86150\_LED1\_PA 

```C++
const uint8_t MAX86150_LED1_PA;
```






### variable MAX86150\_LED2\_PA 

```C++
const uint8_t MAX86150_LED2_PA;
```






### variable MAX86150\_LEDP\_PA 

```C++
const uint8_t MAX86150_LEDP_PA;
```






### variable MAX86150\_LED\_RANGE 

```C++
const uint8_t MAX86150_LED_RANGE;
```






### variable MAX86150\_PART\_ID 

```C++
const uint8_t MAX86150_PART_ID;
```






### variable MAX86150\_PPG\_CONFIG1 

```C++
const uint8_t MAX86150_PPG_CONFIG1;
```






### variable MAX86150\_PPG\_CONFIG2 

```C++
const uint8_t MAX86150_PPG_CONFIG2;
```






### variable MAX86150\_PPG\_PROX\_INT\_THRESH 

```C++
const uint8_t MAX86150_PPG_PROX_INT_THRESH;
```






### variable MAX86150\_SYS\_CONTROL 

```C++
const uint8_t MAX86150_SYS_CONTROL;
```



## Public Functions Documentation




### function max86150\_clear\_fifo 

_Clear FIFO._ 
```C++
void max86150_clear_fifo (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_disable\_slots 

_Disables all FIFO slots data sources._ 
```C++
void max86150_disable_slots (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_get\_fifo\_overflow\_counter 

_Get FIFO overflow counter._ 
```C++
uint8_t max86150_get_fifo_overflow_counter (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 



**Returns:**

FIFO overflow counter 





        



### function max86150\_get\_fifo\_rd\_pointer 

_Get FIFO write pointer._ 
```C++
uint8_t max86150_get_fifo_rd_pointer (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 



**Returns:**

FIFO write pointer 





        



### function max86150\_get\_fifo\_wr\_pointer 

_Get FIFO write pointer._ 
```C++
uint8_t max86150_get_fifo_wr_pointer (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 



**Returns:**

write pointer 





        



### function max86150\_get\_int1 

_Get interrupt 1 register A\_FULL[7] PPG\_RDY[6] ALC\_OVF[5] PROX\_INT[4] PWR\_RDY[0]._ 
```C++
uint8_t max86150_get_int1 (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 



**Returns:**

register value 





        



### function max86150\_get\_int2 

_Get interrupt 2 register VDD\_OOR[7] ECG\_RDY[2]._ 
```C++
uint8_t max86150_get_int2 (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 



**Returns:**

register value 





        



### function max86150\_get\_part\_id 

_Get part ID. Should be 0x1E for this part._ 
```C++
uint8_t max86150_get_part_id (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context



**Returns:**

return 





        



### function max86150\_get\_register 

_Read register field._ 
```C++
uint8_t max86150_get_register (
    const max86150_context_t * ctx,
    uint8_t reg,
    uint8_t mask
) 
```





**Returns:**

Register value 





        



### function max86150\_powerup 

_Takes chip out ofpower-save mode._ 
```C++
void max86150_powerup (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_read\_fifo\_samples 

_Reads all data available in FIFO._ 
```C++
uint32_t max86150_read_fifo_samples (
    const max86150_context_t * ctx,
    uint32_t * buffer,
    max86150_slot_type * slots,
    uint8_t numSlots
) 
```





**Parameters:**


* `ctx` Device context 
* `buffer` Buffer to store FIFO data. Should be at least 32\*3\*elementsPerSample (max 384 bytes) 
* `elementsPerSample` Number of elements per sample. Depends on values written to FD1-FD4 



**Returns:**

Number of samples read 





        



### function max86150\_reset 

_This performs full power-on-reset. All registers are reset._ 
```C++
void max86150_reset (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_alc\_ovf\_int\_flag 

_Set ambient light cancellation (ALC) overflow interrupt enable flag._ 
```C++
void max86150_set_alc_ovf_int_flag (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_alm\_full\_int\_flag 

_Set FIFO almost full interrupt enable flag._ 
```C++
void max86150_set_alm_full_int_flag (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_almost\_full\_flag\_options 

_Set FIFO almost full flag options._ 
```C++
void max86150_set_almost_full_flag_options (
    const max86150_context_t * ctx,
    uint8_t options
) 
```





**Parameters:**


* `ctx` Device context 
* `options` 1-bit: 0: Assert on a\_full condition, clear by status reg read, and re-assert on subsequent samples 1: Assert on a\_full condition, clear by status reg read, and not re-assert on subsequent samples 




        



### function max86150\_set\_almost\_full\_int\_options 

_Set FIFO almost full interrupt options._ 
```C++
void max86150_set_almost_full_int_options (
    const max86150_context_t * ctx,
    uint8_t options
) 
```





**Parameters:**


* `ctx` Device context 
* `options` 1-bit: 0: A\_FULL interrupt does not get cleared by FIFO\_DATA register read. It gets cleared by status register read. 1: A\_FULL interrupt gets cleared by FIFO\_DATA register read or status register read. 




        



### function max86150\_set\_almost\_full\_rollover 

_Set whether FIFO rolls over when full._ 
```C++
void max86150_set_almost_full_rollover (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 
* `enable` 1-bit: 0: No rollover - FIFO stop on full 1: Rollover - FIFO auto rolls over on full 




        



### function max86150\_set\_almost\_full\_threshold 

_Set FIFO almost full value (i.e. how many samples till interrupt is triggered)_ 
```C++
void max86150_set_almost_full_threshold (
    const max86150_context_t * ctx,
    uint8_t space
) 
```





**Parameters:**


* `ctx` Device context 
* `space` Remaining FIFO space before intr trigger 




        



### function max86150\_set\_ecg\_ia\_gain 

_Set ECG instrument amplifier gain._ 
```C++
void max86150_set_ecg_ia_gain (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` Gain table: 0: 5, 1: 9.5, 2: 20, 3: 50 (V/V) 




        



### function max86150\_set\_ecg\_pga\_gain 

_Set ECG PGA gain. Gain = 2\*\*value (V/V)_ 
```C++
void max86150_set_ecg_pga_gain (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` 2-bit 




        



### function max86150\_set\_ecg\_rdy\_int\_flag 

_Set new ECG FIFO data ready interrupt enable flag._ 
```C++
void max86150_set_ecg_rdy_int_flag (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_ecg\_sample\_rate 


```C++
void max86150_set_ecg_sample_rate (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` 3-bit Sample Rate Table (Hz) VALUE FS FBW\_70 FBW\_90 0 1600 420 232 1 800 210 116 2 400 105 58 3 200 52 29 4 3200 840 464 5 1600 420 232 6 800 210 116 7 400 105 58 




        



### function max86150\_set\_fifo\_enable 

_Set FIFO enable state._ 
```C++
void max86150_set_fifo_enable (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 
* `enable` Enable/disable FIFO. Clears on enable 




        



### function max86150\_set\_fifo\_overflow\_counter 

_Set FIFO overflow counter._ 
```C++
uint8_t max86150_set_fifo_overflow_counter (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 



**Returns:**

FIFO overflow counter 





        



### function max86150\_set\_fifo\_rd\_pointer 

_Set FIFO read pointer._ 
```C++
void max86150_set_fifo_rd_pointer (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_fifo\_slot 

_Configure FIFO for given slot (element)_ 
```C++
void max86150_set_fifo_slot (
    const max86150_context_t * ctx,
    uint8_t slot,
    max86150_slot_type type
) 
```





**Parameters:**


* `ctx` Device context 
* `slot` Element 0-based index corresponding to FD1, FD2, FD3, FD4 
* `type` Data source to feed 




        



### function max86150\_set\_fifo\_slots 

_Configure all FIFO slots._ 
```C++
void max86150_set_fifo_slots (
    const max86150_context_t * ctx,
    max86150_slot_type * slots
) 
```





**Parameters:**


* `ctx` Device context 
* `slot0` Element (FD1) data source 
* `slot1` Element (FD2) data source 
* `slot2` Element (FD3) data source 
* `slot3` Element (FD4) data source 




        



### function max86150\_set\_fifo\_wr\_pointer 

_Set FIFO write pointer._ 
```C++
void max86150_set_fifo_wr_pointer (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` Write pointer 




        



### function max86150\_set\_led\_current\_range 

_Set LED current range._ 
```C++
void max86150_set_led_current_range (
    const max86150_context_t * ctx,
    uint8_t led,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `led` 0: LED1 (IR), 1: LED2 (RED) 
* `value` 2-bit \| 0: 50, 1: 100 (mA) 




        



### function max86150\_set\_led\_pulse\_amplitude 


```C++
void max86150_set_led_pulse_amplitude (
    const max86150_context_t * ctx,
    uint8_t led,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `led` 0: LED1 (IR), 1: LED2 (RED), 2: LEDP (proximity) 
* `value` 7-bit \| Pulse amplitude = 0.2\*value\*(LEDn\_RANGE+1) (mA) 




        



### function max86150\_set\_ppg\_adc\_range 

_Set PPG ADC range._ 
```C++
void max86150_set_ppg_adc_range (
    const max86150_context_t * ctx,
    uint8_t range
) 
```





**Parameters:**


* `ctx` Device context 
* `value` 2-bit \| Full scale = 2\*\*(14+value) nA, LSB = 7.8125 \* (2 \*\* value) 




        



### function max86150\_set\_ppg\_pulse\_width 

_Set PPG pulse width._ 
```C++
void max86150_set_ppg_pulse_width (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` 2-bit \| 0: 50 1: 100 2: 200 3: 400 (us) 




        



### function max86150\_set\_ppg\_rdy\_int\_flag 

_Set new PPG FIFO data ready interrupt enable flag._ 
```C++
void max86150_set_ppg_rdy_int_flag (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_ppg\_sample\_average 

_Set PPG sample averaging. Will average and decimate adjacent samples._ 
```C++
void max86150_set_ppg_sample_average (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` avg = min(2\*\*value, 32) 




        



### function max86150\_set\_ppg\_sample\_rate 

_Set PPG sample rate and pulse rate. NOTE: If rate cant be acheived, then highest available will be selected._ 
```C++
void max86150_set_ppg_sample_rate (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 
* `value` 4-bit \| Samples/sec: [10, 20, 50, 84, 100, 200, 400, 800, 1000, 1600, 3200, 10, 20, 50, 84, 100] Pulses/sec: [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2] 




        



### function max86150\_set\_prox\_int\_flag 

_Set proximity interrupt enable flag._ 
```C++
void max86150_set_prox_int_flag (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_proximity\_threshold 

_Sets the IR ADC count that triggers the beginning of the PPG mode._ 
```C++
void max86150_set_proximity_threshold (
    const max86150_context_t * ctx,
    uint8_t value
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_set\_register 

_Set register field._ 
```C++
int max86150_set_register (
    const max86150_context_t * ctx,
    uint8_t reg,
    uint8_t value,
    uint8_t mask
) 
```





**Returns:**

0 if successful 





        



### function max86150\_set\_vdd\_oor\_int\_flag 

_Set VDD Out-of-Range indicator interrupt flag._ 
```C++
void max86150_set_vdd_oor_int_flag (
    const max86150_context_t * ctx,
    uint8_t enable
) 
```





**Parameters:**


* `ctx` Device context 




        



### function max86150\_shutdown 

_Put chip into power-save mode. Registers retain their values._ 
```C++
void max86150_shutdown (
    const max86150_context_t * ctx
) 
```





**Parameters:**


* `ctx` Device context 




        

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-i2c/src/ns_max86150_driver.c`

