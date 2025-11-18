Name:
=====
 tdm_loopback


Description:
============
 An example to show basic TDM operation.




Purpose:
========
This example demonstrates TDM (Time Division Multiplexing)
 loopback functionality for audio signal integrity testing and validation. The
 application showcases I2S interface configuration in TDM mode, dual
 interface support, DMA-based data transfer, and buffer management.

Section: Key Features
=====================

 1. TDM Mode Support: Demonstrates Time Division Multiplexing with
    two phases of data format in a single frame

 2. Dual I2S Interface: Supports I2S0 and I2S1 with configurable
    master/slave roles for flexible audio routing

 3. Data Verification: Compares transmitted and received data to ensure
    accurate loopback operation and signal integrity

 4. Ping Pong Buffering: Implements software-managed ping-pong buffer
    mechanism for continuous data streaming

 5. Hardware Acceleration: Utilizes DMA for efficient data transfer
    between I2S interfaces with minimal CPU overhead

Section: Functionality
======================

 The application performs the following operations:
 - Configures I2S interfaces in TDM mode with dual-phase support
 - Implements loopback between I2S TX and RX interfaces
 - Provides data verification and integrity checking
 - Implements ping-pong buffering for continuous streaming
 - Utilizes DMA for efficient data transfer
 - Monitors and reports TDM operation status via SWO/ITM

Section: Usage
==============

 1. Connect I2S pins according to the specified pin configuration
 2. Compile and download the application to target device
 3. Monitor SWO/ITM output for TDM operation status and results
 4. Verify loopback data integrity and signal quality
 5. Test continuous operation and buffer management

Section: Configuration
======================

 - TDM Mode: Dual-phase data format in a single frame
 - DMA: Hardware-accelerated data transfer
 - Buffer Management: Ping-pong buffering for continuous streaming
 - SWO/ITM: Output for status and results (1MHz)


 In TDM mode, I2S supports two kinds of data format in one frame which were
 called phase 1 and phase 2. The number of channels can be different in both
 phases but the total number shouldn't be greater than 8.

 NOTE: Usage of software-implemented ping pong machine
 step 1: Prepare 2 blocks of buffer.
  - sTransfer0.ui32RxTargetAddr = addr1;
  - sTransfer0.ui32RxTargetAddrReverse = addr2;
  - am_hal_i2s_dma_configure(pI2S0Handle, &g_sI2S0Config, &sTransfer0);
 step 2: Call am_hal_i2s_interrupt_service() in the ISR to restart DMA operation,
 the ISR helps automatically switch to the reverse buffer.
 step 3: Fetch the valid data by calling am_hal_i2s_dma_get_buffer().

 The required pin connections are as follows:

 - No peripheral card is connected.
 - mikrobus level shifter is set to 1.8v

 Apollo510 EVB

     I2S1                                I2S0
     --------------------                ----------------
     GPIO[16] CLK (mikrobus RST)         GPIO[ 5]  CLK
     GPIO[19] DIN                        GPIO[ 4]  DIN
     GPIO[17] DOUT (mikrobus AN)         GPIO[ 6]  DOUT (mikrobus MOSI)
     GPIO[18] WS                         GPIO[ 7]  WS (mikrobus MISO)
     GND                                 GND

 Apollo510B EVB

     I2S1                                I2S0
     --------------------                ----------------
     GPIO[16] CLK (mikrobus RST)         GPIO[ 5]  CLK
     GPIO[ 3] DIN                        GPIO[ 4]  DIN
     GPIO[17] DOUT (mikrobus AN)         GPIO[ 6]  DOUT (mikrobus MOSI)
     GPIO[10] WS (mikrobus CS)           GPIO[ 7]  WS (mikrobus MISO)
     GND                                 GND

 Apollo330mP evb (Single I2S Self-Loopback)

     I2S0 Self-Loopback
     --------------------
     GPIO[109]  CLK                       (No external connection)
     GPIO[ 29]  DIN  <----------------->  GPIO[30]  DOUT
     GPIO[ 31]  WS                        (No external connection)
     GND

     Note: For Apollo510L and Apollo330P devices, only I2S0 is available.
     The loopback is achieved by connecting I2S0_DIN to I2S0_DOUT directly.


 Printing takes place over the ITM at 1MHz.




******************************************************************************


