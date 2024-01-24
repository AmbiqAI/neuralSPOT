Name:
=====
 hello_fault


Description:
============
 A simple example that causes a hard-fault.


Purpose:
========
This example demonstrates the extended hard fault handler which can
assist the user in decoding a fault condition. The handler pulls the
registers that the Cortex M4 automatically loads onto the stack and
combines them with various other fault information into a single
data structure saved locally.  It can optionally print out the fault
data structure (assuming the stdio printf has previously been enabled
and is still enabled at the time of the fault).

Printing takes place over the ITM at 1M Baud.


******************************************************************************


