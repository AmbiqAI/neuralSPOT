Name:
=====
 ios_fifo


Description:
============
 Example device used for demonstrating the use of the IOS FIFO.




Purpose:
========
This example demonstrates IOS FIFO device functionality
 for data exchange with an Apollo IO Master (IOM) controller. The application
 implements a protocol for data accumulation, command handling, and synchronized
 data transfer using FIFO and interrupt-driven communication.

Section: Key Features
=====================

 1. IOS FIFO Data Exchange: Implements FIFO-based data transfer
    protocol for efficient communication with IOM controller

 2. Command Handling: Supports start/stop data accumulation and block
    acknowledgment commands from controller

 3. Interrupt Driven Operation: Uses interrupts to synchronize data
    availability and transfer between device and controller

 4. Sensor Emulation: Emulates sensor data input using CTimer events
    for demonstration and testing

 5. Multi-Protocol Support: Operates over both SPI and I2C interfaces
    for flexible connectivity

Section: Functionality
======================

 The application performs the following operations:
 - Implements FIFO-based data accumulation and transfer protocol
 - Handles commands from controller for data flow control
 - Emulates sensor data input using timers
 - Synchronizes data transfer using interrupts
 - Supports both SPI and I2C communication modes
 - Provides pin configuration for various Apollo EVB platforms

Section: Usage
==============

 1. Connect two EVBs as device and controller using specified pin configuration
 2. Compile and download the application to the device EVB
 3. Run the companion controller example on the second EVB
 4. Observe synchronized data exchange between device and controller
 5. Monitor ITM output for status and progress (controller side)

Section: Configuration
======================

 - Communication Mode: SPI or I2C selectable
 - FIFO Address: Data transfer at address 0x7F, command at 0x80
 - Interrupts: Used for data availability and synchronization
 - Pin Mapping: Platform-specific pin configuration for Apollo EVBs

 The ios_fifo example has no print output.
 The controller example does use the ITM SWO to let the user know progress and
 status of the demonstration.

 This example implements the device part of a protocol for data exchange with
 an Apollo IO Master (IOM).  The controller sends one byte commands on SPI/I2C by
 writing to offset 0x80.

 The command is issued by the controller to Start/Stop Data accumulation, and also
 to acknowledge read-complete of a block of data.

 On the IOS side, once it is asked to start accumulating data (using START
 command), two CTimer based events emulate sensors sending data to IOS.
 When IOS has some data for controller, it implements a state machine,
 synchronizing with the controller.

 The IOS interrupts the controller to indicate data availability. The controller then
 reads the available data (as indicated by FIFOCTR) by READing using IOS FIFO
 (at address 0x7F). The IOS keeps accumulating any new data coming in the
 background.

 Controller sends an acknowledgment to IOS once it has finished reading a block
 of data initiated by IOS (partially or complete). IOS interrupts the controller
 again if and when it has more data for the controller to read, and the cycle
 repeats - till controller indicates that it is no longer interested in receiving
 data by sending STOP command.

 Printing takes place over the ITM at 1MHz.

Additional Information:
=======================
 In order to run this example, a controller (e.g. a second EVB) must be set
 up to run the controller example, ios_fifo_controller. The two EVBs can be connected
 using fly leads between the two EVBs as follows.


 The pin jumpers should be connected as follows, as defined in bsp_pins.src:

 Apollo510 EVB

 SPI:
     CONTROLLER (ios_fifo_controller)        DEVICE (ios_fifo)
     --------------------                    ----------------
     GPIO[47] IOM5 SPI SCK                   GPIO[0]  IOS IOSFD_SCK
     GPIO[48] IOM5 SPI MOSI                  GPIO[1]  IOS IOSFD_MOSI
     GPIO[49] IOM5 SPI MISO                  GPIO[2]  IOS IOSFD_MISO
     GPIO[60] IOM5 SPI nCE                   GPIO[3]  IOS IOSFD_CE
     GPIO[4]  REQ_ACK (device to controller) GPIO[4]  IOS IOSFD_INT
     GND                                     GND

 Pin 0, 1, 2, 3, 4, 47, 48, 49 : J8
 Pin 60                        : J9

 Apollo510B EVB

 SPI:
     CONTROLLER (ios_fifo_controller)         DEVICE (ios_fifo)
     --------------------                     ----------------
     GPIO[22]  IOM7 SPI SCK                   GPIO[0]  IOS IOSFD_SCK
     GPIO[23]  IOM7 SPI MOSI                  GPIO[1]  IOS IOSFD_MOSI
     GPIO[24]  IOM7 SPI MISO                  GPIO[2]  IOS IOSFD_MISO
     GPIO[5]   IOM7 SPI nCE                   GPIO[3]  IOS IOSFD_CE
     GPIO[4]   REQ_ACK (device to controller) GPIO[4]  IOS IOSFD_INT
     GND                                      GND

 Pin 0, 1, 2, 3, 4, 5, 22, 23, 24   : J8

 Apollo330mP EVB

 SPI:
     CONTROLLER (ios_fifo_controller)         DEVICE (ios_fifo)
     --------------------                     ----------------
     GPIO[5]   IOM0 SPI SCK                   GPIO[0]  IOS IOSFD_SCK
     GPIO[6]   IOM0 SPI MOSI                  GPIO[1]  IOS IOSFD_MOSI
     GPIO[7]   IOM0 SPI MISO                  GPIO[2]  IOS IOSFD_MISO
     GPIO[17]  IOM0 SPI nCE                   GPIO[3]  IOS IOSFD_CE
     GPIO[4]   REQ_ACK (device to controller) GPIO[4]  IOS IOSFD_INT
     GND                                      GND

 Pin 0, 1, 2, 3, 4   : J8
 Pin 5, 6, 7, 17     : 13



******************************************************************************


