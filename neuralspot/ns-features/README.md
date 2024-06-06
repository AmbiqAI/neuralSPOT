# NeuralSPOT Features Library
ns-features is a feature extraction library designed to facilitate the extraction of features from sensor data. It provides a collection of functions and utilities to process sensor values and compute corresponding features, such as quaternion values and Euler angles from accelerometer and gyroscope data.


## Features

The ns-features library includes the following features:

### Sensor Fusion

The library offers functions to compute quaternion values and Euler angles from accelerometer and gyro data. Both Euler angles and quaternions are used to represent an object's orientation in 3D space. However, quaternion values provide a more concise and efficient representation of rotations. 

| Feature              | Function                | Description                                                  | Usage                                 |
| -------------------- | ----------------------- | ------------------------------------------------------------ | -------------------------------------- |
| Quaternion Computation | mpu6050_mahonyUpdate      | Computes and normalizes quaternion values from accelerometer and gyro data. | `mpu6050_mahonyUpdate(float gx, float gy, float gz, float ax, float ay, float az)` |
| Euler Angle Computation | mpu6050_getRollPitchYaw      | Computes roll, pitch, and yaw angles from quaternion values. | `mpu6050_getRollPitchYaw(double *roll, double *pitch, double *yaw)` |

Example for using ns-features:

- [Quaternion visualization](../../examples/quaternion/README.md)


