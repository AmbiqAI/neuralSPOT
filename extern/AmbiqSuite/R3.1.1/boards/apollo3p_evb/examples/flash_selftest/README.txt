Name:
=====
 flash_selftest


Description:
============
 An example to test all onboard flash.


Purpose:
This example runs a series of test patterns on all instances of the device
flash.  It performs many of the same tests that the hardware BIST (built
in self test) uses at production test.

Results are saved in coded form to a defined data word (g_result) which
tracks any failure.  Further, g_result can be given an absolute address
location so that an outside system will know where to find the results.

Results output is also configurable such that the simplified results
(pass/fail, done, etc.) can be output to GPIO bits.

The test must be loaded and executed in SRAM.  Therefore a J-Link Commander
batch file is provided here to assist with that.
Alternatively the program can be loaded with a debugger and run from there.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
Using the J-Link Commander batch file on an Apollo3 Blue EVB:
- The Commander script file is one of either selftest_commander_gcc.jlink,
selftest_commander_iar.jlink, or selftest_commander_keil.jlink.
- Requires Segger J-Link v6.60 or later.
- As shipped in the SDK, the J-Link Commander scripts should be correctly
configured to run the given binary. If the test is modified, the commander
script may need an update of the SP and PC.  The first two word values in
the vector table of your compiled binary determine the required values.
- Use the following command line at a DOS prompt.
jlink -CommanderScript selftest_commander_xxx.jlink
- The flash self test stores results to address 0x10030000.
0xFAE00000 = Pass, the flash tested good.
0xFAE0xxxx = Fail, where xxxx is a failure code.
- If USE_TIMER is enabled, the run time of the selftest is stored in two
words at 0x10030004 and 0x1003008.  The first word is the whole number
of seconds, the second is the fractional part to 3 decimals.  Therefore
the two values show the total run time in the form:  ss.fff



******************************************************************************


