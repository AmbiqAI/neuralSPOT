# neuralSPOT IMU Driver Library

This library provides a lightweight wrapper around the ICM-45605 IMU sensor and other supported IMUs, making it easy to initialize, configure, calibrate, and read 6‑degree‑of‑freedom (6DOF) data on Ambiq Apollo MCUs.

---

## Table of Contents

1. [Features](#features)
3. [Installation](#installation)
4. [Quick Start](#quick-start)
5. [Configuration Options](#configuration-options)
6. [APIs](#apis)
7. [Code Examples](#code-examples)
   * [Polling Example](#polling-example)
   * [Interrupt‑Driven Example](#interrupt‑driven-example)
8. [Calibration](#calibration)
9. [Versioning and Compatibility](#versioning-and-compatibility)
10. [License](#license)

---

## Features

* Easy SPI initialization for ICM-45605 IMU
* Automatic soft reset and FSR/ODR configuration
* Built‑in calibration routine for accel and gyro biases
* Optional data‑ready interrupt support with frame buffering
* Simple C API for embedded real‑time applications


## Installation

1. Clone the `neuralSPOT` repository:

   ```bash
   git clone https://github.com/AmbiqAI/neuralSPOT.git
   ```

2. Add the `neuralspot/ns-imu` folder to your build system include path.

3. Link against `ns-imu/src/ns_imu.c`, `ns_imu_icm45605_driver.c`, and other core source files.

## Quick Start

1. Include the API header:

   ```c
   #include "ns_imu.h"
   ```

2. Declare and configure `ns_imu_config_t`:

   ```c
   ns_imu_config_t imu_cfg = {
     .api               = &ns_imu_current_version,
     .sensor            = NS_IMU_SENSOR_ICM45605,
     .iom               = 0,                             // SPI port
     .accel_fsr         = ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G,
     .gyro_fsr          = GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS,
     .accel_odr         = ACCEL_CONFIG0_ACCEL_ODR_50_HZ,
     .gyro_odr          = GYRO_CONFIG0_GYRO_ODR_50_HZ,
     .accel_ln_bw       = IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4,
     .gyro_ln_bw        = IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4,
     .calibrate         = true,                          // run calibration
     .frame_available_cb= NULL,                          // polling mode
     .frame_size        = 0,
     .frame_buffer      = NULL
   };

   ns_imu_sensor_data_t imu_data;
   ```

3. Initialize core and power rails, then configure IMU:

   ```c
   NS_TRY(ns_core_init(&core_cfg), "Core init failed.");
   NS_TRY(ns_power_config(&ns_development_default), "Power init failed.");
   NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Perf mode failed.");

   if (ns_imu_configure(&imu_cfg) != NS_STATUS_SUCCESS) {
       ns_lp_printf("IMU init failed\n");
   }
   ```

4. Read data in your loop:

   ```c
   while (1) {
     if (ns_imu_get_data(&imu_cfg, &imu_data) == NS_STATUS_SUCCESS) {
         ns_lp_printf("Accel: %f, %f, %f | Gyro: %f, %f, %f\n",
             imu_data.accel_g[0], imu_data.accel_g[1], imu_data.accel_g[2],
             imu_data.gyro_dps[0], imu_data.gyro_dps[1], imu_data.gyro_dps[2]);
     }
     ns_delay_us(20000); // 50 Hz polling
   }
   ```

## Configuration Options

| Field                       | Description                                 |
| --------------------------- | ------------------------------------------- |
| `sensor`                    | IMU sensor type (`NS_IMU_SENSOR_ICM45605`)  |
| `iom`                       | SPI/I2C instance                            |
| `accel_fsr`                 | Accel full‑scale range (e.g., 4 G)          |
| `gyro_fsr`                  | Gyro full‑scale range (e.g., 2000 dps)      |
| `accel_odr`                 | Accel output data rate (e.g., 50 Hz)        |
| `gyro_odr`                  | Gyro output data rate                       |
| `accel_ln_bw`, `gyro_ln_bw` | Low‑noise bandwidth dividers                |
| `calibrate`                 | Enable built‑in calibration                 |
| `frame_available_cb`        | Data‑ready callback (NULL for polling)      |
| `frame_size`                | Number of samples per frame                 |
| `frame_buffer`              | Buffer pointer for interrupt-driven samples |

## APIs

* `uint32_t ns_imu_configure(ns_imu_config_t *cfg);`
* `uint32_t ns_imu_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data);`
* `uint32_t ns_imu_ICM45605_init(ns_imu_config_t *cfg);`
* `uint32_t ns_imu_ICM_45606_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data);`
* `uint32_t ns_imu_ICM45605_configure_interrupts(ns_imu_config_t *cfg);`
* `uint32_t ns_imu_ICM_45605_handle_interrupt(void);`

Refer to `ns-imu/includes-api/ns_imu.h` and `ns_imu_icm45605_driver.h` for full signatures.

## Code Examples

### Polling Example

See `examples/imc/src/imc.cc`:

```c
ns_imu_configure(&imu_cfg);
while (1) {
  ns_imu_get_data(&imu_cfg, &imu_data);
  // process imu_data
  ns_delay_us(20000);
}
```

### Interrupt‑Driven Example

```c
// Allocate buffer for 20 frames
ns_imu_sensor_data_t imu_frame[20];

void frame_cb(void *arg) {
    ns_imu_config_t *cfg = (ns_imu_config_t *)arg;
    for (int i = 0; i < cfg->frame_size; i++) {
        // handle imu_frame[i]
    }
}

imu_cfg.frame_available_cb = frame_cb;
imu_cfg.frame_size        = 20;
imu_cfg.frame_buffer      = imu_frame;

ns_imu_configure(&imu_cfg);
// MPU will interrupt and call frame_cb internally
while (1) {
  ns_deep_sleep();
}
```

## Calibration

When `calibrate = true`, the driver averages 250 samples (plus 10 warm‑up) to compute accel/gyro biases. Stationary orientation assumed: accel = \[0,0,1g], gyro = \[0,0,0].

## Versioning and Compatibility

* Current API: **v1.0.0** (`NS_IMU_CURRENT_VERSION`)
* Oldest supported: **v0.0.1**
* Sensor: ICM-45605

## License

Licensed under the Ambiq AI neuralSPOT SDK license. See [LICENSE](../LICENSE) for details.
