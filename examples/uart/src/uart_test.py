import serial
import time

# Define your serial port and baud rate
SERIAL_PORT = '/dev/tty.usbmodem0004831355981'  # Change to your actual port
BAUD_RATE = 115200  # Adjust as necessary

def main():
    try:
        # Open the serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud.")

        # Give the microcontroller some time to reset
        time.sleep(2)

        # # Send a command to the microcontroller (optional, if needed)
        # command = "Hello, MCU!\n"
        # ser.write(command.encode())
        # print(f"Sent: {command.strip()}")

         # Continuous loop to alternately send and receive data
        while True:
            # Send a command to the microcontroller
            command = "Hello, MCU!\n"
            ser.write(command.encode())
            print(f"Sent: {command.strip()}")

            # Wait for a response from the microcontroller
            time.sleep(0.5)  # Small delay to allow MCU to process and respond

            if ser.in_waiting > 0:  # Check if data is available to read
                response = ser.readline().decode().strip()
                if response:
                    print(f"Received: {response}")
                else:
                    print("Received empty response.")
            else:
                print("No response received.")

            # Small delay before the next iteration
            time.sleep(1)
    except serial.SerialException as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
