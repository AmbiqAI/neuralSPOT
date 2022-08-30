#!/usr/bin/python

import sys, random, argparse
import erpc
import struct
import ns_audio_rpc

audioBufferSize = ns_audio_rpc.common.max_audio_buffer_length

class AudioServiceHandler(ns_audio_rpc.interface.Ins_audio_rpc):
    def erpcDumpAudioBuffer(self, audioCommand):
        data = struct.unpack('<'+'f'*(len(audioCommand.buf.buf)//4), audioCommand.buf.buf)
        column = 0
        row = 0
        for d in data:
            if row < audioCommand.buf.bufferLength//4: # ignore padding at end of buffer
                print(d, end = "", file = outFile)
                if column < 6:
                    print(",", end = " ", file = outFile)
                    column = column + 1
                else:
                    print("", file = outFile)
                    column = 0
            row = row + 1

        sys.stdout.flush()
        return 1
        # return ns_audio_rpc.common.nsAudioRPCStatus_success

if __name__ == "__main__":
    # parse cmd parameters
    argParser = argparse.ArgumentParser(description='NeuralSPOT Audio RPC Facility')
    argParser.add_argument('-t', '--tty', default=None, help='Serial device (default value is None)')
    argParser.add_argument('-B', '--baud', default='115200', help='Baud (default value is 115200)')
    argParser.add_argument('-o', '--out', default='data.csv', help='File where data will be written (default is data.csv')

    args = argParser.parse_args()
    transport = erpc.transport.SerialTransport(args.tty, int(args.baud))
    outFile = open(args.out, 'w')
    
    handler = AudioServiceHandler()
    service = ns_audio_rpc.server.ns_audio_rpcService(handler)
    server = erpc.simple_server.SimpleServer(transport, erpc.basic_codec.BasicCodec)
    server.add_service(service)
    print('\r\nWait for client to send a eRPC request')
    sys.stdout.flush()
    server.run()
