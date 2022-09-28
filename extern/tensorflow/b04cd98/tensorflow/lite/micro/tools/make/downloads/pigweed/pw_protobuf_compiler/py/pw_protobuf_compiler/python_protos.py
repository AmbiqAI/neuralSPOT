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
"""Tools for compiling and importing Python protos on the fly."""

import importlib.util
import logging
import os
from pathlib import Path
import subprocess
import shlex
import tempfile
from types import ModuleType
from typing import (Dict, Generic, Iterable, Iterator, List, NamedTuple, Set,
                    Tuple, TypeVar, Union)

_LOG = logging.getLogger(__name__)

PathOrStr = Union[Path, str]


def compile_protos(
    output_dir: PathOrStr,
    proto_files: Iterable[PathOrStr],
    includes: Iterable[PathOrStr] = ()) -> None:
    """Compiles proto files for Python by invoking the protobuf compiler.

    Proto files not covered by one of the provided include paths will have their
    directory added as an include path.
    """
    proto_paths: List[Path] = [Path(f).resolve() for f in proto_files]
    include_paths: Set[Path] = set(Path(d).resolve() for d in includes)

    for path in proto_paths:
        if not any(include in path.parents for include in include_paths):
            include_paths.add(path.parent)

    cmd: Tuple[PathOrStr, ...] = (
        'protoc',
        '--python_out',
        os.path.abspath(output_dir),
        *(f'-I{d}' for d in include_paths),
        *proto_paths,
    )

    _LOG.debug('%s', ' '.join(shlex.quote(str(c)) for c in cmd))
    process = subprocess.run(cmd, capture_output=True)

    if process.returncode:
        _LOG.error('protoc invocation failed!\n%s\n%s',
                   ' '.join(shlex.quote(str(c)) for c in cmd),
                   process.stderr.decode())
        process.check_returncode()


def _import_module(name: str, path: str) -> ModuleType:
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)  # type: ignore[union-attr]
    return module


def import_modules(directory: PathOrStr) -> Iterator:
    """Imports modules in a directory and yields them."""
    parent = os.path.dirname(directory)

    for dirpath, _, files in os.walk(directory):
        path_parts = os.path.relpath(dirpath, parent).split(os.sep)

        for file in files:
            name, ext = os.path.splitext(file)

            if ext == '.py':
                yield _import_module(f'{".".join(path_parts)}.{name}',
                                     os.path.join(dirpath, file))


def compile_and_import(proto_files: Iterable[PathOrStr],
                       includes: Iterable[PathOrStr] = (),
                       output_dir: PathOrStr = None) -> Iterator:
    """Compiles protos and imports their modules; yields the proto modules.

    Args:
      proto_files: paths to .proto files to compile
      includes: include paths to use for .proto compilation
      output_dir: where to place the generated modules; a temporary directory is
          used if omitted

    Yields:
      the generated protobuf Python modules
    """

    if output_dir:
        compile_protos(output_dir, proto_files, includes)
        yield from import_modules(output_dir)
    else:
        with tempfile.TemporaryDirectory(prefix='compiled_protos_') as tempdir:
            compile_protos(tempdir, proto_files, includes)
            yield from import_modules(tempdir)


def compile_and_import_file(proto_file: PathOrStr,
                            includes: Iterable[PathOrStr] = (),
                            output_dir: PathOrStr = None):
    """Compiles and imports the module for a single .proto file."""
    return next(iter(compile_and_import([proto_file], includes, output_dir)))


def compile_and_import_strings(contents: Iterable[str],
                               includes: Iterable[PathOrStr] = (),
                               output_dir: PathOrStr = None) -> Iterator:
    """Compiles protos in one or more strings."""

    if isinstance(contents, str):
        contents = [contents]

    with tempfile.TemporaryDirectory(prefix='proto_sources_') as path:
        protos = []

        for proto in contents:
            # Use a hash of the proto so the same contents map to the same file
            # name. The protobuf package complains if it seems the same contents
            # in files with different names.
            protos.append(Path(path, f'protobuf_{hash(proto):x}.proto'))
            protos[-1].write_text(proto)

        yield from compile_and_import(protos, includes, output_dir)


T = TypeVar('T')


class _NestedPackage(Generic[T]):
    """Facilitates navigating protobuf packages as attributes."""
    def __init__(self, package: str):
        self._packages: Dict[str, _NestedPackage[T]] = {}
        self._items: List[T] = []
        self._package = package

    def _add_package(self, subpackage: str, package: '_NestedPackage') -> None:
        self._packages[subpackage] = package

    def _add_item(self, item) -> None:
        if item not in self._items:  # Don't store the same item multiple times.
            self._items.append(item)

    def __getattr__(self, attr: str):
        """Look up subpackages or package members."""
        if attr in self._packages:
            return self._packages[attr]

        for item in self._items:
            if hasattr(item, attr):
                return getattr(item, attr)

        raise AttributeError(
            f'Proto package "{self._package}" does not contain "{attr}"')

    def __getitem__(self, subpackage: str) -> '_NestedPackage[T]':
        """Support accessing nested packages by name."""
        result = self

        for package in subpackage.split('.'):
            result = result._packages[package]

        return result

    def __dir__(self) -> List[str]:
        """List subpackages and members of modules as attributes."""
        attributes = list(self._packages)

        for item in self._items:
            for attr, value in vars(item).items():
                # Exclude private variables and modules from dir().
                if not attr.startswith('_') and not isinstance(
                        value, ModuleType):
                    attributes.append(attr)

        return attributes

    def __iter__(self) -> Iterator['_NestedPackage[T]']:
        """Iterate over nested packages."""
        return iter(self._packages.values())

    def __repr__(self) -> str:
        msg = [f'ProtoPackage({self._package!r}']

        public_members = [
            i for i in vars(self)
            if i not in self._packages and not i.startswith('_')
        ]
        if public_members:
            msg.append(f'members={str(public_members)}')

        if self._packages:
            msg.append(f'subpackages={str(list(self._packages))}')

        return ', '.join(msg) + ')'

    def __str__(self) -> str:
        return self._package


class Packages(NamedTuple):
    """Items in a protobuf package structure; returned from as_package."""
    items_by_package: Dict[str, List]
    packages: _NestedPackage


def as_packages(items: Iterable[Tuple[str, T]],
                packages: Packages = None) -> Packages:
    """Places items in a proto-style package structure navigable by attributes.

    Args:
      items: (package, item) tuples to insert into the package structure
      packages: if provided, update this Packages instead of creating a new one
    """
    if packages is None:
        packages = Packages({}, _NestedPackage(''))

    for package, item in items:
        packages.items_by_package.setdefault(package, []).append(item)

        entry = packages.packages
        subpackages = package.split('.')

        # pylint: disable=protected-access
        for i, subpackage in enumerate(subpackages, 1):
            if subpackage not in entry._packages:
                entry._add_package(subpackage,
                                   _NestedPackage('.'.join(subpackages[:i])))

            entry = entry._packages[subpackage]

        entry._add_item(item)
        # pylint: enable=protected-access

    return packages


PathOrModule = Union[str, Path, ModuleType]


class Library:
    """A collection of protocol buffer modules sorted by package.

    In Python, each .proto file is compiled into a Python module. The Library
    class makes it simple to navigate a collection of Python modules
    corresponding to .proto files, without relying on the location of these
    compiled modules.

    Proto messages and other types can be directly accessed by their protocol
    buffer package name. For example, the foo.bar.Baz message can be accessed
    in a Library called `protos` as:

      protos.packages.foo.bar.Baz

    A Library also provides the modules_by_package dictionary, for looking up
    the list of modules in a particular package, and the modules() generator
    for iterating over all modules.
    """
    @classmethod
    def from_paths(cls, protos: Iterable[PathOrModule]) -> 'Library':
        """Creates a Library from paths to proto files or proto modules."""
        paths: List[PathOrStr] = []
        modules: List[ModuleType] = []

        for proto in protos:
            if isinstance(proto, (Path, str)):
                paths.append(proto)
            else:
                modules.append(proto)

        if paths:
            modules += compile_and_import(paths)
        return Library(modules)

    @classmethod
    def from_strings(cls,
                     contents: Iterable[str],
                     includes: Iterable[PathOrStr] = (),
                     output_dir: PathOrStr = None) -> 'Library':
        """Creates a proto library from protos in the provided strings."""
        return cls(compile_and_import_strings(contents, includes, output_dir))

    def __init__(self, modules: Iterable[ModuleType]):
        """Constructs a Library from an iterable of modules.

        A Library can be constructed with modules dynamically compiled by
        compile_and_import. For example:

            protos = Library(compile_and_import(list_of_proto_files))
        """
        self.modules_by_package, self.packages = as_packages(
            (m.DESCRIPTOR.package, m)  # type: ignore[attr-defined]
            for m in modules)

    def modules(self) -> Iterable:
        """Allows iterating over all protobuf modules in this library."""
        for module_list in self.modules_by_package.values():
            yield from module_list
