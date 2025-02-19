# Concepts
The ini file specifies which files contain tests, which tests to run in those tests, and which tests can be run sequentially without a reset (which is effectively a recompile).

```ini
[test suite name]
test_file = boo.c
test_list = foo boo choo

[test suite name]
test_file = foo.c # can be different or same as above
test_list = a b c
```

There will be ini files for smoke, full regression, slow tests, etc.


## Quick Theory of Operations
The ns_perf test framework does the following:
1. Creates a testrunner binary based on a config.ini file, as specified above (puts it in projects/test by default)
2. Flashes the binary
3. Monitors the SWO output
4. Looks for errors/passes/fails/ etc and prints those out to the console.

## Example ini
```ini
[mahoney_update_value_tests]
test_file = mahoney_update_tests
test_list = MahonyUpdateTest_AllZeroAccelValues MahonyUpdateTest_LargeAccelGyroValues

[mahoney_update_error_tests]
test_file = mahoney_update_tests
test_list = MahonyUpdateTest_NegativeInputs MahonyUpdateTest_NullPointer
```

## Example Unity-based test
```c
// test with negative acceleration and gyro values
void MahonyUpdateTest_NegativeInputs() {
    // Set sample negative accel and gyro values
    float gx = -0.1f, gy = -0.2f, gz = -0.3f;
    float ax = -0.4f, ay = -0.5f, az = -0.6f;
    // call Mahony update function
    mpu6050_mahonyUpdate(gx, gy, gz, ax, ay, az);

    // check if the quaternion is updated
    TEST_ASSERT_NOT_EQUAL(q0, 1.0f);
    TEST_ASSERT_NOT_EQUAL(q1, 0.0f);
    TEST_ASSERT_NOT_EQUAL(q2, 0.0f);
    TEST_ASSERT_NOT_EQUAL(q3, 0.0f);
}
```

### How to invoke
`python -m ns_test --platform apollo4p_blue_kxr_evb`

#### CLI Parameters
```optional arguments:
  --seed SEED           Random Seed (default: 42)
  --configfile CONFIGFILE
                        Test configuration file ini format (default: ../neuralspot/ns-features/tests/mahoney_full_regression.ini)
  --test-directory TEST_DIRECTORY
                        Test directory (default: ../neuralspot/ns-features/tests)
  --working-directory WORKING_DIRECTORY
                        Directory where generated test artifacts will be placed (default: projects/tests)
  --platform PLATFORM   Platform to run tests on (default: apollo4p_evb)
  --verbosity VERBOSITY
                        Verbosity level (0-4) (default: 1)

help:
  -h, --help            show this help message and exit
  ```

### Example output
```txt
*** Generate Tests for EVB based on ../neuralspot/ns-features/tests
*** Run Tests on EVB
['mahoney_update_value_tests', 'mahoney_update_error_tests']
Running test suite:  mahoney_update_value_tests
CPU SPEED:  95409752
SWO SPEED:  1000000
Serial Wire Viewer
Press Ctrl-C to Exit
Reading data from port 0:

Starting MahonyUpdateTest_AllZeroAccelValues
mahoney_update_tests_mahoney_update_value_tests_main:60:MahonyUpdateTest_AllZeroAccelValues:FAIL: Expected 0.00082 Was 0.0
mahoney_update_tests_mahoney_update_value_tests_main:34:MahonyUpdateTest_LargeAccelGyroValues:PASS

-----------------------
2 Tests 1 Failures 0 Ignored
FAIL
Running test suite:  mahoney_update_error_tests
mahoney_update_tests_mahoney_update_error_tests_main:33:MahonyUpdateTest_NegativeInputs:PASS
mahoney_update_tests_mahoney_update_error_tests_main:158:MahonyUpdateTest_NullPointer:FAIL: Expected 0.0 Was 0.707

-----------------------
2 Tests 1 Failures 0 Ignored
FAIL
Ran 2 with total of 4 tests, 2 failures, 0 errors
```
