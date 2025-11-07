Name:
=====
 coremark_pro


Description:
============
 EEMBC COREMARK PRO advanced processor benchmark suite.




Purpose:
========
This example implements the comprehensive EEMBC CoreMark-PRO
          benchmark suite, designed to evaluate both integer and floating-point
          performance across a wide range of real-world algorithms. It extends
          the basic CoreMark test with more complex workloads suitable for
          higher-performance processors.

Section: Key Features
=====================

 1. Integer Workloads: JPEG, ZIP, XML, SHA-256, CoreMark
 2. Floating-point Tests: FFT, Linear algebra, Neural networks
 3. Memory Subsystem: Tests with various memory footprints
 4. Algorithm Coverage: Diverse computational workloads
 5. Performance Analysis: Detailed scoring per workload

Section: Functionality
======================

 - Comprehensive processor benchmarking
 - Multiple algorithm implementations
 - Memory subsystem stress testing
 - Performance scoring and analysis
 - Platform configuration options

Section: Usage
==============

 1. Configure platform-specific settings
 2. Build and load the application
 3. Monitor results via SWO output
 4. Analyze individual workload scores
 5. Compare results across configurations

Section: Configuration
======================

 - Build options: FreeRTOS vs. Baremetal
 - System configuration settings
 - Output via SWO at 1MHz
 - Memory allocation strategies


 CoreMark(tm) PRO is a comprehensive, processor benchmark that works
 with and enhances the market-proven industry-standard EEMBC CoreMark(tm) benchmark.
 While CoreMark stresses the CPU pipeline, CoreMark-PRO tests the entire
 processor, adding support for multicore technology, a combination
 of integer and floating-point workloads, and data sets for utilizing larger
 memory subsystems. Together, EEMBC CoreMark and CoreMark-PRO provide a standard
 benchmark covering the spectrum from low-end microcontrollers to high-performance
 computing processors.

 The EEMBC CoreMark-PRO benchmark contains five prevalent integer workloads and
 four popular floating-point workloads.

 The integer workloads include:

 * JPEG compression
 * ZIP compression
 * XML parsing
 * SHA-256 Secure Hash Algorithm
 * A more memory-intensive version of the original CoreMark

 The floating-point workloads include:

 * Radix-2 Fast Fourier Transform (FFT)
 * Gaussian elimination with partial pivoting derived from LINPACK
 * A simple neural-net
 * A greatly improved version of the Livermore loops benchmark using the following 24 FORTRAN
   kernels converted to C (all of these reported as a single score of the `loops.c` workload).
   The standard Livermore loops include:
   *   Kernel 1 -- hydro fragment
   *   Kernel 2 -- ICCG excerpt (Incomplete Cholesky Conjugate Gradient)
   *   Kernel 3 -- inner product
   *   Kernel 4 -- banded linear equations
   *   Kernel 5 -- tri-diagonal elimination, below diagonal
   *   Kernel 6 -- general linear recurrence equations
   *   Kernel 7 -- equation of state fragment
   *   Kernel 8 -- ADI integration
   *   Kernel 9 -- integrate predictors
   *   Kernel 10 -- difference predictors
   *   Kernel 11 -- first sum
   *   Kernel 12 -- first difference
   *   Kernel 13 -- 2-D PIC (Particle In Cell)
   *   Kernel 14 -- 1-D PIC (pticle In Cell)
   *   Kernel 15 -- Casual Fortran.
   *   Kernel 16 -- Monte Carlo search loop
   *   Kernel 17 -- implicit, conditional computation
   *   Kernel 18 -- 2-D explicit hydrodynamics fragment
   *   Kernel 19 -- general linear recurrence equations
   *   Kernel 20 -- Discrete ordinates transport, conditional recurrence on xx
   *   Kernel 21 -- matrix*matrix product
   *   Kernel 22 -- Planckian distribution
   *   Kernel 23 -- 2-D implicit hydrodynamics fragment
   *   Kernel 24 -- find location of first minimum in array

 More info may be found at the [EEMBC CoreMark-PRO website](https://www.eembc.org/coremark-pro/).

 The Coremark run begins by first outputing a banner (to the SWO)
 indicating that it has started.

 Text is output to the SWO at 1MHz.


******************************************************************************


