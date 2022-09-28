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
"""Checks and fixes formatting for source files.

This uses clang-format, gn format, gofmt, and python -m yapf to format source
code. These tools must be available on the path when this script is invoked!
"""

import argparse
import collections
import difflib
import logging
import os
from pathlib import Path
import re
import subprocess
import sys
from typing import Callable, Collection, Dict, Iterable, List, NamedTuple
from typing import Optional, Pattern, Tuple, Union

try:
    import pw_presubmit
except ImportError:
    # Append the pw_presubmit package path to the module search path to allow
    # running this module without installing the pw_presubmit package.
    sys.path.append(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__))))
    import pw_presubmit

from pw_presubmit import cli, git_repo
from pw_presubmit.tools import exclude_paths, file_summary, log_run, plural

_LOG: logging.Logger = logging.getLogger(__name__)


def _colorize_diff_line(line: str) -> str:
    if line.startswith('--- ') or line.startswith('+++ '):
        return pw_presubmit.color_bold_white(line)
    if line.startswith('-'):
        return pw_presubmit.color_red(line)
    if line.startswith('+'):
        return pw_presubmit.color_green(line)
    if line.startswith('@@ '):
        return pw_presubmit.color_aqua(line)
    return line


def colorize_diff(lines: Iterable[str]) -> str:
    """Takes a diff str or list of str lines and returns a colorized version."""
    if isinstance(lines, str):
        lines = lines.splitlines(True)

    return ''.join(_colorize_diff_line(line) for line in lines)


def _diff(path, original: bytes, formatted: bytes) -> str:
    return colorize_diff(
        difflib.unified_diff(
            original.decode(errors='replace').splitlines(True),
            formatted.decode(errors='replace').splitlines(True),
            f'{path}  (original)', f'{path}  (reformatted)'))


Formatter = Callable[[str, bytes], bytes]


def _diff_formatted(path, formatter: Formatter) -> Optional[str]:
    """Returns a diff comparing a file to its formatted version."""
    with open(path, 'rb') as fd:
        original = fd.read()

    formatted = formatter(path, original)

    return None if formatted == original else _diff(path, original, formatted)


def _check_files(files, formatter: Formatter) -> Dict[Path, str]:
    errors = {}

    for path in files:
        difference = _diff_formatted(path, formatter)
        if difference:
            errors[path] = difference

    return errors


def _clang_format(*args: str, **kwargs) -> bytes:
    return log_run(['clang-format', '--style=file', *args],
                   stdout=subprocess.PIPE,
                   check=True,
                   **kwargs).stdout


def clang_format_check(files: Iterable[Path]) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    return _check_files(files, lambda path, _: _clang_format(path))


def clang_format_fix(files: Iterable) -> None:
    """Fixes formatting for the provided files in place."""
    _clang_format('-i', *files)


def check_gn_format(files: Iterable[Path]) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    return _check_files(
        files, lambda _, data: log_run(['gn', 'format', '--stdin'],
                                       input=data,
                                       stdout=subprocess.PIPE,
                                       check=True).stdout)


def fix_gn_format(files: Iterable[Path]) -> None:
    """Fixes formatting for the provided files in place."""
    log_run(['gn', 'format', *files], check=True)


def check_go_format(files: Iterable[Path]) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    return _check_files(
        files, lambda path, _: log_run(
            ['gofmt', path], stdout=subprocess.PIPE, check=True).stdout)


def fix_go_format(files: Iterable[Path]) -> None:
    """Fixes formatting for the provided files in place."""
    log_run(['gofmt', '-w', *files], check=True)


def _yapf(*args, **kwargs) -> subprocess.CompletedProcess:
    return log_run(['python', '-m', 'yapf', '--style', '{based_on_style:pep8,indent_width:2}', '--parallel', *args],
                   capture_output=True,
                   **kwargs)


_DIFF_START = re.compile(r'^--- (.*)\s+\(original\)$', flags=re.MULTILINE)


def check_py_format(files: Iterable[Path]) -> Dict[Path, str]:
    """Checks formatting; returns {path: diff} for files with bad formatting."""
    process = _yapf('--diff', *files)

    errors: Dict[Path, str] = {}

    if process.stdout:
        raw_diff = process.stdout.decode(errors='replace')

        matches = tuple(_DIFF_START.finditer(raw_diff))
        for start, end in zip(matches, (*matches[1:], None)):
            errors[Path(start.group(1))] = colorize_diff(
                raw_diff[start.start():end.start() if end else None])

    if process.stderr:
        _LOG.error('yapf encountered an error:\n%s',
                   process.stderr.decode(errors='replace').rstrip())
        errors.update({file: '' for file in files if file not in errors})

    return errors


def fix_py_format(files: Iterable):
    """Fixes formatting for the provided files in place."""
    _yapf('--in-place', *files, check=True)


_TRAILING_SPACE = re.compile(rb'[ \t]+$', flags=re.MULTILINE)


def _check_trailing_space(paths: Iterable[Path], fix: bool) -> Dict[Path, str]:
    """Checks for and optionally removes trailing whitespace."""
    errors = {}

    for path in paths:
        with path.open('rb') as fd:
            contents = fd.read()

        corrected = _TRAILING_SPACE.sub(b'', contents)
        if corrected != contents:
            errors[path] = _diff(path, contents, corrected)

            if fix:
                with path.open('wb') as fd:
                    fd.write(corrected)

    return errors


def check_trailing_space(files: Iterable[Path]) -> Dict[Path, str]:
    return _check_trailing_space(files, fix=False)


def fix_trailing_space(files: Iterable[Path]) -> None:
    _check_trailing_space(files, fix=True)


def print_format_check(errors: Dict[Path, str],
                       show_fix_commands: bool) -> None:
    """Prints and returns the result of a check_*_format function."""
    if not errors:
        # Don't print anything in the all-good case.
        return

    # Show the format fixing diff suggested by the tooling (with colors).
    _LOG.warning('Found %d files with formatting errors. Format changes:',
                 len(errors))
    for diff in errors.values():
        print(diff, end='')

    # Show a copy-and-pastable command to fix the issues.
    if show_fix_commands:

        def path_relative_to_cwd(path):
            try:
                return Path(path).resolve().relative_to(Path.cwd().resolve())
            except ValueError:
                return Path(path).resolve()

class CodeFormat(NamedTuple):
    language: str
    extensions: Collection[str]
    exclude: Collection[str]
    check: Callable[[Iterable], Dict[Path, str]]
    fix: Callable[[Iterable], None]


C_FORMAT: CodeFormat = CodeFormat(
    'C and C++',
    frozenset(['.h', '.hh', '.hpp', '.c', '.cc', '.cpp', '.inc', '.inl']),
    (r'\.pb\.h$', r'\.pb\.c$'), clang_format_check, clang_format_fix)

PROTO_FORMAT: CodeFormat = CodeFormat('Protocol buffer', ('.proto', ), (),
                                      clang_format_check, clang_format_fix)

JAVA_FORMAT: CodeFormat = CodeFormat('Java', ('.java', ), (),
                                     clang_format_check, clang_format_fix)

JAVASCRIPT_FORMAT: CodeFormat = CodeFormat('JavaScript', ('.js', ), (),
                                           clang_format_check,
                                           clang_format_fix)

GO_FORMAT: CodeFormat = CodeFormat('Go', ('.go', ), (), check_go_format,
                                   fix_go_format)

PYTHON_FORMAT: CodeFormat = CodeFormat('Python', ('.py', ), (),
                                       check_py_format, fix_py_format)

GN_FORMAT: CodeFormat = CodeFormat('GN', ('.gn', '.gni'), (), check_gn_format,
                                   fix_gn_format)

# TODO(pwbug/191): Add real code formatting support for Bazel and CMake
BAZEL_FORMAT: CodeFormat = CodeFormat('Bazel', ('BUILD', ), (),
                                      check_trailing_space, fix_trailing_space)

CMAKE_FORMAT: CodeFormat = CodeFormat('CMake', ('CMakeLists.txt', '.cmake'),
                                      (), check_trailing_space,
                                      fix_trailing_space)

RST_FORMAT: CodeFormat = CodeFormat('reStructuredText', ('.rst', ), (),
                                    check_trailing_space, fix_trailing_space)

MARKDOWN_FORMAT: CodeFormat = CodeFormat('Markdown', ('.md', ), (),
                                         check_trailing_space,
                                         fix_trailing_space)

CODE_FORMATS: Tuple[CodeFormat, ...] = (
    C_FORMAT,
    JAVA_FORMAT,
    JAVASCRIPT_FORMAT,
    PROTO_FORMAT,
    GO_FORMAT,
    PYTHON_FORMAT,
    GN_FORMAT,
    BAZEL_FORMAT,
    CMAKE_FORMAT,
    RST_FORMAT,
    MARKDOWN_FORMAT,
)


def presubmit_check(code_format: CodeFormat, **filter_paths_args) -> Callable:
    """Creates a presubmit check function from a CodeFormat object."""
    filter_paths_args.setdefault('endswith', code_format.extensions)
    filter_paths_args.setdefault('exclude', code_format.exclude)

    @pw_presubmit.filter_paths(**filter_paths_args)
    def check_code_format(ctx: pw_presubmit.PresubmitContext):
        errors = code_format.check(ctx.paths)
        print_format_check(
            errors,
            # When running as part of presubmit, show the fix command help.
            show_fix_commands=True,
        )
        if errors:
            raise pw_presubmit.PresubmitFailure

    language = code_format.language.lower().replace('+', 'p').replace(' ', '_')
    check_code_format.__name__ = f'{language}_format'

    return check_code_format


def presubmit_checks(**filter_paths_args) -> Tuple[Callable, ...]:
    """Returns a tuple with all supported code format presubmit checks."""
    return tuple(
        presubmit_check(fmt, **filter_paths_args) for fmt in CODE_FORMATS)


class CodeFormatter:
    """Checks or fixes the formatting of a set of files."""
    def __init__(self, files: Iterable[Path]):
        self.paths = list(files)
        self._formats: Dict[CodeFormat, List] = collections.defaultdict(list)

        for path in self.paths:
            for code_format in CODE_FORMATS:
                if any(path.as_posix().endswith(e)
                       for e in code_format.extensions):
                    self._formats[code_format].append(path)

    def check(self) -> Dict[Path, str]:
        """Returns {path: diff} for files with incorrect formatting."""
        errors: Dict[Path, str] = {}

        for code_format, files in self._formats.items():
            _LOG.debug('Checking %s', ', '.join(str(f) for f in files))
            errors.update(code_format.check(files))

        return collections.OrderedDict(sorted(errors.items()))

    def fix(self) -> None:
        """Fixes format errors for supported files in place."""
        for code_format, files in self._formats.items():
            code_format.fix(files)
            _LOG.info('Formatted %s',
                      plural(files, code_format.language + ' file'))


def _file_summary(files: Iterable[Union[Path, str]], base: Path) -> List[str]:
    try:
        return file_summary(
            Path(f).resolve().relative_to(base.resolve()) for f in files)
    except ValueError:
        return []


def format_paths_in_repo(paths: Collection[Union[Path, str]],
                         exclude: Collection[Pattern[str]], fix: bool,
                         base: str) -> int:
    """Checks or fixes formatting for files in a Git repo."""
    files = [Path(path).resolve() for path in paths if os.path.isfile(path)]
    repo = git_repo.root() if git_repo.is_repo() else None

    # If this is a Git repo, list the original paths with git ls-files or diff.
    if repo:
        _LOG.info(
            'Formatting %s',
            git_repo.describe_files(repo, Path.cwd(), base, paths, exclude))

        # Add files from Git and remove duplicates.
        files = sorted(
            set(exclude_paths(exclude, git_repo.list_files(base, paths)))
            | set(files))
    elif base:
        _LOG.critical(
            'A base commit may only be provided if running from a Git repo')
        return 1

    return format_files(files, fix, repo=repo)


def format_files(paths: Collection[Union[Path, str]],
                 fix: bool,
                 repo: Optional[Path] = None) -> int:
    """Checks or fixes formatting for the specified files."""
    formatter = CodeFormatter(Path(p) for p in paths)

    _LOG.info('Checking formatting for %s', plural(formatter.paths, 'file'))

    for line in _file_summary(paths, repo if repo else Path.cwd()):
        print(line, file=sys.stderr)

    errors = formatter.check()
    print_format_check(errors, show_fix_commands=(not fix))

    if errors:
        if fix:
            formatter.fix()
            # TODO: This should perhaps check that the fixes were successful.
            _LOG.info('Formatting fixes applied successfully')
            return 0

        _LOG.error('Formatting errors found')
        return 1

    _LOG.info('Congratulations! No formatting changes needed')
    return 0


def arguments(git_paths: bool) -> argparse.ArgumentParser:
    """Creates an argument parser for format_files or format_paths_in_repo."""

    parser = argparse.ArgumentParser(description=__doc__)

    if git_paths:
        cli.add_path_arguments(parser)
    else:

        def existing_path(arg: str) -> Path:
            path = Path(arg)
            if not path.is_file():
                raise argparse.ArgumentTypeError(
                    f'{arg} is not a path to a file')

            return path

        parser.add_argument('paths',
                            metavar='path',
                            nargs='+',
                            type=existing_path,
                            help='File paths to check')

    parser.add_argument('--fix',
                        action='store_true',
                        help='Apply formatting fixes in place.')
    return parser


def main() -> int:
    """Check and fix formatting for source files."""
    return format_paths_in_repo(**vars(arguments(git_paths=True).parse_args()))


if __name__ == '__main__':
    try:
        # If pw_cli is available, use it to initialize logs.
        from pw_cli import log

        log.install(logging.INFO)
    except ImportError:
        # If pw_cli isn't available, display log messages like a simple print.
        logging.basicConfig(format='%(message)s', level=logging.INFO)

    sys.exit(main())
