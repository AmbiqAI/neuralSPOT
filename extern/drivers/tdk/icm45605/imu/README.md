# Overview

The eMD driver is TDK Invensense's reference code to drive our IMU from a microcontroller-based system. It is coded in C language and organized around modules. 

## Common files

All modules rely on the following files. 

Files:
* `imu/inv_imu_transport.h`: Definition of the abstraction layer used to communicate with the IMU.
* `imu/inv_imu_transport.c`: Implementation of the abstraction layer used to communicate with the IMU.
* `imu/inv_imu.h`: Describes IMU specificities and capabilities.
* `imu/inv_imu_regmap_le.h`: Exposes register map using bit-field representation in Little Endian.
* `imu/inv_imu_regmap_be.h`: Exposes register map using bit-field representation in Big Endian.
* `imu/inv_imu_defs.h`: Defines possible values for most used registers.

By default, the driver will include the Little Endian variant of the register map (`imu/inv_imu_regmap_le.h`) which suits the majority of microcontroller. In case your microcontroller is Big Endian, you might have to change it in the `imu/inv_imu_defs.h`:
```C
/* Include regmap (le = little endian, be = big endian) */
#include "imu/inv_imu_regmap_le.h"
/* #include "imu/inv_imu_regmap_be.h" */
``` 

## Basic driver

The **basic driver** contains the basic functionalities required to operate our IMU.

Depends on:
* **Common files**

Files:
* `imu/inv_imu_driver.h`: Definition of the basic driver API.
* `imu/inv_imu_driver.c`: Implementation of the basic driver API.
* `imu/inv_imu_version.h`: Contains the driver's version as a string. 

## Advanced driver

The **advanced driver** provides functionalities for specific applications, requiring in-depth understanding of the IMU. It also provides API with a higher abstraction level than those exposed in the **basic driver**. 

Depends on:
* **Basic driver**
* **Common files**

Files:
* `imu/inv_imu_driver_advanced.h`: Definition of the advanced driver API.
* `imu/inv_imu_driver_advanced.c`: Implementation of the advanced driver API.

## Self-test

The **self-test** module contains the code required to operate IMU's self-test.

Depends on:
* **Advanced driver**
* **Basic driver**
* **Common files**

Files:
* `imu/inv_imu_selftest.h`: Definition of the self-test module API.
* `imu/inv_imu_selftest.c`: Implementation of the self-test module API.

## EDMP

The **EDMP** module provides API to operate algorithm features (APEX) on-chip. The following features are available:
* Pedometer (step count and step detector)
* Tap detection
* FreeFall detection, including Low-G and High-G detection.
* Tilt detection
* Raise-to-wake detection
* Significant Motion Detection

Depends on:
* **Basic driver**
* **Common files**

Files:
* `imu/inv_imu_edmp.h`: Definition of the EDMP module API.
* `imu/inv_imu_edmp.c`: Implementation of the EDMP module API.
* `imu/inv_imu_edmp_memmap.h`: Exposes memory map of the EDMP (addresses and size of the required variables).
* `imu/inv_imu_edmp_defs.h`: Defines useful values for EDMP.

## Aux1/2

The **Aux1** and **Aux2** modules contains the code required to operate the auxiliary ports of the IMU. Please note that not all our IMU support Aux1 and Aux2 port. Please refer to the datasheet of your device.

Depends on:
* **Common files**

Files:
* `imu/inv_imu_driver_aux1.h`: Definition of the AUX1 module API.
* `imu/inv_imu_driver_aux1.c`: Implementation of the AUX1 module API.
* `imu/inv_imu_driver_aux2.h`: Definition of the AUX2 module API.
* `imu/inv_imu_driver_aux2.c`: Implementation of the AUX2 module API.

# Initializing driver

## Using basic driver

Please, follow these steps:
* On your application code, create a local variable of type `inv_imu_device_t`: `inv_imu_device_t imu_dev;`
* Initialize transport layer: `transport` structure inside `inv_imu_device_t`
  * Provide an implementation of the `read_reg`, `write_reg` and `sleep_us` functions and initialize the corresponding pointers.
  * Configure the `serif_type` field which indicates the serial interface used. Available options are listed in `inv_imu_transport.h`.
* Wait for the device to be properly supplied.
* Optionally, you can check the IMU whoami here.
* It is then recommend to trigger a soft-reset to ensure the part is in a known state.

Example of initialization in C:
```C
inv_imu_device_t imu_dev;
uint8_t          whoami;

/* Transport layer initialization */
imu_dev.transport.read_reg   = si_io_imu_read_reg;
imu_dev.transport.write_reg  = si_io_imu_write_reg;
imu_dev.transport.sleep_us   = si_sleep_us;
imu_dev.transport.serif_type = UI_SPI4;

/* Wait 3 ms to ensure device is properly supplied  */
si_sleep_us(3000);

/* Check whoami */
rc |= inv_imu_get_who_am_i(&imu_dev, &whoami);
if (whoami != INV_IMU_WHOAMI)
	return ERROR_CODE;

/* Trigger soft-reset */
rc |= inv_imu_soft_reset(&imu_dev);
```

## Using advanced driver

Please, follow these steps:
* On your application code, create a local variable of type `inv_imu_device_t`: `inv_imu_device_t imu_dev;`
* You might want to also create a pointer of type `inv_imu_adv_var_t` which will give you easy access to advanced variables: `inv_imu_adv_var_t *e = (inv_imu_adv_var_t *)imu_dev.adv_var;`
* Initialize transport layer: `transport` structure inside `inv_imu_device_t`
  * Provide an implementation of the `read_reg`, `write_reg` and `sleep_us` functions and initialize the corresponding pointers.
  * Configure the `serif_type` field which indicates the serial interface used. Available options are listed in `inv_imu_transport.h`.
* Initialize sensor event callback pointer in `inv_imu_adv_var_t` structure. This function will be called when a new sensor data will be available.
* Call the `inv_imu_adv_init` function. It will automatically check the whoami and trigger a soft-reset.

Example of initialization in C:
```C
inv_imu_device_t   imu_dev;
inv_imu_adv_var_t *e = (inv_imu_adv_var_t *)imu_dev.adv_var;

/* Transport layer initialization */
imu_dev.transport.read_reg   = si_io_imu_read_reg;
imu_dev.transport.write_reg  = si_io_imu_write_reg;
imu_dev.transport.sleep_us   = si_sleep_us;
imu_dev.transport.serif_type = UI_SPI4;

/* Init sensor event callback */
e->sensor_event_cb = sensor_event_cb;

/* Init advanced driver */
rc |= inv_imu_adv_init(&imu_dev);
```
