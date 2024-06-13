import configparser
import logging as log
import os
import shutil

# import numpy as np
from ns_utils import createFromTemplate, xxd_c_dump


# Generate test enclosures for target platform
def generateTestRunner(params, test_suite_name, test_config):
    platform = params.platform
    test_directory = params.test_directory
    working_directory = params.working_directory

    # Get lists of test belonging to the test suite
    test_file_name = test_config["test_file_name"]
    test_list = test_config["test_list"]

    # Convert test_list string to list
    test_list = test_list.split(" ")

    # Make destination directory
    os.makedirs(working_directory, exist_ok=True)

    # Generate test enclosures from templates
    rm = {
        "NS_AT_CONTAINER": test_file_name,
        "NS_AT_NAME": test_suite_name,
        "NS_AT_RUN_TEST_LIST": test_list,
    }

    createFromTemplate(
        f"autotest/templates/template_test_main.c",
        f"{working_directory}/{test_file_name}_{test_suite_name}_test_main.c",
        rm,
    )

    # Create module.mk file
    createFromTemplate(
        f"autotest/templates/template_module.mk", f"{working_directory}/module.mk", rm
    )

    # Copy in unity directory
    shutil.copytree(f"autotest/unity", f"{working_directory}/unity", dirs_exist_ok=True)

    # Copy in test files
    shutil.copy(f"{test_directory}/{test_file_name}.*", f"{working_directory}/")

    # Compile test enclosures

    # Windows sucks
    if os.name == "posix":
        ws_null = "/dev/null"
        ws_j = "-j"
        ws_and = "&&"
        ws_p = "-p"
    else:
        ws_null = "NUL"
        ws_j = ""
        ws_and = "&"
        ws_p = ""

    if params.verbosity > 3:
        makefile_result = os.system(f"cd {d}/{n} {ws_and} make {ws_j}")
    else:
        makefile_result = os.system(f"cd {d}/{n} {ws_and} make {ws_j} >{ws_null} 2>&1")

    if makefile_result != 0:
        log.error("Makefile failed to build minimal example library")
        exit("Makefile failed to build minimal example library")


def runTests(params, test_config):
    platform = params.platform
    working_directory = params.working_directory

    # Get list of test suites, create bin for each one via generateTestRunner
    test_suite_list = test_config.sections()

    for test_suite_name in test_suite_list:
        # Generate test enclosures for target platform
        generateTestRunner(params, test_suite_name, test_config)

        # flash and run tests
        if params.verbosity > 2:
            log.info(f"Running test suite {test_suite_name}")
        else:
            log.debug(f"Running test suite {test_suite_name}")

    # Get lists of test belonging to the test suite
    test_file_name = test_config["test_file_name"]
    test_list = test_config["test_list"]

    # Convert test_list string to list
    test_list = test_list.split(" ")

    # Run tests
    for test in test_list:
        if params.verbosity > 2:
            log.info(f"Running test {test}")
        else:
            log.debug(f"Running test {test}")

        # Windows sucks
        if os.name == "posix":
            ws_null = "/dev/null"
            ws_and = "&&"
        else:
            ws_null = "NUL"
            ws_and = "&"

        if params.verbosity > 3:
            test_result = os.system(f"cd {working_directory} {ws_and} ./test_{test}")
        else:
            test_result = os.system(
                f"cd {working_directory} {ws_and} ./test_{test} >{ws_null} 2>&1"
            )

        if test_result != 0:
            log.error(f"Test {test} failed")
            exit(f"Test {test} failed")

    log.info("All tests passed")
    return 0
