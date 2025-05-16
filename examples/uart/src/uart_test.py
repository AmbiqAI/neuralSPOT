import serial
import time
import serial.tools.list_ports
import random

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

BAUD_RATE = 115200

def main():
    SERIAL_PORT = find_tty()
    try:
        # Open the serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud.")

        # Give the microcontroller some time to reset
        time.sleep(2)
        # Continuous loop to alternately send and receive data
        while True:
            # Generate a random command
            command_size = random.randint(1, 25)
            # ser.write(f"{command_size:03}".encode())
            print(f'sending over {command_size} bytes')
            command = ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789', k=command_size))
            time.sleep(0.01)
            ser.write(command.encode())
            print(f"Sent: {command.strip()}")
            time.sleep(2)
            # Wait for a response from the microcontroller
            try:
                response = ser.read(command_size + 1)
                if response:
                    response = response.decode('utf-8').strip()
                    print(f"Received: {response}\n\n")
                    if response != command:
                        print("Error: Received response does not match sent command.")

                else:
                    print("Received empty response.\n\n")
            except OSError as e:
                print(f"Error reading from serial port: {e}")
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