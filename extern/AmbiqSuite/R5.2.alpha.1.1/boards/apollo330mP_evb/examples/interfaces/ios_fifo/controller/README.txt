Name:
=====
 ios_fifo_controller


Description:
============
 Advanced IOS FIFO controller demonstrating data exchange protocols.




Purpose:
========
This example implements a sophisticated data exchange protocol
 between two EVBs using IOM-IOS FIFO communication. The controller manages
 data flow with command-based control, implementing a state machine for
 synchronized data transfer with simulated sensor data handling on the
 device side.

Section: Key Features
=====================

 1. Command Protocol: Implements one-byte command protocol for
    controlling data accumulation and transfer operations

 2. State Machine: Manages synchronized data exchange with
    comprehensive state tracking and flow control

 3. Sensor Data Handling: Supports simulated sensor data
    accumulation with CTimer-based event generation

Section: Functionality
======================

 The application provides:
 - Command-based control for Start/Stop operations
 - Block data transfer acknowledgment handling
 - Real-time status monitoring via ITM SWO
 - Synchronized data exchange with device

Section: Usage
==============

 1. Configure two EVBs with controller and device firmware
 2. Connect EVBs using appropriate SPI/I2C connections
 3. Monitor operation via ITM SWO at 1MHz
 4. Observe data exchange and state transitions

Additional Information:
=======================
 Requires a companion device running ios_fifo example.
 The device implements sensor simulation with CTimer events.

 The IOS interrupts the controller to indicate data availability. The controller then
 reads the available data (as indicated by FIFOCTR) by READing using IOS FIFO
 (at address 0x7F).  The IOS keeps accumulating any new data coming in the
 background.

 Controller sends an acknowledgement to IOS once it has finished reading a block
 of data initiated by IOS (partitally or complete). IOS interrupts the controller
 again if and when it has more data for the controller to read, and the cycle
 repeats - till controller indicates that it is no longer interested in receiving
 data by sending STOP command.

Additional Information:
=======================
 In order to run this example, a device (e.g. a second EVB) must be set
 up to run the companion example, ios_fifo. The two EVBs can be connected
 using fly leads between the two EVBs as follows.


 The pin jumpers should be connected as follows, as defined in bsp_pins.src:

 Apollo510 EVB

 SPI:
     CONTROLLER (ios_fifo_controller)             DEVICE (ios_fifo)
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


