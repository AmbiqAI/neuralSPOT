import serial
import time
import serial.tools.list_ports
import logging
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

BAUD_RATE = 115200            # Update this to your baud rate

# Configure logging
logging.basicConfig(filename='uart_stress_test.log', level=logging.INFO, format='%(asctime)s - %(message)s')

def main():
    SERIAL_PORT = find_tty()
    try:
        # Open the serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.01)
        print(f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud.")
        logging.info(f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud.")

        # Give the microcontroller some time to reset
        time.sleep(2)
        count = 0
        # Continuous loop to alternately send and receive data
        while True:
            # Generate a random command
            command_size = random.randint(1, 256)
            command = ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789', k=command_size)) + '\n'
            # send size of command first
            # ser.write((chr(command_size)).encode())
            ser.write(command.encode())
            print(f"Sent: {command.strip()}")
            logging.info(f"Sent: {command.strip()}")

            # Wait for a response from the microcontroller
            try:
                response = ser.readline().decode().strip()
                # response = ser.read(command_size)
                if response:
                    # response = response.decode('utf-8').strip()
                    print(f"Received: {response}", )
                    logging.info(f"Received: {response}")
                    # print(f"Received (raw bytes): {[hex(b) for b in response]}")

                   # Verify data integrity
                    if response != command.strip():
                        logging.error(f"Data mismatch: Sent: {command.strip()}, Received: {response}")

                else:
                    print("Received empty response.")
                    logging.warning("Received empty response.")
            except OSError as e:
                print(f"Error reading from serial port: {e}")
                logging.error(f"Error reading from serial port: {e}")
            count += 1
            # Small delay before the next iteration
            time.sleep(1)

    except serial.SerialException as e:
        print(f"Error: {e}")
        logging.error(f"Error: {e}")
    except OSError as e:
        print(f"OS Error: {e}")
        logging.error(f"OS Error: {e}")
    finally:
        # Close the serial port
        if ser.is_open:
            ser.close()
            print("Closed serial port.")
            logging.info("Closed serial port.")

if __name__ == "__main__":
    main()