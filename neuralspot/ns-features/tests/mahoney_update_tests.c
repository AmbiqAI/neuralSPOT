#include "quaternion.h"
#include "unity/unity.h"

static ns_mahony_cfg_t mcfg;

void mahoney_update_tests_pre_test_hook() {
    // initialize quaternion values

    ns_mahony_init(&mcfg);
}

void mahoney_update_tests_post_test_hook() {
    // clean up after the tests if needed
}

void MahonyUpdateTest_AllZeroAccelValues() {
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
    // TEST_ASSERT_FLOAT_WITHIN(0.001, 1.0, mcfg.q0);
    // TEST_ASSERT_FLOAT_WITHIN(0.002, 0.0f, mcfg.q1);
    // TEST_ASSERT_FLOAT_WITHIN(0.002, 0.0f, mcfg.q2);
    // TEST_ASSERT_FLOAT_WITHIN(0.002, 0.0f, mcfg.q3);
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

    // float gx = 0.1f, gy = 0.2f, gz = 0.3f;
    float gx = 0.0f, gy = 0.0f, gz = 0.0f;
    // set all accelerometer values to zero
    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    int status = ns_mahony_update(0, gx, gy, gz, ax, ay, az);
    // call the function to update mahony filter with a NULL pointer
    TEST_ASSERT_EQUAL(1, status);

    // set sample quaternion values
    mcfg.q0 = 0.707f;
    mcfg.q1 = 0.0f;
    mcfg.q2 = 0.0f;
    mcfg.q3 = 0.707f;


    double qw, qy, qz;
    // call the function to get quaternion values with a NULL pointer
    status = ns_get_quaternion(&mcfg, &qw, 0, &qy, &qz);
    TEST_ASSERT_EQUAL(1, status);
}
