# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""pw_rpc protoc plugin entrypoint to generate code for RPC services."""

import enum
import sys

import google.protobuf.compiler.plugin_pb2 as plugin_pb2

import pw_rpc.codegen_nanopb as codegen_nanopb
import pw_rpc.codegen_raw as codegen_raw


class Codegen(enum.Enum):
    RAW = 0
    NANOPB = 1


def process_proto_request(codegen: Codegen,
                          req: plugin_pb2.CodeGeneratorRequest,
                          res: plugin_pb2.CodeGeneratorResponse) -> None:
    """Handles a protoc CodeGeneratorRequest message.

    Generates code for the files in the request and writes the output to the
    specified CodeGeneratorResponse message.

    Args:
      req: A CodeGeneratorRequest for a proto compilation.
      res: A CodeGeneratorResponse to populate with the plugin's output.
    """
    for proto_file in req.proto_file:
        if codegen is Codegen.RAW:
            output_files = codegen_raw.process_proto_file(proto_file)
        elif codegen is Codegen.NANOPB:
            output_files = codegen_nanopb.process_proto_file(proto_file)
        else:
            raise NotImplementedError(f'Unknown codegen type {codegen}')

        for output_file in output_files:
            fd = res.file.add()
            fd.name = output_file.name()
            fd.content = output_file.content()


def main(codegen: Codegen) -> int:
    """Protobuf compiler plugin entrypoint.

    Reads a CodeGeneratorRequest proto from stdin and writes a
    CodeGeneratorResponse to stdout.
    """
    data = sys.stdin.buffer.read()
    request = plugin_pb2.CodeGeneratorRequest.FromString(data)
    response = plugin_pb2.CodeGeneratorResponse()
    process_proto_request(codegen, request, response)
    sys.stdout.buffer.write(response.SerializeToString())
    return 0
