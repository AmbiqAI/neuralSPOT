Name:
=====
 uart_stream_echo_rxDma


Description:
============
 A uart example that demonstrates the stream driver using RX DMA



Purpose:
========
This example demonstrates the usage of the streaming UART driver.<br>
 This is an echo rx-dma example driver
 The RxDMA will load data from the rx FIFO into the DMA buffer. When the number of
 bytes matches the DMA size, and DMA complete interrupt is issues. When this happens,
 The isr will (if enabled) make an RXComplete function callback. And, in double buffer mode,
 it will restart rxDMA in the alternate buffer. It is up to the application to empty the
 full buffer before the alternate is full.
 The RxComplete function has a parameter list that includes the start of the data buffer
 and number of bytes. The received data will be immediately transmitted.
 In this example, the argument struct from the callback is copied to a global, and the
 background loop is notified. There the application simply calls the tx function with the
 RX DMA buffer, and then it clears the buffer full flag (so it can be reused in the ISR)

 This driver expects the uart fifos be enabled.

 Default Configuration:
 By default, this example uses UART2. The I/O pins used are defined in the BSP
 file as AM_BSP_GPIO_UART2_TX and AM_BSP_GPIO_UART2_RX

 Configuration and Operation:
 - These examples require enabling Tx and Rx fifos and queues.
 - It operates in a non-blocking manner using rx threshold callbacks and no tx callbacks.

 To interact with these pins from a PC, the user should obtain a 1.8v uart/usb
 cable (FTDI recommended especially for high-speed data).

 The user can test with the provided python file ser_echo_test.py or can
 test by manually typing a large block of characters via a uart terminal and
 observing the echo

 The SWO output can send Rx/Tx status and error information.
 Swo output can be enabled disabled using the booleans in streamErrorMessageMap[]
 SWO Printing takes place over the ITM at 1M Baud.


******************************************************************************


