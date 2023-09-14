# Mahony filter Quaternion Plotter Example
This example implements a sensor fusion algorithm with the Mahony filter and a quaternion plotter. It collects data from an MPU6050 accelerometer and gyroscopic sensor, calculates the quaternion values and uses eRPC to send the values to be plotted by a python script. This example is based on (http://davidegironi.blogspot.com/2013/02/avr-atmega-mpu6050-gyroscope-and.html#.VzjDKfnhCUk) and (https://github.com/scottshambaugh/mpl_quaternion_views.git).

To use this model, you'll need to connect the sensor (an MPU6050) to the EVB ([instructions here](../../neuralspot/ns-i2c/README.md)) and install and setup eRPC ([instructions here](../../neuralSPOT/examples/rpc_server/README.md))

# Building and Running the Example
```bash
$> cd .../neuralSPOT
$> make clean
$> make -j # makes everything
$> make TARGET=ns-ahrs deploy # flashes HAR to the target EVB
$> make view # connects to EVB SWO via SEGGER Jlink
# open a second terminal. In second terminal:
$> cd examples/ns-ahrs/src
$> python -m quaternion_plotter -t COMx -o quaternion.csv -m server # more details on installation and setup of rpc Server can be found at examples/rpc_server/README.md
If all went well, you can press Button 0, which will start the event loop. At this point, the quaternions will be computed and plotted continuously.
```
# NXP filter
The files in the nxp folder contain the nxp sensor fusion algorithm. The algorithm requires a magnetometer on the sensor, which the MPU6050 does not have. 

