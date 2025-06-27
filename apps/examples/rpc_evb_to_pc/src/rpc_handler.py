#!/usr/bin/python

import argparse
import os
import random
import struct
import sys
import time

sys.path.append("neuralspot/ns-rpc/python/ns-rpc-genericdata/")

import erpc
import GenericDataOperations_EvbToPc
import numpy as np
import soundfile as sf
import serial.tools.list_ports

input_fn = raw_input if sys.version_info[:2] <= (2, 7) else input

# Define the RPC service handlers - one for each EVB-to-PC RPC function


class DataServiceHandler(GenericDataOperations_EvbToPc.interface.Ievb_to_pc):
    def ns_rpc_data_sendBlockToPC(self, block):
        # Example decode of incoming block - unpack to WAV or CSV depending on block.description

        # Audio capture handler
        # global my_calls
        # if my_calls == 5:
        #     print("slow.")
        #     time.sleep(1.5)
        #     print("slow")
        # my_calls = my_calls +1

        if (block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and (
            block.description == "Audio32bPCM_to_WAV"
        ):
            # Data is a 32 stereo bit PCM sample
            # printDataBlock(block)
            data = struct.unpack("<" + "h" * (len(block.buffer) // 2), block.buffer)
            data = np.array(data)
            wDataL = np.array([0] * (block.length // 4), dtype=float)
            wDataR = np.array([0] * (block.length // 4), dtype=float)

            # Copy it into numpy array as a float
            for i in range(block.length // 4):
                wDataL[i] = data[2 * i]
                wDataR[i] = data[2 * i + 1]

            # print(wDataL)
            # print(wDataR)
            wDataL = wDataL / 32768.0
            wDataR = wDataR / 32768.0

            outFileNameL = outFileName + "_Left.wav"
            outFileNameR = outFileName + "_Right.wav"

            if os.path.isfile(outFileNameL):
                with sf.SoundFile(outFileNameL, mode="r+") as wfile:
                    wfile.seek(0, sf.SEEK_END)
                    wfile.write(wDataL)
                with sf.SoundFile(outFileNameR, mode="r+") as wfile:
                    wfile.seek(0, sf.SEEK_END)
                    wfile.write(wDataR)
            else:
                sf.write(
                    outFileNameL, wDataL, samplerate=16000
                )  # writes to the new file
                sf.write(
                    outFileNameR, wDataR, samplerate=16000
                )  # writes to the new file

        elif (block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and (
            block.description == "Audio16bPCM_to_WAV"
        ):
            # Data is a 16 bit Mono PCM sample
            data = struct.unpack("<" + "h" * (len(block.buffer) // 2), block.buffer)
            data = np.array(data)
            wData = np.array([0] * (block.length // 2), dtype=float)
            outFileNameMono = outFileName + "_mono.wav"
            # Copy it into numpy array as a float
            for i in range(block.length // 2):
                wData[i] = data[i]
            wData = wData / 32768.0
            if os.path.isfile(outFileNameMono):
                with sf.SoundFile(outFileNameMono, mode="r+") as wfile:
                    wfile.seek(0, sf.SEEK_END)
                    wfile.write(wData)
            else:
                sf.write(
                    outFileNameMono, wData, samplerate=16000
                )  # writes to the new file

        # MPU6050 capture handler
        elif (block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and (
            block.description == "MPU6050-Data-to-CSV"
        ):
            ncols = 7
            # Data is a 32bit Float MPU packed sample
            data = struct.unpack(f"<{len(block.buffer)//4}f", block.buffer)
            # Convert data into 2D list with ncols columns. We drop leftover values
            data = [
                data[i : i + ncols]
                for i in range(0, ncols * (len(data) // ncols), ncols)
            ]
            with open(outFileName, "a+", encoding="UTF-8") as out_file:
                for row in data:
                    out_file.write(", ".join((f"{v:0.2f}" for v in row)) + "\n")
                    out_file.flush()
                # END FOR
            # END WITH
        sys.stdout.flush()
        return 0

    def ns_rpc_data_fetchBlockFromPC(self, block):
        print("Got a ns_rpc_data_fetchBlockFromPC call.")
        sys.stdout.flush()
        return 0

    def ns_rpc_data_computeOnPC(self, in_block, result_block):
        # print("Got a ns_rpc_data_computeOnPC call.")

        # Example Computation
        if (
            in_block.cmd == GenericDataOperations_EvbToPc.common.command.extract_cmd
        ) and (in_block.description == "CalculateMFCC_Please"):
            result_block.value = GenericDataOperations_EvbToPc.common.dataBlock(
                description="*\0",
                dType=GenericDataOperations_EvbToPc.common.dataType.uint8_e,
                cmd=GenericDataOperations_EvbToPc.common.command.generic_cmd,
                buffer=bytearray([0, 1, 2, 3]),
                length=4,
            )

        # print(result_block)
        sys.stdout.flush()
        return 0

    def ns_rpc_data_remotePrintOnPC(self, msg):
        print("%s" % msg)
        sys.stdout.flush()
        return 0


def runServer(transport):
    handler = DataServiceHandler()
    service = GenericDataOperations_EvbToPc.server.evb_to_pcService(handler)
    server = erpc.simple_server.SimpleServer(transport, erpc.basic_codec.BasicCodec)
    server.add_service(service)
    print("\r\nServer started - waiting for EVB to send an eRPC request")
    sys.stdout.flush()
    server.run()


def printDataBlock(block):
    print("Description: %s" % block.description)
    print("Length: %s" % block.length)
    print("cmd: %s" % block.cmd)
    print("dType: %s" % block.dType)
    for i in range(len(block.buffer)):
        print("0x%x " % block.buffer[i], end="")
    print("")

def find_tty(transport):
    # Find the TTY of our device by scanning serial USB or UART devices
    # The VID of our device is alway 0xCAFE
    # This is a hack to find the tty of our device, it will break if there are multiple devices with the same VID
    # or if the VID changes
    
    tty = None
    ports = serial.tools.list_ports.comports()

    if transport == 'USB':
        # Look for USB CDC
        for p in ports:
            # print(p)
            # print(p.vid)
            if p.vid == 51966:
                tty = p.device
                print("Found USB CDC device %s" % tty)
                break
    else:
        # Look for UART
        for p in ports:
            # print(p)
            # print(p.vid)
            if p.vid == 4966:
                tty = p.device
                print("Found UART device %s" % tty)
                break

    return tty

if __name__ == "__main__":
    # parse cmd parameters
    argParser = argparse.ArgumentParser(description="NeuralSPOT GenericData RPC Demo")
    argParser.add_argument(
        "-o",
        "--out",
        default="audio.wav",
        help="File where data will be written (default is audio.wav",
    )

    args = argParser.parse_args()
    outFileName = args.out
    tty = find_tty('USB')
    transport = erpc.transport.SerialTransport(tty, 115200)
    runServer(transport)
