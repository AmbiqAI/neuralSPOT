#!/usr/bin/python

import argparse
import os
import random
import struct
import sys
import time

sys.path.append("../neuralspot/ns-rpc/python/ns-rpc-genericdata/")

import wave

import erpc
import GenericDataOperations_EvbToPc
import GenericDataOperations_PcToEvb
import numpy as np
import soundfile as sf
from pyogg import OpusDecoder, OpusEncoder

input_fn = raw_input if sys.version_info[:2] <= (2, 7) else input


class DataServiceHandler(GenericDataOperations_EvbToPc.interface.Ievb_to_pc):
    def ns_rpc_data_sendBlockToPC(self, block):
        # Decode incoming Opus frames into PCM, and write to wav file

        if (block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and (
            block.description == "Audio16k_OPUS"
        ):
            # Data is a 16 bit Mono PCM sample
            print(len(block.buffer))
            data = struct.unpack("<" + "c" * (len(block.buffer)), block.buffer)
            data = np.array(data)
            print(data)

            # data contains Opus frame
            decoded_pcm = opus_decoder.decode(data)

            print(len(decoded_pcm))
            wave_write.writeframes(decoded_pcm)

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
    print("\r\nOpus Decoder Server started - waiting for EVB to send an eRPC request")
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


if __name__ == "__main__":
    # parse cmd parameters
    argParser = argparse.ArgumentParser(description="NeuralSPOT GenericData RPC Demo")

    argParser.add_argument(
        "-t",
        "--tty",
        default="/dev/tty.usbmodem1234561",
        help="Serial device (default value is None)",
    )
    argParser.add_argument(
        "-B", "--baud", default="115200", help="Baud (default value is 115200)"
    )
    argParser.add_argument(
        "-o",
        "--out",
        default="audio.wav",
        help="File where data will be written (default is audio.wav",
    )

    args = argParser.parse_args()
    transport = erpc.transport.SerialTransport(args.tty, int(args.baud))
    output_filename = args.out

    opus_decoder = OpusDecoder()
    opus_decoder.set_channels(1)
    opus_decoder.set_sampling_frequency(16000)

    # Open an output wav for the decoded PCM
    wave_write = wave.open(output_filename, "wb")
    wave_write.setnchannels(1)
    wave_write.setframerate(16000)
    wave_write.setsampwidth(2)

    my_calls = 0

    runServer(transport)
