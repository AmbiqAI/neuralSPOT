Name:
=====
 ios_lram_host


Description:
============
 Example host used for demonstrating the use of the IOS LRAM.


Purpose:
========
This host component runs on one EVB and is used in conjunction with
the companion slave example, ios_lram, which runs on a second EVB.

The host example uses the ITM SWO to let the user know progress and
status of the demonstration.  The SWO is configured at 1M baud.

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
In order to run this example, a slave device (e.g. a second EVB) must be set
up to run the companion example, ios_lram.  The two boards can be connected
using fly leads between the two boards as follows.

Pin connections for the I/O Master board to the I/O Slave board.
SPI:
HOST (ios_lram_host)                    SLAVE (ios_lram)
--------------------                    ----------------
GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
GPIO[5]  IOM0 SPI SCK                   GPIO[0]  IOS SPI SCK
GPIO[7]  IOM0 SPI MOSI                  GPIO[1]  IOS SPI MOSI
GPIO[6]  IOM0 SPI MISO                  GPIO[2]  IOS SPI MISO
GPIO[11] IOM0 SPI nCE                   GPIO[3]  IOS SPI nCE
GND                                     GND

I2C:
HOST (ios_lram_host)                    SLAVE (ios_lram)
--------------------                    ----------------
GPIO[10] GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt
GPIO[5]  IOM0 I2C SCL                   GPIO[0]  IOS I2C SCL
GPIO[6]  IOM0 I2C SDA                   GPIO[1]  IOS I2C SDA
GND                                     GND


******************************************************************************


