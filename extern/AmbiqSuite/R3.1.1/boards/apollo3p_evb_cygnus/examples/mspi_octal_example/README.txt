Name:
=====
 mspi_octal_example


Description:
============
 Example of the MSPI operation with Octal SPI Flash.


Purpose:
========
This example configures an MSPI connected flash device in Octal mode
and performs various operations - verifying the correctness of the same
Operations include - Erase, Write, Read, and XIP

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
this example can work on:
Apollo3p_evb + Cygnus
Target hardware uses 1.8V power supply voltage.
Actual Octal flash on Cygnus board is ATXP128 (Device ID: 0x00A91F) instead of ATXP032
Define ADESTO_ATXP032



******************************************************************************


