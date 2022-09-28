#!/usr/bin/env python3
# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Arduino Core Installer."""

import argparse
import logging
import operator
import os
import platform
import shutil
import stat
import subprocess
import sys
import time
from typing import Dict, List

import pw_arduino_build.file_operations as file_operations

_LOG = logging.getLogger(__name__)


class ArduinoCoreNotSupported(Exception):
    """Exception raised when a given core can not be installed."""


# yapf: disable
_ARDUINO_CORE_ARTIFACTS: Dict[str, Dict] = {
    # pylint: disable=line-too-long
    "teensy": {
        "Linux": {
            "arduino-ide": {
                "url": "https://downloads.arduino.cc/arduino-1.8.13-linux64.tar.xz",
                "file_name": "arduino-1.8.13-linux64.tar.xz",
                "sha256": "1b20d0ec850a2a63488009518725f058668bb6cb48c321f82dcf47dc4299b4ad",
            },
            "teensyduino": {
                "url": "https://www.pjrc.com/teensy/td_154-beta4/TeensyduinoInstall.linux64",
                "file_name": "TeensyduinoInstall.linux64",
                "sha256": "76c58babb7253b65a33d73d53f3f239c2e2ccf8602c771d69300a67d82723730",
            },
        },
        # TODO(tonymd): Handle 32-bit Linux Install?
        "Linux32": {
            "arduino-ide": {
                "url": "https://downloads.arduino.cc/arduino-1.8.13-linux32.tar.xz",
                "file_name": "arduino-1.8.13-linux32.tar.xz",
                "sha256": "",
            },
            "teensyduino": {
                "url": "https://www.pjrc.com/teensy/td_153/TeensyduinoInstall.linux32",
                "file_name": "TeensyduinoInstall.linux32",
                "sha256": "",
            },
        },
        # TODO(tonymd): Handle ARM32 (Raspberry Pi) Install?
        "LinuxARM32": {
            "arduino-ide": {
                "url": "https://downloads.arduino.cc/arduino-1.8.13-linuxarm.tar.xz",
                "file_name": "arduino-1.8.13-linuxarm.tar.xz",
                "sha256": "",
            },
            "teensyduino": {
                "url": "https://www.pjrc.com/teensy/td_153/TeensyduinoInstall.linuxarm",
                "file_name": "TeensyduinoInstall.linuxarm",
                "sha256": "",
            },
        },
        # TODO(tonymd): Handle ARM64 Install?
        "LinuxARM64": {
            "arduino-ide": {
                "url": "https://downloads.arduino.cc/arduino-1.8.13-linuxaarch64.tar.xz",
                "file_name": "arduino-1.8.13-linuxaarch64.tar.xz",
                "sha256": "",
            },
            "teensyduino": {
                "url": "https://www.pjrc.com/teensy/td_153/TeensyduinoInstall.linuxaarch64",
                "file_name": "TeensyduinoInstall.linuxaarch64",
                "sha256": "",
            },
        },
        "Darwin": {
            "teensyduino": {
                "url": "https://www.pjrc.com/teensy/td_154-beta4/Teensyduino_MacOS_Catalina.zip",
                "file_name": "Teensyduino_MacOS_Catalina.zip",
                "sha256": "7ca579c12d8f3a8949dbeec812b8dbef13242d575baa707dc7f02bc452c1f4a1",
            },
        },
        "Windows": {
            "arduino-ide": {
                "url": "https://downloads.arduino.cc/arduino-1.8.13-windows.zip",
                "file_name": "arduino-1.8.13-windows.zip",
                "sha256": "78d3e96827b9e9b31b43e516e601c38d670d29f12483e88cbf6d91a0f89ef524",
            },
            "teensyduino": {
                "url": "https://www.pjrc.com/teensy/td_154-beta4/TeensyduinoInstall.exe",
                "file_name": "TeensyduinoInstall.exe",
                "sha256": "f7bcc2ed45e10a5d7b003bedabcde12fb1b8cf7ef9081e2503cd668569642a90",
            },
        }
    },
    "adafruit-samd": {
        "all": {
            "core": {
                "version": "1.6.2",
                "url": "https://github.com/adafruit/ArduinoCore-samd/archive/1.6.2.tar.gz",
                "sha256": "5875f5bc05904c10e6313f02653f28f2f716db639d3d43f5a1d8a83d15339d64",
            }
        },
        "Linux": {},
        "Darwin": {},
        "Windows": {},
    },
    "arduino-samd": {
        "all": {
            "core": {
                "version": "1.8.8",
                "url": "http://downloads.arduino.cc/cores/samd-1.8.8.tar.bz2",
                "file_name": "samd-1.8.8.tar.bz2",
                "sha256": "7b93eb705cba9125d9ee52eba09b51fb5fe34520ada351508f4253abbc9f27fa",
            }
        },
        "Linux": {},
        "Darwin": {},
        "Windows": {},
    },
    "stm32duino": {
        "all": {
            "core": {
                "version": "1.9.0",
                "url": "https://github.com/stm32duino/Arduino_Core_STM32/archive/1.9.0.tar.gz",
                "sha256": "4f75ba7a117d90392e8f67c58d31d22393749b9cdd3279bc21e7261ec06c62bf",
            }
        },
        "Linux": {},
        "Darwin": {},
        "Windows": {},
    },
}
# yapf: enable


def install_core_command(args: argparse.Namespace):
    install_prefix = os.path.realpath(
        os.path.expanduser(os.path.expandvars(args.prefix)))
    install_dir = os.path.join(install_prefix, args.core_name)
    cache_dir = os.path.join(install_prefix, ".cache", args.core_name)

    if args.core_name in supported_cores():
        shutil.rmtree(install_dir, ignore_errors=True)
        os.makedirs(install_dir, exist_ok=True)
        os.makedirs(cache_dir, exist_ok=True)

    if args.core_name == "teensy":
        if platform.system() == "Linux":
            install_teensy_core_linux(install_prefix, install_dir, cache_dir)
        elif platform.system() == "Darwin":
            install_teensy_core_mac(install_prefix, install_dir, cache_dir)
        elif platform.system() == "Windows":
            install_teensy_core_windows(install_prefix, install_dir, cache_dir)
    elif args.core_name == "adafruit-samd":
        install_adafruit_samd_core(install_prefix, install_dir, cache_dir)
    elif args.core_name == "stm32duino":
        install_stm32duino_core(install_prefix, install_dir, cache_dir)
    elif args.core_name == "arduino-samd":
        install_arduino_samd_core(install_prefix, install_dir, cache_dir)
    else:
        raise ArduinoCoreNotSupported(
            "Invalid core '{}'. Supported cores: {}".format(
                args.core_name, ", ".join(supported_cores())))


def supported_cores():
    return _ARDUINO_CORE_ARTIFACTS.keys()


def get_windows_process_names() -> List[str]:
    result = subprocess.run("wmic process get description",
                            capture_output=True)
    output = result.stdout.decode().splitlines()
    return [line.strip() for line in output if line]


def install_teensy_core_windows(install_prefix, install_dir, cache_dir):
    """Download and install Teensyduino artifacts for Windows."""
    teensy_artifacts = _ARDUINO_CORE_ARTIFACTS["teensy"][platform.system()]

    arduino_artifact = teensy_artifacts["arduino-ide"]
    arduino_zipfile = file_operations.download_to_cache(
        url=arduino_artifact["url"],
        expected_sha256sum=arduino_artifact["sha256"],
        cache_directory=cache_dir)

    teensyduino_artifact = teensy_artifacts["teensyduino"]
    teensyduino_installer = file_operations.download_to_cache(
        url=teensyduino_artifact["url"],
        expected_sha256sum=teensyduino_artifact["sha256"],
        cache_directory=cache_dir)

    file_operations.extract_archive(arduino_zipfile, install_dir, cache_dir)

    # "teensy" here should match args.core_name
    teensy_core_dir = os.path.join(install_prefix, "teensy")

    # Change working directory for installation
    original_working_dir = os.getcwd()
    os.chdir(install_prefix)

    install_command = [teensyduino_installer, "--dir=teensy"]
    _LOG.info("  Running: %s", " ".join(install_command))
    _LOG.info("    Please click yes on the Windows 'User Account Control' "
              "dialog.")
    _LOG.info("    You should see: 'Verified publisher: PRJC.COM LLC'")

    def wait_for_process(process_name,
                         timeout=30,
                         result_operator=operator.truth):
        start_time = time.time()
        while result_operator(process_name in get_windows_process_names()):
            time.sleep(1)
            if time.time() > start_time + timeout:
                _LOG.error(
                    "Error: Installation Failed.\n"
                    "Please click yes on the Windows 'User Account Control' "
                    "dialog.")
                sys.exit(1)

    # Run Teensyduino installer with admin rights (non-blocking)
    # User Account Control (UAC) will prompt the user for consent
    import ctypes  # pylint: disable=import-outside-toplevel
    ctypes.windll.shell32.ShellExecuteW(
        None,  # parent window handle
        "runas",  # operation
        teensyduino_installer,  # file to run
        subprocess.list2cmdline(install_command),  # command parameters
        install_prefix,  # working directory
        1)  # Display mode (SW_SHOWNORMAL: Activates and displays a window)

    # Wait for teensyduino_installer to start running
    wait_for_process("TeensyduinoInstall.exe", result_operator=operator.not_)

    _LOG.info("Waiting for TeensyduinoInstall.exe to finish.")
    # Wait till teensyduino_installer is finished
    wait_for_process("TeensyduinoInstall.exe", timeout=360)

    if not os.path.exists(os.path.join(teensy_core_dir, "hardware", "teensy")):
        _LOG.error(
            "Error: Installation Failed.\n"
            "Please try again and ensure Teensyduino is installed in "
            "the folder:\n"
            "%s", teensy_core_dir)
        sys.exit(1)
    else:
        _LOG.info("Install complete!")

    file_operations.remove_empty_directories(install_dir)
    os.chdir(original_working_dir)


def install_teensy_core_mac(unused_install_prefix, install_dir, cache_dir):
    """Download and install Teensyduino artifacts for Mac."""
    teensy_artifacts = _ARDUINO_CORE_ARTIFACTS["teensy"][platform.system()]

    teensyduino_artifact = teensy_artifacts["teensyduino"]
    teensyduino_zip = file_operations.download_to_cache(
        url=teensyduino_artifact["url"],
        expected_sha256sum=teensyduino_artifact["sha256"],
        cache_directory=cache_dir)

    extracted_files = file_operations.extract_archive(
        teensyduino_zip,
        install_dir,
        cache_dir,
        remove_single_toplevel_folder=False)
    toplevel_folder = sorted(extracted_files)[0]
    os.symlink(os.path.join(toplevel_folder, "Contents", "Java", "hardware"),
               os.path.join(install_dir, "hardware"),
               target_is_directory=True)


def install_teensy_core_linux(install_prefix, install_dir, cache_dir):
    """Download and install Teensyduino artifacts for Windows."""
    teensy_artifacts = _ARDUINO_CORE_ARTIFACTS["teensy"][platform.system()]

    arduino_artifact = teensy_artifacts["arduino-ide"]
    arduino_tarfile = file_operations.download_to_cache(
        url=arduino_artifact["url"],
        expected_sha256sum=arduino_artifact["sha256"],
        cache_directory=cache_dir)

    teensyduino_artifact = teensy_artifacts["teensyduino"]
    teensyduino_installer = file_operations.download_to_cache(
        url=teensyduino_artifact["url"],
        expected_sha256sum=teensyduino_artifact["sha256"],
        cache_directory=cache_dir)

    file_operations.extract_archive(arduino_tarfile, install_dir, cache_dir)
    os.chmod(teensyduino_installer,
             os.stat(teensyduino_installer).st_mode | stat.S_IEXEC)

    original_working_dir = os.getcwd()
    os.chdir(install_prefix)
    # "teensy" here should match args.core_name
    install_command = [teensyduino_installer, "--dir=teensy"]
    subprocess.run(install_command)

    file_operations.remove_empty_directories(install_dir)
    os.chdir(original_working_dir)


def install_arduino_samd_core(install_prefix: str, install_dir: str,
                              cache_dir: str):
    artifacts = _ARDUINO_CORE_ARTIFACTS["arduino-samd"]["all"]["core"]
    core_tarfile = file_operations.download_to_cache(
        url=artifacts["url"],
        expected_sha256sum=artifacts["sha256"],
        cache_directory=cache_dir)

    package_path = os.path.join(install_dir, "hardware", "samd",
                                artifacts["version"])
    os.makedirs(package_path, exist_ok=True)
    file_operations.extract_archive(core_tarfile, package_path, cache_dir)
    original_working_dir = os.getcwd()
    os.chdir(install_prefix)
    # TODO(tonymd): Fetch core/tools as specified by:
    # http://downloads.arduino.cc/packages/package_index.json
    os.chdir(original_working_dir)
    return True


def install_adafruit_samd_core(install_prefix: str, install_dir: str,
                               cache_dir: str):
    artifacts = _ARDUINO_CORE_ARTIFACTS["adafruit-samd"]["all"]["core"]
    core_tarfile = file_operations.download_to_cache(
        url=artifacts["url"],
        expected_sha256sum=artifacts["sha256"],
        cache_directory=cache_dir)

    package_path = os.path.join(install_dir, "hardware", "samd",
                                artifacts["version"])
    os.makedirs(package_path, exist_ok=True)
    file_operations.extract_archive(core_tarfile, package_path, cache_dir)

    original_working_dir = os.getcwd()
    os.chdir(install_prefix)
    # TODO(tonymd): Fetch platform specific tools as specified by:
    # https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
    # Specifically:
    #   https://github.com/ARM-software/CMSIS_5/archive/5.4.0.tar.gz
    os.chdir(original_working_dir)
    return True


def install_stm32duino_core(install_prefix, install_dir, cache_dir):
    artifacts = _ARDUINO_CORE_ARTIFACTS["stm32duino"]["all"]["core"]
    core_tarfile = file_operations.download_to_cache(
        url=artifacts["url"],
        expected_sha256sum=artifacts["sha256"],
        cache_directory=cache_dir)

    package_path = os.path.join(install_dir, "hardware", "stm32",
                                artifacts["version"])
    os.makedirs(package_path, exist_ok=True)
    file_operations.extract_archive(core_tarfile, package_path, cache_dir)
    original_working_dir = os.getcwd()
    os.chdir(install_prefix)
    # TODO(tonymd): Fetch platform specific tools as specified by:
    # https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json
    os.chdir(original_working_dir)
    return True
