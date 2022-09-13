#!/usr/bin/python

import sys, random, argparse
import erpc
import GenericDataOperations_EVB_to_PC
import GenericDataOperations_PC_to_EVB
import soundfile as sf
import numpy as np
import struct
import os

# Define the RPC service handlers - one for each EVB-to-PC RPC function

class DataServiceHandler(GenericDataOperations_EVB_to_PC.interface.Ievb_to_pc):
    def ns_rpc_data_sendBlockToPC(self, block):
        # Example decode of incoming block
        return 1

        if ((block.cmd == GenericDataOperations_EVB_to_PC.common.command.write_cmd) and 
            (block.description == "Audio16bPCM_to_WAV")):
            # AudioCommand.buf.buf is a 16 bit PCM sample
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
            sys.stdout.flush()
            return 1

    
    def ns_rpc_data_fetchBlockFromPC(self, block):
        print("Got a ns_rpc_data_fetchBlockFromPC call.")
        sys.stdout.flush()
        return 1

    def ns_rpc_data_computeOnPC(self, in_block, result_block):
        #print("Got a ns_rpc_data_computeOnPC call.")

        # Example Computation
        if ((in_block.cmd == GenericDataOperations_EVB_to_PC.common.command.extract_cmd) and 
            (in_block.description == "CalculateMFCC_Please")):
            result_block.value = GenericDataOperations_EVB_to_PC.common.dataBlock(
                description = "*\0"
                ,dType = GenericDataOperations_EVB_to_PC.common.dataType.uint8_e
                ,cmd = GenericDataOperations_EVB_to_PC.common.command.generic_cmd
                ,buffer = bytearray([0, 1, 2, 3])
                ,length = 4)

        # print(result_block)
        sys.stdout.flush()
        return 1
        
    def ns_rpc_data_remotePrintOnPC(self, msg):
        print("%s" % msg)
        sys.stdout.flush()
        return 1

if __name__ == "__main__":
    # parse cmd parameters
    argParser = argparse.ArgumentParser(description='NeuralSPOT GenericData RPC Demo')
    argParser.add_argument('-t', '--tty', default=None, help='Serial device (default value is None)')
    argParser.add_argument('-B', '--baud', default='115200', help='Baud (default value is 115200)')
    argParser.add_argument('-o', '--out', default='audio.wav', help='File where data will be written (default is data.csv')

    args = argParser.parse_args()
    transport = erpc.transport.SerialTransport(args.tty, int(args.baud))
    outFileName= args.out


    handler = DataServiceHandler()
    service = GenericDataOperations_EVB_to_PC.server.evb_to_pcService(handler)
    server = erpc.simple_server.SimpleServer(transport, erpc.basic_codec.BasicCodec)
    server.add_service(service)
    print('\r\nServer started - waiting for client to send a eRPC request')
    sys.stdout.flush()
    server.run()
