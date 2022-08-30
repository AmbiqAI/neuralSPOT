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
#include "ns-rpc-audio.h"
#include "ns_peripherals_button.h"
#include "ns-usb.h"
#include "ns_peripherals_power.h"

#define NUMSAMPLES 32
#define AXES 3
float g_sensorData[NUMSAMPLES*2][7]; // 32 samples of gryo, accel and temp 

int
main(void) {
    uint8_t buffer[32];
    void *mpuHandle;
    int16_t accelX, accelY, accelZ, gyroX, gyroY, gyroZ, temperature_regval;
    float temperature;
    float accelVals[AXES];
    float gyroVals[AXES];
    uint32_t i, axis = 0;

    ns_itm_printf_enable();
    ns_debug_printf_enable();

    am_hal_interrupt_master_enable();
    ns_power_config(&ns_development_default);

    #ifdef NS_RPC_AUDIO
        ns_rpc_audio_init();
    #endif

    // Button global - will be set by neuralSPOT button helper
    int g_intButtonPressed = 0;

    // Button Peripheral Config Struct
    ns_button_config_t button_config = {
        .button_0_enable = true,
        .button_1_enable = false,
        .button_0_flag = &g_intButtonPressed,
        .button_1_flag = NULL
    };
    ns_peripheral_button_init(&button_config);

    ns_printf("Press Button 0 to begin calibration\n");
    ns_printf("Please place your sensor on a flat surface until calibration is finished\n");
    while (g_intButtonPressed == 0) {
        tud_task(); // tinyusb device task for RPC
        ns_delay_us(1000);
    }
    g_intButtonPressed = 0;
    ns_printf("Beginning MPU Calibration...\n");

    ns_i2c_interface_init(1, MPU_I2CADDRESS_AD0_LOW, &mpuHandle);
    mpu6050_finish_init(mpuHandle);
    if(mpu6050_calibration()) {
        ns_printf("Calibration Failed!\n");
        return 1;
    }
    ns_printf("MPU Calibration successful. Sampling data...\n");

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

        accelGravity(accelVals, accelX, accelY, accelZ, ACCEL_FS_16G);
        gyroDegPerSec(gyroVals, gyroX, gyroY, gyroZ, GYRO_FS_500DPS);

        // Capture data in RPC buffer
        for (axis = 0; axis<AXES; axis++) {
            g_sensorData[i][axis] = accelVals[axis];
            g_sensorData[i][axis+AXES] = gyroVals[axis];
        }
        g_sensorData[i][6] = temperature;
        i++;

        #ifdef NS_RPC_AUDIO
            tud_task(); // tinyusb device task for RPC
            if (i==NUMSAMPLES) {
                ns_rpc_audio_send_buffer((uint8_t*)g_sensorData, NUMSAMPLES * 7 * sizeof(float));
                i = 0;
            }
        #endif
        // am_util_stdio_printf("%d] %f, %f, %f\n", i, accelVals[0], accelVals[1], accelVals[2]);
        // for (axis = 0; axis<AXES; axis++) {
        //     ns_printf("%f, %f, %f, ", g_sensorData[i][axis]);
        //     ns_printf("%f, %f, %f, ", g_sensorData[i][axis+AXES]);
        //     ns_printf("%f\n", g_sensorData[i][6]);
        // }

        ns_delay_us(5000);
    }
}
