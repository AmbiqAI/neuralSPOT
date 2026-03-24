"""Legacy NSX entrypoint shim.

NSX tooling is now owned by neuralspotx.
"""

from __future__ import annotations

import sys


def main() -> None:
    raise SystemExit(
        "NSX CLI has moved to neuralspotx.\n"
        "Use:\n"
        "  cd neuralspotx\n"
        "  uv sync\n"
        "  uv run nsx --help"
    )


if __name__ == "__main__":
    main()
