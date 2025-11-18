# *****************************************************************************
#
#    socid_to_bin.py
#
#    Utility functions.
#
#    Usage : python3 socid_to_bin.py socid.txt -o socid.bin
#
#    This is an example script that can be used to convert socid.txt into socid.bin file.
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2025, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
#
# *****************************************************************************

import argparse

def lines_to_hex(lines):
    for line in lines:
        if line.strip():
            for x in line.split(' : '):
                yield int(x, 0)

def hex_to_bytes(hex_array):
    for x in hex_array:
        yield x & 0xFF
        yield (x & 0xFF00) >> 8
        yield (x & 0xFF0000) >> 16
        yield (x & 0xFF000000) >> 24

def socid_to_bytes(L):
    return bytes(hex_to_bytes(lines_to_hex(L)))

def main():
    parser = argparse.ArgumentParser('Conver')
    parser.add_argument('socid_file')
    parser.add_argument('-o', dest='output', required=True)

    args = parser.parse_args()

    with open(args.socid_file, "r") as f:
        output_bytes = socid_to_bytes(f)
        with open(args.output, "wb") as output_file:
            output_file.write(output_bytes)

if __name__ == '__main__':
    main()
