Name:
=====
 uart_stream


Description:
============
 A uart example that demonstrates the stream driver.



Purpose:
========
This example demonstrates UART streaming driver
 functionality with DMA and interrupt management capabilities.
 The application showcases UART communication using FIFO,
 interrupt-driven, and DMA-based transmission modes. The driver supports
 multiple DMA queue types including double buffering and circular queues,
 enabling efficient handling of arbitrarily large transmit buffers.

Section: Key Features
=====================

 1. Multiple Test Scenarios: Provides five different test scenarios
    for UART streaming validation and demonstration

 2. DMA Based Transmission: Implements DMA-based UART transmission
    for high-performance data transfer with low CPU overhead

 3. Interrupt Driven Operation: Implements interrupt-based data
    handling for real-time transmission and reception

 4. Buffer Management: Provides buffer management with
    double buffering and circular queue support

 5. Non Blocking Operation: Implements non-blocking UART operations
    for efficient system resource utilization

Section: Functionality
======================

 The application performs the following operations:
 - Initializes UART with streaming configuration
 - Implements multiple test scenarios for validation
 - Provides DMA-based transmission with buffer management
 - Implements interrupt-driven data handling
 - Demonstrates bidirectional communication capabilities
 - Monitors UART status and provides debug output via SWO

Section: Usage
==============

 1. Compile and download the application to target device
 2. Connect UART pins to PC using UART/USB cable (1.8V logic)
 3. Use provided Python scripts for testing
 4. Monitor SWO output for UART status and error information
 5. Test different scenarios using the various test modes

Section: Configuration
======================

 - UART_BAUDRATE: Configurable UART communication rate (default: 115200)
 - DMA_QUEUE_TYPE: DMA queue type (double buffering or circular queue)
 - TX_BUFFER_SIZE: Configurable transmit buffer size
 - AM_DEBUG_PRINTF: Enables detailed debug output via SWO

 This example consists of five different test scenarios:
 - uart_stream_test1: Simple alternating data buffer transmission
 - uart_stream_test2: Transmit buffer overflow testing
 - uart_stream_test3: Large DMA buffer transmit overflow testing
 - uart_stream_test4: Maximum transmit output with full buffer
 - uart_echo_test: Bidirectional communication with Python script
 - uart_echo_rx_dma_test: Rx DMA with echo via Tx FIFO transfers

 Default Configuration:
 By default, this example uses UART2. The I/O pins used are defined in the BSP
 file as AM_BSP_GPIO_UART2_TX and AM_BSP_GPIO_UART2_RX

 The SWO output will send Rx/Tx status and error information.
 SWO Printing takes place over the ITM at 1MHz.


******************************************************************************


