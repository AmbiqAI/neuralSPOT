# TFLM Profiling
The example demonstrates several performance profiling facilities offered by neuralSPOT:
* Basic Per-Layer Performance Profiling
* Full PMU (performance management unit) Per-layer profiling (CortexM55-based platforms)
* GPIO signaling for off-board for use by devices such as a Joulescope
* Ambiq register dump to SWO

The example uses one of Ambiq's larger models (the arrhythmia detector from Heartkit) as the model-under-test

> *NOTE*: This example works for TFLM and HeliaRT

---

## Hardware Requirements
Any Apollo EVB (AP3, AP4, and AP5), though the profiling will be different for each.

## Building and Running the Example
> *IMPORTANT*: Note the make CLI flags (MLDEBUG and MLPROFILE) needed to build this example, which are not enabled by default for the rest of neuralSPOT. These flags do two things:
* Link in the right TFLM/HeliaRT with an enabled microProfiler
* Compile in statistics data structures (these can be fairly large, so they're not compiled in by default)

```bash
$> make clean # gets rid of any non-MLDEBUG/MLPROFILE object files which would otherwise lead to linking errors
$> make EXAMPLE=examples/tflm_profiling MLDEBUG=1 MLPROFILE=1 deploy
$> make view
```

## Results
The SWO output can be divided into 3 sections:
1. Register printouts
2. Basic per-layer counters (single run showing 4 PMU (AP5), or DWT (AP3/AP4) per layer)
3. (AP5 only) Full PMU counters per layer
