import abc
import os
import sys
from ctypes import create_string_buffer

sys.path.append("../../../")
from oem_tools_pkg.common_utils import cryptolayer
from key_table import import_key_table

class Interface(abc.ABC):

    @abc.abstractmethod
    def sign(self, keyIdx: str, dataIn: bytes) -> bytes:
        '''Return signature for data'''

    def get_signature(self, keyIdx: str, dataIn: bytes) -> bytes:
        signature = self.sign(keyIdx, dataIn)
        return signature

class LocalInterface(Interface):
    """ Ambiq Signature Provider for local keys signing. """

    def __init__(self, **kwargs) -> None:
        self.params = kwargs
        self.keysFile = kwargs["keytable"]

    def get_signature_size(self):
        signature_size = 384 #256 aligned with SB_CERT_RSA_KEY_SIZE_IN_BITS defined in cc_pka_hw_plat_defs.h
        return signature_size

    def set_keys_table(self, keysFile):
        self.keysFile = keysFile

    def sign(self, keyIdx: str, dataIn: bytes) -> bytes:
        try:

            keys = import_key_table(self.keysFile)
            self.keyFilePath = keys[keyIdx].filename
            self.pwdFilePath = keys[keyIdx].pass_file

            currDir = os.getcwd()
            os.chdir(os.path.dirname(os.path.abspath(self.keysFile)))

            signature = cryptolayer.Common.rsa_sign(dataIn, self.keyFilePath, self.pwdFilePath)

            signature = signature[::-1]  # reversing the signature for compatibility reasons
            os.chdir(currDir)

        except NameError:
            sys.exit(1)

        return signature

