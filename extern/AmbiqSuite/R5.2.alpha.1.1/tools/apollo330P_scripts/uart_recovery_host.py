#!/usr/bin/env python3

# *****************************************************************************
#
#    uart_recovery_host.py
#
#    Uart host for MRAM recovery fo both Ambiq and OEM images
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2025, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
#
# *****************************************************************************
import serial
import argparse
import time
import os
from am_defines import *
from uart_wired_update import *


def main():
    # Open a serial port, and communicate with Device
    #
    # We will use a UART timeout value of 12 second. This should be long
    # enough that the slave will respond to us, but short enough that a human
    # operator should see errors quickly..
    # Max flashing time depends on the amount of SRAM available.
    # For very large images, the flashing happens page by page.
    # However if the image can fit in the free SRAM, it could take a long time
    # for the whole image to be flashed at the end.
    # The largest image which can be stored depends on the max SRAM.
    # Assuming worst case ~100 ms/page of flashing time, and allowing for the
    # image to be close to occupying full SRAM (256K) which is 128 pages.

    # Timeout for the UART Connection
    timeout = 12
    print('Connecting with Apollo330P over serial port {}...'.format(args.port), flush=True)

    with serial.Serial(args.port, args.baud, timeout=timeout, rtscts=False) as ser:
        ser.setRTS(False)  # Ensure RTS is initially low so that we don't wait
        send_file(ser, timeout)

def validate_file(file_path, file_type):
    """Validate if the given file exists."""
    if file_path and not os.path.isfile(file_path):
        print(f"Error: {file_type} file does not exist: {file_path}")
        sys.exit()
    return bool(file_path)

def send_file(ser, timeout):
    abort_oem = False
    # Get the current timestamp
    start_time = time.time()

    # Validate files and determine recovery types
    has_ambiq = validate_file(args.binfile_ambiq, "Ambiq")
    has_oem = validate_file(args.binfile_oem, "OEM")

    if not has_ambiq and not has_oem:
        print("Error: No image supplied. Exiting...")
        sys.exit()

    if has_ambiq:
        loop_counter = 2
    elif has_oem:
         loop_counter = 1

    abort_oem = False if has_oem else True

    if loop_counter == 0:
        print("Error: No image supplied. Exiting...")
        sys.exit()

    # Start the transmitions if device found
    while (loop_counter > 0):

        if ((time.time() - start_time) >= timeout):
            print("UART timeout reached. Exiting...")
            sys.exit()

        # Wait for CTS to become LOW - gpio in progress
        if not ser.cts:
            recovery_start_time = time.time()
            print("CTS signal acquired from Apollo330P over serial port")
            time.sleep(1)

            recovery_type, max_storage = send_hello(ser)
            if recovery_type == AM_MRAM_RECOVERY_TYPE_AMBIQ and has_ambiq:
                connect_device(ser, recovery_type, max_storage)
                loop_counter -=  1
            elif recovery_type == AM_MRAM_RECOVERY_TYPE_OEM:
                if abort_oem:
                    print("Warning: OEM image not supplied. Sending abort for OEM image...")
                    send_abort(ser)
                    break
                else:
                    connect_device(ser, recovery_type, max_storage)
                    loop_counter -= 1
            else:
                print(f"Invalid recovery type: {recovery_type}. Terminating script...")
                exit()

            # Wait for CTS to become LOW again indicating recovery image has finished sending & processing.
            while not ser.cts:
                time.sleep(0.01)

            recovery_end_time = time.time()

            print("\nFile sent successfully.")
            print(f"Duration: {(recovery_end_time - recovery_start_time)}")

            start_time = time.time()

        else:
            # Sleep briefly to avoid busy waiting
            time.sleep(0.01)

    print("Done with sending MRAM Recovery updates.")

#******************************************************************************
#
# @Desp: Decode Hello Message.
# @return: None
#
#******************************************************************************
def decode_hello_msg(response, recovery_type):
    print("Received Hello response from the Apollo330P device")
    word = word_from_bytes(response, 4)
    print("Length of the message = ", hex((word >> 16)))
    print("Version = ", hex(word_from_bytes(response, 8)))
    print("Max Storage = ", hex(word_from_bytes(response, 12)))
    print("Status = ", hex(word_from_bytes(response, 16)))
    print("State = ", hex(word_from_bytes(response, 20)))
    print("chipId0 = ", hex(word_from_bytes(response, 24)))
    print("chipId1 = ", hex(word_from_bytes(response, 28)))

    if (recovery_type == AM_MRAM_RECOVERY_TYPE_AMBIQ):
        print("SDCert = ", hex(word_from_bytes(response, 32)))
        print("Resv = ", hex(word_from_bytes(response, 36)))
    elif (recovery_type == AM_MRAM_RECOVERY_TYPE_OEM):
        if ((hex(word_from_bytes(response, 8))) == "0xffff0100"):
            print("SDCert = ", hex(word_from_bytes(response, 36)))
            print("Resv = ", hex(word_from_bytes(response, 40)))
        else:
            print("SDCert = ", hex(word_from_bytes(response, 32)))
            print("Chip Revision = ", hex(word_from_bytes(response,36)))
    print("SOCId = ", end='')
    for x in range(40, 72, 4):
        print(hex(word_from_bytes(response, x)) + " ", end='')
    print("\nAMInfo = ", end='')
    for x in range(72, 96, 4):
        print(hex(word_from_bytes(response, x)) + " ", end='')

def send_abort(ser):
    # Send OTA Desc
    print('Sending Abort command.')
    abortMsg = bytearray([0x00]*8)
    fill_word(abortMsg, 0, ((12 << 16) | AM_SECBOOT_WIRED_MSGTYPE_ABORT))
    fill_word(abortMsg, 4, -1)
    send_ackd_command(abortMsg, ser)

#******************************************************************************
#
# @Desp: Send Hello Message.
# @return: type of recovery required
#
#******************************************************************************
def send_hello(ser):
    # Send Hello
    #generate mutable byte array for the header
    hello = bytearray([0x00]*4)
    # Generate the HELLO message
    fill_word(hello, 0, ((8 << 16) | AM_SECBOOT_WIRED_MSGTYPE_HELLO))
    # Send the message
    ser.reset_input_buffer()
    response = send_command(hello, 96, ser)
    print("Sending Hello to the device")

    # Extract the status from the header of the response message
    word = word_from_bytes(response, 4)

    # Extract the recovery type from the header of the response message
    recovery_type = int(hex(word_from_bytes(response,8)), 16) & 0x0000FFFF
    if ((word & 0xFFFF) == AM_SECBOOT_WIRED_MSGTYPE_STATUS):
        # decode_hello_msg(response, recovery_type)
        print("Received Hello response from the Apollo330P device")
        max_storage = word_from_bytes(response, 12)
        return (recovery_type, max_storage)
    else:
        # Received Wrong message
        print("Apollo330P device did not respond with the expected response.")
        print("Response")
        word = word_from_bytes(response, 4)
        print("  msgType = ", hex(word & 0xFFFF))
        print("  Length  = ", hex(word >> 16))
        print("Data dump:")
        print([("0x%02X" % (int(n))) for n in response])
        print("!!!Wired Upgrade Unsuccessful!!!....Terminating the script")
        exit()

#******************************************************************************
#
# Communicate with Device
#
# Given a serial port, connects to the target device using the
# UART.
#
#******************************************************************************
def connect_device(ser, recovery_type, max_storage):

    if (args.binfile_ambiq != '' or args.binfile_oem != ''):
        #application = b''
        # Read the binary file from the command line.
        if (recovery_type == AM_MRAM_RECOVERY_TYPE_AMBIQ):
            print("\nProcessing Ambiq recovery image.")
            with open(args.binfile_ambiq, mode='rb') as binfile:
                application = binfile.read()
        elif (recovery_type == AM_MRAM_RECOVERY_TYPE_OEM):
            with open(args.binfile_oem, mode='rb') as binfile:
                print("\nProcessing OEM recovery image.")
                application = binfile.read()

        # Scan the binary file for segment headers.
        segment_data = list(read_all_headers(application))
        # Check to make sure all of our segments are a reasonable size
        def get_segment_size(s):
            return s.size

        largest_segment = max(segment_data, key=get_segment_size)

        print('Found {} segments with max size 0x{:0X}.'.format(len(segment_data), largest_segment.size))

        # It is assumed that maxSize is 256b multiple
        maxImageSize = max_storage

        # Each Block of image consists of AM_WU_IMAGEHDR_SIZE Bytes Image header and the Image blob
        maxUpdateSize = AM_WU_IMAGEHDR_SIZE + maxImageSize

        if largest_segment.size > maxUpdateSize:
            print('Error: Wired update image has segments of size {}B, but this chip only has space for {}B.'
                        .format(largest_segment.size, maxUpdateSize))
            raise OTASegmentsTooLarge

        # Send Update command
        print('Sending Update Command.')

        numUpdates = len(segment_data)
        print("number of updates needed = ", numUpdates)

        for segment in reversed(segment_data):
            start = segment.offset
            end = segment.offset + segment.size
            crc = crc32(application[start:end])
            applen = segment.size
            print("Sending block of size ", str(hex(applen)), " from ", str(hex(start)), " to ", str(hex(end)))

            update = bytearray([0x00]*16)
            fill_word(update, 0, ((20 << 16) | AM_SECBOOT_WIRED_MSGTYPE_UPDATE))
            fill_word(update, 4, applen)
            fill_word(update, 8, crc)
            # Size = 0 => We're not piggybacking any data to IMAGE command
            fill_word(update, 12, 0)

            send_ackd_command(update, ser)

            # Loop over the bytes in the image, and send them to the target.
            resp = 0
            # Max chunk size is AM_MAX_RCVY_MSG_SIZE adjusted for the header for Data message
            maxChunkSize = AM_MAX_RCVY_MSG_SIZE - 12
            for x in range(0, applen, maxChunkSize):
                # Split the application into chunks of maxChunkSize bytes.
                # This is the max chunk size supported by the UART bootloader
                if ((x + maxChunkSize) > applen):
                    chunk = application[start+x:start+applen]
                    # print(str(hex(start+x)), " to ", str(hex(applen)))
                else:
                    chunk = application[start+x:start+x+maxChunkSize]
                    # print(str(hex(start+x)), " to ", str(hex(start + x + maxChunkSize)))

                chunklen = len(chunk)

                # Build a data packet with a "data command" a "length" and the actual
                # payload bytes, and send it to the target.
                dataMsg = bytearray([0x00]*8);
                fill_word(dataMsg, 0, (((chunklen + 12) << 16) | AM_SECBOOT_WIRED_MSGTYPE_DATA))
                # seqNo
                fill_word(dataMsg, 4, x)

                print('.', end='', flush=True)
                send_ackd_command(dataMsg + chunk, ser)


#******************************************************************************
#
# Send ACK'd command
#
# Sends a command, and waits for an ACK.
#
#******************************************************************************
def send_ackd_command(command, ser):
    for numTries in range(1, 5 , 1):

        # Determine if incoming command is data message
        datatrans = True if ((command[0]) == AM_SECBOOT_WIRED_MSGTYPE_DATA) else False
        if datatrans:
            seqNum = word_from_bytes(command, 4)

        response = send_command(command, 20, ser)

        word = word_from_bytes(response, 4)
        if ((word & 0xFFFF) == AM_SECBOOT_WIRED_MSGTYPE_ACK):
            # Received ACK
            if (word_from_bytes(response, 12) != AM_SECBOOT_WIRED_ACK_STATUS_SUCCESS):
                print("Received NACK")
                print("msgType = ", hex(word_from_bytes(response, 8)))
                print("error = ", hex(word_from_bytes(response, 12)))
                print("seqNo = ", hex(word_from_bytes(response, 16)))
                if datatrans and seqNum != word_from_bytes(response, 16):
                    print(f"Received NACK for seq number {hex(word_from_bytes(response, 16))}, expected {seqNum}! Exiting...")
                    sys.exit()
                if (numTries < 4):
                    print("Retry # ", numTries)
                else:
                    print("Exceed number of retries")
                time.sleep(0.2)
            else:
                # Received ACK.
                break
        else:
            print("!!!Wired Upgrade Unsuccessful!!!....unexpected response - Terminating the script")
            exit()
    if (numTries == 4):
        print("!!!Wired Upgrade Unsuccessful!!!....numTries exceeded - Terminating the script")
        exit()

    return response

#******************************************************************************
#
# Send command
#
# Sends a command, and waits for the response.
#
#******************************************************************************
def send_command(params, response_len, ser):

    # Compute crc
    crc = crc32(params)

    #print([hex(n) for n in int_to_bytes(crc)])
    #print([hex(n) for n in params])
    # send crc first
    ser.write(int_to_bytes(crc))
    # ser.flush()
    # Next, send the parameters.
    ser.write(params)
    # ser.flush()
    response = ''
    response = ser.read(response_len)

    # Make sure we got the number of bytes we asked for.
    if len(response) != response_len:
        print('No response for command 0x{:08X}'.format(word_from_bytes(params, 0) & 0xFFFF))
        n = len(response)
        if (n != 0):
            print("received bytes ", len(response))
            print([hex(n) for n in response])
        raise NoResponseError

    return response

#******************************************************************************
#
# Send a command that uses an array of bytes as its parameters.
#
#******************************************************************************
def send_bytewise_command(command, params, response_len, ser):
    # Send the command first.
    ser.write(int_to_bytes(command))

    # Next, send the parameters.
    ser.write(params)

    response = ''
    response = ser.read(response_len)

    # Make sure we got the number of bytes we asked for.
    if len(response) != response_len:
        print('No response for command 0x{:08X}'.format(command))
        raise NoResponseError

    return response

#******************************************************************************
#
# Send image.
#
#******************************************************************************
def send_data(data, ser):
    send_ackd_command(command, ser)

#******************************************************************************
#
# Data type for the header data in wired update images.
#
#******************************************************************************
class SegmentHeader():
    def __init__(self, size, offset):
        self.size = size
        self.offset = offset

#******************************************************************************
#
# Given a filename of a wired update file, return a list of all of the parset
# segment headers.
#
#******************************************************************************
def read_headers_from_file(binary_file):
    with open(binary_file, "rb") as b:
        binary_data = b.read()
        headers = read_all_headers(binary_data)
        return list(headers)

#******************************************************************************
#
# Given a stream of bytes, search for segment header and yield a list of the
# results.
#
#******************************************************************************
def read_all_headers(binary_data):
    offset = 0
    while offset < len(binary_data):
        block_size = read_header(binary_data, offset)
        yield SegmentHeader(block_size, offset)
        offset = offset + block_size

#******************************************************************************
#
# Parse the size data from a header at address "offset" within binary array
# "binary data"
#
#******************************************************************************
def read_header(binary_data, offset):
    header = list(bytes_to_words(binary_data[offset:offset+16]))
    blobSize = read_bitfield(header, 0, 0, 23)
    return blobSize

#******************************************************************************
#
# Helper function for reading a bitfield
#
#******************************************************************************
def read_bitfield(array, word, lsb, size):
    return ((array[word] >> lsb) & (2 ** size - 1))

#******************************************************************************
#
# Helper function for converting a bytestream to a stream of words.
#
#******************************************************************************
def bytes_to_words(bytestream):
    word = 0
    for n, b in enumerate(bytestream):
        word = word | (b << 8 * (n % 4))
        if n % 4 == 3:
            yield word
            word = 0

#******************************************************************************
#
# Errors
#
#******************************************************************************
class BootError(Exception):
    pass

class NoAckError(BootError):
    pass

class NoResponseError(BootError):
    pass

class OTASegmentsTooLarge(Exception):
    pass

#******************************************************************************
#
#   Main Program
#
#******************************************************************************
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'UART MRAM Recovery Host')

    parser.add_argument('port', help = 'Serial COMx Port')

    parser.add_argument('-b', dest='baud', default=115200, type=int,
                        help = 'Baud Rate (default is 115200)')

    parser.add_argument('-fa', dest='binfile_ambiq', default='',
                        help = 'Ambiq Binary file to program into the target device')

    parser.add_argument('-fo', dest='binfile_oem', default='',
                        help = 'OEM Binary file to program into the target device')

    args = parser.parse_args()

    main()

