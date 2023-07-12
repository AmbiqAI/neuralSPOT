#include "Adafruit_AHRS_NXPFusion.cpp"
#include <stdio.h>
#include "Adafruit_AHRS_NXPmatrix.c"
int main() {
    Adafruit_NXPSensorFusion filter;
    filter.begin(100);
    filter.update(0.061037, 0.061037, -0.045778, 0, 10, 12, 1000, 2000, 3000);
    float roll = filter.getRoll();
    float pitch = filter.getPitch();
    float heading = filter.getYaw();
    float qw, qx, qy, qz;
    filter.getQuaternion(&qw, &qx, &qy, &qz);
    printf("roll: %f\n", roll);
    printf("pitch: %f\n", pitch);
    printf("yaw: %f\n", heading);
    printf("quaternion: %f, %f, %f, %f\n", qw, qx, qy, qz);
    return 0;
}