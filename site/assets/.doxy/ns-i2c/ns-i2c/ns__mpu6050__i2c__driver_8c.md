

# File ns\_mpu6050\_i2c\_driver.c



[**FileList**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-i2c**](dir_cc1f492d5d9f73ec0b0ac0581cc456e0.md) **>** [**src**](dir_d6c56226b0c7cd8d6baff594b5865597.md) **>** [**ns\_mpu6050\_i2c\_driver.c**](ns__mpu6050__i2c__driver_8c.md)

[Go to the source code of this file](ns__mpu6050__i2c__driver_8c_source.md)



* `#include "ns_mpu6050_i2c_driver.h"`
* `#include "am_bsp.h"`
* `#include "am_mcu_apollo.h"`
* `#include "am_util.h"`
* `#include "ns_i2c_register_driver.h"`
* `#include <limits.h>`
* `#include <stdlib.h>`





































## Public Functions

| Type | Name |
| ---: | :--- |
|  uint32\_t | [**mpu6050\_accel\_fsr\_value**](#function-mpu6050_accel_fsr_value) ([**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) range) <br>_Acceleration scale in units of G._  |
|  float | [**mpu6050\_accel\_resolution**](#function-mpu6050_accel_resolution) ([**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) range) <br>_Acceleration scale LSB resolution._  |
|  float | [**mpu6050\_accel\_to\_gravity**](#function-mpu6050_accel_to_gravity) (int16\_t val, [**mpu6050\_accel\_fs\_t**](ns__mpu6050__i2c__driver_8h.md#enum-mpu6050_accel_fs_t) range) <br>_Convert acceleration value to G._  |
|  uint32\_t | [**mpu6050\_calibrate**](#function-mpu6050_calibrate) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Calibrate device offsets. Device must be still on a flat surface._  |
|  uint32\_t | [**mpu6050\_calibration**](#function-mpu6050_calibration) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Calibrate device offsets. Device must be still on a flat surface._  |
|  uint32\_t | [**mpu6050\_configure\_fifo**](#function-mpu6050_configure_fifo) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_fifo\_config\_t**](structmpu6050__fifo__config__t.md) \* fifoConfig) <br>_Configure FIFO._  |
|  uint32\_t | [**mpu6050\_configure\_interrupt**](#function-mpu6050_configure_interrupt) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, [**mpu6050\_int\_config\_t**](structmpu6050__int__config__t.md) \* intConfig) <br>_Configure interrupts._  |
|  uint32\_t | [**mpu6050\_device\_reset**](#function-mpu6050_device_reset) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr) <br>_Hard reset device._  |
|  uint32\_t | [**mpu6050\_fifo\_pop**](#function-mpu6050_fifo_pop) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int16\_t \* value) <br>_Pop next value from FIFO._  |
|  uint32\_t | [**mpu6050\_get\_accel\_offset**](#function-mpu6050_get_accel_offset) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t axis, uint16\_t \* offset) <br> |
|  uint32\_t | [**mpu6050\_get\_accel\_values**](#function-mpu6050_get_accel_values) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, int16\_t \* x, int16\_t \* y, int16\_t \* z) <br>_Get current acceleration values._  |
|  uint32\_t | [**mpu6050\_get\_fifo\_count**](#function-mpu6050_get_fifo_count) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint16\_t \* count) <br>_Get FIFO count._  |
|  uint32\_t | [**mpu6050\_get\_gyro\_offset**](#function-mpu6050_get_gyro_offset) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t axis, uint16\_t \* offset) <br> |
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


## Public Static Functions

| Type | Name |
| ---: | :--- |
|  int8\_t | [**get\_high\_bits**](#function-get_high_bits) (int16\_t regReading) <br> |
|  int8\_t | [**get\_low\_bits**](#function-get_low_bits) (int16\_t regReading) <br> |
|  uint32\_t | [**read\_word\_register**](#function-read_word_register) ([**ns\_i2c\_config\_t**](structns__i2c__config__t.md) \* cfg, uint32\_t devAddr, uint8\_t reg, uint16\_t \* value) <br> |

























## Macros

| Type | Name |
| ---: | :--- |
| define  | [**ACCEL\_CONFIG**](ns__mpu6050__i2c__driver_8c.md#define-accel_config)  `0x1C`<br> |
| define  | [**ACCEL\_XOUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-accel_xout_h)  `0x3B`<br> |
| define  | [**ACCEL\_XOUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-accel_xout_l)  `0x3C`<br> |
| define  | [**ACCEL\_YOUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-accel_yout_h)  `0x3D`<br> |
| define  | [**ACCEL\_YOUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-accel_yout_l)  `0x3E`<br> |
| define  | [**ACCEL\_ZOUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-accel_zout_h)  `0x3F`<br> |
| define  | [**ACCEL\_ZOUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-accel_zout_l)  `0x40`<br> |
| define  | [**ACONFIG\_FS\_SEL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_fs_sel_bit)  `4`<br> |
| define  | [**ACONFIG\_FS\_SEL\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_fs_sel_length)  `2`<br> |
| define  | [**ACONFIG\_HPF\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_hpf_bit)  `2`<br> |
| define  | [**ACONFIG\_HPF\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_hpf_length)  `3`<br> |
| define  | [**ACONFIG\_XA\_ST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_xa_st_bit)  `7`<br> |
| define  | [**ACONFIG\_YA\_ST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_ya_st_bit)  `6`<br> |
| define  | [**ACONFIG\_ZA\_ST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-aconfig_za_st_bit)  `5`<br> |
| define  | [**BANKSEL\_CFG\_USER\_BANK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-banksel_cfg_user_bank_bit)  `5`<br> |
| define  | [**BANKSEL\_MEM\_SEL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-banksel_mem_sel_bit)  `4`<br> |
| define  | [**BANKSEL\_MEM\_SEL\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-banksel_mem_sel_length)  `5`<br> |
| define  | [**BANKSEL\_PRFTCH\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-banksel_prftch_en_bit)  `6`<br> |
| define  | [**BANK\_SEL**](ns__mpu6050__i2c__driver_8c.md#define-bank_sel)  `0x6D`<br> |
| define  | [**CONFIG**](ns__mpu6050__i2c__driver_8c.md#define-config)  `0x1A`<br> |
| define  | [**CONFIG\_DLPF\_CFG\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-config_dlpf_cfg_bit)  `2`<br> |
| define  | [**CONFIG\_DLPF\_CFG\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-config_dlpf_cfg_length)  `3`<br> |
| define  | [**CONFIG\_EXT\_SYNC\_SET\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-config_ext_sync_set_bit)  `5`<br> |
| define  | [**CONFIG\_EXT\_SYNC\_SET\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-config_ext_sync_set_length)  `3`<br> |
| define  | [**CONFIG\_FIFO\_MODE\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-config_fifo_mode_bit)  `6`<br> |
| define  | [**DMP\_INT\_STATUS**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status)  `0x39`<br> |
| define  | [**DMP\_INT\_STATUS\_0**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status_0)  `0`<br> |
| define  | [**DMP\_INT\_STATUS\_1**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status_1)  `1`<br> |
| define  | [**DMP\_INT\_STATUS\_2**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status_2)  `2`<br> |
| define  | [**DMP\_INT\_STATUS\_3**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status_3)  `3`<br> |
| define  | [**DMP\_INT\_STATUS\_4**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status_4)  `4`<br> |
| define  | [**DMP\_INT\_STATUS\_5**](ns__mpu6050__i2c__driver_8c.md#define-dmp_int_status_5)  `5`<br> |
| define  | [**EXT\_SENS\_DATA\_00**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_00)  `0x49`<br> |
| define  | [**EXT\_SENS\_DATA\_01**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_01)  `0x4A`<br> |
| define  | [**EXT\_SENS\_DATA\_02**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_02)  `0x4B`<br> |
| define  | [**EXT\_SENS\_DATA\_03**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_03)  `0x4C`<br> |
| define  | [**EXT\_SENS\_DATA\_04**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_04)  `0x4D`<br> |
| define  | [**EXT\_SENS\_DATA\_05**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_05)  `0x4E`<br> |
| define  | [**EXT\_SENS\_DATA\_06**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_06)  `0x4F`<br> |
| define  | [**EXT\_SENS\_DATA\_07**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_07)  `0x50`<br> |
| define  | [**EXT\_SENS\_DATA\_08**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_08)  `0x51`<br> |
| define  | [**EXT\_SENS\_DATA\_09**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_09)  `0x52`<br> |
| define  | [**EXT\_SENS\_DATA\_10**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_10)  `0x53`<br> |
| define  | [**EXT\_SENS\_DATA\_11**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_11)  `0x54`<br> |
| define  | [**EXT\_SENS\_DATA\_12**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_12)  `0x55`<br> |
| define  | [**EXT\_SENS\_DATA\_13**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_13)  `0x56`<br> |
| define  | [**EXT\_SENS\_DATA\_14**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_14)  `0x57`<br> |
| define  | [**EXT\_SENS\_DATA\_15**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_15)  `0x58`<br> |
| define  | [**EXT\_SENS\_DATA\_17**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_17)  `0x5A`<br> |
| define  | [**EXT\_SENS\_DATA\_18**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_18)  `0x5B`<br> |
| define  | [**EXT\_SENS\_DATA\_19**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_19)  `0x5C`<br> |
| define  | [**EXT\_SENS\_DATA\_20**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_20)  `0x5D`<br> |
| define  | [**EXT\_SENS\_DATA\_21**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_21)  `0x5E`<br> |
| define  | [**EXT\_SENS\_DATA\_22**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_22)  `0x5F`<br> |
| define  | [**EXT\_SENS\_DATA\_23**](ns__mpu6050__i2c__driver_8c.md#define-ext_sens_data_23)  `0x60`<br> |
| define  | [**FF\_DUR**](ns__mpu6050__i2c__driver_8c.md#define-ff_dur)  `0x1E`<br> |
| define  | [**FF\_THR**](ns__mpu6050__i2c__driver_8c.md#define-ff_thr)  `0x1D`<br> |
| define  | [**FIFO\_ACCEL\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_accel_en_bit)  `3`<br> |
| define  | [**FIFO\_COUNT\_H**](ns__mpu6050__i2c__driver_8c.md#define-fifo_count_h)  `0x72`<br> |
| define  | [**FIFO\_COUNT\_L**](ns__mpu6050__i2c__driver_8c.md#define-fifo_count_l)  `0x73`<br> |
| define  | [**FIFO\_EN**](ns__mpu6050__i2c__driver_8c.md#define-fifo_en)  `0x23`<br> |
| define  | [**FIFO\_R\_W**](ns__mpu6050__i2c__driver_8c.md#define-fifo_r_w)  `0x74`<br> |
| define  | [**FIFO\_SLV\_0\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_slv_0_en_bit)  `0`<br> |
| define  | [**FIFO\_SLV\_1\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_slv_1_en_bit)  `1`<br> |
| define  | [**FIFO\_SLV\_2\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_slv_2_en_bit)  `2`<br> |
| define  | [**FIFO\_TEMP\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_temp_en_bit)  `7`<br> |
| define  | [**FIFO\_XGYRO\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_xgyro_en_bit)  `6`<br> |
| define  | [**FIFO\_YGYRO\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_ygyro_en_bit)  `5`<br> |
| define  | [**FIFO\_ZGYRO\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-fifo_zgyro_en_bit)  `4`<br> |
| define  | [**GCONFIG\_FCHOICE\_B**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_fchoice_b)  `1`<br> |
| define  | [**GCONFIG\_FCHOICE\_B\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_fchoice_b_length)  `2`<br> |
| define  | [**GCONFIG\_FS\_SEL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_fs_sel_bit)  `4`<br> |
| define  | [**GCONFIG\_FS\_SEL\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_fs_sel_length)  `2`<br> |
| define  | [**GCONFIG\_XG\_ST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_xg_st_bit)  `7`<br> |
| define  | [**GCONFIG\_YG\_ST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_yg_st_bit)  `6`<br> |
| define  | [**GCONFIG\_ZG\_ST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-gconfig_zg_st_bit)  `5`<br> |
| define  | [**GYRO\_CONFIG**](ns__mpu6050__i2c__driver_8c.md#define-gyro_config)  `0x1B`<br> |
| define  | [**GYRO\_XOUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-gyro_xout_h)  `0x43`<br> |
| define  | [**GYRO\_XOUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-gyro_xout_l)  `0x44`<br> |
| define  | [**GYRO\_YOUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-gyro_yout_h)  `0x45`<br> |
| define  | [**GYRO\_YOUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-gyro_yout_l)  `0x46`<br> |
| define  | [**GYRO\_ZOUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-gyro_zout_h)  `0x47`<br> |
| define  | [**GYRO\_ZOUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-gyro_zout_l)  `0x48`<br> |
| define  | [**I2CMST\_CTRL\_CLOCK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_ctrl_clock_bit)  `3`<br> |
| define  | [**I2CMST\_CTRL\_CLOCK\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_ctrl_clock_length)  `4`<br> |
| define  | [**I2CMST\_CTRL\_MULT\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_ctrl_mult_en_bit)  `7`<br> |
| define  | [**I2CMST\_CTRL\_P\_NSR\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_ctrl_p_nsr_bit)  `4`<br> |
| define  | [**I2CMST\_CTRL\_SLV\_3\_FIFO\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_ctrl_slv_3_fifo_en_bit)  `5`<br> |
| define  | [**I2CMST\_CTRL\_WAIT\_FOR\_ES\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_ctrl_wait_for_es_bit)  `6`<br> |
| define  | [**I2CMST\_DLY\_ES\_SHADOW\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_dly_es_shadow_bit)  `7`<br> |
| define  | [**I2CMST\_DLY\_SLV0\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_dly_slv0_en_bit)  `0`<br> |
| define  | [**I2CMST\_DLY\_SLV1\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_dly_slv1_en_bit)  `1`<br> |
| define  | [**I2CMST\_DLY\_SLV2\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_dly_slv2_en_bit)  `2`<br> |
| define  | [**I2CMST\_DLY\_SLV3\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_dly_slv3_en_bit)  `3`<br> |
| define  | [**I2CMST\_DLY\_SLV4\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_dly_slv4_en_bit)  `4`<br> |
| define  | [**I2CMST\_STAT\_LOST\_ARB\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_lost_arb_bit)  `5`<br> |
| define  | [**I2CMST\_STAT\_PASS\_THROUGH\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_pass_through_bit)  `7`<br> |
| define  | [**I2CMST\_STAT\_SLV0\_NACK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_slv0_nack_bit)  `0`<br> |
| define  | [**I2CMST\_STAT\_SLV1\_NACK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_slv1_nack_bit)  `1`<br> |
| define  | [**I2CMST\_STAT\_SLV2\_NACK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_slv2_nack_bit)  `2`<br> |
| define  | [**I2CMST\_STAT\_SLV3\_NACK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_slv3_nack_bit)  `3`<br> |
| define  | [**I2CMST\_STAT\_SLV4\_DONE\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_slv4_done_bit)  `6`<br> |
| define  | [**I2CMST\_STAT\_SLV4\_NACK\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2cmst_stat_slv4_nack_bit)  `4`<br> |
| define  | [**I2C\_MST\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_mst_ctrl)  `0x24`<br> |
| define  | [**I2C\_MST\_DELAY\_CRTL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_mst_delay_crtl)  `0x67`<br> |
| define  | [**I2C\_MST\_STATUS**](ns__mpu6050__i2c__driver_8c.md#define-i2c_mst_status)  `0x36`<br> |
| define  | [**I2C\_SLV0\_ADDR**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv0_addr)  `0x25`<br> |
| define  | [**I2C\_SLV0\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv0_ctrl)  `0x27`<br> |
| define  | [**I2C\_SLV0\_DO**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv0_do)  `0x63`<br> |
| define  | [**I2C\_SLV0\_REG**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv0_reg)  `0x26`<br> |
| define  | [**I2C\_SLV1\_ADDR**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv1_addr)  `0x28`<br> |
| define  | [**I2C\_SLV1\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv1_ctrl)  `0x2A`<br> |
| define  | [**I2C\_SLV1\_DO**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv1_do)  `0x64`<br> |
| define  | [**I2C\_SLV1\_REG**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv1_reg)  `0x29`<br> |
| define  | [**I2C\_SLV2\_ADDR**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv2_addr)  `0x2B`<br> |
| define  | [**I2C\_SLV2\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv2_ctrl)  `0x2D`<br> |
| define  | [**I2C\_SLV2\_DO**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv2_do)  `0x65`<br> |
| define  | [**I2C\_SLV2\_REG**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv2_reg)  `0x2C`<br> |
| define  | [**I2C\_SLV3\_ADDR**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv3_addr)  `0x2E`<br> |
| define  | [**I2C\_SLV3\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv3_ctrl)  `0x30`<br> |
| define  | [**I2C\_SLV3\_DO**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv3_do)  `0x66`<br> |
| define  | [**I2C\_SLV3\_REG**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv3_reg)  `0x2F`<br> |
| define  | [**I2C\_SLV4\_ADDR**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_addr)  `0x31`<br> |
| define  | [**I2C\_SLV4\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_ctrl)  `0x34`<br> |
| define  | [**I2C\_SLV4\_DI**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_di)  `0x35`<br> |
| define  | [**I2C\_SLV4\_DO**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_do)  `0x33`<br> |
| define  | [**I2C\_SLV4\_DONE\_INT\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_done_int_bit)  `6`<br> |
| define  | [**I2C\_SLV4\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_en_bit)  `7`<br> |
| define  | [**I2C\_SLV4\_MST\_DELAY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_mst_delay_bit)  `4`<br> |
| define  | [**I2C\_SLV4\_MST\_DELAY\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_mst_delay_length)  `5`<br> |
| define  | [**I2C\_SLV4\_REG**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_reg)  `0x32`<br> |
| define  | [**I2C\_SLV4\_REG\_DIS\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv4_reg_dis_bit)  `5`<br> |
| define  | [**I2C\_SLV\_BYTE\_SW\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_byte_sw_bit)  `6`<br> |
| define  | [**I2C\_SLV\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_en_bit)  `7`<br> |
| define  | [**I2C\_SLV\_GRP\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_grp_bit)  `4`<br> |
| define  | [**I2C\_SLV\_ID\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_id_bit)  `6`<br> |
| define  | [**I2C\_SLV\_ID\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_id_length)  `7`<br> |
| define  | [**I2C\_SLV\_LEN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_len_bit)  `3`<br> |
| define  | [**I2C\_SLV\_LEN\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_len_length)  `4`<br> |
| define  | [**I2C\_SLV\_REG\_DIS\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_reg_dis_bit)  `5`<br> |
| define  | [**I2C\_SLV\_RNW\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-i2c_slv_rnw_bit)  `7`<br> |
| define  | [**INT\_CFG\_ANYRD\_2CLEAR\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_anyrd_2clear_bit)  `4`<br> |
| define  | [**INT\_CFG\_CLOCKOUT\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_clockout_en_bit)  `0`<br> |
| define  | [**INT\_CFG\_FSYNC\_INT\_MODE\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_fsync_int_mode_en_bit)  `2`<br> |
| define  | [**INT\_CFG\_FSYNC\_LEVEL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_fsync_level_bit)  `3`<br> |
| define  | [**INT\_CFG\_I2C\_BYPASS\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_i2c_bypass_en_bit)  `1`<br> |
| define  | [**INT\_CFG\_LATCH\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_latch_en_bit)  `5`<br> |
| define  | [**INT\_CFG\_LEVEL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_level_bit)  `7`<br> |
| define  | [**INT\_CFG\_OPEN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_cfg_open_bit)  `6`<br> |
| define  | [**INT\_ENABLE**](ns__mpu6050__i2c__driver_8c.md#define-int_enable)  `0x38`<br> |
| define  | [**INT\_ENABLE\_DMP\_RDY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_dmp_rdy_bit)  `1`<br> |
| define  | [**INT\_ENABLE\_FIFO\_OFLOW\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_fifo_oflow_bit)  `4`<br> |
| define  | [**INT\_ENABLE\_FREEFALL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_freefall_bit)  `7`<br> |
| define  | [**INT\_ENABLE\_I2C\_MST\_FSYNC\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_i2c_mst_fsync_bit)  `3`<br> |
| define  | [**INT\_ENABLE\_MOTION\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_motion_bit)  `6`<br> |
| define  | [**INT\_ENABLE\_PLL\_RDY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_pll_rdy_bit)  `2`<br> |
| define  | [**INT\_ENABLE\_RAW\_DATA\_RDY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_raw_data_rdy_bit)  `0`<br> |
| define  | [**INT\_ENABLE\_ZEROMOT\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_enable_zeromot_bit)  `5`<br> |
| define  | [**INT\_PIN\_CONFIG**](ns__mpu6050__i2c__driver_8c.md#define-int_pin_config)  `0x37`<br> |
| define  | [**INT\_STATUS**](ns__mpu6050__i2c__driver_8c.md#define-int_status)  `0x3A`<br> |
| define  | [**INT\_STATUS\_DMP\_RDY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_dmp_rdy_bit)  `1`<br> |
| define  | [**INT\_STATUS\_FIFO\_OFLOW\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_fifo_oflow_bit)  `4`<br> |
| define  | [**INT\_STATUS\_FREEFALL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_freefall_bit)  `7`<br> |
| define  | [**INT\_STATUS\_I2C\_MST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_i2c_mst_bit)  `3`<br> |
| define  | [**INT\_STATUS\_MOTION\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_motion_bit)  `6`<br> |
| define  | [**INT\_STATUS\_PLL\_RDY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_pll_rdy_bit)  `2`<br> |
| define  | [**INT\_STATUS\_RAW\_DATA\_RDY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_raw_data_rdy_bit)  `0`<br> |
| define  | [**INT\_STATUS\_ZEROMOT\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-int_status_zeromot_bit)  `5`<br> |
| define  | [**MEM\_R\_W**](ns__mpu6050__i2c__driver_8c.md#define-mem_r_w)  `0x6F`<br> |
| define  | [**MEM\_START\_ADDR**](ns__mpu6050__i2c__driver_8c.md#define-mem_start_addr)  `0x6E`<br> |
| define  | [**MOTION\_DUR**](ns__mpu6050__i2c__driver_8c.md#define-motion_dur)  `0x20`<br> |
| define  | [**MOTION\_THR**](ns__mpu6050__i2c__driver_8c.md#define-motion_thr)  `0x1F`<br> |
| define  | [**MPU6050\_RA\_XA\_OFFS\_H**](ns__mpu6050__i2c__driver_8c.md#define-mpu6050_ra_xa_offs_h)  `0x06`<br> |
| define  | [**MPU6050\_RA\_XA\_OFFS\_L**](ns__mpu6050__i2c__driver_8c.md#define-mpu6050_ra_xa_offs_l)  `0x07`<br> |
| define  | [**MPU6050\_RA\_YA\_OFFS\_H**](ns__mpu6050__i2c__driver_8c.md#define-mpu6050_ra_ya_offs_h)  `0x08`<br> |
| define  | [**MPU6050\_RA\_YA\_OFFS\_L**](ns__mpu6050__i2c__driver_8c.md#define-mpu6050_ra_ya_offs_l)  `0x09`<br> |
| define  | [**MPU6050\_RA\_ZA\_OFFS\_H**](ns__mpu6050__i2c__driver_8c.md#define-mpu6050_ra_za_offs_h)  `0x0A`<br> |
| define  | [**MPU6050\_RA\_ZA\_OFFS\_L**](ns__mpu6050__i2c__driver_8c.md#define-mpu6050_ra_za_offs_l)  `0x0B`<br> |
| define  | [**PRGM\_START\_H**](ns__mpu6050__i2c__driver_8c.md#define-prgm_start_h)  `0x70`<br> |
| define  | [**PRGM\_START\_L**](ns__mpu6050__i2c__driver_8c.md#define-prgm_start_l)  `0x71`<br> |
| define  | [**PWR1\_CLKSEL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_clksel_bit)  `2`<br> |
| define  | [**PWR1\_CLKSEL\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_clksel_length)  `3`<br> |
| define  | [**PWR1\_CYCLE\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_cycle_bit)  `5`<br> |
| define  | [**PWR1\_DEVICE\_RESET\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_device_reset_bit)  `7`<br> |
| define  | [**PWR1\_GYRO\_STANDBY\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_gyro_standby_bit)  `4`<br> |
| define  | [**PWR1\_SLEEP\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_sleep_bit)  `6`<br> |
| define  | [**PWR1\_TEMP\_DIS\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr1_temp_dis_bit)  `3`<br> |
| define  | [**PWR2\_LP\_WAKE\_CTRL\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_lp_wake_ctrl_bit)  `7`<br> |
| define  | [**PWR2\_LP\_WAKE\_CTRL\_LENGTH**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_lp_wake_ctrl_length)  `2`<br> |
| define  | [**PWR2\_STBY\_XA\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_xa_bit)  `5`<br> |
| define  | [**PWR2\_STBY\_XG\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_xg_bit)  `2`<br> |
| define  | [**PWR2\_STBY\_XYZA\_BITS**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_xyza_bits)  `1 &lt;&lt; PWR2\_STBY\_XA\_BIT \| 1 &lt;&lt; PWR2\_STBY\_YA\_BIT \| 1 &lt;&lt; PWR2\_STBY\_ZA\_BIT`<br> |
| define  | [**PWR2\_STBY\_XYZG\_BITS**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_xyzg_bits)  `1 &lt;&lt; PWR2\_STBY\_XG\_BIT \| 1 &lt;&lt; PWR2\_STBY\_YG\_BIT \| 1 &lt;&lt; PWR2\_STBY\_ZG\_BIT`<br> |
| define  | [**PWR2\_STBY\_YA\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_ya_bit)  `4`<br> |
| define  | [**PWR2\_STBY\_YG\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_yg_bit)  `1`<br> |
| define  | [**PWR2\_STBY\_ZA\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_za_bit)  `3`<br> |
| define  | [**PWR2\_STBY\_ZG\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-pwr2_stby_zg_bit)  `0`<br> |
| define  | [**PWR\_MGMT1**](ns__mpu6050__i2c__driver_8c.md#define-pwr_mgmt1)  `0x6B`<br> |
| define  | [**PWR\_MGMT2**](ns__mpu6050__i2c__driver_8c.md#define-pwr_mgmt2)  `0x6C`<br> |
| define  | [**SIGNAL\_PATH\_RESET**](ns__mpu6050__i2c__driver_8c.md#define-signal_path_reset)  `0x68`<br> |
| define  | [**SMPLRT\_DIV**](ns__mpu6050__i2c__driver_8c.md#define-smplrt_div)  `0x19`<br> |
| define  | [**SPATH\_ACCEL\_RST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-spath_accel_rst_bit)  `1`<br> |
| define  | [**SPATH\_GYRO\_RST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-spath_gyro_rst_bit)  `2`<br> |
| define  | [**SPATH\_TEMP\_RST\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-spath_temp_rst_bit)  `0`<br> |
| define  | [**TEMP\_OUT\_H**](ns__mpu6050__i2c__driver_8c.md#define-temp_out_h)  `0x41`<br> |
| define  | [**TEMP\_OUT\_L**](ns__mpu6050__i2c__driver_8c.md#define-temp_out_l)  `0x42`<br> |
| define  | [**USERCTRL\_DMP\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_dmp_en_bit)  `7`<br> |
| define  | [**USERCTRL\_DMP\_RESET\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_dmp_reset_bit)  `3`<br> |
| define  | [**USERCTRL\_FIFO\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_fifo_en_bit)  `6`<br> |
| define  | [**USERCTRL\_FIFO\_RESET\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_fifo_reset_bit)  `2`<br> |
| define  | [**USERCTRL\_I2C\_IF\_DIS\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_i2c_if_dis_bit)  `4`<br> |
| define  | [**USERCTRL\_I2C\_MST\_EN\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_i2c_mst_en_bit)  `5`<br> |
| define  | [**USERCTRL\_I2C\_MST\_RESET\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_i2c_mst_reset_bit)  `1`<br> |
| define  | [**USERCTRL\_SIG\_COND\_RESET\_BIT**](ns__mpu6050__i2c__driver_8c.md#define-userctrl_sig_cond_reset_bit)  `0`<br> |
| define  | [**USER\_CTRL**](ns__mpu6050__i2c__driver_8c.md#define-user_ctrl)  `0x6A`<br> |
| define  | [**WHO\_AM\_I**](ns__mpu6050__i2c__driver_8c.md#define-who_am_i)  `0x75`<br> |
| define  | [**XG\_OFFSET\_H**](ns__mpu6050__i2c__driver_8c.md#define-xg_offset_h)  `0x13`<br> |
| define  | [**XG\_OFFSET\_L**](ns__mpu6050__i2c__driver_8c.md#define-xg_offset_l)  `0x14`<br> |
| define  | [**YG\_OFFSET\_H**](ns__mpu6050__i2c__driver_8c.md#define-yg_offset_h)  `0x15`<br> |
| define  | [**YG\_OFFSET\_L**](ns__mpu6050__i2c__driver_8c.md#define-yg_offset_l)  `0x16`<br> |
| define  | [**ZG\_OFFSET\_H**](ns__mpu6050__i2c__driver_8c.md#define-zg_offset_h)  `0x17`<br> |
| define  | [**ZG\_OFFSET\_L**](ns__mpu6050__i2c__driver_8c.md#define-zg_offset_l)  `0x18`<br> |
| define  | [**ZRMOTION\_DUR**](ns__mpu6050__i2c__driver_8c.md#define-zrmotion_dur)  `0x22`<br> |
| define  | [**ZRMOTION\_THR**](ns__mpu6050__i2c__driver_8c.md#define-zrmotion_thr)  `0x21`<br> |

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



### function mpu6050\_calibration 

_Calibrate device offsets. Device must be still on a flat surface._ 
```C++
uint32_t mpu6050_calibration (
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



### function mpu6050\_get\_accel\_offset 

```C++
uint32_t mpu6050_get_accel_offset (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t axis,
    uint16_t * offset
) 
```




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



### function mpu6050\_get\_gyro\_offset 

```C++
uint32_t mpu6050_get_gyro_offset (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t axis,
    uint16_t * offset
) 
```




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
## Public Static Functions Documentation




### function get\_high\_bits 

```C++
static int8_t get_high_bits (
    int16_t regReading
) 
```




<hr>



### function get\_low\_bits 

```C++
static int8_t get_low_bits (
    int16_t regReading
) 
```




<hr>



### function read\_word\_register 

```C++
static uint32_t read_word_register (
    ns_i2c_config_t * cfg,
    uint32_t devAddr,
    uint8_t reg,
    uint16_t * value
) 
```




<hr>
## Macro Definition Documentation





### define ACCEL\_CONFIG 

```C++
#define ACCEL_CONFIG `0x1C`
```




<hr>



### define ACCEL\_XOUT\_H 

```C++
#define ACCEL_XOUT_H `0x3B`
```




<hr>



### define ACCEL\_XOUT\_L 

```C++
#define ACCEL_XOUT_L `0x3C`
```




<hr>



### define ACCEL\_YOUT\_H 

```C++
#define ACCEL_YOUT_H `0x3D`
```




<hr>



### define ACCEL\_YOUT\_L 

```C++
#define ACCEL_YOUT_L `0x3E`
```




<hr>



### define ACCEL\_ZOUT\_H 

```C++
#define ACCEL_ZOUT_H `0x3F`
```




<hr>



### define ACCEL\_ZOUT\_L 

```C++
#define ACCEL_ZOUT_L `0x40`
```




<hr>



### define ACONFIG\_FS\_SEL\_BIT 

```C++
#define ACONFIG_FS_SEL_BIT `4`
```




<hr>



### define ACONFIG\_FS\_SEL\_LENGTH 

```C++
#define ACONFIG_FS_SEL_LENGTH `2`
```




<hr>



### define ACONFIG\_HPF\_BIT 

```C++
#define ACONFIG_HPF_BIT `2`
```




<hr>



### define ACONFIG\_HPF\_LENGTH 

```C++
#define ACONFIG_HPF_LENGTH `3`
```




<hr>



### define ACONFIG\_XA\_ST\_BIT 

```C++
#define ACONFIG_XA_ST_BIT `7`
```




<hr>



### define ACONFIG\_YA\_ST\_BIT 

```C++
#define ACONFIG_YA_ST_BIT `6`
```




<hr>



### define ACONFIG\_ZA\_ST\_BIT 

```C++
#define ACONFIG_ZA_ST_BIT `5`
```




<hr>



### define BANKSEL\_CFG\_USER\_BANK\_BIT 

```C++
#define BANKSEL_CFG_USER_BANK_BIT `5`
```




<hr>



### define BANKSEL\_MEM\_SEL\_BIT 

```C++
#define BANKSEL_MEM_SEL_BIT `4`
```




<hr>



### define BANKSEL\_MEM\_SEL\_LENGTH 

```C++
#define BANKSEL_MEM_SEL_LENGTH `5`
```




<hr>



### define BANKSEL\_PRFTCH\_EN\_BIT 

```C++
#define BANKSEL_PRFTCH_EN_BIT `6`
```




<hr>



### define BANK\_SEL 

```C++
#define BANK_SEL `0x6D`
```




<hr>



### define CONFIG 

```C++
#define CONFIG `0x1A`
```




<hr>



### define CONFIG\_DLPF\_CFG\_BIT 

```C++
#define CONFIG_DLPF_CFG_BIT `2`
```




<hr>



### define CONFIG\_DLPF\_CFG\_LENGTH 

```C++
#define CONFIG_DLPF_CFG_LENGTH `3`
```




<hr>



### define CONFIG\_EXT\_SYNC\_SET\_BIT 

```C++
#define CONFIG_EXT_SYNC_SET_BIT `5`
```




<hr>



### define CONFIG\_EXT\_SYNC\_SET\_LENGTH 

```C++
#define CONFIG_EXT_SYNC_SET_LENGTH `3`
```




<hr>



### define CONFIG\_FIFO\_MODE\_BIT 

```C++
#define CONFIG_FIFO_MODE_BIT `6`
```




<hr>



### define DMP\_INT\_STATUS 

```C++
#define DMP_INT_STATUS `0x39`
```




<hr>



### define DMP\_INT\_STATUS\_0 

```C++
#define DMP_INT_STATUS_0 `0`
```




<hr>



### define DMP\_INT\_STATUS\_1 

```C++
#define DMP_INT_STATUS_1 `1`
```




<hr>



### define DMP\_INT\_STATUS\_2 

```C++
#define DMP_INT_STATUS_2 `2`
```




<hr>



### define DMP\_INT\_STATUS\_3 

```C++
#define DMP_INT_STATUS_3 `3`
```




<hr>



### define DMP\_INT\_STATUS\_4 

```C++
#define DMP_INT_STATUS_4 `4`
```




<hr>



### define DMP\_INT\_STATUS\_5 

```C++
#define DMP_INT_STATUS_5 `5`
```




<hr>



### define EXT\_SENS\_DATA\_00 

```C++
#define EXT_SENS_DATA_00 `0x49`
```




<hr>



### define EXT\_SENS\_DATA\_01 

```C++
#define EXT_SENS_DATA_01 `0x4A`
```




<hr>



### define EXT\_SENS\_DATA\_02 

```C++
#define EXT_SENS_DATA_02 `0x4B`
```




<hr>



### define EXT\_SENS\_DATA\_03 

```C++
#define EXT_SENS_DATA_03 `0x4C`
```




<hr>



### define EXT\_SENS\_DATA\_04 

```C++
#define EXT_SENS_DATA_04 `0x4D`
```




<hr>



### define EXT\_SENS\_DATA\_05 

```C++
#define EXT_SENS_DATA_05 `0x4E`
```




<hr>



### define EXT\_SENS\_DATA\_06 

```C++
#define EXT_SENS_DATA_06 `0x4F`
```




<hr>



### define EXT\_SENS\_DATA\_07 

```C++
#define EXT_SENS_DATA_07 `0x50`
```




<hr>



### define EXT\_SENS\_DATA\_08 

```C++
#define EXT_SENS_DATA_08 `0x51`
```




<hr>



### define EXT\_SENS\_DATA\_09 

```C++
#define EXT_SENS_DATA_09 `0x52`
```




<hr>



### define EXT\_SENS\_DATA\_10 

```C++
#define EXT_SENS_DATA_10 `0x53`
```




<hr>



### define EXT\_SENS\_DATA\_11 

```C++
#define EXT_SENS_DATA_11 `0x54`
```




<hr>



### define EXT\_SENS\_DATA\_12 

```C++
#define EXT_SENS_DATA_12 `0x55`
```




<hr>



### define EXT\_SENS\_DATA\_13 

```C++
#define EXT_SENS_DATA_13 `0x56`
```




<hr>



### define EXT\_SENS\_DATA\_14 

```C++
#define EXT_SENS_DATA_14 `0x57`
```




<hr>



### define EXT\_SENS\_DATA\_15 

```C++
#define EXT_SENS_DATA_15 `0x58`
```




<hr>



### define EXT\_SENS\_DATA\_17 

```C++
#define EXT_SENS_DATA_17 `0x5A`
```




<hr>



### define EXT\_SENS\_DATA\_18 

```C++
#define EXT_SENS_DATA_18 `0x5B`
```




<hr>



### define EXT\_SENS\_DATA\_19 

```C++
#define EXT_SENS_DATA_19 `0x5C`
```




<hr>



### define EXT\_SENS\_DATA\_20 

```C++
#define EXT_SENS_DATA_20 `0x5D`
```




<hr>



### define EXT\_SENS\_DATA\_21 

```C++
#define EXT_SENS_DATA_21 `0x5E`
```




<hr>



### define EXT\_SENS\_DATA\_22 

```C++
#define EXT_SENS_DATA_22 `0x5F`
```




<hr>



### define EXT\_SENS\_DATA\_23 

```C++
#define EXT_SENS_DATA_23 `0x60`
```




<hr>



### define FF\_DUR 

```C++
#define FF_DUR `0x1E`
```




<hr>



### define FF\_THR 

```C++
#define FF_THR `0x1D`
```




<hr>



### define FIFO\_ACCEL\_EN\_BIT 

```C++
#define FIFO_ACCEL_EN_BIT `3`
```




<hr>



### define FIFO\_COUNT\_H 

```C++
#define FIFO_COUNT_H `0x72`
```




<hr>



### define FIFO\_COUNT\_L 

```C++
#define FIFO_COUNT_L `0x73`
```




<hr>



### define FIFO\_EN 

```C++
#define FIFO_EN `0x23`
```




<hr>



### define FIFO\_R\_W 

```C++
#define FIFO_R_W `0x74`
```




<hr>



### define FIFO\_SLV\_0\_EN\_BIT 

```C++
#define FIFO_SLV_0_EN_BIT `0`
```




<hr>



### define FIFO\_SLV\_1\_EN\_BIT 

```C++
#define FIFO_SLV_1_EN_BIT `1`
```




<hr>



### define FIFO\_SLV\_2\_EN\_BIT 

```C++
#define FIFO_SLV_2_EN_BIT `2`
```




<hr>



### define FIFO\_TEMP\_EN\_BIT 

```C++
#define FIFO_TEMP_EN_BIT `7`
```




<hr>



### define FIFO\_XGYRO\_EN\_BIT 

```C++
#define FIFO_XGYRO_EN_BIT `6`
```




<hr>



### define FIFO\_YGYRO\_EN\_BIT 

```C++
#define FIFO_YGYRO_EN_BIT `5`
```




<hr>



### define FIFO\_ZGYRO\_EN\_BIT 

```C++
#define FIFO_ZGYRO_EN_BIT `4`
```




<hr>



### define GCONFIG\_FCHOICE\_B 

```C++
#define GCONFIG_FCHOICE_B `1`
```




<hr>



### define GCONFIG\_FCHOICE\_B\_LENGTH 

```C++
#define GCONFIG_FCHOICE_B_LENGTH `2`
```




<hr>



### define GCONFIG\_FS\_SEL\_BIT 

```C++
#define GCONFIG_FS_SEL_BIT `4`
```




<hr>



### define GCONFIG\_FS\_SEL\_LENGTH 

```C++
#define GCONFIG_FS_SEL_LENGTH `2`
```




<hr>



### define GCONFIG\_XG\_ST\_BIT 

```C++
#define GCONFIG_XG_ST_BIT `7`
```




<hr>



### define GCONFIG\_YG\_ST\_BIT 

```C++
#define GCONFIG_YG_ST_BIT `6`
```




<hr>



### define GCONFIG\_ZG\_ST\_BIT 

```C++
#define GCONFIG_ZG_ST_BIT `5`
```




<hr>



### define GYRO\_CONFIG 

```C++
#define GYRO_CONFIG `0x1B`
```




<hr>



### define GYRO\_XOUT\_H 

```C++
#define GYRO_XOUT_H `0x43`
```




<hr>



### define GYRO\_XOUT\_L 

```C++
#define GYRO_XOUT_L `0x44`
```




<hr>



### define GYRO\_YOUT\_H 

```C++
#define GYRO_YOUT_H `0x45`
```




<hr>



### define GYRO\_YOUT\_L 

```C++
#define GYRO_YOUT_L `0x46`
```




<hr>



### define GYRO\_ZOUT\_H 

```C++
#define GYRO_ZOUT_H `0x47`
```




<hr>



### define GYRO\_ZOUT\_L 

```C++
#define GYRO_ZOUT_L `0x48`
```




<hr>



### define I2CMST\_CTRL\_CLOCK\_BIT 

```C++
#define I2CMST_CTRL_CLOCK_BIT `3`
```




<hr>



### define I2CMST\_CTRL\_CLOCK\_LENGTH 

```C++
#define I2CMST_CTRL_CLOCK_LENGTH `4`
```




<hr>



### define I2CMST\_CTRL\_MULT\_EN\_BIT 

```C++
#define I2CMST_CTRL_MULT_EN_BIT `7`
```




<hr>



### define I2CMST\_CTRL\_P\_NSR\_BIT 

```C++
#define I2CMST_CTRL_P_NSR_BIT `4`
```




<hr>



### define I2CMST\_CTRL\_SLV\_3\_FIFO\_EN\_BIT 

```C++
#define I2CMST_CTRL_SLV_3_FIFO_EN_BIT `5`
```




<hr>



### define I2CMST\_CTRL\_WAIT\_FOR\_ES\_BIT 

```C++
#define I2CMST_CTRL_WAIT_FOR_ES_BIT `6`
```




<hr>



### define I2CMST\_DLY\_ES\_SHADOW\_BIT 

```C++
#define I2CMST_DLY_ES_SHADOW_BIT `7`
```




<hr>



### define I2CMST\_DLY\_SLV0\_EN\_BIT 

```C++
#define I2CMST_DLY_SLV0_EN_BIT `0`
```




<hr>



### define I2CMST\_DLY\_SLV1\_EN\_BIT 

```C++
#define I2CMST_DLY_SLV1_EN_BIT `1`
```




<hr>



### define I2CMST\_DLY\_SLV2\_EN\_BIT 

```C++
#define I2CMST_DLY_SLV2_EN_BIT `2`
```




<hr>



### define I2CMST\_DLY\_SLV3\_EN\_BIT 

```C++
#define I2CMST_DLY_SLV3_EN_BIT `3`
```




<hr>



### define I2CMST\_DLY\_SLV4\_EN\_BIT 

```C++
#define I2CMST_DLY_SLV4_EN_BIT `4`
```




<hr>



### define I2CMST\_STAT\_LOST\_ARB\_BIT 

```C++
#define I2CMST_STAT_LOST_ARB_BIT `5`
```




<hr>



### define I2CMST\_STAT\_PASS\_THROUGH\_BIT 

```C++
#define I2CMST_STAT_PASS_THROUGH_BIT `7`
```




<hr>



### define I2CMST\_STAT\_SLV0\_NACK\_BIT 

```C++
#define I2CMST_STAT_SLV0_NACK_BIT `0`
```




<hr>



### define I2CMST\_STAT\_SLV1\_NACK\_BIT 

```C++
#define I2CMST_STAT_SLV1_NACK_BIT `1`
```




<hr>



### define I2CMST\_STAT\_SLV2\_NACK\_BIT 

```C++
#define I2CMST_STAT_SLV2_NACK_BIT `2`
```




<hr>



### define I2CMST\_STAT\_SLV3\_NACK\_BIT 

```C++
#define I2CMST_STAT_SLV3_NACK_BIT `3`
```




<hr>



### define I2CMST\_STAT\_SLV4\_DONE\_BIT 

```C++
#define I2CMST_STAT_SLV4_DONE_BIT `6`
```




<hr>



### define I2CMST\_STAT\_SLV4\_NACK\_BIT 

```C++
#define I2CMST_STAT_SLV4_NACK_BIT `4`
```




<hr>



### define I2C\_MST\_CTRL 

```C++
#define I2C_MST_CTRL `0x24`
```




<hr>



### define I2C\_MST\_DELAY\_CRTL 

```C++
#define I2C_MST_DELAY_CRTL `0x67`
```




<hr>



### define I2C\_MST\_STATUS 

```C++
#define I2C_MST_STATUS `0x36`
```




<hr>



### define I2C\_SLV0\_ADDR 

```C++
#define I2C_SLV0_ADDR `0x25`
```




<hr>



### define I2C\_SLV0\_CTRL 

```C++
#define I2C_SLV0_CTRL `0x27`
```




<hr>



### define I2C\_SLV0\_DO 

```C++
#define I2C_SLV0_DO `0x63`
```




<hr>



### define I2C\_SLV0\_REG 

```C++
#define I2C_SLV0_REG `0x26`
```




<hr>



### define I2C\_SLV1\_ADDR 

```C++
#define I2C_SLV1_ADDR `0x28`
```




<hr>



### define I2C\_SLV1\_CTRL 

```C++
#define I2C_SLV1_CTRL `0x2A`
```




<hr>



### define I2C\_SLV1\_DO 

```C++
#define I2C_SLV1_DO `0x64`
```




<hr>



### define I2C\_SLV1\_REG 

```C++
#define I2C_SLV1_REG `0x29`
```




<hr>



### define I2C\_SLV2\_ADDR 

```C++
#define I2C_SLV2_ADDR `0x2B`
```




<hr>



### define I2C\_SLV2\_CTRL 

```C++
#define I2C_SLV2_CTRL `0x2D`
```




<hr>



### define I2C\_SLV2\_DO 

```C++
#define I2C_SLV2_DO `0x65`
```




<hr>



### define I2C\_SLV2\_REG 

```C++
#define I2C_SLV2_REG `0x2C`
```




<hr>



### define I2C\_SLV3\_ADDR 

```C++
#define I2C_SLV3_ADDR `0x2E`
```




<hr>



### define I2C\_SLV3\_CTRL 

```C++
#define I2C_SLV3_CTRL `0x30`
```




<hr>



### define I2C\_SLV3\_DO 

```C++
#define I2C_SLV3_DO `0x66`
```




<hr>



### define I2C\_SLV3\_REG 

```C++
#define I2C_SLV3_REG `0x2F`
```




<hr>



### define I2C\_SLV4\_ADDR 

```C++
#define I2C_SLV4_ADDR `0x31`
```




<hr>



### define I2C\_SLV4\_CTRL 

```C++
#define I2C_SLV4_CTRL `0x34`
```




<hr>



### define I2C\_SLV4\_DI 

```C++
#define I2C_SLV4_DI `0x35`
```




<hr>



### define I2C\_SLV4\_DO 

```C++
#define I2C_SLV4_DO `0x33`
```




<hr>



### define I2C\_SLV4\_DONE\_INT\_BIT 

```C++
#define I2C_SLV4_DONE_INT_BIT `6`
```




<hr>



### define I2C\_SLV4\_EN\_BIT 

```C++
#define I2C_SLV4_EN_BIT `7`
```




<hr>



### define I2C\_SLV4\_MST\_DELAY\_BIT 

```C++
#define I2C_SLV4_MST_DELAY_BIT `4`
```




<hr>



### define I2C\_SLV4\_MST\_DELAY\_LENGTH 

```C++
#define I2C_SLV4_MST_DELAY_LENGTH `5`
```




<hr>



### define I2C\_SLV4\_REG 

```C++
#define I2C_SLV4_REG `0x32`
```




<hr>



### define I2C\_SLV4\_REG\_DIS\_BIT 

```C++
#define I2C_SLV4_REG_DIS_BIT `5`
```




<hr>



### define I2C\_SLV\_BYTE\_SW\_BIT 

```C++
#define I2C_SLV_BYTE_SW_BIT `6`
```




<hr>



### define I2C\_SLV\_EN\_BIT 

```C++
#define I2C_SLV_EN_BIT `7`
```




<hr>



### define I2C\_SLV\_GRP\_BIT 

```C++
#define I2C_SLV_GRP_BIT `4`
```




<hr>



### define I2C\_SLV\_ID\_BIT 

```C++
#define I2C_SLV_ID_BIT `6`
```




<hr>



### define I2C\_SLV\_ID\_LENGTH 

```C++
#define I2C_SLV_ID_LENGTH `7`
```




<hr>



### define I2C\_SLV\_LEN\_BIT 

```C++
#define I2C_SLV_LEN_BIT `3`
```




<hr>



### define I2C\_SLV\_LEN\_LENGTH 

```C++
#define I2C_SLV_LEN_LENGTH `4`
```




<hr>



### define I2C\_SLV\_REG\_DIS\_BIT 

```C++
#define I2C_SLV_REG_DIS_BIT `5`
```




<hr>



### define I2C\_SLV\_RNW\_BIT 

```C++
#define I2C_SLV_RNW_BIT `7`
```




<hr>



### define INT\_CFG\_ANYRD\_2CLEAR\_BIT 

```C++
#define INT_CFG_ANYRD_2CLEAR_BIT `4`
```




<hr>



### define INT\_CFG\_CLOCKOUT\_EN\_BIT 

```C++
#define INT_CFG_CLOCKOUT_EN_BIT `0`
```




<hr>



### define INT\_CFG\_FSYNC\_INT\_MODE\_EN\_BIT 

```C++
#define INT_CFG_FSYNC_INT_MODE_EN_BIT `2`
```




<hr>



### define INT\_CFG\_FSYNC\_LEVEL\_BIT 

```C++
#define INT_CFG_FSYNC_LEVEL_BIT `3`
```




<hr>



### define INT\_CFG\_I2C\_BYPASS\_EN\_BIT 

```C++
#define INT_CFG_I2C_BYPASS_EN_BIT `1`
```




<hr>



### define INT\_CFG\_LATCH\_EN\_BIT 

```C++
#define INT_CFG_LATCH_EN_BIT `5`
```




<hr>



### define INT\_CFG\_LEVEL\_BIT 

```C++
#define INT_CFG_LEVEL_BIT `7`
```




<hr>



### define INT\_CFG\_OPEN\_BIT 

```C++
#define INT_CFG_OPEN_BIT `6`
```




<hr>



### define INT\_ENABLE 

```C++
#define INT_ENABLE `0x38`
```




<hr>



### define INT\_ENABLE\_DMP\_RDY\_BIT 

```C++
#define INT_ENABLE_DMP_RDY_BIT `1`
```




<hr>



### define INT\_ENABLE\_FIFO\_OFLOW\_BIT 

```C++
#define INT_ENABLE_FIFO_OFLOW_BIT `4`
```




<hr>



### define INT\_ENABLE\_FREEFALL\_BIT 

```C++
#define INT_ENABLE_FREEFALL_BIT `7`
```




<hr>



### define INT\_ENABLE\_I2C\_MST\_FSYNC\_BIT 

```C++
#define INT_ENABLE_I2C_MST_FSYNC_BIT `3`
```




<hr>



### define INT\_ENABLE\_MOTION\_BIT 

```C++
#define INT_ENABLE_MOTION_BIT `6`
```




<hr>



### define INT\_ENABLE\_PLL\_RDY\_BIT 

```C++
#define INT_ENABLE_PLL_RDY_BIT `2`
```




<hr>



### define INT\_ENABLE\_RAW\_DATA\_RDY\_BIT 

```C++
#define INT_ENABLE_RAW_DATA_RDY_BIT `0`
```




<hr>



### define INT\_ENABLE\_ZEROMOT\_BIT 

```C++
#define INT_ENABLE_ZEROMOT_BIT `5`
```




<hr>



### define INT\_PIN\_CONFIG 

```C++
#define INT_PIN_CONFIG `0x37`
```




<hr>



### define INT\_STATUS 

```C++
#define INT_STATUS `0x3A`
```




<hr>



### define INT\_STATUS\_DMP\_RDY\_BIT 

```C++
#define INT_STATUS_DMP_RDY_BIT `1`
```




<hr>



### define INT\_STATUS\_FIFO\_OFLOW\_BIT 

```C++
#define INT_STATUS_FIFO_OFLOW_BIT `4`
```




<hr>



### define INT\_STATUS\_FREEFALL\_BIT 

```C++
#define INT_STATUS_FREEFALL_BIT `7`
```




<hr>



### define INT\_STATUS\_I2C\_MST\_BIT 

```C++
#define INT_STATUS_I2C_MST_BIT `3`
```




<hr>



### define INT\_STATUS\_MOTION\_BIT 

```C++
#define INT_STATUS_MOTION_BIT `6`
```




<hr>



### define INT\_STATUS\_PLL\_RDY\_BIT 

```C++
#define INT_STATUS_PLL_RDY_BIT `2`
```




<hr>



### define INT\_STATUS\_RAW\_DATA\_RDY\_BIT 

```C++
#define INT_STATUS_RAW_DATA_RDY_BIT `0`
```




<hr>



### define INT\_STATUS\_ZEROMOT\_BIT 

```C++
#define INT_STATUS_ZEROMOT_BIT `5`
```




<hr>



### define MEM\_R\_W 

```C++
#define MEM_R_W `0x6F`
```




<hr>



### define MEM\_START\_ADDR 

```C++
#define MEM_START_ADDR `0x6E`
```




<hr>



### define MOTION\_DUR 

```C++
#define MOTION_DUR `0x20`
```




<hr>



### define MOTION\_THR 

```C++
#define MOTION_THR `0x1F`
```




<hr>



### define MPU6050\_RA\_XA\_OFFS\_H 

```C++
#define MPU6050_RA_XA_OFFS_H `0x06`
```




<hr>



### define MPU6050\_RA\_XA\_OFFS\_L 

```C++
#define MPU6050_RA_XA_OFFS_L `0x07`
```




<hr>



### define MPU6050\_RA\_YA\_OFFS\_H 

```C++
#define MPU6050_RA_YA_OFFS_H `0x08`
```




<hr>



### define MPU6050\_RA\_YA\_OFFS\_L 

```C++
#define MPU6050_RA_YA_OFFS_L `0x09`
```




<hr>



### define MPU6050\_RA\_ZA\_OFFS\_H 

```C++
#define MPU6050_RA_ZA_OFFS_H `0x0A`
```




<hr>



### define MPU6050\_RA\_ZA\_OFFS\_L 

```C++
#define MPU6050_RA_ZA_OFFS_L `0x0B`
```




<hr>



### define PRGM\_START\_H 

```C++
#define PRGM_START_H `0x70`
```




<hr>



### define PRGM\_START\_L 

```C++
#define PRGM_START_L `0x71`
```




<hr>



### define PWR1\_CLKSEL\_BIT 

```C++
#define PWR1_CLKSEL_BIT `2`
```




<hr>



### define PWR1\_CLKSEL\_LENGTH 

```C++
#define PWR1_CLKSEL_LENGTH `3`
```




<hr>



### define PWR1\_CYCLE\_BIT 

```C++
#define PWR1_CYCLE_BIT `5`
```




<hr>



### define PWR1\_DEVICE\_RESET\_BIT 

```C++
#define PWR1_DEVICE_RESET_BIT `7`
```




<hr>



### define PWR1\_GYRO\_STANDBY\_BIT 

```C++
#define PWR1_GYRO_STANDBY_BIT `4`
```




<hr>



### define PWR1\_SLEEP\_BIT 

```C++
#define PWR1_SLEEP_BIT `6`
```




<hr>



### define PWR1\_TEMP\_DIS\_BIT 

```C++
#define PWR1_TEMP_DIS_BIT `3`
```




<hr>



### define PWR2\_LP\_WAKE\_CTRL\_BIT 

```C++
#define PWR2_LP_WAKE_CTRL_BIT `7`
```




<hr>



### define PWR2\_LP\_WAKE\_CTRL\_LENGTH 

```C++
#define PWR2_LP_WAKE_CTRL_LENGTH `2`
```




<hr>



### define PWR2\_STBY\_XA\_BIT 

```C++
#define PWR2_STBY_XA_BIT `5`
```




<hr>



### define PWR2\_STBY\_XG\_BIT 

```C++
#define PWR2_STBY_XG_BIT `2`
```




<hr>



### define PWR2\_STBY\_XYZA\_BITS 

```C++
#define PWR2_STBY_XYZA_BITS `1 << PWR2_STBY_XA_BIT | 1 << PWR2_STBY_YA_BIT | 1 << PWR2_STBY_ZA_BIT`
```




<hr>



### define PWR2\_STBY\_XYZG\_BITS 

```C++
#define PWR2_STBY_XYZG_BITS `1 << PWR2_STBY_XG_BIT | 1 << PWR2_STBY_YG_BIT | 1 << PWR2_STBY_ZG_BIT`
```




<hr>



### define PWR2\_STBY\_YA\_BIT 

```C++
#define PWR2_STBY_YA_BIT `4`
```




<hr>



### define PWR2\_STBY\_YG\_BIT 

```C++
#define PWR2_STBY_YG_BIT `1`
```




<hr>



### define PWR2\_STBY\_ZA\_BIT 

```C++
#define PWR2_STBY_ZA_BIT `3`
```




<hr>



### define PWR2\_STBY\_ZG\_BIT 

```C++
#define PWR2_STBY_ZG_BIT `0`
```




<hr>



### define PWR\_MGMT1 

```C++
#define PWR_MGMT1 `0x6B`
```




<hr>



### define PWR\_MGMT2 

```C++
#define PWR_MGMT2 `0x6C`
```




<hr>



### define SIGNAL\_PATH\_RESET 

```C++
#define SIGNAL_PATH_RESET `0x68`
```




<hr>



### define SMPLRT\_DIV 

```C++
#define SMPLRT_DIV `0x19`
```




<hr>



### define SPATH\_ACCEL\_RST\_BIT 

```C++
#define SPATH_ACCEL_RST_BIT `1`
```




<hr>



### define SPATH\_GYRO\_RST\_BIT 

```C++
#define SPATH_GYRO_RST_BIT `2`
```




<hr>



### define SPATH\_TEMP\_RST\_BIT 

```C++
#define SPATH_TEMP_RST_BIT `0`
```




<hr>



### define TEMP\_OUT\_H 

```C++
#define TEMP_OUT_H `0x41`
```




<hr>



### define TEMP\_OUT\_L 

```C++
#define TEMP_OUT_L `0x42`
```




<hr>



### define USERCTRL\_DMP\_EN\_BIT 

```C++
#define USERCTRL_DMP_EN_BIT `7`
```




<hr>



### define USERCTRL\_DMP\_RESET\_BIT 

```C++
#define USERCTRL_DMP_RESET_BIT `3`
```




<hr>



### define USERCTRL\_FIFO\_EN\_BIT 

```C++
#define USERCTRL_FIFO_EN_BIT `6`
```




<hr>



### define USERCTRL\_FIFO\_RESET\_BIT 

```C++
#define USERCTRL_FIFO_RESET_BIT `2`
```




<hr>



### define USERCTRL\_I2C\_IF\_DIS\_BIT 

```C++
#define USERCTRL_I2C_IF_DIS_BIT `4`
```




<hr>



### define USERCTRL\_I2C\_MST\_EN\_BIT 

```C++
#define USERCTRL_I2C_MST_EN_BIT `5`
```




<hr>



### define USERCTRL\_I2C\_MST\_RESET\_BIT 

```C++
#define USERCTRL_I2C_MST_RESET_BIT `1`
```




<hr>



### define USERCTRL\_SIG\_COND\_RESET\_BIT 

```C++
#define USERCTRL_SIG_COND_RESET_BIT `0`
```




<hr>



### define USER\_CTRL 

```C++
#define USER_CTRL `0x6A`
```




<hr>



### define WHO\_AM\_I 

```C++
#define WHO_AM_I `0x75`
```




<hr>



### define XG\_OFFSET\_H 

```C++
#define XG_OFFSET_H `0x13`
```




<hr>



### define XG\_OFFSET\_L 

```C++
#define XG_OFFSET_L `0x14`
```




<hr>



### define YG\_OFFSET\_H 

```C++
#define YG_OFFSET_H `0x15`
```




<hr>



### define YG\_OFFSET\_L 

```C++
#define YG_OFFSET_L `0x16`
```




<hr>



### define ZG\_OFFSET\_H 

```C++
#define ZG_OFFSET_H `0x17`
```




<hr>



### define ZG\_OFFSET\_L 

```C++
#define ZG_OFFSET_L `0x18`
```




<hr>



### define ZRMOTION\_DUR 

```C++
#define ZRMOTION_DUR `0x22`
```




<hr>



### define ZRMOTION\_THR 

```C++
#define ZRMOTION_THR `0x21`
```




<hr>

------------------------------
The documentation for this class was generated from the following file `neuralSPOT/neuralspot/ns-i2c/src/ns_mpu6050_i2c_driver.c`

