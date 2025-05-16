/**
 * @file ns_imu_icm45605_driver.h
 * @author Ambiq
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

uint32_t ns_imu_ICM45605_init(ns_imu_config_t *cfg);
uint32_t ns_imu_ICM_45606_get_data(ns_imu_config_t *cfg, ns_imu_sensor_data_t *data);
