# AGENTS.md

This document defines working rules for contributors (human and AI) in this repository.
The goal is to reduce regressions, stop accidental behavior changes, and keep AutoDeploy maintainable.

## 1) Core Principles

1. Make behavior explicit, not implicit.
2. Prefer small, reviewable commits with clear intent.
3. Keep runtime-specific logic separated from generic flow.
4. Never change generated files without changing their source templates/generators.
5. Every bug fix should include at least one reproducible validation step.

## 2) Repository Boundaries

1. `neuralspot/`, `extern/`, `tools/`, `docs/`, `tests/` are in-scope for normal changes.
2. Do not commit temporary build outputs under `build/` or generated project artifacts under `projects/autodeploy/*` unless the change explicitly targets templates or generated-output verification.

## 3) AutoDeploy (AD) Scope and Design Rules

AD currently does multiple jobs. When touching AD:

1. Keep profiling paths stable first (compile/deploy/invoke/stats/power).
2. Treat optional flows (`--create-aot-profile`, `--create-library`, `--create-ambiqsuite-example`) as isolated features. A failure in optional flow must not silently corrupt base profiling flow.
3. Do not assume AOT and TFLM share identical layer identity/count semantics.
4. Do not hardcode runtime/version behavior in app C/C++ code when module configuration already owns it.
5. Prefer one source of truth for runtime capabilities:
   - runtime metadata from selected TF/AOT module
   - explicit validator protocol fields

## 4) Generated Code Contract

1. Source-of-truth hierarchy:
   - Templates in `tools/autodeploy/templates/**`
   - Python generators in `tools/autodeploy/*.py`
   - Generated outputs in `projects/autodeploy/**`
2. Never hand-edit generated files as a final fix.
3. If you patch generated output to debug:
   - reproduce in template/generator
   - regenerate
   - verify generated diff matches intended behavior
4. Serialization rules:
   - Generated C headers must only contain valid C literals
   - No Python/NumPy tokens (`np.*`, `array(...)`, `dtype=...`) in emitted C

## 5) PMU/Stats Protocol Safety Rules

1. Keep stats chunking and tensor chunking state separate.
2. Chunk helpers must be side-effect clear:
   - `done` checks should not mutate state
   - `advance` should be the only progression mutation
3. Host-side fetch logic must be tolerant of transitional packets and provide actionable errors.
4. Full PMU capture should be fixed-width per row for a given run. Treat ragged rows as protocol/data-shape defects.
5. If runtime cannot provide PMU stream, fail with explicit reason instead of generic parse mismatch.
6. Maintain consistent preamble indexing between EVB and host parsers. Document index mapping in code comments where parsed.

## 6) Runtime and Version Management

1. Keep TF runtime/version ownership in `extern/tensorflow/<version>/module.mk` and related metadata.
2. Avoid source-level hardcoded compatibility flags when module `DEFINES` can control behavior.
3. When removing an old runtime version:
   - remove directory
   - remove all references in docs/scripts/config defaults
   - run grep verification to ensure no stale pointers remain
4. Optional private/runtime plugins (e.g., HeliaAOT) must not require dirtying repo metadata for normal use.

## 7) Build System Rules

1. Top-level Makefile should allow module-level defines to propagate as intended.
2. Do not introduce hidden compile-flag dependencies by local manual defines in unrelated files.
3. Any build-system change must include:
   - one AD flow validation
   - one non-AD target validation

## 8) Testing and Validation Expectations

For behavior changes in AD/profiling:

1. Baseline (no AOT):
   - compile/deploy/invoke passes
   - stats CSV/XLSX generated
2. AOT requested but unavailable:
   - graceful disable path and clear warning
3. AOT available:
   - AOT run executes and reports separately
4. Full PMU capture:
   - no protocol crash
   - per-layer export succeeds
5. If changing serialization/generation:
   - generated C compiles for at least one int model and one float model

Record exact command(s) used in PR description.

## 9) Commit and PR Hygiene

1. Group commits by purpose:
   - protocol fix
   - serializer fix
   - docs update
2. Avoid mixed commits combining refactor + functional change + formatting noise.
3. Prefer conventional commit prefixes (`fix:`, `feat:`, `docs:`, `refactor:`, `chore:`), especially for release automation.
4. PR description should include:
   - what changed
   - why
   - risk areas
   - validation commands and outcomes

## 10) Release Automation Notes

1. If release workflows depend on conventional commits, ensure merge strategy preserves releasable commit messages.
2. Verify release trigger expectations before merging (squash/rebase/merge-commit behavior can change detected commit subjects).

## 11) Documentation Expectations

1. Update docs when behavior or workflow changes.
2. Keep user commands copy-paste ready.
3. For hardware hookup instructions (e.g., Joulescope/AP510), provide explicit pin mapping tables and source doc location.

## 12) When in Doubt

1. Prioritize correctness and debuggability over convenience.
2. Prefer explicit failures over silent fallback when data integrity is at risk.
3. If a change impacts both EVB and host protocol parsing, review both sides together before merging.
