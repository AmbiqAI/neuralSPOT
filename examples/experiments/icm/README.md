# ICM (ICM-45605) Example

This example demonstrates how to use the neuralSPOT IMU driver to collect accelerometer, gyroscope, and temperature data from the ICM-45605 sensor on an Ambiq Apollo MCU. It shows both initialization, calibration, and interrupt-driven frame buffering using a user callback.

---

## Contents

```text
examples/icm/
├── src/
│   └── icm.cc         # Main example application
└── README.md          # This file
```

## Description

The `imc.cc` application configures the ICM-45605 IMU to sample at 50 Hz, performs an initial bias calibration, and then uses a data-ready interrupt to collect a frame of sensor samples. Upon filling the frame buffer, a user callback prints all samples.

This example is based on Ambiq’s Human Activity Recognition (HAR) model, demonstrating how to integrate raw IMU data into higher‑level AI workflows.

## Prerequisites

* NeuralSPOT SDK (with `neuralspot/ns-imu` module)
* Valid hardware setup:

  * Apollo510 EVB
  * ICM-45605 Mikroe Board (MIKRO-6040)
  * J13 jumper selecting 3.3v

## Building and Running the Example

1. **Flash the Firmware**

   ```bash
    make EXAMPLE=experiments/icm && make EXAMPLE=experiments/icm TARGET=icm deploy && make view
   ```
2. **Monitor Output**

   * On reset, you should see:

     ```console
     Ready to test IMC
     NS_IMU: Calibrating ICM-45605
     IMU Init Success
     Frame available
     Frame 0: Accel: 0.00 0.00 1.00, Gyro: 0.00 0.00 0.00, Temp: 25.00
     Frame 1: ...
     ```
   * Each `Frame available` indicates the buffer is ready.

## Code Walkthrough

1. **Callback Definition**

   ```c++
   void my_frame_available_cb(void *arg) {
       ns_imu_config_t *cfg = (ns_imu_config_t *)arg;
       for (int i = 0; i < cfg->frame_size; i++) {
           // Print each sample
       }
   }
   ```
2. **Configuration Structure**

   ```c++
   ns_imu_config_t imu_cfg = {
       .api               = &ns_imu_V1_0_0,
       .sensor            = NS_IMU_SENSOR_ICM45605,
       .iom               = 0,                    // SPI0
       .accel_fsr         = ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G,
       .gyro_fsr          = GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS,
       .accel_odr         = ACCEL_CONFIG0_ACCEL_ODR_50_HZ,
       .gyro_odr          = GYRO_CONFIG0_GYRO_ODR_50_HZ,
       .accel_ln_bw       = IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4,
       .gyro_ln_bw        = IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4,
       .calibrate         = true,                // perform bias calibration
       .frame_available_cb= my_frame_available_cb,
       .frame_size        = 20,                  // number of samples per interrupt
       .frame_buffer      = imu_frame
   };
   ```
3. **Initialization & Main Loop**

   ```c++
   ns_core_init(...);
   ns_power_config(...);
   ns_set_performance_mode(...);
   ns_imu_configure(&imu_cfg);

   while (1) {
     ns_deep_sleep();  // wake on IMU interrupt
   }
   ```

## Troubleshooting

* **No ****************************************************************************`Frame available`**************************************************************************** messages**:

  * Check SPI wiring and chip select wiring.
  * Ensure the mikrobus power selector (J13) is set to 3.3v.
* **Incorrect sensor readings**:

  * Keep board stationary during calibration.
  * Verify `calibrate = true` in config.
* **Build errors**:

  * Confirm include paths and linking of `inv_imu_driver.c`.

---

For more details on the IMU driver API, see [neuralspot/ns-imu/includes-api/ns\_imu.h](../../neuralspot/ns-imu/includes-api/ns_imu.h).
