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
r"""Decodes and detokenizes strings from binary or Base64 input.

The main class provided by this module is the Detokenize class. To use it,
construct it with the path to an ELF or CSV database, a tokens.Database,
or a file object for an ELF file or CSV. Then, call the detokenize method with
encoded messages, one at a time. The detokenize method returns a
DetokenizedString object with the result.

For example,

  from pw_tokenizer import detokenize

  detok = detokenize.Detokenizer('path/to/my/image.elf')
  print(detok.detokenize(b'\x12\x34\x56\x78\x03hi!'))

This module also provides a command line interface for decoding and detokenizing
messages from a file or stdin.
"""

import argparse
import base64
import binascii
from datetime import datetime
import io
import logging
import os
from pathlib import Path
import re
import string
import struct
import sys
import time
from typing import (BinaryIO, Callable, Dict, List, Iterable, Iterator, Match,
                    NamedTuple, Optional, Pattern, Tuple, Union)

try:
    from pw_tokenizer import database, decode, tokens
except ImportError:
    # Append this path to the module search path to allow running this module
    # without installing the pw_tokenizer package.
    sys.path.append(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__))))
    from pw_tokenizer import database, decode, tokens

ENCODED_TOKEN = struct.Struct('<I')
_LOG = logging.getLogger('pw_tokenizer')


class DetokenizedString:
    """A detokenized string, with all results if there are collisions."""
    def __init__(self,
                 token: Optional[int],
                 format_string_entries: Iterable[tuple],
                 encoded_message: bytes,
                 show_errors: bool = False):
        self.token = token
        self.encoded_message = encoded_message
        self._show_errors = show_errors

        self.successes: List[decode.FormattedString] = []
        self.failures: List[decode.FormattedString] = []

        decode_attempts: List[Tuple[Tuple, decode.FormattedString]] = []

        for entry, fmt in format_string_entries:
            result = fmt.format(encoded_message[ENCODED_TOKEN.size:],
                                show_errors)

            # Sort competing entries so the most likely matches appear first.
            # Decoded strings are prioritized by whether they
            #
            #   1. decoded all bytes for all arguments without errors,
            #   2. decoded all data,
            #   3. have the fewest decoding errors,
            #   4. decoded the most arguments successfully, or
            #   5. have the most recent removal date, if they were removed.
            #
            # This must match the collision resolution logic in detokenize.cc.
            score: Tuple = (
                all(arg.ok() for arg in result.args) and not result.remaining,
                not result.remaining,  # decoded all data
                -sum(not arg.ok() for arg in result.args),  # fewest errors
                len(result.args),  # decoded the most arguments
                entry.date_removed or datetime.max)  # most recently present

            decode_attempts.append((score, result))

        # Sort the attempts by the score so the most likely results are first.
        decode_attempts.sort(key=lambda value: value[0], reverse=True)

        # Split out the successesful decodes from the failures.
        for score, result in decode_attempts:
            if score[0]:
                self.successes.append(result)
            else:
                self.failures.append(result)

    def ok(self) -> bool:
        """True if exactly one string decoded the arguments successfully."""
        return len(self.successes) == 1

    def matches(self) -> List[decode.FormattedString]:
        """Returns the strings that matched the token, best matches first."""
        return self.successes + self.failures

    def best_result(self) -> Optional[decode.FormattedString]:
        """Returns the string and args for the most likely decoded string."""
        for string_and_args in self.matches():
            return string_and_args

        return None

    def error_message(self) -> str:
        """If detokenization failed, returns a descriptive message."""
        if self.ok():
            return ''

        if not self.matches():
            if self.token is None:
                return 'missing token'

            return 'unknown token {:08x}'.format(self.token)

        if len(self.matches()) == 1:
            return 'decoding failed for {!r}'.format(self.matches()[0].value)

        return '{} matches'.format(len(self.matches()))

    def __str__(self) -> str:
        """Returns the string for the most likely result."""
        result = self.best_result()
        if result:
            return result[0]

        if self._show_errors:
            return '<[ERROR: {}|{!r}]>'.format(self.error_message(),
                                               self.encoded_message)
        return ''

    def __repr__(self) -> str:
        if self.ok():
            message = repr(str(self))
        else:
            message = 'ERROR: {}|{!r}'.format(self.error_message(),
                                              self.encoded_message)

        return '{}({})'.format(type(self).__name__, message)


class _TokenizedFormatString(NamedTuple):
    entry: tokens.TokenizedStringEntry
    format: decode.FormatString


class Detokenizer:
    """Main detokenization class; detokenizes strings and caches results."""
    def __init__(self, *token_database_or_elf, show_errors: bool = False):
        """Decodes and detokenizes binary messages.

        Args:
          *token_database_or_elf: a path or file object for an ELF or CSV
              database, a tokens.Database, or an elf_reader.Elf
          show_errors: if True, an error message is used in place of the %
              conversion specifier when an argument fails to decode
        """
        self.database = database.load_token_database(*token_database_or_elf)
        self.show_errors = show_errors

        # Cache FormatStrings for faster lookup & formatting.
        self._cache: Dict[int, List[_TokenizedFormatString]] = {}

    def lookup(self, token: int) -> List[_TokenizedFormatString]:
        """Returns (TokenizedStringEntry, FormatString) list for matches."""
        try:
            return self._cache[token]
        except KeyError:
            format_strings = [
                _TokenizedFormatString(entry, decode.FormatString(str(entry)))
                for entry in self.database.token_to_entries[token]
            ]
            self._cache[token] = format_strings
            return format_strings

    def detokenize(self, encoded_message: bytes) -> DetokenizedString:
        """Decodes and detokenizes a message as a DetokenizedString."""
        if len(encoded_message) < ENCODED_TOKEN.size:
            return DetokenizedString(None, (), encoded_message,
                                     self.show_errors)

        token, = ENCODED_TOKEN.unpack_from(encoded_message)
        return DetokenizedString(token, self.lookup(token), encoded_message,
                                 self.show_errors)


class AutoUpdatingDetokenizer:
    """Loads and updates a detokenizer from database paths."""
    class _DatabasePath:
        """Tracks the modified time of a path or file object."""
        def __init__(self, path):
            self.path = path if isinstance(path, (str, Path)) else path.name
            self._modified_time: Optional[float] = self._last_modified_time()

        def updated(self) -> bool:
            """True if the path has been updated since the last call."""
            modified_time = self._last_modified_time()
            if modified_time is None or modified_time == self._modified_time:
                return False

            self._modified_time = modified_time
            return True

        def _last_modified_time(self) -> Optional[float]:
            try:
                return os.path.getmtime(self.path)
            except FileNotFoundError:
                return None

        def load(self) -> tokens.Database:
            try:
                return database.load_token_database(self.path)
            except FileNotFoundError:
                return database.load_token_database()

    def __init__(self,
                 *paths_or_files,
                 min_poll_period_s: float = 1.0) -> None:
        self.paths = tuple(self._DatabasePath(path) for path in paths_or_files)
        self.min_poll_period_s = min_poll_period_s
        self._last_checked_time: float = time.time()
        self._detokenizer = Detokenizer(*(path.load() for path in self.paths))

    def detokenize(self, data: bytes) -> DetokenizedString:
        """Updates the token database if it has changed, then detokenizes."""
        if time.time() - self._last_checked_time >= self.min_poll_period_s:
            self._last_checked_time = time.time()

            if any(path.updated() for path in self.paths):
                _LOG.info('Changes detected; reloading token database')
                self._detokenizer = Detokenizer(*(path.load()
                                                  for path in self.paths))

        return self._detokenizer.detokenize(data)


_Detokenizer = Union[Detokenizer, AutoUpdatingDetokenizer]


class PrefixedMessageDecoder:
    """Parses messages that start with a prefix character from a byte stream."""
    def __init__(self, prefix: Union[str, bytes], chars: Union[str, bytes]):
        """Parses prefixed messages.

        Args:
          prefix: one character that signifies the start of a message
          chars: characters allowed in a message
        """
        self._prefix = prefix.encode() if isinstance(prefix, str) else prefix

        if isinstance(chars, str):
            chars = chars.encode()

        # Store the valid message bytes as a set of binary strings.
        self._message_bytes = frozenset(chars[i:i + 1]
                                        for i in range(len(chars)))

        if len(self._prefix) != 1 or self._prefix in self._message_bytes:
            raise ValueError(
                'Invalid prefix {!r}: the prefix must be a single '
                'character that is not a valid message character.'.format(
                    prefix))

        self.data = bytearray()

    def _read_next(self, fd: BinaryIO) -> Tuple[bytes, int]:
        """Returns the next character and its index."""
        char = fd.read(1)
        index = len(self.data)
        self.data += char
        return char, index

    def read_messages(self,
                      binary_fd: BinaryIO) -> Iterator[Tuple[bool, bytes]]:
        """Parses prefixed messages; yields (is_message, contents) chunks."""
        message_start = None

        while True:
            # This reads the file character-by-character. Non-message characters
            # are yielded right away; message characters are grouped.
            char, index = self._read_next(binary_fd)

            # If in a message, keep reading until the message completes.
            if message_start is not None:
                if char in self._message_bytes:
                    continue

                yield True, self.data[message_start:index]
                message_start = None

            # Handle a non-message character.
            if not char:
                return

            if char == self._prefix:
                message_start = index
            else:
                yield False, char

    def transform(self, binary_fd: BinaryIO,
                  transform: Callable[[bytes], bytes]) -> Iterator[bytes]:
        """Yields the file with a transformation applied to the messages."""
        for is_message, chunk in self.read_messages(binary_fd):
            yield transform(chunk) if is_message else chunk


def _detokenize_prefixed_base64(
        detokenizer: _Detokenizer, prefix: bytes,
        recursion: int) -> Callable[[Match[bytes]], bytes]:
    """Returns a function that decodes prefixed Base64 with the detokenizer."""
    def decode_and_detokenize(match: Match[bytes]) -> bytes:
        """Decodes prefixed base64 with the provided detokenizer."""
        original = match.group(0)

        try:
            detokenized_string = detokenizer.detokenize(
                base64.b64decode(original[1:], validate=True))
            if detokenized_string.matches():
                result = str(detokenized_string).encode()

                if recursion > 0 and original != result:
                    result = detokenize_base64(detokenizer, result, prefix,
                                               recursion - 1)

                return result
        except binascii.Error:
            pass

        return original

    return decode_and_detokenize


BASE64_PREFIX = b'$'
DEFAULT_RECURSION = 9


def _base64_message_regex(prefix: bytes) -> Pattern[bytes]:
    """Returns a regular expression for prefixed base64 tokenized strings."""
    return re.compile(
        # Base64 tokenized strings start with the prefix character ($)
        re.escape(prefix) + (
            # Tokenized strings contain 0 or more blocks of four Base64 chars.
            br'(?:[A-Za-z0-9+/\-_]{4})*'
            # The last block of 4 chars may have one or two padding chars (=).
            br'(?:[A-Za-z0-9+/\-_]{3}=|[A-Za-z0-9+/\-_]{2}==)?'))


def detokenize_base64_live(detokenizer: _Detokenizer,
                           input_file: BinaryIO,
                           output: BinaryIO,
                           prefix: Union[str, bytes] = BASE64_PREFIX,
                           recursion: int = DEFAULT_RECURSION) -> None:
    """Reads chars one-at-a-time and decodes messages; SLOW for big files."""
    prefix_bytes = prefix.encode() if isinstance(prefix, str) else prefix

    base64_message = _base64_message_regex(prefix_bytes)

    def transform(data: bytes) -> bytes:
        return base64_message.sub(
            _detokenize_prefixed_base64(detokenizer, prefix_bytes, recursion),
            data)

    for message in PrefixedMessageDecoder(
            prefix, string.ascii_letters + string.digits + '+/-_=').transform(
                input_file, transform):
        output.write(message)

        # Flush each line to prevent delays when piping between processes.
        if b'\n' in message:
            output.flush()


def detokenize_base64_to_file(detokenizer: _Detokenizer,
                              data: bytes,
                              output: BinaryIO,
                              prefix: Union[str, bytes] = BASE64_PREFIX,
                              recursion: int = DEFAULT_RECURSION) -> None:
    """Decodes prefixed Base64 messages in data; decodes to an output file."""
    prefix = prefix.encode() if isinstance(prefix, str) else prefix
    output.write(
        _base64_message_regex(prefix).sub(
            _detokenize_prefixed_base64(detokenizer, prefix, recursion), data))


def detokenize_base64(detokenizer: _Detokenizer,
                      data: bytes,
                      prefix: Union[str, bytes] = BASE64_PREFIX,
                      recursion: int = DEFAULT_RECURSION) -> bytes:
    """Decodes and replaces prefixed Base64 messages in the provided data.

    Args:
      detokenizer: the detokenizer with which to decode messages
      data: the binary data to decode
      prefix: one-character byte string that signals the start of a message
      recursion: how many levels to recursively decode

    Returns:
      copy of the data with all recognized tokens decoded
    """
    output = io.BytesIO()
    detokenize_base64_to_file(detokenizer, data, output, prefix, recursion)
    return output.getvalue()


def _follow_and_detokenize_file(detokenizer: _Detokenizer,
                                file: BinaryIO,
                                output: BinaryIO,
                                prefix: Union[str, bytes],
                                poll_period_s: float = 0.01) -> None:
    """Polls a file to detokenize it and any appended data."""

    try:
        while True:
            data = file.read()
            if data:
                detokenize_base64_to_file(detokenizer, data, output, prefix)
                output.flush()
            else:
                time.sleep(poll_period_s)
    except KeyboardInterrupt:
        pass


def _handle_base64(databases, input_file: BinaryIO, output: BinaryIO,
                   prefix: str, show_errors: bool, follow: bool) -> None:
    """Handles the base64 command line option."""
    # argparse.FileType doesn't correctly handle - for binary files.
    if input_file is sys.stdin:
        input_file = sys.stdin.buffer

    if output is sys.stdout:
        output = sys.stdout.buffer

    detokenizer = Detokenizer(tokens.Database.merged(*databases),
                              show_errors=show_errors)

    if follow:
        _follow_and_detokenize_file(detokenizer, input_file, output, prefix)
    elif input_file.seekable():
        # Process seekable files all at once, which is MUCH faster.
        detokenize_base64_to_file(detokenizer, input_file.read(), output,
                                  prefix)
    else:
        # For non-seekable inputs (e.g. pipes), read one character at a time.
        detokenize_base64_live(detokenizer, input_file, output, prefix)


def _parse_args() -> argparse.Namespace:
    """Parses and return command line arguments."""

    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.set_defaults(handler=lambda **_: parser.print_help())

    subparsers = parser.add_subparsers(help='Encoding of the input.')

    base64_help = 'Detokenize Base64-encoded data from a file or stdin.'
    subparser = subparsers.add_parser(
        'base64',
        description=base64_help,
        parents=[database.token_databases_parser()],
        help=base64_help)
    subparser.set_defaults(handler=_handle_base64)
    subparser.add_argument(
        '-i',
        '--input',
        dest='input_file',
        type=argparse.FileType('rb'),
        default=sys.stdin.buffer,
        help='The file from which to read; provide - or omit for stdin.')
    subparser.add_argument(
        '-f',
        '--follow',
        action='store_true',
        help=('Detokenize data appended to input_file as it grows; similar to '
              'tail -f.'))
    subparser.add_argument('-o',
                           '--output',
                           type=argparse.FileType('wb'),
                           default=sys.stdout.buffer,
                           help=('The file to which to write the output; '
                                 'provide - or omit for stdout.'))
    subparser.add_argument(
        '-p',
        '--prefix',
        default=BASE64_PREFIX,
        help=('The one-character prefix that signals the start of a '
              'Base64-encoded message. (default: $)'))
    subparser.add_argument(
        '-s',
        '--show_errors',
        action='store_true',
        help=('Show error messages instead of conversion specifiers when '
              'arguments cannot be decoded.'))

    return parser.parse_args()


def main() -> int:
    args = _parse_args()

    handler = args.handler
    del args.handler

    handler(**vars(args))
    return 0


if __name__ == '__main__':
    if sys.version_info[0] < 3:
        sys.exit('ERROR: The detokenizer command line tools require Python 3.')
    sys.exit(main())
