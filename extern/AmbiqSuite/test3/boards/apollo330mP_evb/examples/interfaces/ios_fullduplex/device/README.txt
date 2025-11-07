Name:
=====
 ios_fullduplex


Description:
============
 Example to demonstrate the use of IOSFD




Purpose:
========
This example demonstrates IOS full duplex functionality
 for bidirectional data transfer with an Apollo IO Master (IOM) controller.
 The application implements full duplex communication using IOSFD,
 enabling simultaneous data transmission and reception.

Section: Key Features
=====================

 1. Full Duplex Communication: Implements bidirectional data transfer
    between IOM and IOS for simultaneous transmission and reception

 2. Pattern Verification: Sends and verifies data patterns for accuracy
    and data integrity validation

 3. FIFO Based Transfer: Utilizes FIFO (address 0x7F) for efficient
    data exchange between device and controller

 4. Interrupt Driven Operation: Uses interrupts for synchronization
    and efficient data flow control

 5. Multi-Platform Support: Provides pin configuration for various
    Apollo EVB platforms (Apollo5, Apollo510, Apollo510B)

Section: Functionality
======================

 The application performs the following operations:
 - Implements full duplex communication between IOM and IOS
 - Sends data patterns from IOS to IOM for verification
 - Verifies data accuracy and integrity
 - Utilizes FIFO-based data transfer at address 0x7F
 - Provides interrupt-driven synchronization
 - Supports multiple Apollo EVB platforms

Section: Usage
==============

 1. Connect two EVBs as device and controller using specified pin configuration
 2. Compile and download the application to the device EVB
 3. Run the companion controller example on the second EVB
 4. Observe full duplex data transfer and pattern verification
 5. Monitor SWO output for status and verification results

Section: Configuration
======================

 - FIFO Address: Data transfer at address 0x7F
 - SPI Interface: Full duplex SPI communication
 - SWO: Output for status and results (1MHz, 8-n-1 mode)
 - Pin Mapping: Platform-specific pin configuration for Apollo EVBs

 SWO is configured in 1MHz, 8-n-1 mode.

 The example utilizes an IOM<->IOS transfer between two EVBs. Running them
 requires jumpering pins in order to connect the IOM to the IOS.


 The pin jumpers should be connected as follows, as defined in bsp_pins.src:

 Apollo510 EVB

 SPI:
     CONTROLLER (ios_fullduplex_controller)  DEVICE (ios_fullduplex)
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
     CONTROLLER (ios_fullduplex_controller)   DEVICE (ios_fullduplex)
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
     CONTROLLER (ios_fullduplex_controller)   DEVICE (ios_fullduplex)
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


