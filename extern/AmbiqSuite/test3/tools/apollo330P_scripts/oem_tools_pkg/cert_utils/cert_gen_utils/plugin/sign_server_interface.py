import requests
import base64
from plugin.signature_interface import Interface

# from cert_dbg_util_data import *
class HTTPInterface(Interface):
    """ Ambiq Signature Provider for remote signing service. """

    def __init__(self, **kwargs) -> None:
        """Initialize the HTTP signature provider.
        :param keyIdx: index of the key to use
        """
        self.url = kwargs["url"]
        self.userID = kwargs["user-id"]
        self.password = kwargs["password"]

    def sign(self, keyIdx: str, dataIn: bytes) -> bytes:
        """Return the signature for the given data.
        :param dataIn: Data to sign
        :return: Signature as bytes object
        """

        # Ensure keyIdx are of type integer if not already
        if type(keyIdx) is not int:
            keyIdx = int(keyIdx, 0)

        endpoint = "{url}/sign/{keyIdx}".format(url=self.url, keyIdx=keyIdx)
        params = {"data": dataIn}
        response = requests.put(endpoint, data=params, timeout=30)
        signature = response.json()["signature"]

        # Must turn json str type back into bytes object
        signature = base64.b64decode(signature)
        return signature

