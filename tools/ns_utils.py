import gzip
import math
import os
import pickle
import sys
import time
import serial.tools.list_ports
import pkg_resources
import yaml
import numpy as np
import logging as log

import erpc
import neuralspot.rpc.GenericDataOperations_PcToEvb as GenericDataOperations_PcToEvb


def createFromTemplate(templateFile, destinationFile, replaceMap):
    # print("Here %s, %s" % (templateFile, destinationFile))
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
    loc: str = "const",
):
    """Generate C like char array of hex values from binary source. Equivalent to `xxd -i src_path > dst_path`
        but with added features to provide # columns and variable name.
    Args:
        src_path (str): Binary file source path
        dst_path (str): C file destination path
        var_name (str, optional): C variable name. Defaults to 'mut_model'.
        chunk_len (int, optional): # of elements per row. Defaults to 12.
        is_header (bool, optional): Generate header file. Defaults to True.

    """
    var_len = 0
    with open(src_path, "rb", encoding=None) as rfp, open(
        dst_path, "w", encoding="UTF-8"
    ) as wfp:
        if is_header:
            wfp.write(f"#ifndef __{var_name.upper()}_H{os.linesep}")
            wfp.write(f"#define __{var_name.upper()}_H{os.linesep}")

        if loc == "const":
            prefix = "const"
        elif loc == "sram":
            prefix = "AM_SHARED_RW"
        else: # TCM
            prefix = "NS_PUT_IN_TCM"

        wfp.write(f"alignas(16) {prefix} unsigned char {var_name}[] = {{{os.linesep}")
        for chunk in iter(lambda: rfp.read(chunk_len), b""):
            wfp.write(
                "  " + ", ".join((f"0x{c:02x}" for c in chunk)) + f", {os.linesep}"
            )
            var_len += len(chunk)
        wfp.write(f"}};{os.linesep}")

        # print("location of model is %s" % prefix)
        wfp.write(f"unsigned int {var_name}_len = {var_len};{os.linesep}")
        wfp.write(f"#define {var_name}_LEN {var_len} {os.linesep}")
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


class TensorDetails:
    def __init__(self, details):
        self.shape = details["shape"]
        self.type = details["dtype"]
        self.scale, self.zeroPoint = details["quantization"]
        self.words = np.prod(self.shape)
        self.bytes = self.words * np.dtype(self.type).itemsize

    def __str__(self):
        return (
            f"[INFO] Data Type = {self.type}\n"
            + f"[INFO] Length in Words = {self.words}\n"
            + f"[INFO] Length in Bytes = {self.bytes}"
        )


class ModelDetails:
    def __init__(self, interpreter):
        self.input_details = interpreter.get_input_details()
        self.output_details = interpreter.get_output_details()
        self.numInputs = len(self.input_details)
        self.numOutputs = len(self.output_details)
        self.inputTensors = []
        self.totalInputTensorBytes = 0
        for i in range(self.numInputs):
            self.inputTensors.append(TensorDetails(self.input_details[i]))
            self.totalInputTensorBytes += self.inputTensors[i].bytes

        self.outputTensors = []
        self.totalOutputTensorBytes = 0
        for i in range(self.numOutputs):
            self.outputTensors.append(TensorDetails(self.output_details[i]))
            self.totalOutputTensorBytes += self.outputTensors[i].bytes

    def __str__(self):
        return (
            f"[INFO] Number of Input Tensors = {self.numInputs}\n"
            + f"[INFO] Number of output Tensors = {self.numOutputs}\n"
            # + f"[INFO] Input Tensor Details:\n"
            # + str(self.inputTensors)
            # + f"[INFO] Output Tensor Details:\n"
            # + str(self.outputTensors)
        )

def read_pmu_definitions(params):
    if params.platform not in ["apollo510_eb", "apollo510_evb"] and params.pmu_config_file != "default":
        print("[WARNING] PMU definitions are only available for Apollo3")
        return None
    
    if params.pmu_config_file == "default":
        # Read PMU definitions from yaml file
        yaml_path = pkg_resources.resource_filename(__name__, 'autodeploy/profiles/ns_pmu_default.yaml')
    else:
        yaml_path = params.pmu_config_file    

    # Read PMU definitions from yaml file
    # yaml_path = pkg_resources.resource_filename(__name__, 'ns_pmu.yaml')

    with open(yaml_path, "r") as stream:
        try:
            pmu_defs = yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            print(exc)
            exit("Error reading PMU definitions")
    return pmu_defs

def next_power_of_2(x):
    return 1 if x == 0 else 2 ** math.ceil(math.log2(x))


def reset_dut(params):
    # Windows sucks
    if os.name == "posix":
        ws3 = "/dev/null"
        ws = "-j"
        ws1 = "&&"
    else:
        ws3 = "NUL"
        ws = ""
        ws1 = "&"
        # d = d.replace("/", "\\")
    ps = f"PLATFORM={params.platform} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"

    makefile_result = os.system(f"cd {params.neuralspot_rootdir} {ps} {ws1} make reset >{ws3} 2>&1")
    time.sleep(2)  # give jlink a chance to settle

tty = None

def find_tty(params):
    # Find the TTY of our device by scanning serial USB or UART devices
    # The VID of our device is alway 0xCAFE
    # This is a hack to find the tty of our device, it will break if there are multiple devices with the same VID
    # or if the VID changes
    tty = None
    ports = serial.tools.list_ports.comports()

    if params.transport is 'USB':
        # Look for USB CDC
        for p in ports:
            # print(p)
            # print(p.vid)
            if p.vid == 51966:
                tty = p.device
                log.info("Found USB CDC device %s" % tty)
                break
    else:
        # Look for UART
        for p in ports:
            # print(p)
            # print(p.vid)
            if p.vid == 4966:
                tty = p.device
                log.info("Found UART device %s" % tty)
                break

    return tty

def rpc_connect_as_client(params):
    try:
        # Find the TTY of our device by scanning serial USB devices
        # The VID of our device is alway 0xCAFE
        if params.tty is 'auto':
            tty = find_tty(params)
            if tty is None:
                print(f"Couldn't find tty device on {params.transport}, trying a reset")
                # reset pyserial
                
                reset_dut(params)
                tty = find_tty(params)
                if tty is None:
                    print("Couldn't find tty device after reset")
                    exit(1)

        transport = erpc.transport.SerialTransport(tty, 115200)
        clientManager = erpc.client.ClientManager(
            transport, erpc.basic_codec.BasicCodec
        )
        client = GenericDataOperations_PcToEvb.client.pc_to_evbClient(clientManager)
        return client
    except:
        print("Couldn't establish RPC connection EVB USB device %s" % tty)
