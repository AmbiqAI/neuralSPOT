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
"""pw_cli"""

import setuptools  # type: ignore

setuptools.setup(
    name='pw_cli',
    version='0.0.1',
    author='Pigweed Authors',
    author_email='pigweed-developers@googlegroups.com',
    description='Pigweed swiss-army knife',
    packages=setuptools.find_packages(),
    package_data={'pw_cli': ['py.typed']},
    zip_safe=False,
    entry_points={'console_scripts': ['pw = pw_cli.__main__:main']},
)
