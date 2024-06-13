// #include "gtest.h"
// #include "gtest.h"
#include "mpu6050.h"

// test fixture
// class MahonyUpdateTest : public ::testing::Test {
// protected:
//     float q0, q1, q2, q3; // quaternion values
//     float integralFBx,  integralFBy, integralFBz;

//     void SetUp() override {
//         // initialize quaternion values
//         q0 = 1.0f;
//         q1 = 0.0f;
//         q2 = 0.0f;
//         q3 = 0.0f;
//         integralFBx = 0.0f;
//         integralFBy = 0.0f;
//         integralFBz = 0.0f;

//     }

//     void TearDown() override {
//         // clean up after the tests if needed
//     }
// };

float q0, q1, q2, q3; // quaternion values
float integralFBx, integralFBy, integralFBz;

void MahonyUpdateTest_pre_test_hook() {
    // initialize quaternion values
    q0 = 1.0f;
    q1 = 0.0f;
    q2 = 0.0f;
    q3 = 0.0f;
    integralFBx = 0.0f;
    integralFBy = 0.0f;
    integralFBz = 0.0f;
}

MahonyUpdateTest_post_test_hook() {
    // clean up after the tests if needed
}

void MahonyUpdateTest_AllZeroAccelValues() {
    // set sample gyro values
    float gx = 0.1f, gy = 0.2f, gz = 0.3f;

    // set all accelerometer values to zero
    float ax = 0.0f, ay = 0.0f, az = 0.0f;

    // call the Mahony update function
    mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

    // check if the quaternion remains unchanged
    EXPECT_FLOAT_EQ(q0, 1.0f);
    EXPECT_FLOAT_EQ(q1, 0.0f);
    EXPECT_FLOAT_EQ(q2, 0.0f);
    EXPECT_FLOAT_EQ(q3, 0.0f);
}

// // test all zero accel values
// TEST_F(MahonyUpdateTest, AllZeroAccelValues) {
//     // set sample gyro values
//     float gx = 0.1f, gy = 0.2f, gz = 0.3f;

//     // set all accelerometer values to zero
//     float ax = 0.0f, ay = 0.0f, az = 0.0f;

//     // call the Mahony update function
//     mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

//     // check if the quaternion remains unchanged
//     EXPECT_FLOAT_EQ(q0, 1.0f);
//     EXPECT_FLOAT_EQ(q1, 0.0f);
//     EXPECT_FLOAT_EQ(q2, 0.0f);
//     EXPECT_FLOAT_EQ(q3, 0.0f);
// }

// test with large accel and gyro values
void MahonyUpdateTest_LargeAccelGyroValues() {
    // Set sample large accel and gyro values
    float gx = 10000.0f, gy = -20000.0f, gz = 30000.0f;
    float ax = -40000.0f, ay = 50000.0f, az = -60000.0f;

    // call Mahony update function
    mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

    // check if the quaternion is updated
    EXPECT_NE(q0, 1.0f);
    EXPECT_NE(q1, 0.0f);
    EXPECT_NE(q2, 0.0f);
    EXPECT_NE(q3, 0.0f);
}

// TEST_F(MahonyUpdateTest, LargeAccelGyroValues) {
//     // Set sample large accel and gyro values
//     float gx = 10000.0f, gy = -20000.0f, gz = 30000.0f;
//     float ax = -40000.0f, ay = 50000.0f, az = -60000.0f;

//     // call Mahony update function
//     mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

//     // check if the quaternion is updated
//     EXPECT_NE(q0, 1.0f);
//     EXPECT_NE(q1, 0.0f);
//     EXPECT_NE(q2, 0.0f);
//     EXPECT_NE(q3, 0.0f);
// }

// test with negative acceleration and gyro values
void MahonyUpdateTest_NegativeInputs() {
    // Set sample negative accel and gyro values
    float gx = -0.1f, gy = -0.2f, gz = -0.3f;
    float ax = -0.4f, ay = -0.5f, az = -0.6f;

    // call Mahony update function
    mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

    // check if the quaternion is updated
    EXPECT_NE(q0, 1.0f);
    EXPECT_NE(q1, 0.0f);
    EXPECT_NE(q2, 0.0f);
    EXPECT_NE(q3, 0.0f);
}

// TEST_F(MahonyUpdateTest, NegativeInputs) {
//     // Set sample negative accel and gyro values
//     float gx = -0.1f, gy = -0.2f, gz = -0.3f;
//     float ax = -0.4f, ay = -0.5f, az = -0.6f;

//     // call Mahony update function
//     mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

//     // check if the quaternion is updated
//     EXPECT_NE(q0, 1.0f);
//     EXPECT_NE(q1, 0.0f);
//     EXPECT_NE(q2, 0.0f);
//     EXPECT_NE(q3, 0.0f);
// }

// test to get quaternion values with NULL
void MahonyUpdateTest_NullPointer() {
    // set sample quaternion values
    q0 = 0.707f;
    q1 = 0.0f;
    q2 = 0.0f;
    q3 = 0.707f;

    double qw, qx, qy, qz;

    // call the function to get quaternion values with a NULL pointer
    mpu6050_getQuaternion(&qw, nullptr, &qy, &qz);

    EXPECT_FLOAT_EQ(qw, 0.707f);
    EXPECT_FLOAT_EQ(qy, 0.0f);
    EXPECT_FLOAT_EQ(qz, 0.707f);
}
// TEST_F(MahonyUpdateTest, NullPointer) {
//     // set sample quaternion values
//     q0 = 0.707f;
//     q1 = 0.0f;
//     q2 = 0.0f;
//     q3 = 0.707f;

//     double qw, qx, qy, qz;

//     // call the function to get quaternion values with a NULL pointer
//     mpu6050_getQuaternion(&qw, nullptr, &qy, &qz);

//     EXPECT_FLOAT_EQ(qw, 0.707f);
//     EXPECT_FLOAT_EQ(qy, 0.0f);
//     EXPECT_FLOAT_EQ(qz, 0.707f);
// }

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
