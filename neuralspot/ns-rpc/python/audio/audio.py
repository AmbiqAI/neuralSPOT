#!/usr/bin/python

import sys, random, argparse
import erpc
import ns_audio_rpc
import soundfile as sf
import numpy as np
import struct
import os

audioBufferSize = ns_audio_rpc.common.max_audio_buffer_length

class AudioServiceHandler(ns_audio_rpc.interface.Ins_audio_rpc):
    s = 0
    def erpcDumpAudioBuffer(self, audioCommand):
        # print(self.s, end="")
        print(".", end="")
        # self.s = self.s + 1
        # AudioCommand.buf.buf is a 16 bit PCM sample
        data = struct.unpack('<'+'h'*(len(audioCommand.buf.buf)//2), audioCommand.buf.buf)
        data = np.array(data)
        wData = np.array([0]*(audioCommand.buf.bufferLength//2), dtype=float)

        # Copy it into numpy array as a float
        for i in range(audioCommand.buf.bufferLength//2):
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
        # return ns_audio_rpc.common.nsAudioRPCStatus_success

if __name__ == "__main__":
    # parse cmd parameters
    argParser = argparse.ArgumentParser(description='NeuralSPOT Audio RPC Facility')
    argParser.add_argument('-t', '--tty', default=None, help='Serial device (default value is None)')
    argParser.add_argument('-B', '--baud', default='115200', help='Baud (default value is 115200)')
    argParser.add_argument('-o', '--out', default='audio.wav', help='File where data will be written (default is data.csv')

    args = argParser.parse_args()
    transport = erpc.transport.SerialTransport(args.tty, int(args.baud))
    outFileName= args.out


    handler = AudioServiceHandler()
    service = ns_audio_rpc.server.ns_audio_rpcService(handler)
    server = erpc.simple_server.SimpleServer(transport, erpc.basic_codec.BasicCodec)
    server.add_service(service)
    print('\r\nWait for client to send a eRPC request')
    sys.stdout.flush()
    server.run()
