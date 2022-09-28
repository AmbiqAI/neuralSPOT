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
"""stm32f429i_disc1_utils"""

import setuptools  # type: ignore

setuptools.setup(
    name='stm32f429i_disc1_utils',
    version='0.0.1',
    author='Pigweed Authors',
    author_email='pigweed-developers@googlegroups.com',
    description=
    'Target-specific python scripts for the stm32f429i-disc1 target',
    packages=setuptools.find_packages(),
    package_data={'stm32f429i_disc1_utils': ['py.typed']},
    zip_safe=False,
    entry_points={
        'console_scripts': [
            'stm32f429i_disc1_unit_test_runner = '
            '    stm32f429i_disc1_utils.unit_test_runner:main',
            'stm32f429i_disc1_detector = '
            '    stm32f429i_disc1_utils.stm32f429i_detector:main',
            'stm32f429i_disc1_test_server = '
            '    stm32f429i_disc1_utils.unit_test_server:main',
            'stm32f429i_disc1_test_client = '
            '    stm32f429i_disc1_utils.unit_test_client:main',
        ]
    },
    install_requires=['pyserial', 'coloredlogs'],
)
