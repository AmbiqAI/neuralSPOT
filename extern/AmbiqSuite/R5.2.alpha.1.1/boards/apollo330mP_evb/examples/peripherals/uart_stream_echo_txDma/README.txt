Name:
=====
 uart_stream_echo_tx_Dma


Description:
============
 A uart example that demonstrates the stream driver using TX DMA



Purpose:
========
This example demonstrates TX DMA usage of the streaming UART driver.<br>
 This driver allows the application to append UART Tx data to the
 Tx queue with interrupt code managing queue transmission.
 It supports fifo/Interrupt or TX DMA based UART transmission
 It supports two different DMA types of DMA queue, double buffering, and
 a circular queue.
 This allows the user to specify an arbitrarily large TX buffer and the driver
 will manage the DMA size limit.

 This driver expects the uart FIFOs be enabled.

 Similarly, the interrupt code will move received data into the Rx queue
 and the application periodically reads from the Rx queue.

 The Rx callback has been enabled here.
 Once the number of bytes in the Rx buffer
 exceeds the threshold, the callback is made (from the ISR). Optionally, the callback
 can be disabled and the rx buffer read via polling.
 This example will immediately retransmit the received data via TX DMA.

 The associated ISR handler am_hal_uart_interrupt_queue_service() will return
 status in a bitfield, suitable for use as a callback or polling.

 Default Configuration:
 By default, this example uses UART2. The I/O pins used are defined in the BSP
 file as AM_BSP_GPIO_UART2_TX and AM_BSP_GPIO_UART2_RX

 Configuration and Operation:
 - These examples require enabling Tx and Rx fifos and queues.

 To interact with these pins from a PC, the user should obtain a 1.8v uart/usb
 cable (FTDI, etc.).

 to test use the provided python program ser_echo_test.py
 or test with a serial terminal by manually typing a block of data 64 to 256 bytes
 and observe the echo once the internal RX callback size has been reached.
 The swo output will report status during example operation.<

 The SWO output will send Rx/Tx status and error information.
 SWO Printing takes place over the ITM at 1M Baud.


******************************************************************************


