# Algo Experiments
This directory contains variant implementations of an original algorithm.

# Compiling
Only one of the variants should be compiled - choosing which is compiled is hacky: change the file extension to ".c" and the file extension of the other ones to ".c.dnc" (do not compile).

Algo.c contains the main function. The variants are defined in orig-algorithm.c, adam-algorithm.c, etc.

# Running
The firmware will first run a PMU characterization of 200 runs of the algorithm, then go into 'interactive' mode meant to run the algorithm periodically for measurement using a joulescope.

Press BTN0 until the algorithm is running once every second, and measure the duration of the compute burst on the joulescope waveform.