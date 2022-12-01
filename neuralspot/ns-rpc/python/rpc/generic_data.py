#!/usr/bin/python

import sys, random, argparse
import erpc
import GenericDataOperations_PcToEvb
import GenericDataOperations_EvbToPc
import soundfile as sf
import numpy as np
import struct
import os
import time

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

        if ((block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and
            (block.description == "Audio16bPCM_to_WAV")):
            # Data is a 16 bit PCM sample
            data = struct.unpack('<'+'h'*(len(block.buffer)//2), block.buffer)
            data = np.array(data)
            wData = np.array([0]*(block.length//2), dtype=float)

            # Copy it into numpy array as a float
            for i in range(block.length//2):
                wData[i] = data[i]
            wData = wData / 32768.
            if (os.path.isfile(outFileName)):
                with sf.SoundFile(outFileName, mode = 'r+') as wfile:
                    wfile.seek(0,sf.SEEK_END)
                    wfile.write(wData)
            else:
                sf.write(outFileName, wData, samplerate = 16000) # writes to the new file

        # MPU6050 capture handler
        elif ((block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and
            (block.description == "MPU6050-Data-to-CSV")):
            ncols = 7
            # Data is a 32bit Float MPU packed sample
            data = struct.unpack(f'<{len(block.buffer)//4}f', block.buffer)
            # Convert data into 2D list with ncols columns. We drop leftover values
            data = [data[i:i+ncols] for i in range(0, ncols*(len(data)//ncols), ncols)]
            with open(outFileName, 'a+', encoding='UTF-8') as out_file:
                for row in data:
                    out_file.write(", ".join((f"{v:0.2f}" for v in row))+"\n")
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
        #print("Got a ns_rpc_data_computeOnPC call.")

        # Example Computation
        if ((in_block.cmd == GenericDataOperations_EvbToPc.common.command.extract_cmd) and
            (in_block.description == "CalculateMFCC_Please")):
            result_block.value = GenericDataOperations_EvbToPc.common.dataBlock(
                description = "*\0"
                ,dType = GenericDataOperations_EvbToPc.common.dataType.uint8_e
                ,cmd = GenericDataOperations_EvbToPc.common.command.generic_cmd
                ,buffer = bytearray([0, 1, 2, 3])
                ,length = 4)

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
    print('\r\nServer started - waiting for EVB to send an eRPC request')
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

def runClient(transport):
    clientManager = erpc.client.ClientManager(transport, erpc.basic_codec.BasicCodec)
    client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
    print('\r\nClient started - press enter send remote procedure calls to EVB')
    input_fn()

    while True:
        outBlock = GenericDataOperations_PcToEvb.common.dataBlock(
                description = "Message to EVB"
                ,dType = GenericDataOperations_PcToEvb.common.dataType.uint8_e
                ,cmd = GenericDataOperations_PcToEvb.common.command.generic_cmd
                ,buffer = bytearray([0, 10, 20, 30])
                ,length = 4)

        print('\r\nSending ns_rpc_data_sendBlockToEVB\r\n=========')
        printDataBlock(outBlock)
        stat = client.ns_rpc_data_sendBlockToEVB(outBlock)
        print('=========')

        print('\r\nSending example_fetchBlockFromEVB\r\n=========')
        retBlock = erpc.Reference()
        stat = client.ns_rpc_data_fetchBlockFromEVB(retBlock)
        print("Recieved dataBlock:")
        printDataBlock(retBlock.value)
        print('=========')

        print('\r\nSending example_computeOnEVB\r\n=========')
        print("Sent dataBlock:")
        printDataBlock(outBlock)
        stat = client.ns_rpc_data_computeOnEVB(outBlock, retBlock)
        print("Recieved dataBlock:")
        printDataBlock(retBlock.value)
        print('=========')

        # wait for key press
        print('\r\n*** Press Enter do it again...')
        sys.stdout.flush()
        input_fn()

if __name__ == "__main__":
    # parse cmd parameters
    argParser = argparse.ArgumentParser(description='NeuralSPOT GenericData RPC Demo')
    argParser.add_argument('-m', '--mode', default='server', help='eRPC Mode (client or server, default is server)')
    argParser.add_argument('-t', '--tty', default=None, help='Serial device (default value is None)')
    argParser.add_argument('-B', '--baud', default='115200', help='Baud (default value is 115200)')
    argParser.add_argument('-o', '--out', default='audio.wav', help='File where data will be written (default is audio.wav')

    args = argParser.parse_args()
    transport = erpc.transport.SerialTransport(args.tty, int(args.baud))
    outFileName= args.out
    my_calls = 0

    if args.mode == 'client':
        runClient(transport)
    else:
        runServer(transport)
