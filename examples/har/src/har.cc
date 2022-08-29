/**
 * @file har.cc
 * @author Carlos Morales
 * @brief HAR, but just i2c invocation for now
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <cstdlib>
#include <cstring>

#include "ns_ambiqsuite_harness.h"
#include "ns_i2c_register_driver.h"
#include "mpu6050_i2c_driver.h"

int
main(void) {
    uint8_t buffer[32];
    void *mpuHandle;
    int16_t accelX, accelY, accelZ, gyroX, gyroY, gyroZ, temperature_regval;
    float temperature;

    ns_itm_printf_enable();
    ns_debug_printf_enable();

    ns_delay_us(1000000);
    ns_i2c_interface_init(1, MPU_I2CADDRESS_AD0_LOW, &mpuHandle);
    mpu6050_finish_init(mpuHandle);

    ns_printf("Beginning MPU Calibration!\n\n");
    ns_printf("Please place your sensor on a flat surface until calibration is finished\n");
    if(mpu6050_calibration()) {
        ns_printf("Calibration Failed!\n");
        return 1;
    }

    ns_delay_us(1000000);
    while (1) {
        read_sensors(buffer);
        
        // decode the buffer
        accelX = buffer[0] << 8 | buffer[1];
        accelY = buffer[2] << 8 | buffer[3];
        accelZ = buffer[4] << 8 | buffer[5];

        temperature_regval = (buffer[6]<<8 | buffer[7]);
        temperature = temperature_regval/340.0+36.53;

        gyroX = buffer[8] << 8 | buffer[9];
        gyroY = buffer[10] << 8 | buffer[11];
        gyroZ = buffer[12] << 8 | buffer[13];

        ns_printf("Raw Accel Data: [%d %d %d]  \t", accelX, accelY, accelZ);
        ns_printf("Raw Gyro Data: [%d %d %d] \t", gyroX, gyroY, gyroZ);
        ns_printf("Temperature: [%f]\n", temperature);
        ns_delay_us(1000000);

    }
}
