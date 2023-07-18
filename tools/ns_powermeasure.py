import logging as log

import numpy as np
import pydantic_argparse
from autodeploy.measure_power import generatePowerBinary, measurePower

if __name__ == "__main__":
    # td, i, v, p, c, ee = measurePower()
    # energy = (ee["value"] / 100) * 1000000  # Joules
    # print(f"Ran 100 Invokes: {energy:.3f} uJ per Invoke")
    # # print(f"Joulescope Results! {i}, {v}, {p}, {c}, {e}")

    td, i, v, p, c, eee = measurePower()
    energy = (eee["value"] / 100) * 1000000  # Joules
    # print(f"Ran 100 Invokes: {energy:.3f} uJ per Invoke"
    t = f"{td.seconds}{(td.microseconds):3f}"
    t = td.total_seconds()
    print(
        f"100 invokes at took {t}s at 192Mhz: {t*1000/100} ms and {energy:.3f} uJ per Invoke"
    )
