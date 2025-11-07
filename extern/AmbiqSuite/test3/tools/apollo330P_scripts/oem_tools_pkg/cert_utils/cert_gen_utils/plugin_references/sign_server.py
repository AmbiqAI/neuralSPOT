from flask import Flask, request, Response
import base64
import os
import sys
from ctypes import *

sys.path.append("../../../../")
from oem_tools_pkg.common_utils import cryptolayer
import key_table

app = Flask(__name__)

# Path to public key table file
KEY_PATH = os.path.abspath("../../../../keys.ini")
keytable = key_table.import_key_table(KEY_PATH)

# Path to apollo5a_scripts dir to resolve pathing in keytable
BASE_DIR_PATH = os.path.abspath("../../../../")

#256 aligned with SB_CERT_RSA_KEY_SIZE_IN_BITS defined in cc_pka_hw_plat_defs.h
RSA_SIGNATURE_SIZE_BYTES = 384

@app.route("/sign/<int:keyIdx>", methods=["PUT"])
def sign(keyIdx: int):
    dataIn = request.form["data"]
    DataInSize = len(dataIn)
    Signature = create_string_buffer(RSA_SIGNATURE_SIZE_BYTES)

    PrivKeyFile = os.path.join(BASE_DIR_PATH, keytable[keyIdx].filename)
    PassphraseFile = os.path.join(BASE_DIR_PATH, keytable[keyIdx].pass_file)

    signature = cryptolayer.Common.rsa_sign(dataIn, PrivKeyFile, PassphraseFile)

    return {"signature": signature}

@app.route("/verify/<int:keyIdx>", methods=["GET"])
def verify(keyIdx: int):
    privateKey = keytable[keyIdx].filename

    if privateKey == None:
        print(f"Private key not found using index: {keyIdx}.")
        sys.exit(1)

if __name__ == "__main__":

    # DO NOT RUN DEBUG IN PRODUCTION ENV
    app.run(debug=True)
