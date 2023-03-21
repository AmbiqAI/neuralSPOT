import gzip
import math
import os
import pickle
import sys
import time

import numpy as np

sys.path.append("../neuralspot/ns-rpc/python/ns-rpc-genericdata/")
import erpc
import GenericDataOperations_PcToEvb


def createFromTemplate(templateFile, destinationFile, replaceMap):
    with open(templateFile, "r") as templatefile:
        template = templatefile.read()
    with open(destinationFile, "w+") as writefile:
        for f in replaceMap:
            template = template.replace(str(f), str(replaceMap[f]))
        writefile.write(template)


def load_pkl(file: str, compress: bool = True):
    """Load pickled file.

    Args:
        file (str): File path (.pkl)
        compress (bool, optional): If file is compressed. Defaults to True.

    Returns:
        Any: Contents of pickle
    """
    if compress:
        with gzip.open(file, "rb") as fh:
            return pickle.load(fh)
    else:
        with open(file, "rb") as fh:
            return pickle.load(fh)


def get_dataset(params):
    fn = params.dataset
    if fn and os.path.isfile(fn):
        # print("Loading augmented dataset from " + fn)
        ds = load_pkl(fn)
        return ds["X"], ds["y"], ds["XT"], ds["yt"]


def xxd_c_dump(
    src_path: str,
    dst_path: str,
    var_name: str = "mut_model",
    chunk_len: int = 12,
    is_header: bool = True,
):
    """Generate C like char array of hex values from binary source. Equivalent to `xxd -i src_path > dst_path`
        but with added features to provide # columns and variable name.
    Args:
        src_path (str): Binary file source path
        dst_path (str): C file destination path
        var_name (str, optional): C variable name. Defaults to 'g_model'.
        chunk_len (int, optional): # of elements per row. Defaults to 12.
    """
    var_len = 0
    with open(src_path, "rb", encoding=None) as rfp, open(
        dst_path, "w", encoding="UTF-8"
    ) as wfp:
        if is_header:
            wfp.write(f"#ifndef __{var_name.upper()}_H{os.linesep}")
            wfp.write(f"#define __{var_name.upper()}_H{os.linesep}")

        wfp.write(f"const unsigned char {var_name}[] = {{{os.linesep}")
        for chunk in iter(lambda: rfp.read(chunk_len), b""):
            wfp.write(
                "  " + ", ".join((f"0x{c:02x}" for c in chunk)) + f", {os.linesep}"
            )
            var_len += len(chunk)
        wfp.write(f"}};{os.linesep}")
        wfp.write(f"const unsigned int {var_name}_len = {var_len};{os.linesep}")
        if is_header:
            wfp.write(f"#endif // __{var_name.upper()}_H{os.linesep}")


def printDataBlock(block):
    print("Description: %s" % block.description)
    print("Length: %s" % block.length)
    print("cmd: %s" % block.cmd)
    print("dType: %s" % block.dType)
    # print(block.buffer)
    for i in range(len(block.buffer)):
        print("0x%x " % block.buffer[i], end="")
    print("")


def getDetails(interpreter):
    # Get input/output details, configure EVB
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    numberOfInputs = len(input_details)

    if numberOfInputs > 1:
        print(
            "Warning: model has %d inputs but this scripts only supports 1"
            % numberOfInputs
        )

    numberOfOutputs = len(output_details)
    if numberOfOutputs > 1:
        print(
            "Warning: model has %d outputs but this scripts only supports 1"
            % numberOfOutputs
        )

    inputShape = input_details[0]["shape"]
    inputLength = np.prod(inputShape)  # assuming bytes
    print("[INFO] input tensor total size: %d" % inputLength)
    # print(input_details[0]["shape"])
    print("[INFO] input tensor shape " + repr(input_details[0]["shape"]))

    outputShape = output_details[0]["shape"]
    outputLength = np.prod(outputShape)  # assuming bytes
    print("[INFO] output tensor total size: %d" % outputLength)
    print("[INFO] output tensor shape " + repr(output_details[0]["shape"]))

    return (
        numberOfInputs,
        numberOfOutputs,
        inputShape,
        outputShape,
        inputLength,
        outputLength,
    )


def next_power_of_2(x):
    return 1 if x == 0 else 2 ** math.ceil(math.log2(x))


def reset_dut():
    makefile_result = os.system("cd .. && make reset >/dev/null 2>&1")
    time.sleep(2)  # give jlink a chance to settle


def rpc_connect_as_client(params):
    try:
        transport = erpc.transport.SerialTransport(params.tty, int(params.baud))
        clientManager = erpc.client.ClientManager(
            transport, erpc.basic_codec.BasicCodec
        )
        client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
        return client
    except:
        print("Couldn't establish RPC connection EVB USB device %s" % params.tty)
