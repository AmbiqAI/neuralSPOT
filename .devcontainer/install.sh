#!/bin/bash
set -euo pipefail

# pip older than 26.2.2 still trusts the emSign roots, so it is upgraded from the
# hash-pinned wheel given in the Python maintainers' notice; see AmbiqAI/neuralSPOT#308.
if ! python - <<'PY'
import sys
from importlib.metadata import version
from pip._vendor.packaging.version import Version
sys.exit(Version(version("pip")) < Version("26.2.2"))
PY
then
    python -m pip install "pip @ https://files.pythonhosted.org/packages/3f/06/3e4420302363fbb71be3c7f35ce042f54bfca330e3e66b3a2e3c4cca579f/pip-26.2.2-py3-none-any.whl#sha256=fd7f4dfa8190e014013b9f08f3e973f4e4c26ffd179a204895533a7254813a70"
fi
pip install .
