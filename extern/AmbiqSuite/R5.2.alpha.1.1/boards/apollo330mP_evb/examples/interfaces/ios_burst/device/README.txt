Name:
=====
 ios_burst


Description:
============
 Example used for demonstrating the ios burst mode.




Purpose:
========
This example demonstrates IOS burst mode functionality
 for high-performance data transfer with an Apollo IO Master (IOM) controller.
 The application implements burst mode operations, including unlimited
 burst writes and FIFO-based data exchange.

Section: Key Features
=====================

 1. Burst Mode Operation: Implements high-performance burst mode
    for unlimited size data transfers between IOM and IOS

 2. Dual Transfer Modes: Supports both burst write (IOM to IOS)
    and FIFO read/write operations for flexible data exchange

 3. High Throughput Communication: Enables efficient data transfer
    with minimal overhead for high-performance applications

 4. Interrupt Driven Operation: Uses interrupts for synchronization
    and efficient data flow control between device and controller

 5. Multi-Platform Support: Provides pin configuration for various
    Apollo EVB platforms (Apollo5, Apollo510, Apollo510B)

Section: Functionality
======================

 The application performs the following operations:
 - Implements burst mode data transfer from controller to device
 - Supports FIFO-based data exchange for controller read operations
 - Provides interrupt-driven synchronization between device and controller
 - Implements sensor data emulation for demonstration purposes
 - Supports unlimited burst write operations with LRAM burst read
 - Provides platform-specific pin configuration for various EVBs

Section: Usage
==============

 1. Connect two EVBs as device and controller using specified pin configuration
 2. Compile and download the application to the device EVB
 3. Run the companion controller example on the second EVB
 4. Observe high-throughput burst mode data transfer
 5. Monitor ITM output for status and progress (controller side)

Section: Configuration
======================

 - Burst Mode: Unlimited size burst writes with LRAM burst read
 - FIFO Mode: Standard FIFO read/write operations (max 1023 bytes)
 - SPI Interface: High-speed SPI communication
 - Pin Mapping: Platform-specific pin configuration for Apollo EVBs

 In this pair of examples, IOM and IOS are switched as below for each mode:
 controller burst write : IOM burst write - IOS LRAM burst read
 (controller writes unlimited size of data to device)
 controller read : IOM FIFO read - IOS FIFO write
 (controller reads data from device max. 1023 bytes)

 Printing takes place over the ITM at 1MHz.

Additional Information:
=======================
 In order to run this example, a controller (e.g. a second EVB) must be set
 up to run the controller example, ios_burst_controller. The two EVBs can be connected
 using fly leads as follow.


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


