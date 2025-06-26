#!/usr/bin/python

import random
import struct
import sys
import time

sys.path.append("neuralspot/ns-rpc/python/ns-rpc-genericdata/")

import erpc
import GenericDataOperations_PcToEvb
import serial.tools.list_ports

input_fn = raw_input if sys.version_info[:2] <= (2, 7) else input


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
    print("\r\nClient started - press enter send remote procedure calls to EVB")
    input_fn()

    while True:
        outBlock = GenericDataOperations_PcToEvb.common.dataBlock(
            description="Message to EVB",
            dType=GenericDataOperations_PcToEvb.common.dataType.uint8_e,
            cmd=GenericDataOperations_PcToEvb.common.command.generic_cmd,
            buffer=bytearray([0, 10, 20, 30]),
            length=4,
        )

        print("\r\nSending ns_rpc_data_sendBlockToEVB\r\n=========")
        printDataBlock(outBlock)
        stat = client.ns_rpc_data_sendBlockToEVB(outBlock)
        print("=========")

        print("\r\nSending example_fetchBlockFromEVB\r\n=========")
        retBlock = erpc.Reference()
        stat = client.ns_rpc_data_fetchBlockFromEVB(retBlock)
        print("Recieved dataBlock:")
        printDataBlock(retBlock.value)
        print("=========")

        print("\r\nSending example_computeOnEVB\r\n=========")
        print("Sent dataBlock:")
        printDataBlock(outBlock)
        stat = client.ns_rpc_data_computeOnEVB(outBlock, retBlock)
        print("Recieved dataBlock:")
        printDataBlock(retBlock.value)
        print("=========")

        # wait for key press
        print("\r\n*** Press Enter do it again...")
        sys.stdout.flush()
        input_fn()

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
    tty = find_tty('USB')
    transport = erpc.transport.SerialTransport(tty, 115200)
    runClient(transport)
