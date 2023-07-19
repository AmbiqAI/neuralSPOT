#!/usr/bin/env python3
"""
Ambiq's AI Enablement library.

It implements an AI-centric API for common tasks such as collecting audio
from Ambiq's peripherals, computing features from that audio, controlling
power modes, reading accelerometer and gyroscopic data from i2c sensors,
and a variety of helper functions and tools which make developing AI
features on Ambiq hardware easier.
"""

from modulefinder import packagePathMap

from setuptools import setup

# TODO: get canoncial version once available
# version_str = open("version.txt", "r", encoding="utf-8").read()
version_str = "0.0.1"

setup(
    name="neuralspot",
    version=version_str,
    author="Ambiq AI",
    description=__doc__.split(".")[0].strip(),
    long_description=__doc__.split(".")[1].strip(),
    url="https://github.com/ambiqai/neuralSPOT",
    packages=[
        "neuralspot",
        "neuralspot.rpc",
        "neuralspot.rpc.GenericDataOperations_EvbToPc",
        "neuralspot.rpc.GenericDataOperations_PcToEvb",
    ],
    package_dir={
        "neuralspot.rpc": "neuralspot/ns-rpc/python/ns-rpc-genericdata",
        "neuralspot.rpc.GenericDataOperations_EvbToPc": "neuralspot/ns-rpc/python/ns-rpc-genericdata/GenericDataOperations_EvbToPc",
        "neuralspot.rpc.GenericDataOperations_PcToEvb": "neuralspot/ns-rpc/python/ns-rpc-genericdata/GenericDataOperations_PcToEvb",
    },
    install_requires=[
        "numpy>=1.21.6",
        "soundfile>=0.10.3",
        "pyserial",
        "pyjoulescope",
        "erpc @ git+https://github.com/ambiqai/erpc.git#egg=erpc&subdirectory=erpc_python=1.9.1",
    ],
)
