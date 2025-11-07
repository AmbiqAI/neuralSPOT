These files were created to test various parts of the Apollo5 example uart_async.

## seria1_test.py 
This will read and evaluate the serial output from the uart example tests:
* uart_async_test2.c
* uart_async_test3.c
* uart_async_test4.c\

Each of these tests will ouptut in increasing sequece of two byte values
This serial1_test.py will look for out of sequence transmissions.



## ser_echo_test.py
This will create random sized blocks of random data and send them to the uart_async example test uart_echo_test.c at a programmable rate.

Uart_echo_test.c will simply echo everything it reads in out back out the serial port using either DMA or fifo based transfers

ser_echo_test.py will read the input side of the serial port and compare
that with what was sent. 


