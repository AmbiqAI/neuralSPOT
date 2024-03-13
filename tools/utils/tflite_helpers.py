import json
import os
import re
import sys
from pathlib import Path

import numpy as np
import utils.schema_py_generated as schema_fb


def TensorTypeToName(tensor_type):
    """Converts a numerical enum to a readable tensor type."""
    for name, value in schema_fb.TensorType.__dict__.items():
        if value == tensor_type:
            return name
    return None


def BuiltinCodeToName(code):
    """Converts a builtin op code enum to a readable name."""
    for name, value in schema_fb.BuiltinOperator.__dict__.items():
        if value == code:
            return name
    return None


def NameListToString(name_list):
    """Converts a list of integers to the equivalent ASCII string."""
    if isinstance(name_list, str):
        return name_list
    else:
        result = ""
        if name_list is not None:
            for val in name_list:
                result = result + chr(int(val))
        return result


class OpCodeMapper:
    """Maps an opcode index to an op name."""

    def __init__(self, data):
        self.code_to_name = {}
        for idx, d in enumerate(data["operator_codes"]):
            self.code_to_name[idx] = BuiltinCodeToName(d["builtin_code"])
            if self.code_to_name[idx] == "CUSTOM":
                self.code_to_name[idx] = NameListToString(d["custom_code"])
        # print(self.code_to_name)

    def __call__(self, x):
        if x not in self.code_to_name:
            s = "<UNKNOWN>"
        else:
            s = self.code_to_name[x]
        return "%s" % s
        # return "%s (%d)" % (s, x)


class DataSizeMapper:
    """For buffers, report the number of bytes."""

    def __call__(self, x):
        if x is not None:
            return "%d bytes" % len(x)
        else:
            return "--"


def FlatbufferToDict(fb, preserve_as_numpy):
    """Converts a hierarchy of FB objects into a nested dict.

    We avoid transforming big parts of the flat buffer into python arrays. This
    speeds conversion from ten minutes to a few seconds on big graphs.

    Args:
      fb: a flat buffer structure. (i.e. ModelT)
      preserve_as_numpy: true if all downstream np.arrays should be preserved.
        false if all downstream np.array should become python arrays
    Returns:
      A dictionary representing the flatbuffer rather than a flatbuffer object.
    """
    if isinstance(fb, int) or isinstance(fb, float) or isinstance(fb, str):
        return fb
    elif hasattr(fb, "__dict__"):
        result = {}
        for attribute_name in dir(fb):
            attribute = fb.__getattribute__(attribute_name)
            if not callable(attribute) and attribute_name[0] != "_":
                snake_name = CamelCaseToSnakeCase(attribute_name)
                preserve = True if attribute_name == "buffers" else preserve_as_numpy
                result[snake_name] = FlatbufferToDict(attribute, preserve)
        return result
    elif isinstance(fb, np.ndarray):
        return fb if preserve_as_numpy else fb.tolist()
    elif hasattr(fb, "__len__"):
        return [FlatbufferToDict(entry, preserve_as_numpy) for entry in fb]
    else:
        return fb


def CamelCaseToSnakeCase(camel_case_input):
    """Converts an identifier in CamelCase to snake_case."""
    s1 = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", camel_case_input)
    return re.sub("([a-z0-9])([A-Z])", r"\1_\2", s1).lower()


def CreateDictFromFlatbuffer(buffer_data):
    model_obj = schema_fb.Model.GetRootAsModel(buffer_data, 0)
    model = schema_fb.ModelT.InitFromObj(model_obj)
    return FlatbufferToDict(model, preserve_as_numpy=False)


def CreateAddFromSnakeOpName(snake):
    components = snake.split("_")
    addName = "Add"
    for s in components:
        if s == "MATMUL":
            s = "MatMul"
        elif s != "2D":
            s = s.lower().title()
        addName += s

    return addName


class TensorAnalyze:
    """Maps a list of tensor indices to a tooltip hoverable indicator of more."""

    def __init__(self, subgraph_data):
        self.data = subgraph_data

    def __call__(self, x):
        r = {}
        if x is None:
            return r

        for i in x:
            tensor = self.data["tensors"][i]

            r["name"] = NameListToString(tensor["name"]) + ", "
            r["type"] = TensorTypeToName(tensor["type"]) + ", "
            r["shape"] = tensor["shape"] if "shape" in tensor else []

        return r
