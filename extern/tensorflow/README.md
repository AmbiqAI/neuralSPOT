# TensorFlow Runtime Variants in neuralSPOT

This directory contains packaged TensorFlow runtime variants consumed by neuralSPOT builds.

## Current workflow

We no longer build/package these runtimes manually in neuralSPOT.

Runtime packaging is automated in HeliaRT release pipelines. neuralSPOT only imports released runtime bundles and wires them into build config.

In practice:
1. Take a new HeliaRT release bundle.
2. Add the new runtime variant directory under `extern/tensorflow/`.
3. Ensure `module.mk` points to the packaged libs/includes for that variant.
4. Optionally update defaults (for example `tools/ns_platform.yaml`) to select the new version by default.
5. Validate build + autodeploy for the target platforms.

## Variant naming

Typical variant names in this repo:
- `helia_rt_vX_Y_Z` for HeliaRT runtime variants.
- `ns_tflm_vX_Y_Z` for TFLM-compatible variants from the same release stream.

Each variant directory is self-contained and includes:
- `module.mk`
- `lib/` prebuilt runtime archives
- required headers/minimal source tree expected by neuralSPOT build system

## Notes

- neuralSPOT should treat HeliaRT releases as the source of truth for runtime packaging.
- If runtime internals/API layout changes across versions, handle compatibility in neuralSPOT glue/build macros (for example profiler-related defines), not by rebuilding runtime artifacts locally.
