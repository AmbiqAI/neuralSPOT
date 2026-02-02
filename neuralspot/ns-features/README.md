# neuralSPOT Features Library
ns-features is a feature extraction library designed to facilitate the extraction of features from sensor data. It provides a collection of functions and utilities to process sensor values and compute corresponding features, such as quaternion values and Euler angles from accelerometer and gyroscope data.


## Features

The ns-features library includes the following features:

### Sensor Fusion

The library offers functions to compute quaternion values and Euler angles from accelerometer and gyro data. Both Euler angles and quaternions are used to represent an object's orientation in 3D space. However, quaternion values provide a more concise and efficient representation of rotations.

| Feature                   | Function              | Description                                                                                          | Usage                                                                                             |
|---------------------------|-----------------------|------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------|
| Initialization Function   | `ns_mahony_init`      | Initializes the Mahony filter configuration structure with default values. Returns status codes to indicate success or specific errors.                            | `ns_mahony_init(ns_mahony_cfg_t *cfg)`                                                  |
| Quaternion Computation    | `ns_mahony_update`    | Computes and normalizes quaternion values from accelerometer and gyro data.                         | `ns_mahony_update(ns_mahony_cfg_t *cfg, float gx, float gy, float gz, float ax, float ay, float az)` |
| Euler Angle Computation   | `ns_get_RollPitchYaw` | Computes roll, pitch, and yaw angles from quaternion values.                                          | `ns_get_RollPitchYaw(ns_mahony_cfg_t *cfg, double *roll, double *pitch, double *yaw)`             |
| Get Quaternion Values     | `ns_get_quaternion`   | Retrieves quaternion values (`qw`, `qx`, `qy`, `qz`) from the Mahony filter configuration structure. | `ns_get_quaternion(ns_mahony_cfg_t *cfg, double *qw, double *qx, double *qy, double *qz)` |



Example for using ns-features:

- [Quaternion visualization](../../apps/examples/quaternion/README.md)
