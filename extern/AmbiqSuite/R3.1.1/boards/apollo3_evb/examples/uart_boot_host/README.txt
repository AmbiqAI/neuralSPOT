Name:
=====
 uart_boot_host


Description:
============
 Converts UART Wired transfer commands to SPI or I2C for use with

SBL SPI/I2C testing.

Purpose:
========
This example running on an intermediate board, along with the
standard uart_wired_update script running on host PC, can be used as a
way to communicate with the Apollo SBL (secure boot loader).

Printing takes place over the ITM at 1M Baud.

Additional Information:
=======================
PIN fly lead connections assumed:
HOST (this board)                       SLAVE (Apollo SBL target)
--------                                --------
Apollo3 SPI or I2C common connections:
GPIO[2]  GPIO Interrupt (slave to host) GPIO[4]  GPIO interrupt (SLINT)
GPIO[16] OVERRIDE pin   (host to slave) GPIO[16] Override pin or n/c
GPIO[17] Slave reset (host to slave)    Reset Pin or n/c
GND                                     GND

Apollo3 SPI additional connections:
GPIO[5]  IOM0 SPI CLK                   GPIO[0]  IOS SPI SCK
GPIO[6]  IOM0 SPI MISO                  GPIO[2]  IOS SPI MISO
GPIO[7]  IOM0 SPI MOSI                  GPIO[1]  IOS SPI MOSI
GPIO[11] IOM0 SPI nCE                   GPIO[3]  IOS SPI nCE

Apollo3 I2C additional connections:
GPIO[5]  I2C SCL                        GPIO[0]  I2C SCL
GPIO[6]  I2C SDA                        GPIO[1]  I2C SDA

Reset and Override pin connections from Host are optional, but using them
automates the entire process.

SPI or I2C mode can be handled in a couple of ways:
- SPI mode is the default (i.e. don't press buttons or tie pins low).
- For I2C, press button2 during reset and hold it until the program begins,
i.e. you see the "I2C clock = " msg.
Alternatively the button2 pin can be tied low.
- To specifically force either SPI or I2C mode, define the USE_SPI macro
appropriately.
- Note that on the Apollo3 EVB, button2 is labelled as 'BTN4', which is
the button located nearest the end of the board.
Also on the Apollo3 EVB, BTN4 uses pin 19.  It happens that the header
pin for pin 19 on the EVB is adjacent to a ground pin, so a jumper can
be used to assert I2C mode.

General procedure:
- Update any of the pin defines below as needed for your particular setup.
Typical ones that may need to be changed are marked with the phrase
"Platform specific pin".
- Make all of the above connections between the host and the target device.
This procedure assumes that the optional SLINT and Reset connections
above are made.
- Compile and load this program onto the host device, e.g. an Apollo3 EVB.
- Start the UART script (e.g. uart_wired_update.py, which can be used for
loading a binary onto a target device) using the appropriate parameters.
- Press the reset button on the host device.
- This program will take the UART commands and convert them to SPI or I2C
signals to be used for transmitting data to the target device.



******************************************************************************


