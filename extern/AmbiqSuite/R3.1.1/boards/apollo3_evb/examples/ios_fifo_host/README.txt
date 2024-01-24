Name:
=====
 ios_fifo_host


Description:
============
 Example host used for demonstrating the use of the IOS FIFO.


Purpose:
========
This host component runs on one EVB and is used in conjunction with
the companion slave example, ios_fifo, which runs on a second EVB.

The host example uses the ITM SWO to let the user know progress and
status of the demonstration.  The SWO is configured at 1M baud.
The ios_fifo example has no print output.

This example implements the host part of a protocol for data exchange with
an Apollo IO Slave (IOS).  The host sends one byte commands on SPI/I2C by
writing to offset 0x80.

The command is issued by the host to Start/Stop Data accumulation, and also
to acknowledge read-complete of a block of data.

On the IOS side, once it is asked to start accumulating data (using START
command), two CTimer based events emulate sensors sending data to IOS.
When IOS has some data for host, it implements a state machine,
synchronizing with the host.

The IOS interrupts the host to indicate data availability. The host then
reads the available data (as indicated by FIFOCTR) by READing using IOS FIFO
(at address 0x7F).  The IOS keeps accumulating any new data coming in the
background.

Host sends an acknowledgement to IOS once it has finished reading a block
of data initiated by IOS (partitally or complete). IOS interrupts the host
again if and when it has more data for the host to read, and the cycle
repeats - till host indicates that it is no longer interested in receiving
data by sending STOP command.

Additional Information:
=======================
In order to run this example, a slave device (e.g. a second EVB) must be set
up to run the companion example, ios_fifo.  The two boards can be connected
using fly leads between the two boards as follows.

Pin connections for the I/O Master board to the I/O Slave board.
SPI:
HOST (ios_fifo_host)                    SLAVE (ios_fifo)
--------------------                    ----------------
GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
GPIO[5]  IOM0 SPI SCK                   GPIO[0]  IOS SPI SCK
GPIO[7]  IOM0 SPI MOSI                  GPIO[1]  IOS SPI MOSI
GPIO[6]  IOM0 SPI MISO                  GPIO[2]  IOS SPI MISO
GPIO[11] IOM0 SPI nCE                   GPIO[3]  IOS SPI nCE
GND                                     GND

I2C:
HOST (ios_fifo_host)                    SLAVE (ios_fifo)
--------------------                    ----------------
GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
GPIO[5]  IOM0 I2C SCL                   GPIO[0]  IOS I2C SCL
GPIO[6]  IOM0 I2C SDA                   GPIO[1]  IOS I2C SDA
GND                                     GND


******************************************************************************


