# neuralSPOT UART Demonstration and Testing
This example demonstrates and tests UART communication between a Python script (uart_test.py) and an Apollo MCU running the provided uart.cc code. The Python script generates a random string of alphanumeric characters, sends the size of the string to the MCU, followed by the string itself. The MCU verifies the size and sends the string back to the Python script for verification.


## How It Works

### MCU Code (uart.cc)
The MCU code continuously checks for available UART data. When data is available, it performs the following steps:

1. Receive the Number of Bytes:
    - The MCU receives the size of the string from the Python script.
    - It converts the received size to an integer.
2. Receive the String:
    - The MCU receives the string of the specified size from the Python script.
    - It verifies that the received string length matches the specified size.
3. Echo the String:
    - The MCU sends the received string back to the Python script for verification.
    - Ns-uart lets you choose between blocking and nonblocking send/receive.

### Python Script (uart_test.py)
The Python script performs the following steps:

1. Generate Random String:
    - The script generates a random string of alphanumeric characters with a length between 1 and 255.
2. Send Size and String:
    - The script sends the size of the string to the MCU.
    - It then sends the string itself to the MCU.
3. Receive and Verify:
    - The script receives the echoed string from the MCU.
    - It verifies that the received string matches the sent string.

### Usage

```bash
$> cd .../neuralSPOT
$> make EXAMPLE=examples/uart deploy
$> cd apps/examples/uart/src
$> python uart_test.py
```

### Example Output
The Python script will output the generated string, the received string, as well as an error if the response does not match the sent command.
```bash
Opened serial port /dev/cu.usbmodem0011600007091 at 115200 baud.
sending over 37 bytes
Sent: Z6y1xuWKHTpSUCJguVyFgBwXfC9DMhUS2fcex
Received: Z6y1xuWKHTpSUCJguVyFgBwXfC9DMhUS2fcex

sending over 96 bytes
Sent: XogyISLxzi3AE8TyRavpLl33nI5YbeVxgzM6ne35V2c2oOENPOtBlDo5H5c9aavLw3KDTk3QPiEFhxXtt3ghTOpFvipxT4rW
Received: XogyISLxzi3AE8TyRavpLl33nI5YbeVxgzM6ne35V2c2oOENPOtBlDo5H5c9aavLw3KDTk3QPiEFhxXtt3ghTOpFvipxT4rW
```