Name:
=====
 ios_fullduplex_controller


Description:
============
 Full-duplex IOM controller demonstrating bidirectional data transfer.




Purpose:
========
This example demonstrates full-duplex communication between
 IOM and IOS interfaces, implementing bidirectional data transfer with
 pattern verification. It showcases high-speed, simultaneous transmit
 and receive operations with comprehensive data validation.

Section: Key Features
=====================

 1. Full Duplex Operation: Implements simultaneous
    bidirectional data transfer between IOM and IOS

 2. Pattern Verification: Validates data accuracy with
    predefined test patterns and verification

 3. Real-time Monitoring: Provides status updates and
    verification results via SWO interface

Section: Functionality
======================

 The application provides:
 - Bidirectional data transfer using FIFO (address 0x7F)
 - Pattern generation and verification
 - Status monitoring via SWO at 1MHz
 - Error detection and reporting

Section: Usage
==============

 1. Configure two EVBs with controller and device firmware
 2. Connect EVBs using pin jumper connections
 3. Monitor operation via SWO (1MHz, 8-n-1)
 4. Verify pattern accuracy and transfer completion

 Hardware Configuration:

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


