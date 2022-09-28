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
"""The env module defines the environment variables used by Pigweed."""

from typing import Optional

from pw_cli import envparse


def pigweed_environment_parser() -> envparse.EnvironmentParser:
    """Defines Pigweed's environment variables on an EnvironmentParser."""
    parser = envparse.EnvironmentParser(prefix='PW_')

    parser.add_var('PW_BOOTSTRAP_PYTHON')
    parser.add_var('PW_ENABLE_PRESUBMIT_HOOK_WARNING', default=False)
    parser.add_var('PW_EMOJI', type=envparse.strict_bool, default=False)
    parser.add_var('PW_ENVSETUP')
    parser.add_var('PW_ENVSETUP_FULL')
    parser.add_var('PW_ENVSETUP_NO_BANNER',
                   type=envparse.strict_bool,
                   default=False)
    parser.add_var('PW_ENVSETUP_QUIET',
                   type=envparse.strict_bool,
                   default=False)
    parser.add_var('PW_ENVIRONMENT_ROOT')
    parser.add_var('PW_PROJECT_ROOT')
    parser.add_var('PW_ROOT')
    parser.add_var('PW_SKIP_BOOTSTRAP')
    parser.add_var('PW_SUBPROCESS', type=envparse.strict_bool, default=False)
    parser.add_var('PW_USE_COLOR', type=envparse.strict_bool, default=False)
    parser.add_var('PW_USE_GCS_ENVSETUP', type=envparse.strict_bool)

    parser.add_allowed_suffix('_CIPD_INSTALL_DIR')

    parser.add_var('PW_DOCTOR_SKIP_CIPD_CHECKS')

    # TODO(pwbug/274) Remove after some transition time. These are no longer
    # used but may be set by users or downstream projects, or just in currently
    # active shells.
    parser.add_var('PW_CIPD_PACKAGE_FILES')
    parser.add_var('PW_VIRTUALENV_REQUIREMENTS')
    parser.add_var('PW_VIRTUALENV_REQUIREMENTS_APPEND_DEFAULT')
    parser.add_var('PW_VIRTUALENV_SETUP_PY_ROOTS')
    parser.add_var('PW_CARGO_PACKAGE_FILES')
    parser.add_var('PW_CARGO_SETUP', type=envparse.strict_bool, default=False)
    parser.add_var('PW_VIRTUALENV_REQUIREMENTS_APPEND_DEFAULT')

    parser.add_var('PW_BANNER_FUNC')
    parser.add_var('PW_BRANDING_BANNER')
    parser.add_var('PW_BRANDING_BANNER_COLOR', default='magenta')

    return parser


# Internal: memoize environment parsing to avoid unnecessary computation in
# multiple calls to pigweed_environment().
_memoized_environment: Optional[envparse.EnvNamespace] = None


def pigweed_environment() -> envparse.EnvNamespace:
    """Returns Pigweed's parsed environment."""
    global _memoized_environment  # pylint: disable=global-statement

    if _memoized_environment is None:
        _memoized_environment = pigweed_environment_parser().parse_env()

    return _memoized_environment
