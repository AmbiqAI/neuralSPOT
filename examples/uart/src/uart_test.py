import serial
import time
import serial.tools.list_ports

# Configure the serial port

def find_tty():
    # Find the TTY of our device by scanning serial USB devices
    # The VID of our device is alway 0xCAFE
    # This is a hack to find the tty of our device, it will break if there are multiple devices with the same VID
    # or if the VID changes
    tty = None
    ports = serial.tools.list_ports.comports()
    for p in ports:
        if p.vid == 4966:
            tty = p.device
            break

    return tty

BAUD_RATE = 115200            # Update this to your baud rate

def main():
    SERIAL_PORT = find_tty()
    try:
        # Open the serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.01)
        print(f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud.")

        # Give the microcontroller some time to reset
        time.sleep(1)
        count = 0
        # Continuous loop to alternately send and receive data
        while True:
            # Send a command to the microcontroller
            if count % 2 == 0:
                command = "Hello, MCU!\n"
            else:
                command = "How are you?"
            ser.write(command.encode())
            print(f"Sent: {command.strip()}")

            # Wait for a response from the microcontroller

            try:
                if ser.in_waiting > 0:  # Check if data is available to read
                    response = ser.read(256)
                    if response:
                        print(f"Received: ", response.decode('utf-8'))
                        # print(f"Received (raw bytes): {[hex(b) for b in response]}")

                    else:
                        print("Received empty response.")
                else:
                    print("No response received.")
            except OSError as e:
                print(f"Error reading from serial port: {e}")
            count += 1
            # Small delay before the next iteration
            time.sleep(0.5)

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