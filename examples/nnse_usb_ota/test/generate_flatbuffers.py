#!/usr/bin/env python3

import os
import subprocess
import sys

def generate_flatbuffers():
    """Generate Python FlatBuffers code from schema"""
    # Get the directory of this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Path to the FlatBuffers schema file
    schema_file = os.path.join(script_dir, '..', 'src', 'usb_packet.fbs')
    
    # Create output directory if it doesn't exist
    output_dir = os.path.join(script_dir, 'usb_packet_generated')
    os.makedirs(output_dir, exist_ok=True)
    
    # Generate Python code
    try:
        subprocess.run([
            'flatc',
            '--python',
            '-o', output_dir,
            schema_file
        ], check=True)
        print("Successfully generated Python FlatBuffers code")
    except subprocess.CalledProcessError as e:
        print(f"Error generating FlatBuffers code: {e}")
        sys.exit(1)
    except FileNotFoundError:
        print("Error: flatc not found. Please install FlatBuffers compiler")
        sys.exit(1)

if __name__ == "__main__":
    generate_flatbuffers() 