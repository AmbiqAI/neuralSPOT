"""neuralSPOT AI enablement package"""

try:
    from importlib.metadata import version

    __version__ = version(__name__)
except ImportError:
    __version__ = "0.0.0"
