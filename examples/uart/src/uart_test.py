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

BAUD_RATE = 115200            # Update this to your baud rate

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
            command_size = random.randint(1, 10)
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



# #!/usr/bin/env python3
# import time
# import random
# import serial
# from serial.tools import list_ports

# VID = 0xCAFE   # your device’s USB VID
# BAUD = 115200  # must match your firmware



# def main():
#     port = find_tty()
#     print(f"Opening {port} @ {BAUD} baud…")
#     ser = serial.Serial(port, BAUD, timeout=0.2)
#     time.sleep(2)  # allow the MCU to reset into your UART handler

#     try:
#         iteration = 1
#         while True:
#             # 1) Pick a random payload size (1–255)
#             N = random.randint(1, 255)
#             # 2) Build a 3-digit length header + random ASCII payload
#             header = f"{N:03}".encode("ascii")
#             payload = bytes(random.choices(
#                 b"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#                 b"abcdefghijklmnopqrstuvwxyz"
#                 b"0123456789", k=N
#             ))
#             packet = header + payload

#             # 3) Flush any leftovers, send the packet
#             ser.reset_input_buffer()
#             ser.reset_output_buffer()
#             ser.write(packet)
#             ser.flush()
#             print(f"[{iteration:03d}] → len={N:3d}: {payload.decode('ascii',errors='ignore')}")

#             # 4) Read exactly N bytes back, with a small retry loop
#             response = bytearray()
#             deadline = time.time() + 1.0
#             while len(response) < N and time.time() < deadline:
#                 chunk = ser.read(N - len(response))
#                 if chunk:
#                     response.extend(chunk)
#                 else:
#                     # no data yet, give MCU a moment
#                     time.sleep(0.005)

#             # 5) Report results
#             if len(response) != N:
#                 print(f"   ✗ Timeout: got {len(response)} of {N}")
#             else:
#                 resp_text = response.decode("ascii", errors="replace")
#                 print(f"   ← len={len(response):3d}: {resp_text}")
#                 if response != payload:
#                     print("   ✗ MISMATCH!")

#             print()
#             iteration += 1
#             time.sleep(0.2)

#     except KeyboardInterrupt:
#         print("Exiting on user request…")

#     finally:
#         ser.close()
#         print("Closed serial port.")

# if __name__ == "__main__":
#     main()
