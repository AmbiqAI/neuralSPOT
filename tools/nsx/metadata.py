"""Metadata helpers for NSX module orchestration."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

import yaml

SUPPORTED_MODULE_TYPES = {
    "sdk_provider",
    "soc",
    "board",
    "runtime",
    "portable_api",
    "algorithm",
    "tooling",
    "backend_specific",
}


@dataclass(frozen=True)
class RegistryModuleEntry:
    """One module entry from registry.lock.yaml."""

    name: str
    project: str
    revision: str
    metadata: str


def _expect_type(container: dict[str, Any], key: str, expected: type, ctx: str) -> Any:
    if key not in container:
        raise ValueError(f"{ctx}: missing required key '{key}'")
    value = container[key]
    if not isinstance(value, expected):
        raise ValueError(
            f"{ctx}: key '{key}' must be {expected.__name__}, got {type(value).__name__}"
        )
    return value


def load_yaml(path: Path) -> dict[str, Any]:
    if not path.exists():
        raise ValueError(f"YAML file does not exist: {path}")
    try:
        data = yaml.safe_load(path.read_text(encoding="utf-8"))
    except yaml.YAMLError as exc:
        raise ValueError(f"Failed to parse YAML at {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise ValueError(f"YAML root must be a mapping at {path}")
    return data


def load_registry_lock(path: Path) -> dict[str, Any]:
    data = load_yaml(path)
    schema_version = _expect_type(data, "schema_version", int, "registry.lock")
    if schema_version != 1:
        raise ValueError(
            f"registry.lock: unsupported schema_version={schema_version}, expected 1"
        )
    _expect_type(data, "channels", dict, "registry.lock")
    _expect_type(data, "modules", dict, "registry.lock")
    _expect_type(data, "starter_profiles", dict, "registry.lock")
    _expect_type(data, "compat_matrix", dict, "registry.lock")
    return data


def validate_nsx_module_metadata(data: dict[str, Any], module_path: str) -> None:
    """Validate v1 nsx-module.yaml structure and policies."""
    schema_version = _expect_type(data, "schema_version", int, module_path)
    if schema_version != 1:
        raise ValueError(
            f"{module_path}: unsupported schema_version={schema_version}, expected 1"
        )

    module = _expect_type(data, "module", dict, module_path)
    module_name = _expect_type(module, "name", str, f"{module_path}.module")
    module_type = _expect_type(module, "type", str, f"{module_path}.module")
    _expect_type(module, "version", str, f"{module_path}.module")
    if "category" in module and not isinstance(module["category"], str):
        raise ValueError(f"{module_path}.module.category must be string when provided")
    if "provider" in module and not isinstance(module["provider"], str):
        raise ValueError(f"{module_path}.module.provider must be string when provided")
    if module_type not in SUPPORTED_MODULE_TYPES:
        raise ValueError(
            f"{module_path}: module.type='{module_type}' is not supported. "
            f"Allowed: {sorted(SUPPORTED_MODULE_TYPES)}"
        )

    support = _expect_type(data, "support", dict, module_path)
    ambiqsuite = _expect_type(support, "ambiqsuite", bool, f"{module_path}.support")
    zephyr = _expect_type(support, "zephyr", bool, f"{module_path}.support")
    if not ambiqsuite:
        raise ValueError(
            f"{module_path}: support.ambiqsuite=false is invalid for NSX modules"
        )

    build = _expect_type(data, "build", dict, module_path)
    build_cmake = _expect_type(build, "cmake", dict, f"{module_path}.build")
    _expect_type(build_cmake, "package", str, f"{module_path}.build.cmake")
    targets = _expect_type(build_cmake, "targets", list, f"{module_path}.build.cmake")
    if not targets:
        raise ValueError(f"{module_path}: build.cmake.targets must not be empty")

    depends = _expect_type(data, "depends", dict, module_path)
    required = _expect_type(depends, "required", list, f"{module_path}.depends")
    _expect_type(depends, "optional", list, f"{module_path}.depends")
    for idx, dep_name in enumerate(required):
        if not isinstance(dep_name, str):
            raise ValueError(
                f"{module_path}: depends.required[{idx}] must be string module name"
            )

    compatibility = _expect_type(data, "compatibility", dict, module_path)
    boards = _expect_type(compatibility, "boards", list, f"{module_path}.compatibility")
    socs = _expect_type(compatibility, "socs", list, f"{module_path}.compatibility")
    toolchains = _expect_type(
        compatibility, "toolchains", list, f"{module_path}.compatibility"
    )
    for field_name, values in (
        ("boards", boards),
        ("socs", socs),
        ("toolchains", toolchains),
    ):
        if not values:
            raise ValueError(f"{module_path}: compatibility.{field_name} must not be empty")
        if not all(isinstance(item, str) for item in values):
            raise ValueError(
                f"{module_path}: compatibility.{field_name} must contain only strings"
            )

    if zephyr:
        integrations = _expect_type(data, "integrations", dict, module_path)
        zephyr_cfg = _expect_type(integrations, "zephyr", dict, f"{module_path}.integrations")
        _expect_type(zephyr_cfg, "path", str, f"{module_path}.integrations.zephyr")
        _expect_type(zephyr_cfg, "module_yml", str, f"{module_path}.integrations.zephyr")
        _expect_type(zephyr_cfg, "kconfig", str, f"{module_path}.integrations.zephyr")

    if module_type == "board":
        # Board modules must depend on exactly one SoC module.
        # This is checked with resolved module graph in CLI logic. Keep minimal guard here.
        if not required:
            raise ValueError(
                f"{module_path}: board module '{module_name}' must have required dependencies"
            )

    constraints = data.get("constraints", {})
    if constraints:
        if not isinstance(constraints, dict):
            raise ValueError(f"{module_path}: constraints must be mapping when provided")
        if "required_sdk_provider" in constraints and not isinstance(
            constraints["required_sdk_provider"], str
        ):
            raise ValueError(
                f"{module_path}: constraints.required_sdk_provider must be string"
            )


def registry_entry_for_module(registry: dict[str, Any], module_name: str) -> RegistryModuleEntry:
    modules = registry["modules"]
    if module_name not in modules:
        raise ValueError(f"Module '{module_name}' not found in registry.lock")
    entry = modules[module_name]
    if not isinstance(entry, dict):
        raise ValueError(f"registry.lock: modules.{module_name} must be a mapping")
    project = _expect_type(entry, "project", str, f"registry.lock.modules.{module_name}")
    revision = _expect_type(entry, "revision", str, f"registry.lock.modules.{module_name}")
    metadata = _expect_type(entry, "metadata", str, f"registry.lock.modules.{module_name}")
    return RegistryModuleEntry(
        name=module_name,
        project=project,
        revision=revision,
        metadata=metadata,
    )


def is_compatible(
    metadata: dict[str, Any],
    *,
    board: str,
    soc: str,
    toolchain: str,
) -> bool:
    compat = metadata["compatibility"]

    def _ok(values: list[str], current: str) -> bool:
        return "*" in values or current in values

    return (
        _ok(compat["boards"], board)
        and _ok(compat["socs"], soc)
        and _ok(compat["toolchains"], toolchain)
    )
