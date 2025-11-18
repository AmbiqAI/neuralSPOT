Name:
=====
 ios_burst_controller


Description:
============
 Controller demonstrating IOS burst mode operations with high-speed data transfer.




Purpose:
========
This example demonstrates high-speed data transfer using IOS burst
 mode operations between two EVBs. The controller component implements burst
 write and read operations, managing data transfer between IOM and IOS
 interfaces. It provides comprehensive validation of burst mode functionality
 with unlimited write size and configurable read operations.

Section: Key Features
=====================

 1. Burst Mode Operations: Implements high-speed burst mode transfers
    with support for unlimited write size and controlled read operations

 2. Dual EVB Communication: Establishes reliable communication
    between controller and device EVBs using IOM and IOS interfaces

 3. Data Validation: Provides comprehensive data verification
    for ensuring transfer accuracy and reliability

Section: Functionality
======================

 The application provides:
 - IOM burst write to IOS LRAM burst read operations
 - IOM FIFO read from IOS FIFO write operations
 - Real-time status monitoring via ITM SWO interface
 - Comprehensive error checking and validation

Section: Usage
==============

 1. Configure two EVBs with controller and device firmware
 2. Connect EVBs using appropriate pin connections
 3. Monitor operation status via ITM SWO at 1MHz
 4. Verify data transfer accuracy and performance

Additional Information:
=======================
 A second EVB running the ios_burst example is required.
 Connect the EVBs using appropriate fly leads for operation.


 The pin jumpers should be connected as follows, as defined in bsp_pins.src:

 Apollo510 EVB

 SPI:
     CONTROLLER (ios_burst_controller)       DEVICE (ios_burst)
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
     CONTROLLER (ios_burst_controller)        DEVICE (ios_burst)
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
     CONTROLLER (ios_burst_controller)        DEVICE (ios_burst)
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


