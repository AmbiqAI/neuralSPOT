#!/usr/bin/env python3
"""
ambiqsuite_min_integrate.py

Build a minimal AmbiqSuite from a full upstream release and an existing
neuralSPOT "minimal" port. It attempts to rebase your integration changes
in src/** onto a new AmbiqSuite release, copy over libraries, prune sources,
and emit a report with diffs and conflict markers where needed.

Now with progress prints to make each step visible.
"""

from __future__ import annotations
import argparse, hashlib, json, os, re, shutil, sys
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import List, Optional, Tuple, Dict
import difflib

# ------------------------ Progress ------------------------

class Progress:
    def __init__(self, enabled: bool = True):
        self.enabled = enabled

    def log(self, msg: str):
        if self.enabled:
            print(msg, flush=True)

    def step(self, prefix: str, i: int, n: int, tail: str = ""): 
        if self.enabled:
            pct = (i / n * 100.0) if n else 100.0
            if tail:
                print(f"[{prefix}] {i}/{n} ({pct:5.1f}%) {tail}", flush=True)
            else:
                print(f"[{prefix}] {i}/{n} ({pct:5.1f}%)", flush=True)

# ------------------------ Helpers ------------------------

def read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="ignore").replace('\r\n', '\n').replace('\r', '\n')
    except Exception:
        return ""

def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")

def sha1(s: str) -> str:
    return hashlib.sha1(s.encode('utf-8', errors='ignore')).hexdigest()

def list_files(root: Path, exts: Tuple[str, ...]) -> List[Path]:
    return [p for p in root.rglob("*") if p.is_file() and p.suffix.lower() in exts]

def relpath(p: Path, base: Path) -> str:
    try:
        return str(p.relative_to(base).as_posix())
    except Exception:
        return str(p.as_posix())

def is_text_like(path: Path) -> bool:
    # Minimal heuristic for source-like text. Extend if needed.
    return path.suffix.lower() in {".c", ".h", ".cpp", ".hpp", ".S", ".ld", ".mk", ".txt", ".md", ".svd"}

def normalize_for_compare(s: str) -> str:
    # Strip trailing spaces and obvious noise for similarity calculation
    lines = [re.sub(r"\s+$", "", line) for line in s.splitlines()]
    return "\n".join(lines).strip()

def similarity(a: str, b: str) -> float:
    a_norm = normalize_for_compare(a)
    b_norm = normalize_for_compare(b)
    if not a_norm and not b_norm:
        return 1.0
    return difflib.SequenceMatcher(a=a_norm, b=b_norm).ratio()

def unified_diff(a: str, b: str, fromfile: str, tofile: str) -> str:
    return "".join(difflib.unified_diff(
        a.splitlines(keepends=True),
        b.splitlines(keepends=True),
        fromfile=fromfile, tofile=tofile, n=3
    ))

# ------------------ Candidate Search ---------------------

def build_name_index(root: Path, exts: Tuple[str, ...], prog: Progress, label: str) -> Dict[str, List[Path]]:
    prog.log(f"[INDEX] Building filename index for {label} under: {root}")
    count = 0
    index: Dict[str, List[Path]] = {}
    files = list_files(root, exts)
    total = len(files)
    for p in files:
        index.setdefault(p.name, []).append(p)
        count += 1
        if count % 2000 == 0 or count == total:
            prog.step("INDEX", count, total, tail=p.name)
    prog.log(f"[INDEX] {label}: indexed {total} files")
    return index

def best_match(target: Path, upstream_root: Path, name_index: Dict[str, List[Path]], min_ratio: float) -> Tuple[Optional[Path], float]:
    # Prefer same basename; if none, allow any ext-equal file by content (fallback).
    content = read_text(target)
    candidates = name_index.get(target.name, [])
    best_p, best_r = None, -1.0
    if not candidates:
        # broaden search: same extension anywhere (expensive if huge trees)
        candidates = list_files(upstream_root, (target.suffix.lower(),))

    for c in candidates:
        r = similarity(content, read_text(c))
        if r > best_r:
            best_p, best_r = c, r

    if best_r >= min_ratio:
        return best_p, best_r
    return None, 0.0

# --------------- Merge (3-way, conservative) --------------

@dataclass
class MergeResult:
    merged: str
    had_conflicts: bool

def merge_3way(base: str, local: str, remote: str) -> MergeResult:
    """
    Simple 3-way merge with conflict markers.
    """
    base_lines = base.splitlines(keepends=True)
    local_lines = local.splitlines(keepends=True)
    remote_lines = remote.splitlines(keepends=True)

    sm_local = difflib.SequenceMatcher(None, base_lines, local_lines)
    sm_remote = difflib.SequenceMatcher(None, base_lines, remote_lines)

    merged: List[str] = []
    had_conflicts = False

    ops_local = sm_local.get_opcodes()
    ops_remote = sm_remote.get_opcodes()

    pl = pr = 0
    bpos = 0
    len_base = len(base_lines)

    while pl < len(ops_local) or pr < len(ops_remote):
        if pl < len(ops_local):
            tag_l, bl1, bl2, jl1, jl2 = ops_local[pl]
        else:
            tag_l, bl1, bl2, jl1, jl2 = ('equal', len_base, len_base, 0, 0)

        if pr < len(ops_remote):
            tag_r, br1, br2, jr1, jr2 = ops_remote[pr]
        else:
            tag_r, br1, br2, jr1, jr2 = ('equal', len_base, len_base, 0, 0)

        next_b = min(bl2 if pl < len(ops_local) else len_base,
                     br2 if pr < len(ops_remote) else len_base)

        cur_bl1, cur_bl2 = max(bpos, bl1), min(next_b, bl2)
        cur_br1, cur_br2 = max(bpos, br1), min(next_b, br2)

        local_changed = tag_l != 'equal' and cur_bl1 < cur_bl2
        remote_changed = tag_r != 'equal' and cur_br1 < cur_br2

        if not local_changed and not remote_changed:
            merged.extend(base_lines[bpos:next_b])
        elif local_changed and not remote_changed:
            offset_l = cur_bl1 - bl1
            l_slice = local_lines[jl1 + offset_l : jl1 + offset_l + (cur_bl2 - cur_bl1)]
            merged.extend(l_slice)
        elif not local_changed and remote_changed:
            offset_r = cur_br1 - br1
            r_slice = remote_lines[jr1 + offset_r : jr1 + offset_r + (cur_br2 - cur_br1)]
            merged.extend(r_slice)
        else:
            offset_l = cur_bl1 - bl1
            l_slice = local_lines[jl1 + offset_l : jl1 + offset_l + (cur_bl2 - cur_bl1)]
            offset_r = cur_br1 - br1
            r_slice = remote_lines[jr1 + offset_r : jr1 + offset_r + (cur_br2 - cur_br1)]
            if l_slice == r_slice:
                merged.extend(l_slice)
            else:
                had_conflicts = True
                merged.append("<<<<<<< LOCAL\n")
                merged.extend(l_slice)
                merged.append("=======\n")
                merged.extend(r_slice)
                merged.append(">>>>>>> REMOTE\n")

        bpos = next_b
        if pl < len(ops_local) and bpos >= bl2:
            pl += 1
        if pr < len(ops_remote) and bpos >= br2:
            pr += 1

    if bpos < len_base:
        merged.extend(base_lines[bpos:])

    return MergeResult("".join(merged), had_conflicts)


# ------------------------ Main Flow ------------------------

@dataclass
class FileDecision:
    src_rel: str
    upstream_new_rel: Optional[str]
    upstream_old_rel: Optional[str]
    similarity_new: float
    action: str              # 'copied_local', 'copied_remote', 'merged', 'conflict', 'not_found'
    notes: str
    had_conflicts: bool

def copy_upstream_tree(new_upstream: Path, out_root: Path, prog: Progress):
    if out_root.exists() and any(out_root.iterdir()):
        prog.log(f"[COPY] Output dir not empty, skipping full copy: {out_root}")
        return
    prog.log(f"[COPY] Copying new upstream into output: {new_upstream} -> {out_root}")
    shutil.copytree(new_upstream, out_root, dirs_exist_ok=True)
    prog.log("[COPY] Copy complete")

def prune_sources(out_root: Path, keep_list: List[str], prog: Progress) -> int:
    # Remove .c/.cpp everywhere except under third_party/cordio/** and src/**
    keep_set = set(keep_list)
    removed = 0
    prog.log("[PRUNE] Removing .c/.cpp outside src/** and third_party/cordio/** ...")
    for p in out_root.rglob("*"):
        if not p.is_file():
            continue
        rp = relpath(p, out_root)
        if rp in keep_set:
            continue
        if p.suffix.lower() in {".c", ".cpp"}:
            if "/third_party/cordio/" in rp or rp.startswith("third_party/cordio/"):
                continue
            if rp.startswith("src/"):
                continue
            try:
                p.unlink()
                removed += 1
                if removed % 200 == 0:
                    prog.step("PRUNE", removed, removed + 1, tail=rp)
            except Exception as e:
                prog.log(f"[PRUNE] Failed to remove {rp}: {e}")
    prog.log(f"[PRUNE] Removed {removed} files")
    return removed

def mirror_libs(old_port: Path, new_upstream: Path, out_root: Path, report_dir: Path, prog: Progress) -> Dict[str, str]:
    mapping = {}
    old_lib_root = old_port / "lib"
    out_lib_root = out_root / "lib"
    out_lib_root.mkdir(parents=True, exist_ok=True)

    # Index new upstream libraries by name
    prog.log("[LIB] Indexing libraries in new upstream ...")
    new_libs = {}
    new_candidates = [p for p in new_upstream.rglob("*") if p.is_file() and p.suffix.lower() in {".a", ".lib"}]
    for i, p in enumerate(new_candidates, 1):
        new_libs.setdefault(p.name, []).append(p)
        if i % 200 == 0 or i == len(new_candidates):
            prog.step("LIB-IDX", i, len(new_candidates), tail=p.name)

    old_libs = [p for p in old_lib_root.rglob("*") if p.is_file() and p.suffix.lower() in {".a", ".lib"}]
    prog.log(f"[LIB] Mirroring {len(old_libs)} libraries into output ...")
    for i, lib in enumerate(old_libs, 1):
        rel_lib = relpath(lib, old_lib_root)
        target = out_lib_root / rel_lib
        target.parent.mkdir(parents=True, exist_ok=True)
        candidates = new_libs.get(lib.name, [])
        if candidates:
            src = candidates[0]
            shutil.copy2(src, target)
            mapping[rel_lib] = f"NEW:{relpath(src, new_upstream)}"
            prog.step("LIB", i, len(old_libs), tail=f"{rel_lib} -> NEW")
        else:
            shutil.copy2(lib, target)
            mapping[rel_lib] = "FALLBACK_OLD_LIB"
            prog.step("LIB", i, len(old_libs), tail=f"{rel_lib} -> OLD (fallback)")

    write_text(report_dir / "libs_mapping.json", json.dumps(mapping, indent=2))
    prog.log("[LIB] Library mirroring complete")
    return mapping

def ensure_clean_dir(p: Path):
    if p.exists():
        shutil.rmtree(p)
    p.mkdir(parents=True, exist_ok=True)

def main():
    ap = argparse.ArgumentParser(description="Rebase/minimize AmbiqSuite for neuralSPOT.")
    ap.add_argument("--old-port", type=Path, required=True, help="Existing minimal port root (AmbiqSuite/<old_version>).")
    ap.add_argument("--new-upstream", type=Path, required=True, help="Full upstream AmbiqSuite root (new version).")
    ap.add_argument("--out", type=Path, required=True, help="Output minimal AmbiqSuite root (new version).")
    ap.add_argument("--old-upstream", type=Path, required=False, help="(Optional) Full upstream AmbiqSuite root (old version).")
    ap.add_argument("--keep-list", type=Path, required=False, help="Optional JSON list of paths to keep (relative to out root)." )
    ap.add_argument("--hints", type=Path, required=False, help="Optional JSON map: src_rel -> upstream_rel to override matching.")
    ap.add_argument("--copy-upstream", choices=["all", "empty"], default="all")
    ap.add_argument("--similarity", type=float, default=0.55, help="Min similarity for accepting a closest match.")
    ap.add_argument("--no-progress", action="store_true", help="Disable progress prints.")
    args = ap.parse_args()

    prog = Progress(enabled=not args.no_progress)

    old_port = args.old_port.resolve()
    new_upstream = args.new_upstream.resolve()
    out_root = args.out.resolve()
    old_upstream = args.old_upstream.resolve() if args.old_upstream else None

    # Summary header
    prog.log("=== AmbiqSuite Minimal Integrator ===")
    prog.log(f" old-port     : {old_port}")
    prog.log(f" new-upstream : {new_upstream}")
    prog.log(f" out          : {out_root}")
    prog.log(f" old-upstream : {old_upstream if old_upstream else '(none)'}")
    prog.log(f" copy-upstream: {args.copy_upstream}")
    prog.log(f" similarity   : {args.similarity}")
    prog.log("-------------------------------------")

    if args.copy_upstream == "all":
        copy_upstream_tree(new_upstream, out_root, prog)
    else:
        out_root.mkdir(parents=True, exist_ok=True)
        prog.log("[COPY] Skipping upstream copy (empty scaffold mode)")

    report_dir = out_root / "_integration_report"
    report_diffs = report_dir / "diffs"
    report_srcmap = report_dir / "src_map"
    report_dir.mkdir(parents=True, exist_ok=True)
    report_diffs.mkdir(parents=True, exist_ok=True)
    report_srcmap.mkdir(parents=True, exist_ok=True)

    keep_list = []
    if args.keep_list and args.keep_list.exists():
        try:
            keep_list = json.loads(args.keep_list.read_text())
            prog.log(f"[KEEP] Loaded keep-list with {len(keep_list)} entries")
        except Exception as e:
            prog.log(f"[KEEP] Failed to load keep-list: {e}")

    hints: Dict[str, str] = {}
    if args.hints and args.hints.exists():
        try:
            hints = json.loads(args.hints.read_text())
            prog.log(f"[HINTS] Loaded {len(hints)} path hints")
        except Exception as e:
            prog.log(f"[HINTS] Failed to load hints: {e}")

    # Build upstream name indices
    name_index_new = build_name_index(new_upstream, (".c", ".h", ".cpp", ".hpp"), prog, "NEW")
    name_index_old = build_name_index(old_upstream, (".c", ".h", ".cpp", ".hpp"), prog, "OLD") if old_upstream else {}

    # Mirror libs
    mirror_libs(old_port, new_upstream, out_root, report_dir, prog)

    # Rebase src/*
    src_root_old = old_port / "src"
    src_root_out = out_root / "src"
    src_files = [p for p in src_root_old.rglob("*") if p.is_file() and is_text_like(p)]
    prog.log(f"[SRC] Processing {len(src_files)} files from old-port/src/**")
    decisions: List[FileDecision] = []

    for i, src in enumerate(sorted(src_files), 1):
        src_rel = relpath(src, src_root_old)
        local = read_text(src)

        # Hint override?
        new_up_rel = None
        new_sim = 0.0
        if src_rel in hints:
            cand = new_upstream / hints[src_rel]
            if cand.exists():
                new_up_rel = relpath(cand, new_upstream)
                new_sim = similarity(local, read_text(cand))
                new_cand = cand
            else:
                new_cand, new_sim = best_match(src, new_upstream, name_index_new, args.similarity)
                new_up_rel = relpath(new_cand, new_upstream) if new_cand else None
        else:
            new_cand, new_sim = best_match(src, new_upstream, name_index_new, args.similarity)
            new_up_rel = relpath(new_cand, new_upstream) if new_cand else None

        old_up_rel = None
        base_content = ""
        if old_upstream:
            # try best match for base too (or assume same hint)
            if src_rel in hints:
                base_cand = old_upstream / hints[src_rel]
                if not base_cand.exists():
                    base_cand, _ = best_match(src, old_upstream, name_index_old, 0.0)
            else:
                base_cand, _ = best_match(src, old_upstream, name_index_old, 0.0)
            if base_cand:
                old_up_rel = relpath(base_cand, old_upstream)
                base_content = read_text(base_cand)

        out_path = src_root_out / src_rel
        out_path.parent.mkdir(parents=True, exist_ok=True)

        if not new_cand:
            write_text(out_path, local)
            decisions.append(FileDecision(src_rel, None, old_up_rel, 0.0, "copied_local", "No upstream match found", False))
            prog.step("SRC", i, len(src_files), tail=f"{src_rel} -> copied_local (no upstream match)")
            continue

        remote = read_text(new_cand)
        # Short-circuit: if local == old upstream base (no local mods), copy REMOTE as-is.
        if old_upstream and base_content:
            if normalize_for_compare(local) == normalize_for_compare(base_content):
                write_text(out_path, remote)
                decisions.append(FileDecision(src_rel, new_up_rel, old_up_rel, new_sim,
                                              "copied_remote_unmodified_local",
                                              "Local identical to old upstream; use new upstream verbatim",
                                              False))
                prog.step("SRC", i, len(src_files), tail=f"{src_rel} -> copied_remote (local==old_upstream)")
                map_entry = {
                    "src_rel": src_rel,
                    "new_upstream_rel": new_up_rel,
                    "old_upstream_rel": old_up_rel,
                    "similarity_new": new_sim
                }
                write_text((report_dir / "src_map" / f"{src_rel.replace('/', '__')}.json"), json.dumps(map_entry, indent=2))
                continue

        if old_upstream and base_content:
            mr = merge_3way(base_content, local, remote)
            write_text(out_path, mr.merged)
            action = "merged_conflict" if mr.had_conflicts else "merged"
            decisions.append(FileDecision(src_rel, new_up_rel, old_up_rel, new_sim, action, "3-way merge", mr.had_conflicts))
            write_text((report_dir / "diffs" / f"{src_rel.replace('/', '__')}.diff"),
                       unified_diff(local, mr.merged, f"LOCAL:{src_rel}", f"MERGED:{src_rel}"))
            status = "MERGED*" if mr.had_conflicts else "MERGED"
            prog.step("SRC", i, len(src_files), tail=f"{src_rel} -> {status} (sim {new_sim:.3f})")
        else:
            if new_sim >= 0.98:
                write_text(out_path, remote)
                decisions.append(FileDecision(src_rel, new_up_rel, None, new_sim, "copied_remote", "Remote is ~identical", False))
                prog.step("SRC", i, len(src_files), tail=f"{src_rel} -> copied_remote (sim {new_sim:.3f})")
            else:
                mr = merge_3way(remote, local, remote)
                write_text(out_path, mr.merged)
                action = "conflict" if mr.had_conflicts else "copied_local"
                notes = "2-way fallback (no base); conflicts mark local vs remote differences"
                decisions.append(FileDecision(src_rel, new_up_rel, None, new_sim, action, notes, mr.had_conflicts))
                write_text((report_dir / "diffs" / f"{src_rel.replace('/', '__')}.diff"),
                           unified_diff(remote, mr.merged, f"REMOTE:{new_up_rel}", f"OUT:{src_rel}"))
                status = "CONFLICT" if mr.had_conflicts else "copied_local"
                prog.step("SRC", i, len(src_files), tail=f"{src_rel} -> {status} (sim {new_sim:.3f})")

        # Save src mapping
        map_entry = {
            "src_rel": src_rel,
            "new_upstream_rel": new_up_rel,
            "old_upstream_rel": old_up_rel,
            "similarity_new": new_sim
        }
        write_text((report_dir / "src_map" / f"{src_rel.replace('/', '__')}.json"), json.dumps(map_entry, indent=2))

    # Place a fresh copy of new_upstream's third_party/cordio into out (ensures sources are present)
    cordio_new = new_upstream / "third_party" / "cordio"
    if cordio_new.exists():
        dst = out_root / "third_party" / "cordio"
        if dst.exists():
            prog.log("[CORDIO] Replacing existing third_party/cordio with upstream copy")
            shutil.rmtree(dst)
        else:
            prog.log("[CORDIO] Copying third_party/cordio from upstream")
        shutil.copytree(cordio_new, dst)
        prog.log("[CORDIO] Copy complete")
    else:
        prog.log("[CORDIO] WARNING: No third_party/cordio found in new upstream")

    # Prune sources (keep src/** and third_party/cordio/**)
    removed = prune_sources(out_root, keep_list, prog)

    # Write decision report
    report = [asdict(d) for d in decisions]
    write_text(report_dir / "decisions.json", json.dumps(report, indent=2))

    # Also emit a Markdown summary
    lines = ["# Integration Report\n", "| src_rel | upstream_new_rel | upstream_old_rel | sim_new | action | conflicts | notes |", "|---|---|---|---:|---|---:|---|"]
    for d in decisions:
        lines.append(f"| `{d.src_rel}` | `{d.upstream_new_rel or ''}` | `{d.upstream_old_rel or ''}` | {d.similarity_new:.3f} | {d.action} | {int(d.had_conflicts)} | {d.notes} |")
    write_text(report_dir / "decisions.md", "\n".join(lines))

    prog.log("-------------------------------------")
    prog.log("Done.")
    prog.log(f"Output root: {out_root}")
    prog.log(f"Report dir : {report_dir}")
    prog.log(f"Libraries  : {out_root/'lib'}")
    prog.log(f"Pruned     : {removed} .c/.cpp files outside src/** and third_party/cordio/**")


if __name__ == "__main__":
    main()
