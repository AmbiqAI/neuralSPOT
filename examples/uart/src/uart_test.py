import serial
import time

# Configure the serial port
SERIAL_PORT = '/dev/tty.usbmodem0011600012981'  # Update this to your serial port
BAUD_RATE = 115200            # Update this to your baud rate

def main():
    try:
        # Open the serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.01)
        print(f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud.")

        # Give the microcontroller some time to reset
        time.sleep(2)

        # Continuous loop to alternately send and receive data
        while True:
            # Send a command to the microcontroller
            command = "Hello, MCU!\n"
            ser.write(command.encode())
            print(f"Sent: {command.strip()}")

            # Wait for a response from the microcontroller

            try:
                if ser.in_waiting > 0:  # Check if data is available to read
                    response = ser.read(100)
                    if response:
                        # print(f"Received: ", response.decode('utf-8'))
                        print(f"Received (raw bytes): {[hex(b) for b in response]}")

                    else:
                        print("Received empty response.")
                else:
                    print("No response received.")
            except OSError as e:
                print(f"Error reading from serial port: {e}")

            # Small delay before the next iteration
            time.sleep(1)

    except serial.SerialException as e:
        print(f"Error: {e}")
    except OSError as e:
        print(f"OS Error: {e}")
    finally:
        # Close the serial port
        if ser.is_open:
            ser.close()
            print("Closed serial port.")

if __name__ == "__main__":
    main()