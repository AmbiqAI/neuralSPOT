Name:
=====
 uart_ble_bridge


Description:
============
 Converts UART HCI commands to SPI.


Purpose:
========
 This example is primarily designed to enable DTM testing with the
Apollo3 EVB. The example accepts HCI commands over the UART at 115200 baud
and sends them using the BLEIF to the Apollo3 BLE Controller.  Responses from
the BLE Controller are accepted over the BLEIF and sent over the UART.

Printing takes place over the ITM at 1M Baud.



******************************************************************************


