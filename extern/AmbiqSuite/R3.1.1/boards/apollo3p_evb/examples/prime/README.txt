Name:
=====
 prime


Description:
============
 Example that displays the timer count on the LEDs.


Purpose:
========
This example consists of a non-optimized, brute-force routine for computing
the number of prime numbers between 1 and a given value, N. The routine
uses modulo operations to determine whether a value is prime or not. While
obviously not optimal, it is very useful for exercising the core.

For this example, N is 100000, for which the answer is 9592.

For Apollo3 at 48MHz, the time to compute the answer for Keil and IAR:
IAR v8.11.1:        1:43.
Keil ARMCC 4060528: 1:55.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
The goal of this example is to measure current consumption while the core
is working to compute the answer. Power and energy can then be derived
knowing the current and run time.

The example prints an initial banner to the UART port.  After each prime
loop, it enables the UART long enough to print the answer, disables the
UART and starts the computation again.

Text is output to the UART at 115,200 BAUD, 8 bit, no parity.
Please note that text end-of-line is a newline (LF) character only.
Therefore, the UART terminal must be set to simulate a CR/LF.

Note: For minimum power, disable the printing by setting PRINT_UART to 0.

The prime_number() routine is open source and is used here under the
GNU LESSER GENERAL PUBLIC LICENSE Version 3, 29 June 2007.  Details,
documentation, and the full license for this routine can be found in
the third_party/prime_mpi/ directory of the SDK.



******************************************************************************


