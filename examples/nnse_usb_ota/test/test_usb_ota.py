#!/usr/bin/env python3

import usb.core
import usb.util
import struct
import time
import random
import flatbuffers
from usb_packet_generated.web.usb import UsbPacket
from usb_packet_generated.web.usb.UsbDataType import UsbDataType
from usb_packet_generated.web.usb.Platform import Platform
from usb_packet_generated.web.usb.Command import Command

# Constants
VENDOR_ID = 0x1366  # J-Link vendor ID
USB_MAX_PACKET_SIZE = 64  # J-Link device max packet size
USB_HEADER_SIZE = 7  # Size of our header (CRC32 + command + sequence + chunks)
FLATBUFFER_OVERHEAD = 24  # Approximate FlatBuffer overhead
MAX_PAYLOAD = USB_MAX_PACKET_SIZE - USB_HEADER_SIZE - FLATBUFFER_OVERHEAD  # ~33 bytes
MAX_RETRIES = 3
RETRY_DELAY = 0.1  # seconds

class UsbOtaTester:
    def __init__(self):
        self.device = None
        self.interface = None
        self.endpoint_in = None
        self.endpoint_out = None
        self.sequence = 0

    def connect(self):
        """Connect to the USB device"""
        # Find the device
        self.device = usb.core.find(idVendor=VENDOR_ID)
        if self.device is None:
            raise ValueError('Device not found')

        print(f"Found device: {self.device}")
        
        # Reset the device
        try:
            self.device.reset()
            print("Device reset successfully")
            time.sleep(1)  # Wait for device to stabilize
        except usb.core.USBError as e:
            print(f"Warning: Could not reset device: {e}")
        
        # Set configuration
        try:
        self.device.set_configuration()
            print("Configuration set successfully")
        except usb.core.USBError as e:
            print(f"Error setting configuration: {e}")
            raise

        # Get interface
        cfg = self.device.get_active_configuration()
        print(f"Active configuration: {cfg}")
        
        # Try to claim the vendor-specific interface (interface 0)
        try:
            usb.util.claim_interface(self.device, 0)
            print("Vendor-specific interface (0) claimed successfully")
        except usb.core.USBError as e:
            print(f"Error claiming vendor-specific interface: {e}")
            raise

        self.interface = cfg[(0,0)]
        print(f"Vendor-specific Interface: {self.interface}")

        # Find endpoints
        print("Available endpoints:")
        for endpoint in self.interface:
            print(f"  Endpoint: {endpoint.bEndpointAddress} (Direction: {'IN' if usb.util.endpoint_direction(endpoint.bEndpointAddress) == usb.util.ENDPOINT_IN else 'OUT'})")
            if usb.util.endpoint_direction(endpoint.bEndpointAddress) == usb.util.ENDPOINT_IN:
                self.endpoint_in = endpoint
            else:
                self.endpoint_out = endpoint

        if not self.endpoint_in or not self.endpoint_out:
            raise ValueError('Endpoints not found')
        
        print(f"Selected endpoints - IN: {self.endpoint_in.bEndpointAddress}, OUT: {self.endpoint_out.bEndpointAddress}")
        
        # Wait for device to be ready
        time.sleep(1)

    def disconnect(self):
        """Disconnect from the USB device"""
        if self.device:
            usb.util.dispose_resources(self.device)
            self.device = None

    def create_packet(self, command, data_type, data):
        """Create a FlatBuffer packet"""
        builder = flatbuffers.Builder(1024)
        
        # Create data vector
        data_vector = builder.CreateByteVector(data)
        
        # Create packet
        UsbPacket.Start(builder)
        UsbPacket.AddPlatform(builder, Platform.APOLLO3)
        UsbPacket.AddVersion(builder, 1)
        UsbPacket.AddType(builder, data_type)
        UsbPacket.AddData(builder, data_vector)
        packet = UsbPacket.End(builder)
        
        builder.Finish(packet, b"USB1")
        return builder.Output()

    def calculate_crc32(self, data):
        """Calculate CRC32 of data"""
        # Simple CRC32 implementation for testing
        crc = 0xFFFFFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                crc = (crc >> 1) ^ (0xEDB88320 if crc & 1 else 0)
        return crc ^ 0xFFFFFFFF

    def send_packet(self, command, data_type, data, corrupt_crc=False, corrupt_data=False):
        """Send a packet to the device"""
        try:
        # Create FlatBuffer
        fb_data = self.create_packet(command, data_type, data)
            print(f"Created FlatBuffer of size: {len(fb_data)} bytes")
            print(f"FlatBuffer content: {fb_data.hex()}")
        
        # Create header
        crc = self.calculate_crc32(fb_data)
        if corrupt_crc:
            crc ^= 0x12345678  # Corrupt CRC
        
        if corrupt_data:
            # Corrupt some bytes in the FlatBuffer
            fb_data = bytearray(fb_data)
            for i in range(0, len(fb_data), 10):
                fb_data[i] ^= 0xFF
        
            # Calculate number of chunks needed
            total_chunks = (len(fb_data) + MAX_PAYLOAD - 1) // MAX_PAYLOAD
            print(f"Data will be split into {total_chunks} chunks")
            
            # Send each chunk
            for chunk_num in range(total_chunks):
                start_idx = chunk_num * MAX_PAYLOAD
                end_idx = min(start_idx + MAX_PAYLOAD, len(fb_data))
                chunk_data = fb_data[start_idx:end_idx]
                
                # Create header for this chunk
                header = struct.pack('<IBBB', crc, command, self.sequence, total_chunks)
                print(f"Chunk {chunk_num + 1}/{total_chunks} header: {header.hex()}")
        
                # Combine header and chunk data
                packet = header + chunk_data
                print(f"Chunk {chunk_num + 1}/{total_chunks} size: {len(packet)} bytes")
                
                # Ensure packet size doesn't exceed USB_MAX_PACKET_SIZE
                if len(packet) > USB_MAX_PACKET_SIZE:
                    print(f"Warning: Chunk {chunk_num + 1} exceeds max packet size, truncating")
                    packet = packet[:USB_MAX_PACKET_SIZE]
                
                # Send chunk
                print(f"Sending chunk {chunk_num + 1}/{total_chunks} to endpoint 0x{self.endpoint_out.bEndpointAddress:02x}")
                bytes_written = self.device.write(self.endpoint_out.bEndpointAddress, packet, timeout=2000)
                print(f"Chunk {chunk_num + 1}/{total_chunks} bytes written: {bytes_written}")
        
        # Wait for acknowledgment
        try:
                    print(f"Waiting for response on endpoint 0x{self.endpoint_in.bEndpointAddress:02x}")
                    response = self.device.read(self.endpoint_in.bEndpointAddress, USB_MAX_PACKET_SIZE, timeout=2000)
                    print(f"Chunk {chunk_num + 1}/{total_chunks} response: {response.hex() if response else 'None'}")
                    if not response:
                        print(f"Warning: No response for chunk {chunk_num + 1}")
                        return None
                except usb.core.USBError as e:
                    if e.args[0] == 110:  # Timeout
                        print(f"Timeout waiting for chunk {chunk_num + 1} response")
                        return None
                    print(f"USB Error during read: {e}")
                    raise
                
                # Small delay between chunks
                time.sleep(0.1)
            
            self.sequence += 1
            return response  # Return the last response
            
        except usb.core.USBError as e:
            print(f"USB Error during write: {e}")
            raise
        except Exception as e:
            print(f"Unexpected error: {e}")
            raise

    def test_valid_packet(self):
        """Test sending a valid packet"""
        print("\nTesting valid packet...")
        
        # Try J-Link specific initialization sequences
        init_sequences = [
            (b'\x00\x00\x00\x00\x00\x00\x00\x00', "Zero initialization"),
            (b'\x01\x00\x00\x00\x00\x00\x00\x00', "Command initialization"),
            (b'\x02\x00\x00\x00\x00\x00\x00\x00', "Alternative command"),
            (b'\x03\x00\x00\x00\x00\x00\x00\x00', "Third command"),
            # J-Link specific commands
            (b'\x00\x00\x00\x00\x00\x00\x00\x01', "J-Link command 1"),
            (b'\x00\x00\x00\x00\x00\x00\x00\x02', "J-Link command 2"),
            (b'\x00\x00\x00\x00\x00\x00\x00\x03', "J-Link command 3"),
            (b'\x00\x00\x00\x00\x00\x00\x00\x04', "J-Link command 4")
        ]
        
        for init_data, desc in init_sequences:
            print(f"\nTrying {desc}...")
            for retry in range(MAX_RETRIES):
                try:
                    print(f"Sending initialization packet: {init_data.hex()}")
                    bytes_written = self.device.write(self.endpoint_out.bEndpointAddress, init_data, timeout=2000)
                    print(f"Initialization packet sent: {bytes_written} bytes")
                    
                    # Try to read response
                    try:
                        print("Waiting for initialization response...")
                        response = self.device.read(self.endpoint_in.bEndpointAddress, USB_MAX_PACKET_SIZE, timeout=2000)
                        print(f"Initialization response: {response.hex() if response else 'None'}")
                        if response:
                            print(f"✓ Got response for {desc}")
                            break
                    except usb.core.USBError as e:
                        if e.args[0] == 110:  # Timeout
                            print(f"No response to initialization packet (attempt {retry + 1}/{MAX_RETRIES})")
                        else:
                            print(f"Error reading initialization response: {e}")
                    
                    time.sleep(RETRY_DELAY)  # Wait before retry
                except usb.core.USBError as e:
                    print(f"Warning: Could not send initialization packet (attempt {retry + 1}/{MAX_RETRIES}): {e}")
                    time.sleep(RETRY_DELAY)  # Wait before retry
        
        # Now try the actual test packet
        print("\nSending test packet...")
        data = 'Hello, World!'.encode('utf-8')
        for retry in range(MAX_RETRIES):
            try:
        response = self.send_packet(Command.SEND_DATA, UsbDataType.MODEL_DATA, data)
        if response:
            print("✓ Valid packet acknowledged")
                    break
        else:
                    print(f"✗ Valid packet not acknowledged (attempt {retry + 1}/{MAX_RETRIES})")
                    time.sleep(RETRY_DELAY)
            except usb.core.USBError as e:
                print(f"Error sending test packet (attempt {retry + 1}/{MAX_RETRIES}): {e}")
                time.sleep(RETRY_DELAY)

    def test_invalid_crc(self):
        """Test sending a packet with invalid CRC"""
        print("\nTesting invalid CRC...")
        data = 'Hello, World!'.encode('utf-8')
        response = self.send_packet(Command.SEND_DATA, UsbDataType.MODEL_DATA, data, corrupt_crc=True)
        if not response:
            print("✓ Invalid CRC rejected")
        else:
            print("✗ Invalid CRC accepted")

    def test_invalid_data(self):
        """Test sending a packet with corrupted data"""
        print("\nTesting corrupted data...")
        data = 'Hello, World!'.encode('utf-8')
        response = self.send_packet(Command.SEND_DATA, UsbDataType.MODEL_DATA, data, corrupt_data=True)
        if not response:
            print("✓ Corrupted data rejected")
        else:
            print("✗ Corrupted data accepted")

    def test_retry_mechanism(self):
        """Test retry mechanism with initially invalid then valid packet"""
        print("\nTesting retry mechanism...")
        data = 'Hello, World!'.encode('utf-8')
        
        # First attempt with invalid CRC
        print("Sending packet with invalid CRC...")
        response = self.send_packet(Command.SEND_DATA, UsbDataType.MODEL_DATA, data, corrupt_crc=True)
        if not response:
            print("✓ First attempt (invalid) rejected")
            
            # Second attempt with valid packet
            print("Sending valid packet...")
            response = self.send_packet(Command.SEND_DATA, UsbDataType.MODEL_DATA, data)
            if response:
                print("✓ Second attempt (valid) acknowledged")
            else:
                print("✗ Second attempt (valid) not acknowledged")
        else:
            print("✗ First attempt (invalid) accepted")

    def test_chunked_data(self):
        """Test sending chunked data"""
        print("\nTesting chunked data...")
        # Create a large data buffer
        data = bytes([random.randint(0, 255) for _ in range(2000)])
        
        # Split into chunks
        chunks = [data[i:i + MAX_PAYLOAD] for i in range(0, len(data), MAX_PAYLOAD)]
        total_chunks = len(chunks)
        
        print(f"Sending {total_chunks} chunks...")
        for i, chunk in enumerate(chunks):
            # Create FlatBuffer
            fb_data = self.create_packet(Command.SEND_DATA, UsbDataType.MODEL_DATA, chunk)
            
            # Create header
            crc = self.calculate_crc32(fb_data)
            header = struct.pack('<IBBB', crc, Command.SEND_DATA, i, total_chunks)
            
            # Send packet
            packet = header + fb_data
            self.device.write(self.endpoint_out.bEndpointAddress, packet)
            
            # Wait for acknowledgment
            try:
                response = self.device.read(self.endpoint_in.bEndpointAddress, 64, timeout=1000)
                print(f"Chunk {i+1}/{total_chunks} acknowledged")
            except usb.core.USBError as e:
                if e.args[0] == 110:  # Timeout
                    print(f"Chunk {i+1}/{total_chunks} timeout")
                else:
                    raise

def main():
    tester = UsbOtaTester()
    try:
        print("Connecting to device...")
        tester.connect()
        print("Connected successfully")
        
        # Run tests
        tester.test_valid_packet()
        time.sleep(1)  # Small delay between tests
        
        tester.test_invalid_crc()
        time.sleep(1)
        
        tester.test_invalid_data()
        time.sleep(1)
        
        tester.test_retry_mechanism()
        time.sleep(1)
        
        tester.test_chunked_data()
        
    except Exception as e:
        print(f"Error: {e}")
    finally:
        tester.disconnect()
        print("\nDisconnected")

if __name__ == "__main__":
    main() 