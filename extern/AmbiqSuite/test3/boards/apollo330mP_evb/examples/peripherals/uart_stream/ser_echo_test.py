import queue

import serial
import argparse
import random
import threading
import time
import struct
from queue import Queue

BUFFER_SIZE: int = 32  # Size of the circular buffer in tx/rx blocks
TX_IN_PROG_SIZE : int = 24
MAX_BLOCK_SIZE: int = 128


# Circular buffer to store data to be sent and received
saved_data_buff: Queue[bytes] = Queue(maxsize=BUFFER_SIZE)
#size_queue : Queue[int] = Queue(maxsize=TX_IN_PROG_SIZE)
stop_event : threading.Event = threading.Event()  # Event to signal when to stop the send thread
mismatch_count: int = 0  # Counter for consecutive mismatches
tx_group_count: int = 0

def generate_random_block(size: int) -> bytes:
    global tx_group_count
    """Generate a block of random 8-bit data."""
    # Create a bytearray with the first 4 bytes as the up-counting long
    out_bytes: bytearray = bytearray()
    if size >= 4:
        out_bytes.extend(struct.pack("<I", tx_group_count))
        # Append the rest of the random data
        if size > 4:
            out_bytes.extend(random.randint(0, 0xFF) for _ in range(size - 4))
    else:
        out_bytes.extend(random.randint(0, 0xFF) for _ in range(size))

    tx_group_count += 1
    return bytes(out_bytes)

def send_data(ser: serial.Serial, debug_level: int, delay_ms: int) -> None:
    """Thread to send random data blocks to the serial port."""
    delay_sec: float = delay_ms / 1000.0
    while not stop_event.is_set():
        block_size: int = random.randint(1, MAX_BLOCK_SIZE)
        data : bytes = generate_random_block(block_size)

        try:
            saved_data_buff.put(data)
            #size_queue.put(block_size)
            try:
                ser.write(data)
                ser.flush()
                if debug_level >= 2:
                    print(f"Sent: {block_size} - {data.hex()}")
            except serial.SerialException as e:
                print(f"Serial write error: {e}")
        except queue.Full:
            print("Warnibg Circ Buff Full")

        time.sleep(delay_sec)  # Delay between blocks in seconds

def read_data(ser: serial.Serial, debug_level: int) -> None:
    """Thread to read data from the serial port and compare with sent data."""
    global mismatch_count
    while not stop_event.is_set():

        txed_data_block : bytes = saved_data_buff.get(block=True)
        next_read_size : int = len(txed_data_block)


        rx_buffer : bytearray = bytearray()

        try:
            rx_buffer.extend(ser.read(size=next_read_size))
            if debug_level >= 2:
                print(f"Serial read {len(rx_buffer)}")
        except serial.SerialException as e:
            print(f"Serial read error: {e}")
            break

        if len(rx_buffer) >= next_read_size:
            
                # Perform the comparison outside the lock

            tx_data : bytes = txed_data_block[:next_read_size]
            rx_buffer : bytes = rx_buffer[:next_read_size:]

            if tx_data == rx_buffer:
                if debug_level >= 2:
                    print(f"Received correctly: {tx_data.hex()}")
                else:
                    print(f"Received correctly")
                mismatch_count = 0  # Reset mismatch counter on success
            else:
                print(f"Data mismatch!\nRSent:     {tx_data.hex()}\nReceived: {rx_buffer.hex()}")
                mismatch_count += 1
                if mismatch_count >= 3:
                    stop_event.set()  # Signal the send thread to stop
                    break

def main() -> None:
    # Argument parsing
    parser = argparse.ArgumentParser(description="Generate and verify serial data echo.")
    parser.add_argument('-p', '--port', type=str, default="COM4", help="Serial port (e.g., COM4 or /dev/ttyUSB0)")
    parser.add_argument('-b', '--baud', type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument('-t', '--timeout', type=int, default=1, help="Timeout in seconds (default: 1)")
    parser.add_argument('-d', '--debug', type=int, choices=[0, 1, 2], default=1, help="Debug level: 0 (no output), 1 (sent data), 2 (sent and received data)")
    parser.add_argument('--delay', type=int, default=10, help="Delay between blocks in milliseconds (default: 500)")
    args = parser.parse_args()

    # Serial port configuration
    port_name: str = args.port
    baud_rate: int = args.baud
    timeout: int = args.timeout
    debug_level: int = args.debug
    delay_ms: int = args.delay

    try:
        # Open the serial port
        ser: serial.Serial = serial.Serial(port_name, baud_rate, timeout=timeout)
        if debug_level >= 1:
            print("Serial port opened successfully")
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return

    ser.reset_input_buffer()
    ser.reset_output_buffer()

    # Create threads for sending and reading data
    send_thread: threading.Thread = threading.Thread(target=send_data, args=(ser, debug_level, delay_ms), daemon=True)
    read_thread: threading.Thread = threading.Thread(target=read_data, args=(ser, debug_level), daemon=True)

    # Start the threads
    send_thread.start()
    read_thread.start()

    try:
        # Keep the main thread alive
        while not stop_event.is_set():
            time.sleep(1)
    except KeyboardInterrupt:
        print("Stopped by user")
        stop_event.set()

    ser.close()

if __name__ == "__main__":
    main()
