# Copyright 2019 The Pigweed Authors
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
"""pw_arduino_build"""

import setuptools  # type: ignore

setuptools.setup(
    name='pw_arduino_build',
    version='0.0.1',
    author='Pigweed Authors',
    author_email='pigweed-developers@googlegroups.com',
    description='Target-specific python scripts for the arduino target',
    packages=setuptools.find_packages(),
    package_data={'pw_arduino_build': ['py.typed']},
    zip_safe=False,
    entry_points={
        'console_scripts': [
            'arduino_builder = pw_arduino_build.__main__:main',
            'teensy_detector = pw_arduino_build.teensy_detector:main',
            'arduino_unit_test_runner = '
            '    pw_arduino_build.unit_test_runner:main',
            'arduino_test_server = '
            '    pw_arduino_build.unit_test_server:main',
            'arduino_test_client = '
            '    pw_arduino_build.unit_test_client:main',
        ]
    },
    install_requires=[
        'pyserial',
        'coloredlogs',
        'parameterized',
    ])
