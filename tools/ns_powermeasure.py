import logging as log

import numpy as np
import pydantic_argparse
from autodeploy.measure_power import generatePowerBinary, measurePower

if __name__ == "__main__":
    i, v, p, c, e = measurePower()
    energy = (e["value"] / 100) * 1000000  # Joules
    print(f"Ran 100 Invokes: {energy:.3f} uJ per Invoke")
    print(f"Joulescope Results! {i}, {v}, {p}, {c}, {e}")
