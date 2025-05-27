import serial
import time
import serial.tools.list_ports
import random
END_ITER = 5
HIGH_BAUD = 921600
LOW_BAUD  = 115200
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

def main():
    ser = serial.Serial(find_tty(), LOW_BAUD, timeout=1)
    print(f"Opened {ser.port} @ {ser.baudrate} baud")
    time.sleep(2)  # MCU reset

    iteration = 0
    while True:
        # 1) Send one command (always ending in '\n')
        iteration += 1
        size = random.randint(1,25)
        cmd  = ''.join(random.choices('ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789', k=size))
        ser.write((cmd + '\n').encode())
        print(f"[{iteration}] Sent: {cmd!r}")

        # 2) Read exactly one line back
        line = ser.readline()          # reads until '\n' or timeout
        if not line:
            print("Timeout, no reply")
            continue

        # 3) Check for our special switch token
        if line.strip() == b"SWITCH":
            # host flips here
            new_baud = HIGH_BAUD if ser.baudrate == LOW_BAUD else LOW_BAUD
            ser.baudrate = new_baud
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            print(f"Host switched to {new_baud} baud")
            # don’t try to decode or compare as a normal echo
            continue

        # 4) Otherwise it’s an echo of our command:
        # decode safely (ASCII only)
        try:
            text = line.decode('ascii').rstrip('\r\n')
        except UnicodeDecodeError:
            text = line.decode('ascii', errors='ignore').rstrip('\r\n')
        print(f"Received: {text!r}")

        if text != cmd:

            print("Mismatch!")
        time.sleep(1)
if __name__ == "__main__":
    main()