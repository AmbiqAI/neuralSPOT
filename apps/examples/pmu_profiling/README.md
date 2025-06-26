# PMU Profiling Utlity Example
The example demonstrates several PMU profiling facilities offered by neuralSPOT:
* Measuring time using the Tick Timer
* Basic PMU Counter Collection
* Full PMU (performance management unit) collection (CortexM55-based platforms)

The example uses one neuralSPOT's MFCC feature extractor as an example algorithm to be profiled.

---

## Hardware Requirements
Apollo5+ EVB

## Building and Running the Example
```bash
$> make EXAMPLE=examples/pmu_profiling deploy
$> make view
```

## Results
The SWO output can be divided into 3 sections:
1. Time measured for a single run
2. PMU counters (single run showing 4 PMU event counters)
3. Full PMU counters collected via repeated runs