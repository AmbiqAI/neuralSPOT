from typing import Optional

import serial
import argparse

def read_current_buffer(ser: serial.Serial, numBytes: int) -> Optional[int]:
    data: bytes = ser.read(numBytes)
    if len(data)  == 2:
        return int.from_bytes(data, byteorder='little')
    return None


def read_current_value(ser: serial.Serial) -> Optional[int]:
    data: bytes = ser.read(2)
    if len(data) == 2:
        return int.from_bytes(data, byteorder='little')
    return None

def main():
    # Argument parsing
    parser = argparse.ArgumentParser(description="Monitor a serial port for out-of-sequence 16-bit values.")
    #parser.add_argument('-p', '--port', type=str,  required=True, help="Serial port (e.g., COM4 or /dev/ttyUSB0)")
    parser.add_argument('-p', '--port', type=str, default = "COM4", help="Serial port (e.g., COM4 or /dev/ttyUSB0)")
    #parser.add_argument('-b', '--baud', type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument('-b', '--baud', type=int, default=3000000, help="Baud rate (default: 3,000,000)")
    parser.add_argument('-t', '--timeout', type=int, default=2, help="Timeout in seconds (default: 3)")
    args = parser.parse_args()

    # Serial port configuration
    port_name: str = args.port
    baud_rate: int = args.baud
    timeout: int = args.timeout

    try:
        # Open the serial port
        ser = serial.Serial(port_name, baud_rate, timeout=timeout)
        print("Serial port opened successfully")
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return

    previous_value: int = 0xFFFF
    largest_value: int = 0
    sync_lost: bool = False
    overage: bool = False
    print_details: bool = False

    while True:
        try:
            if overage:
                overage = False
                print_details = True
                ser.read(1)

            # Read current value from serial port
            current_value = read_current_value(ser)

            if current_value is not None:
                if current_value > (12 * 1024 / 2):
                    overage = True
                    print(f"Overage: {current_value:#06x} -> {previous_value:#06x}")
                elif current_value > largest_value:
                    largest_value = current_value

                if print_details:
                    print_details = False
                    print(f"details: {current_value} -> {previous_value}")

                if sync_lost:
                    if current_value == (previous_value + 1):
                        sync_lost = False
                        print(f"Resynchronized at value {current_value}")
                        largest_value = 0
                else:
                    # Check for sequence error
                    if current_value != (previous_value + 1):
                        print(f"Out of sequence: {previous_value} -> {current_value}")
                        print(f"Sync lost. Largest value so far: {largest_value}")
                        sync_lost = True
                        largest_value = 0

                previous_value = current_value
            else:
                # If not enough bytes were read, assume sync is lost
                sync_lost = True

        except KeyboardInterrupt:
            print("Stopped by user")
            break
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break

    ser.close()

if __name__ == "__main__":
    main()
