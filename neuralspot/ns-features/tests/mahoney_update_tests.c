#include "quaternion.h"
#include "unity/unity.h"
#include "ns_core.h"
#define M_PI 3.14159265358979323846
static ns_mahony_cfg_t mcfg;

void mahoney_update_tests_pre_test_hook() {
    // initialize quaternion values
    mcfg.api = &ns_mahony_V0_0_1;
    ns_mahony_init(&mcfg);
}

void mahoney_update_tests_post_test_hook() {
    // clean up after the tests if needed
}


void MahonyUpdateTest_AllZeroAccelGyroValues() {
    // set sample gyro values
    // float gx = 0.1f, gy = 0.2f, gz = 0.3f;
    float gx = 0.0f, gy = 0.0f, gz = 0.0f;

    // set all accelerometer values to zero
    float ax = 0.0f, ay = 0.0f, az = 0.0f;

    // call the Mahony update function
    ns_mahony_update(&mcfg, gx, gy, gz, ax, ay, az);

    // check if the quaternion remains unchanged
    TEST_ASSERT_EQUAL_FLOAT(mcfg.q0, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(mcfg.q1, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(mcfg.q2, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(mcfg.q3, 0.0f);

}

// test with large accel and gyro values
void MahonyUpdateTest_LargeAccelGyroValues() {
    // Set sample large accel and gyro values
    float gx = 10000.0f, gy = -20000.0f, gz = 30000.0f;
    float ax = -40000.0f, ay = 50000.0f, az = -60000.0f;

    // call Mahony update function
    ns_mahony_update(&mcfg, gx, gy, gz, ax, ay, az);

    // check if the quaternion is updated
    TEST_ASSERT_NOT_EQUAL(mcfg.q0, 1.0f);
    TEST_ASSERT_NOT_EQUAL(mcfg.q1, 0.0f);
    TEST_ASSERT_NOT_EQUAL(mcfg.q2, 0.0f);
    TEST_ASSERT_NOT_EQUAL(mcfg.q3, 0.0f);
}
void MahonyUpdateTest_RollPitchYaw() {
    mcfg.api = &ns_mahony_V0_0_1;
    mcfg.q0 = 1.0f;
    mcfg.q1 = 0.0f;
    mcfg.q2 = 0.0f;
    mcfg.q3 = 0.0f;
    double roll, pitch, yaw;
    ns_get_RollPitchYaw(&mcfg, &roll, &pitch, &yaw);
    TEST_ASSERT_EQUAL(roll, 0);
    TEST_ASSERT_EQUAL(pitch, 0);
    TEST_ASSERT_EQUAL(yaw, 0);

    // test 90 degree rotation about the x-axis
    mcfg.q0 = 0.7071;
    mcfg.q1 = 0.7071;
    mcfg.q2 = 0;
    mcfg.q3 = 0;
    ns_get_RollPitchYaw(&mcfg, &roll, &pitch, &yaw);
    TEST_ASSERT_FLOAT_WITHIN(0.0002, -1.570816, roll);
    TEST_ASSERT_FLOAT_WITHIN(0.0002, 0, pitch); 
    TEST_ASSERT_FLOAT_WITHIN(0.0002, 0, yaw);

    // test 90 degree rotation about the y-axis
    mcfg.q0 = 0.7071;
    mcfg.q1 = 0;
    mcfg.q2 = 0.7071;
    mcfg.q3 = 0;

    ns_get_RollPitchYaw(&mcfg, &roll, &pitch, &yaw);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0, roll);
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.570816, pitch); 
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0, yaw);

    // test 90 degree rotation about the z-axis
    mcfg.q0 = 0.7071;
    mcfg.q1 = 0;
    mcfg.q2 = 0;
    mcfg.q3 = 0.7071;

    ns_get_RollPitchYaw(&mcfg, &roll, &pitch, &yaw);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0, roll);
    TEST_ASSERT_FLOAT_WITHIN(0.01,0, pitch); 
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.570816, yaw);
}

// test with negative acceleration and gyro values
void MahonyUpdateTest_NegativeInputs() {
    // Set sample negative accel and gyro values
    float gx = -0.1f, gy = -0.2f, gz = -0.3f;
    float ax = -0.4f, ay = -0.5f, az = -0.6f;
    // call Mahony update function
    ns_mahony_update(&mcfg, gx, gy, gz, ax, ay, az);

    // check if the quaternion is updated
    TEST_ASSERT_NOT_EQUAL(mcfg.q0, 1.0f);
    TEST_ASSERT_NOT_EQUAL(mcfg.q1, 0.0f);
    TEST_ASSERT_NOT_EQUAL(mcfg.q2, 0.0f);
    TEST_ASSERT_NOT_EQUAL(mcfg.q3, 0.0f);
}

// test to get quaternion values with NULL
void MahonyUpdateTest_NullPointer() {
    // ns_mahony_init null pointer check
    int status = ns_mahony_init(0);
    TEST_ASSERT_EQUAL(1, status);

    // ns_mahony_update null pointer check
    float gx = 0.0f, gy = 0.0f, gz = 0.0f;
    // set all accelerometer values to zero
    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    status = ns_mahony_update(0, gx, gy, gz, ax, ay, az);
    // call the function to update mahony filter with a NULL pointer
    TEST_ASSERT_EQUAL(1, status);

    // ns_get_quaternion null pointer check
    mcfg.api = &ns_mahony_V0_0_1;
    mcfg.q0 = 0.707f;
    mcfg.q1 = 0.0f;
    mcfg.q2 = 0.0f;
    mcfg.q3 = 0.707f;
    double qw, qy, qz;
    // call the function to get quaternion values with a NULL pointer
    status = ns_get_quaternion(&mcfg, &qw, 0, &qy, &qz);
    TEST_ASSERT_EQUAL(1, status);

    // ns_get_RollPitchYaw null pointer check
    double roll, pitch, yaw;
    status = ns_get_RollPitchYaw(0, &roll, &pitch, &yaw);
    TEST_ASSERT_EQUAL(1, status);

    status = ns_get_RollPitchYaw(&mcfg, 0, &pitch, &yaw);
    TEST_ASSERT_EQUAL(1, status);

    status = ns_get_RollPitchYaw(&mcfg, &roll, 0, &yaw);
    TEST_ASSERT_EQUAL(1, status);

    status = ns_get_RollPitchYaw(&mcfg, &roll, &pitch, 0);
    TEST_ASSERT_EQUAL(1, status);

}

void MahonyUpdateTest_InvalidAPIVersion() {
    const ns_core_api_t ns_mahony_invalid_version = {
        .apiId = NS_MAHONY_API_ID, .version = { .major = 0, .minor = 0, .revision = 5 }
    };
    // set the API version to an invalid version
    mcfg.api = &ns_mahony_invalid_version;
    // call the Mahony init function
    int status = ns_mahony_init(&mcfg);
    // check if the function returns an error
    TEST_ASSERT_EQUAL(2, status);
}