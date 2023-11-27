import multiprocessing

import dash
import msgpack
import msgpack_numpy as m
import plotly
import zmq
from dash import Dash, Input, Output, callback, dcc, html

m.patch()

external_stylesheets = ["https://codepen.io/chriddyp/pen/bWLwgP.css"]

app = Dash(__name__, external_stylesheets=external_stylesheets)
app.layout = html.Div(
    html.Div(
        [
            html.H4("Apollo4 HAR Performance"),
            html.Div(id="live-update-text"),
            dcc.Graph(id="live-update-graph"),
            dcc.Interval(
                id="interval-component",
                interval=1 * 1000,  # in milliseconds
                n_intervals=0,
            ),
        ]
    )
)

data = {"Time": [], "Energy": [], "Watts": [], "Current": []}


def create_zmq_socket(zmq_port="5556", topicfilter="data"):
    """Create a ZMQ SUBSCRIBE socket"""
    context = zmq.Context()
    zmq_socket = context.socket(zmq.SUB)
    zmq_socket.connect("tcp://localhost:%s" % zmq_port)
    zmq_socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)
    print(zmq_socket)
    return zmq_socket


graphData = {
    "SampleNumber": [],
    "Latency": [],
    "Latency APx": [],
    "Joules": [],
    "Joules APx": [],
}
SampleNumber = 0


@callback(
    Output("live-update-graph", "figure"), Input("interval-component", "n_intervals")
)
def update_graph_live(n):

    # Create the graph with subplots
    global queue
    global SampleNumber
    global graphData
    try:
        d = queue.get(block=False)
        print(d)
        graphData["SampleNumber"].pop(0)
        graphData["Latency"].pop(0)
        graphData["Latency APx"].pop(0)
        graphData["Joules"].pop(0)
        graphData["Joules APx"].pop(0)
        graphData["SampleNumber"].append(SampleNumber)
        SampleNumber += 1
        graphData["Latency"].append(d["time"])
        graphData["Latency APx"].append(d["time"] / 6)

        graphData["Joules"].append(d["energy"])
        graphData["Joules APx"].append(d["energy"] / 5)
    except:
        # print("Queue empty")
        pass

    fig = plotly.tools.make_subplots(rows=2, cols=1, vertical_spacing=0.2)
    fig["layout"]["margin"] = {"l": 30, "r": 10, "b": 30, "t": 10}
    fig["layout"]["legend"] = {"x": 0, "y": 1, "xanchor": "left"}

    fig.append_trace(
        {
            "x": graphData["SampleNumber"],
            "y": graphData["Latency"],
            "name": "AP4 Latency",
            "mode": "lines+markers",
            "type": "scatter",
        },
        1,
        1,
    )

    fig.append_trace(
        {
            "x": graphData["SampleNumber"],
            "y": graphData["Latency APx"],
            "name": "APx Latency",
            "mode": "lines+markers",
            "type": "scatter",
        },
        1,
        1,
    )

    fig.append_trace(
        {
            "x": graphData["SampleNumber"],
            "y": graphData["Joules"],
            "text": graphData["Joules"],
            "name": "AP4 Joules per Inference",
            "mode": "lines+markers",
            "type": "scatter",
        },
        2,
        1,
    )

    fig.append_trace(
        {
            "x": graphData["SampleNumber"],
            "y": graphData["Joules APx"],
            "text": graphData["Joules"],
            "name": "APx Joules per Inference",
            "mode": "lines+markers",
            "type": "scatter",
        },
        2,
        1,
    )

    return fig


def dataClient(q):
    print("Starting data client")
    z = create_zmq_socket()
    global data
    while True:
        msg = z.recv()
        print("Before unpack")
        msgdata = msg[len("data") + 1 :]
        d = msgpack.unpackb(msgdata)
        q.put(d)


if __name__ == "__main__":
    global queue
    for i in range(20):
        graphData["SampleNumber"].append(i)
        graphData["Latency"].append(0)
        graphData["Latency APx"].append(0)
        graphData["Joules"].append(0)
        graphData["Joules APx"].append(0)
    SampleNumber = 20
    queue = multiprocessing.Queue()
    p = multiprocessing.Process(target=dataClient, args=(queue,))
    print("Starting process")
    p.start()
    app.run()
