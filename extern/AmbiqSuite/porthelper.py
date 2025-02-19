#!/usr/bin/env python3

"""
AmbiqSuite Export Script

Usage (example):
    python porthelper.py \
        --cpu apollo5 \
        --old-sdk /path/to/original/AmbiqSuite_2.2.0 \
        --old-export /path/to/neuralspot_exported_2.2.0 \
        --new-sdk /path/to/AmbiqSuite_2.3.1 \
        --new-export /path/to/new_neuralspot_exported_2.3.1

The script will:
1. Parse the parameters.
2. Create the `src` directory in the new export, copying in source files that match
   the old exported `src` files (using logic to detect if they changed or if user intervention is needed).
3. Create and populate the `lib` directory by gathering the static libraries from the new AmbiqSuite,
   matched to the specified CPU family (and variant).
4. Copy headers/license files from the new SDK into the new export directory structure,
   skipping certain directories, but always copying `pack` and `cordio`.
5. Alert the user about any conflicts that require a manual merge.
"""

import os
import sys
import shutil
import argparse
import filecmp
from pathlib import Path

###############################################################################
# Helpers
###############################################################################

def find_all_files_with_name(root_dir, filename, exclude_subdir=None):
    """
    Recursively find all files in `root_dir` with the given filename.
    If exclude_subdir is specified, exclude that subdirectory from the search.
    Return a list of full paths.
    """
    matches = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # Optionally exclude a subdir entirely
        if exclude_subdir and exclude_subdir in dirpath:
            continue
        if filename in filenames:
            matches.append(os.path.join(dirpath, filename))
    return matches

def pick_file_interactively(candidates, hint=""):
    """
    Given a list of candidate file paths, present them to the user and let them pick one.
    The 'hint' can help the user with context (e.g. "Looking for CPU variant?").
    Returns the selected file path.
    """
    print("\nMultiple candidates found for the same file. {}".format(hint))
    for i, c in enumerate(candidates):
        print(f"  [{i}] {c}")

    choice = None
    while choice is None:
        val = input("Enter the index of the file you want to use: ")
        try:
            idx = int(val)
            if 0 <= idx < len(candidates):
                choice = candidates[idx]
            else:
                print("Invalid choice, try again.")
        except ValueError:
            print("Invalid input, try again.")
    return choice

def select_best_candidate(candidates, cpu_family, is_freertos=False):
    """
    Attempt to auto-select the best candidate among multiple matches.
    If is_freertos=True, pick the highest version directory name if possible.
    If we see CPU references in the path (e.g. apollo5b, apollo5, etc.), pick
    the one that matches or contains the specified cpu_family. 
    If there's still more than one, fall back to prompting the user.
    """
    if not candidates:
        return None
    if len(candidates) == 1:
        return candidates[0]

    # If it's a FreeRTOS file, try to pick the highest version directory path
    if is_freertos:
        # Simple heuristic: parse numeric parts in the path if there's "FreeRTOSvX.Y.Z"
        # and pick the largest.
        def extract_freertos_version(path):
            # e.g. "FreeRTOSv10.5.1" -> (10,5,1)
            import re
            import math
            match = re.search(r"FreeRTOSv(\d+)\.(\d+)\.(\d+)", path)
            if match:
                return tuple(map(int, match.groups()))
            else:
                return (0,0,0)
        best = None
        best_version = (0,0,0)
        for c in candidates:
            ver = extract_freertos_version(c)
            if ver > best_version:
                best_version = ver
                best = c
        # If best is unique so far, pick it. If there's a tie, we still might need to ask user.
        # Check how many share the same version
        same_version = [c for c in candidates if extract_freertos_version(c) == best_version]
        if len(same_version) == 1:
            return best
        else:
            # There's a tie => prompt user
            return pick_file_interactively(same_version, hint="(multiple FreeRTOS directories share the same version)")

    # If CPU references exist, pick the one that references the user-specified CPU family
    # e.g. "apollo5", "apollo5b" is a sub-match if user says "apollo5" – we look for that substring.
    # If multiple candidates still remain, we prompt the user.
    matches_cpu = []
    for c in candidates:
        # To handle variants like apollo5b, allow partial matches, e.g. "apollo5" in "apollo5b"
        if cpu_family.lower() in os.path.basename(c).lower() or cpu_family.lower() in c.lower():
            matches_cpu.append(c)

    if len(matches_cpu) == 1:
        return matches_cpu[0]
    elif len(matches_cpu) > 1:
        # We may have multiple. Prompt user
        return pick_file_interactively(matches_cpu, hint="(multiple CPU variants matched)")

    # If we cannot do an automatic selection, prompt the user among all
    return pick_file_interactively(candidates)

def safe_makedirs(path):
    """
    Create directories if they do not exist.
    """
    if not os.path.exists(path):
        os.makedirs(path)

def copy_conflict_files(old_src_file, new_sdk_file, dest_path):
    """
    In the conflict scenario, copy two versions with .old and .new suffixes.
    """
    base_name = os.path.basename(old_src_file)
    # Example: if base_name == foo.c
    # copy old -> foo.c.old
    # copy new -> foo.c.new
    old_dest = os.path.join(dest_path, base_name + ".old")
    new_dest = os.path.join(dest_path, base_name + ".new")
    print(f"**** Conflict: copying both versions to:\n     {old_dest}\n     {new_dest}")
    shutil.copy2(old_src_file, old_dest)
    shutil.copy2(new_sdk_file, new_dest)

def copy_file_or_warn(src, dest):
    """
    Copy src to dest, creating directories if needed.
    Warn if src does not exist.
    """
    if not os.path.exists(src):
        print(f"Warning: cannot copy missing file {src}")
        return
    safe_makedirs(os.path.dirname(dest))
    shutil.copy2(src, dest)


###############################################################################
# Main Export Logic
###############################################################################
def export_ambiqsuite(cpu_family, old_sdk, old_export, new_sdk, new_export):
    """
    1. Create new_export/src
       For each file in old_export/src, find the corresponding file in old_sdk, compare,
       and then find a matching file in new_sdk. Copy or produce conflict as needed.
    2. Create new_export/lib
       Copy new static libraries from new_sdk that match the CPU family (and boards).
       This part is user-defined: adapt your internal naming scheme as needed.
    3. Copy the .h (and license, readme, etc.) files from new_sdk to new_export,
       preserving directory structure, skipping excluded directories except fully
       copying 'pack' and 'cordio'.
    """

    # Prepare new_export/src
    new_src_dir = os.path.join(new_export, "src")
    safe_makedirs(new_src_dir)

    old_src_dir = os.path.join(old_export, "src")

    # Gather all files in old_export/src
    old_export_src_files = []
    for root, dirs, files in os.walk(old_src_dir):
        for f in files:
            fullpath = os.path.join(root, f)
            old_export_src_files.append(fullpath)

    # For each file in old_export/src, find the "original" in old_sdk (exclude old_sdk/src).
    # Then pick the best match in new_sdk (exclude new_sdk/src). Compare and copy accordingly.
    for old_src_file in old_export_src_files:
        filename = os.path.basename(old_src_file)

        # 1) Find matching file(s) in old_sdk (excluding old_sdk/src):
        old_sdk_candidates = find_all_files_with_name(
            root_dir=old_sdk,
            filename=filename,
            exclude_subdir=os.path.join(old_sdk, "src")
        )

        # If none found, skip
        if not old_sdk_candidates:
            print(f"**** File {old_src_file} not found in old SDK outside of src/ - skipping")
            continue

        # If multiple matches, attempt an auto-selection or user prompt
        # Check if it might be a FreeRTOS file:
        is_freertos = ("FreeRTOSConfig" in filename or "FreeRTOS" in filename or "rtos.h" in filename)
        old_sdk_file = select_best_candidate(old_sdk_candidates, cpu_family, is_freertos=is_freertos)

        # 2) Compare old_export/src file to old_sdk_file
        # If they are the same, we’ll copy from the "new_sdk"
        # If they differ, then compare old_sdk_file to new_sdk_file.
        #   If old_sdk_file is same as new_sdk_file, we copy from old_export
        #   Otherwise produce conflict
        if not old_sdk_file:
            # Could happen if user canceled?
            print(f"Skipping {old_src_file} because no valid old_sdk_file was chosen.")
            continue

        # Now find the matching new_sdk file (excluding new_sdk/src)
        new_sdk_candidates = find_all_files_with_name(
            root_dir=new_sdk,
            filename=filename,
            exclude_subdir=os.path.join(new_sdk, "src")
        )
        new_sdk_file = select_best_candidate(new_sdk_candidates, cpu_family, is_freertos=is_freertos) \
                       if new_sdk_candidates else None

        # Where to place the final file?
        # We replicate the subdirectory structure from old_export's src into new_export's src.
        rel_path_from_old_src = os.path.relpath(old_src_file, old_src_dir)
        dest_abs_path = os.path.join(new_src_dir, os.path.dirname(rel_path_from_old_src))

        # Ensure the directory structure exists
        safe_makedirs(dest_abs_path)

        # Compare old_export vs old_sdk
        if filecmp.cmp(old_src_file, old_sdk_file, shallow=False):
            # They are the same => copy from new_sdk_file if available
            # If new_sdk_file doesn't exist, just skip or warn
            if new_sdk_file and os.path.exists(new_sdk_file):
                copy_file_or_warn(new_sdk_file, os.path.join(dest_abs_path, filename))
            else:
                # No new version found - might be a newly removed file
                print(f"Warning: {filename} not found in new SDK, skipping copy.")
        else:
            # They differ => compare old_sdk_file vs new_sdk_file
            if not new_sdk_file or not os.path.exists(new_sdk_file):
                # There's no new_sdk_file at all; we have no choice but to copy from old_export or skip.
                # Potentially a conflict, but let's default to copying the old_export version
                print(f"**** No new match for {filename}. Copying from old_export.")
                copy_file_or_warn(old_src_file, os.path.join(dest_abs_path, filename))
            else:
                # new_sdk_file exists => check if old_sdk_file == new_sdk_file
                if filecmp.cmp(old_sdk_file, new_sdk_file, shallow=False):
                    # They are the same => copy from old_export
                    copy_file_or_warn(old_src_file, os.path.join(dest_abs_path, filename))
                else:
                    # Conflict => copy both
                    copy_conflict_files(old_src_file, new_sdk_file, dest_abs_path)

    # 2) Create new_export/lib and populate from new_sdk
    print("\nNow creating the lib structure...\n")
    new_lib_dir = os.path.join(new_export, "lib")
    safe_makedirs(new_lib_dir)

    # Your logic for discovering libraries in new_sdk might vary. Typically, you might look in:
    #   boards/apollo5b_eb/bsp/gcc/bin/libam_bsp.a 
    #   mcu/apollo5b/hal/mcu/gcc/bin/libam_hal.a
    #   ... etc ...
    # Then replicate them in new_export/lib/apollo5b/eb, etc.
    # Here we do a simple example scanning for *.a or *.lib that have the CPU name.
    # You can expand to handle multiple variants, compilers, etc. as needed.

    for root, dirs, files in os.walk(new_sdk):
        for f in files:
            if f.endswith(".a") or f.endswith(".lib"):
                # Check if the path contains the CPU name
                # or path includes boards, etc.
                lower_path = root.lower()
                if cpu_family.lower() in lower_path:
                    # Derive a relative path under new_export/lib
                    # E.g. if root = .../boards/apollo5b_eb_revb/bsp/gcc/bin
                    # we want to create .../lib/apollo5b/eb_revb/gcc/bin
                    # after "boards/" or "mcu/" we replicate the structure.
                    rel = os.path.relpath(root, new_sdk)
                    # We'll just nest it under new_export/lib/<rel>
                    lib_dest_dir = os.path.join(new_lib_dir, rel)
                    safe_makedirs(lib_dest_dir)
                    src_file = os.path.join(root, f)
                    dest_file = os.path.join(lib_dest_dir, f)
                    print(f"Copying library {src_file} -> {dest_file}")
                    shutil.copy2(src_file, dest_file)

    # 3) Copy .h, license, readme files from new_sdk to new_export, skipping certain directories
    #    but fully copying 'pack' and 'cordio' if present.
    print("\nCopying headers and license/readme files from new SDK, preserving structure...\n")
    excluded_dirs = {"ambiq_ble", "debugger_updates", "docs", "tools", "examples", "tests"}
    def should_skip_dir(dname):
        # If directory name is in excluded_dirs, skip entirely
        if dname in excluded_dirs:
            return True
        return False

    def copy_extra_files(src_root, dst_root):
        for dirpath, dirnames, filenames in os.walk(src_root):
            # Figure out subdir relative path
            rel_subdir = os.path.relpath(dirpath, src_root)
            if rel_subdir == ".":
                rel_subdir = ""

            parts = rel_subdir.split(os.sep)
            # If any of the path parts is in excluded_dirs, skip
            if any(should_skip_dir(p) for p in parts):
                continue

            # If 'pack' or 'cordio' is in the path, copy everything
            if "pack" in parts or "cordio" in parts:
                # Copy everything in this dir
                for f in filenames:
                    s = os.path.join(dirpath, f)
                    rel = os.path.relpath(s, src_root)
                    d = os.path.join(dst_root, rel)
                    safe_makedirs(os.path.dirname(d))
                    shutil.copy2(s, d)
                continue

            # Otherwise, copy only .h, .hpp, license, readme, .md, etc.
            for f in filenames:
                low_f = f.lower()
                if (f.endswith(".h") or f.endswith(".hpp") or
                    "license" in low_f or "readme" in low_f or
                    f.endswith(".md")):
                    s = os.path.join(dirpath, f)
                    rel = os.path.relpath(s, src_root)
                    d = os.path.join(dst_root, rel)
                    safe_makedirs(os.path.dirname(d))
                    shutil.copy2(s, d)

    copy_extra_files(new_sdk, new_export)

    print("\nDone exporting AmbiqSuite!")
    print(f"New export is at: {new_export}")
    print("Check for any *.old/*.new conflicts in src/. Merge as necessary.")


###############################################################################
# Main CLI
###############################################################################
def main():
    parser = argparse.ArgumentParser(description="AmbiqSuite Export Script")
    parser.add_argument("--cpu", required=True,
                        help="CPU family (apollo3, apollo4, apollo5, etc.)")
    parser.add_argument("--old-sdk", required=True,
                        help="Path to original 'full' AmbiqSuite SDK used for the old export")
    parser.add_argument("--old-export", required=True,
                        help="Path to the old (already exported) AmbiqSuite used as a template")
    parser.add_argument("--new-sdk", required=True,
                        help="Path to the new AmbiqSuite SDK to export")
    parser.add_argument("--new-export", required=True,
                        help="Path where the new minimal export will be created")
    args = parser.parse_args()

    cpu_family = args.cpu
    old_sdk = os.path.abspath(args.old_sdk)
    old_export = os.path.abspath(args.old_export)
    new_sdk = os.path.abspath(args.new_sdk)
    new_export = os.path.abspath(args.new_export)

    print("=== AmbiqSuite Export Script ===")
    print(f"CPU Family: {cpu_family}")
    print(f"Old SDK: {old_sdk}")
    print(f"Old Export: {old_export}")
    print(f"New SDK: {new_sdk}")
    print(f"New Export: {new_export}")

    export_ambiqsuite(cpu_family, old_sdk, old_export, new_sdk, new_export)


if __name__ == "__main__":
    main()
