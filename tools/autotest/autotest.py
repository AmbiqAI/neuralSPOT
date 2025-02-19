import configparser
import logging as log
import os
import queue
import re
import shutil
import sys
import threading
import time
from string import Template

import pylink

# import numpy as np
from ns_utils import createFromTemplate, xxd_c_dump
from pylink import JLink

exit_flag = threading.Event()

# Generate test enclosures for target platform
def generateTestRunner(params, test_suite_name, tc):
    platform = params.platform
    test_directory = params.test_directory
    # working_directory = params.working_directory
    d = params.working_directory
    test_root = d
    d = f"../{d}"
    test_config = tc[test_suite_name]
    print("Running test suite: ", test_suite_name)
    # print(test_config)
    # print(d)
    os.makedirs(d, exist_ok=True)

    # Get lists of test belonging to the test suite
    test_file_name = test_config["test_file"]
    test_list = test_config["test_list"]
    n = f"{test_file_name}_{test_suite_name}_main"
    d = f"{d}/{n}"

    # Convert test_list string to list
    test_list = test_list.split(" ")

    # turn test_list into a list of strings
    test_list_string = ""
    for test in test_list:
        test_list_string += f"RUN_TEST({test});\n"

    # Make destination directory
    os.makedirs(d, exist_ok=True)

    # Generate test enclosures from templates
    rm = {
        "NS_AT_CONTAINER": test_file_name,
        "NS_AT_NAME": test_file_name,
        "NS_AT_MAIN": f"{n}",
        "NS_AT_RUN_TEST_LIST": test_list_string,
    }

    createFromTemplate(
        f"autotest/templates/template_test_main.c",
        f"{d}/{n}.c",
        rm,
    )

    # Create module.mk file
    createFromTemplate(f"autotest/templates/template_module.mk", f"{d}/module.mk", rm)

    # Copy in unity directory
    shutil.copytree(f"autotest/unity", f"{d}/unity", dirs_exist_ok=True)

    # Copy in test files with a wildcard

    shutil.copy2(f"{test_directory}/{test_file_name}.c", f"{d}/")
    shutil.copy2(f"{test_directory}/{test_file_name}.h", f"{d}/")

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
        print(
            f"cd .. {ws_and} make {ws_j} PLATFORM={platform} AUTODEPLOY=1 ADPATH={test_root} EXAMPLE={n} {ws_and} make PLATFORM={platform} AUTODEPLOY=1 ADPATH={test_root} TARGET={n} EXAMPLE={n} deploy"
        )
        makefile_result = os.system(
            f"cd .. {ws_and} make {ws_j} PLATFORM={platform} AUTODEPLOY=1 ADPATH={test_root} EXAMPLE={n} {ws_and} make PLATFORM={platform} AUTODEPLOY=1 ADPATH={test_root} TARGET={n} EXAMPLE={n} deploy"
        )
    else:
        makefile_result = os.system(
            f"cd .. {ws_and} make {ws_j} PLATFORM={platform} AUTODEPLOY=1 ADPATH={test_root} EXAMPLE={n} >{ws_null} 2>&1 {ws_and} make PLATFORM={platform} AUTODEPLOY=1 ADPATH={test_root} EXAMPLE={n} TARGET={n} deploy >{ws_null} 2>&1"
        )

    if makefile_result != 0:
        log.error("Makefile failed to build testrunner")
        exit_flag.set()
        exit("Makefile failed to build testrunner")
    else:
        return d, len(test_list)


def serial_wire_viewer(jlink, jlink_serial, part, swo_queue):
    # jlink.set_log_file('./output.txt')
    jlink.coresight_configure()
    if "apollo5" not in part:
        jlink.set_reset_strategy(pylink.enums.JLinkResetStrategyCortexM3.RESETPIN)

    # Halt the CPU before reading its clock speed
    jlink.reset()
    jlink.halt()
    cpu_speed = jlink.cpu_speed()
    swo_speed = 1000000
    print("CPU SPEED: ", cpu_speed)
    print("SWO SPEED: ", swo_speed)

    jlink.swo_enable(cpu_speed, swo_speed, 0x01)
    jlink.swo_flush()

    sys.stdout.write("Serial Wire Viewer\n")
    sys.stdout.write("Press Ctrl-C to Exit\n")
    sys.stdout.write("Reading data from port 0:\n\n")

    jlink.reset()

    # Use the `try` loop to catch a keyboard interrupt in order to stop logging
    # serial wire output.

    while not exit_flag.is_set():
        # Check for any bytes in the stream.
        num_bytes = jlink.swo_num_bytes()

        if num_bytes == 0:
            # If no bytes exist, sleep for a bit before trying again.
            # jlink.reset()
            # print(".", end='')
            time.sleep(1)
            continue

        # Returns readable characters in the form of a list of bytes
        data = jlink.swo_read_stimulus(0, num_bytes)

        # # Send bytes individually to the worker thread
        for byte in data:
            swo_queue.put([byte])

        sys.stdout.flush()
    sys.stdout.write("\n")

    # Stop logging serial wire output.
    jlink.swo_stop()

    return 0


main_xml = Template(
    """\
<?xml version="1.0" encoding="UTF-8"?>
<testsuites disabled="" errors="${total_errors}" failures="${total_failures}" name="">
${test_suites}
</testsuites>
"""
)

test_suite_xml = Template(
    """\
<testsuite name="${name}" tests="${num_tests}" errors="${num_errors}" failures="${num_failures}" time="${duration}">
${test_cases}
  <system-out>${swo_output}</system-out>
</testsuite>
"""
)

test_case_xml = Template(
    """\
<testcase name="${name}" classname="" status="${status}">${errors}</testcase>
"""
)

timeout_xml = Template(
    """\
<testcase name="${name}" classname="" status="TIMEOUT">
  <error message="Test timed out." type="Timeout"></error>
</testcase>
"""
)

binary_missing_xml = Template(
    """\
<testcase name="${name}" classname="" status="BINARY MISSING">
  <error message="Binary was not found." type="BinaryMissing"></error>
</testcase>
"""
)

failure_xml = """
<failure message="Unity assert failed." type="FailedAssert"></failure>
"""


class TestResults:
    def __init__(self, timeout=False, binary_missing=False, test_name=None):
        self.test_name = test_name
        self.timeout = timeout
        self.binary_missing = binary_missing
        self.results = []
        self.swo_output = ""
        self.duration = 0

    def add_swo(self, swo_output):
        with open(swo_output, "r") as f:
            lines = f.read()
            self.swo_output = lines

    def add_result(self, test_name, status):
        self.results.append((test_name, status))

    def get_errors(self):
        if self.binary_missing:
            return 1
        elif self.timeout:
            return 1
        else:
            return 0

    def get_failures(self):
        if self.binary_missing:
            return 0
        else:
            failures = 0
            for x in self.results:
                if x[1] != "PASS":
                    failures = failures + 1
            return failures

    def get_main_status(self):
        if self.binary_missing:
            return "BINARY MISSING"
        elif self.timeout:
            return "TIMEOUT"
        else:
            for x in self.results:
                if x[1] != "PASS":
                    return "FAIL"

        return "PASS"

    def to_xml(self):
        errors = 0
        failures = 0
        tests = 0

        test_suites_map = dict()
        test_suites_map["name"] = self.test_name
        test_suites_map["swo_output"] = self.swo_output

        test_case_strings = []

        for test_case in self.results:
            tests = tests + 1

            test_xml_map = dict()
            test_xml_map["name"] = test_case[0]
            test_xml_map["status"] = test_case[1]

            if test_case[1] == "PASS" or test_case[1] == "IGNORE":
                test_xml_map["errors"] = ""
            else:
                test_xml_map["errors"] = textwrap.indent(failure_xml, "  ")
                failures = failures + 1

            S = test_case_xml.substitute(**test_xml_map)
            test_case_strings.append(S)

        if self.timeout:
            S = timeout_xml.substitute(name=self.test_name)
            test_case_strings.append(S)
            errors = errors + 1

        if self.binary_missing:
            errors = errors + 1
            test_suites_map["test_cases"] = binary_missing_xml.substitute(
                name=self.test_name
            )
            test_suites_map["num_failures"] = failures
            test_suites_map["num_errors"] = errors
            test_suites_map["num_tests"] = tests
            test_suites_map["duration"] = self.duration
        else:
            test_cases = "".join(test_case_strings)
            test_suites_map["test_cases"] = textwrap.indent(test_cases, "  ")
            test_suites_map["num_failures"] = failures
            test_suites_map["num_errors"] = errors
            test_suites_map["num_tests"] = tests
            test_suites_map["duration"] = self.duration

        return test_suite_xml.substitute(**test_suites_map)


def save_swo_output(swo_queue, timeout, logfile=None):
    # global isLocked
    start_detected = False
    end_detected = False

    # Open a file for SWO data.
    with open(logfile, "w") as f:
        timeout_time = time.time() + timeout

        while time.time() < timeout_time and not end_detected:
            # print("Waiting for data time remaining: ", timeout_time - time.time(), " seconds")
            try:
                data_list = swo_queue.get()
            except queue.Empty:
                sys.stdout.flush()
                pass

            if exit_flag.is_set():
                return

            # Small state-machine to direct bytes to the correct place.
            if not start_detected:
                if 0x02 in data_list:
                    # print("Start byte", new_bytes)
                    start_detected = True
                    # sys.stdout.write('<TEST START>' + '\n')
            else:
                if 0x03 in data_list:
                    # print("End byte", new_bytes)
                    end_detected = True
                    # sys.stdout.write('<TEST END>' + '\n')
                    break
                elif 0x02 in data_list:
                    # DUPLICATE START BYTES BEING SENT
                    string = "DEADBEEF"
                elif 0x01 in data_list:
                    # Will corrupt XML if not replaced
                    string = "MCU_VALIDATION"
                    sys.stdout.write("WARNING - MCU_VALIDATION FLAG IS SET" + "\n")
                else:
                    try:
                        string = bytes(data_list).decode("utf-8")
                    except UnicodeDecodeError:
                        string = "DEADBEEF"
                    # NEED TO GET DEVS TO NOT INCLUDE AMPERSANDS IN TESTCASE OUTPUT
                    badchar = "&"
                    if badchar in string:
                        string = "aNd"
                    print(string, end="")
                    f.write(string)

                    if b"\n" in data_list:
                        sys.stdout.flush()

    sys.stdout.flush()

    if start_detected is False:
        return False
    elif end_detected is False:
        return False
    else:
        return True


def check_results(filename):
    """Read the SWO log of a test to see if it passed"""

    results_found = False

    pass_expr = re.compile(r"(\d+) Tests (\d+) Failures (\d+) Ignored")
    test_expr = re.compile(r"([^:]*):([^:]*):([^:]*):(.*)")

    results = TestResults()

    with open(filename) as f:
        for line in f:
            results_line = pass_expr.match(line)
            test_line = test_expr.match(line)
            # filename = test_line.groups(0)
            # line_num = test_line.groups(1)
            if test_line:
                testcase_name = test_line.groups()[2]
                test_status = test_line.groups()[3]
                results.add_result(testcase_name, test_status)
            # test_message = test_line.groups(4)

            if results_line:
                results_found = True

    if results_found is False:
        results.timeout = True

    return results


def generateAndFlashTestRunner(params, test_config, swo_queue):
    test_suite_list = test_config.sections()
    errors = 0
    failures = 0
    testcount = 0
    # with open(os.path.join(params.working_directory, 'results-summary.txt'), 'w') as results_file:
    #     results_file.write('Name,Toolchain,Result,Duration,Timeout\n')

    for test_suite_name in test_suite_list:
        # Generate test enclosures for target platform
        d, tests_in_suite = generateTestRunner(params, test_suite_name, test_config)
        swo_log_file = d + f"/{test_suite_name}swo-log.txt"
        testcount = testcount + tests_in_suite

        timeout = 50
        # save results
        start_time = time.time()
        test_complete = save_swo_output(swo_queue, timeout, swo_log_file)
        end_time = time.time()

        duration = end_time - start_time

        test_result = None
        test_result = check_results(swo_log_file)
        test_result.duration = duration
        test_result.add_swo(swo_log_file)

        if test_complete is False:
            test_result.timeout = True

        if test_suite_name is None:
            test_result.test_name = d
        else:
            test_result.test_name = test_suite_name

        # Remove the temporary file if we made one.
        if swo_log_file == "swo-log.tmp":
            os.remove("swo-log.tmp")

        failures = failures + test_result.get_failures()
        errors = errors + test_result.get_errors()
    # test_xml_results.append(test_result.to_xml())
    print(
        f"Ran {len(test_suite_list)} with total of {testcount} tests, {failures} failures, {errors} errors"
    )

    # results_file.write('{},{},{},{}\n'.format(test_name, toolchain, test_result.get_main_status(),
    #                                             test_result.duration, test.getint('timeout')))
    exit_flag.set()


def runTests(params, test_config):
    platform = params.platform
    working_directory = params.working_directory
    # part = "AMAP42KP-KBR" if '4' in params.platform else "AMA3B2KK-KBR"
    if '5' in platform:
        part = 'AP510NFA-CBR'
    elif '4' in platform:
        part = 'AMAP42KP-KBR'
    else:
        part = 'AMA3B2KK-KBR'
    # Get list of test suites, create bin for each one via generateTestRunner
    test_suite_list = test_config.sections()
    print(test_suite_list)
    jlink = pylink.JLink()
    jlink.open()
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect(part, verbose=True)

    swo_queue = queue.Queue()

    try:
        swo_thread = threading.Thread(
            target=serial_wire_viewer, args=(jlink, None, part, swo_queue)
        )
        flash_thread = threading.Thread(
            target=generateAndFlashTestRunner, args=(params, test_config, swo_queue)
        )

        swo_thread.start()
        flash_thread.start()

    except KeyboardInterrupt:
        # Send signal to end all listening threads
        exit_flag.set()
        print("Exiting program due to ctrl-c")

    # Wait for all threads to finish - 5 second timeout
    swo_thread.join(5)
    flash_thread.join(5)

    log.info("All tests passed")
    return 0
