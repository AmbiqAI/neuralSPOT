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
"""Argument parsing code for presubmit checks."""

import argparse
import logging
from pathlib import Path
import re
import shutil
from typing import Callable, Collection, Optional, Sequence

from pw_presubmit import git_repo, presubmit

_LOG = logging.getLogger(__name__)


def add_path_arguments(parser) -> None:
    """Adds common presubmit check options to an argument parser."""

    parser.add_argument(
        'paths',
        metavar='pathspec',
        nargs='*',
        help=('Paths or patterns to which to restrict the checks. These are '
              'interpreted as Git pathspecs. If --base is provided, only '
              'paths changed since that commit are checked.'))
    parser.add_argument(
        '-b',
        '--base',
        metavar='commit',
        help=('Git revision against which to diff for changed files. '
              'If none is provided, the entire repository is used.'))
    parser.add_argument(
        '-e',
        '--exclude',
        metavar='regular_expression',
        default=[],
        action='append',
        type=re.compile,
        help=('Exclude paths matching any of these regular expressions, '
              "which are interpreted relative to each Git repository's root."))


def _add_programs_arguments(exclusive: argparse.ArgumentParser,
                            programs: presubmit.Programs, default: str):
    def presubmit_program(arg: str) -> presubmit.Program:
        if arg not in programs:
            raise argparse.ArgumentTypeError(
                f'{arg} is not the name of a presubmit program')

        return programs[arg]

    exclusive.add_argument('-p',
                           '--program',
                           choices=programs.values(),
                           type=presubmit_program,
                           default=default,
                           help='Which presubmit program to run')

    all_steps = programs.all_steps()

    # The step argument appends steps to a program. No "step" argument is
    # created on the resulting argparse.Namespace.
    class AddToCustomProgram(argparse.Action):
        def __call__(self, parser, namespace, values, unused_option=None):
            if not isinstance(namespace.program, list):
                namespace.program = []

            if values not in all_steps:
                raise parser.error(
                    f'argument --step: {values} is not the name of a '
                    'presubmit check\n\nValid values for --step:\n'
                    f'{{{",".join(sorted(all_steps))}}}')

            namespace.program.append(all_steps[values])

    exclusive.add_argument(
        '--step',
        action=AddToCustomProgram,
        default=argparse.SUPPRESS,  # Don't create a "step" argument.
        help='Provide explicit steps instead of running a predefined program.',
    )


def add_arguments(parser: argparse.ArgumentParser,
                  programs: Optional[presubmit.Programs] = None,
                  default: str = '') -> None:
    """Adds common presubmit check options to an argument parser."""

    add_path_arguments(parser)
    parser.add_argument('-k',
                        '--keep-going',
                        action='store_true',
                        help='Continue instead of aborting when errors occur.')
    parser.add_argument(
        '--output-directory',
        type=Path,
        help='Output directory (default: <repo root>/.presubmit)',
    )
    parser.add_argument(
        '--package-root',
        type=Path,
        help='Package root directory (default: <output directory>/packages)',
    )

    exclusive = parser.add_mutually_exclusive_group()
    exclusive.add_argument(
        '--clear',
        '--clean',
        action='store_true',
        help='Delete the presubmit output directory and exit.',
    )

    if programs:
        if not default:
            raise ValueError('A default must be provided with programs')

        _add_programs_arguments(parser, programs, default)


def run(
        program: Sequence[Callable],
        output_directory: Optional[Path],
        package_root: Path,
        clear: bool,
        root: Path = None,
        repositories: Collection[Path] = (),
        **other_args,
) -> int:
    """Processes arguments from add_arguments and runs the presubmit.

    Args:
      root: base path from which to run presubmit checks; defaults to the root
          of the current directory's repository
      repositories: roots of Git repositories on which to run presubmit checks;
          defaults to the root of the current directory's repository
      program: from the --program option
      output_directory: from --output-directory option
      package_root: from --package-root option
      clear: from the --clear option
      **other_args: remaining arguments defined by by add_arguments

    Returns:
      exit code for sys.exit; 0 if succesful, 1 if an error occurred
    """
    if root is None:
        root = git_repo.root()

    if not repositories:
        repositories = [root]

    if output_directory is None:
        output_directory = root / '.presubmit'

    if not package_root:
        package_root = output_directory / 'packages'

    _LOG.debug('Using environment at %s', output_directory)

    if clear:
        _LOG.info('Clearing presubmit output directory')

        if output_directory.exists():
            shutil.rmtree(output_directory)
            _LOG.info('Deleted %s', output_directory)

        return 0

    if presubmit.run(program,
                     root,
                     repositories,
                     output_directory=output_directory,
                     package_root=package_root,
                     **other_args):
        return 0

    return 1
