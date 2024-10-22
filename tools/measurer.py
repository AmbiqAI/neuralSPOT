import random
import sys
import time

import msgpack
import msgpack_numpy as m
import numpy as np
import zmq
from neuralspot.tools.autodeploy.measure_power import measurePower

m.patch()

# Create ZMQ socket
port = "5556"
context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:%s" % port)


def send_msg(socket, d, topic="data"):
    """Send data over ZMQ PubSub socket
    Args:
        socket: zmq.socket instance
        topic: topic to put in ZMQ topic field (str)
    """
    msg = msgpack.packb(d)
    print(msg)
    unp = msgpack.unpackb(msg)
    print(unp)
    sndString = b"data " + msg
    print(sndString)
    return socket.send(sndString)


while True:
    # Send message to socket
    #
    td, i, v, p, c, e = measurePower()
    d = {
        "time": td.total_seconds(),
        "current": i["value"],
        "voltage": v["value"],
        "power": p["value"],
        "charge": c["value"],
        "energy": e["value"],
    }
    send_msg(socket, d, "data")
    print(d)
