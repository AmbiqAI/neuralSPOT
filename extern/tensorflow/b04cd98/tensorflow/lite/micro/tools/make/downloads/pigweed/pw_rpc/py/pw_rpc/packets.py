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
"""Functions for working with pw_rpc packets."""

from google.protobuf import message
from pw_status import Status
from pw_rpc import packet_pb2

DecodeError = message.DecodeError
Message = message.Message

PacketType = packet_pb2.PacketType


def decode(data: bytes):
    packet = packet_pb2.RpcPacket()
    packet.MergeFromString(data)
    return packet


def decode_payload(packet, payload_type):
    payload = payload_type()
    payload.MergeFromString(packet.payload)
    return payload


def _ids(rpc: tuple) -> tuple:
    return tuple(item if isinstance(item, int) else item.id for item in rpc)


def encode_request(rpc: tuple, request: message.Message) -> bytes:
    channel, service, method = _ids(rpc)

    return packet_pb2.RpcPacket(
        type=packet_pb2.PacketType.REQUEST,
        channel_id=channel,
        service_id=service,
        method_id=method,
        payload=request.SerializeToString()).SerializeToString()


def encode_response(rpc: tuple, response: message.Message) -> bytes:
    channel, service, method = _ids(rpc)

    return packet_pb2.RpcPacket(
        type=packet_pb2.PacketType.RESPONSE,
        channel_id=channel,
        service_id=service,
        method_id=method,
        payload=response.SerializeToString()).SerializeToString()


def encode_client_error(packet, status: Status) -> bytes:
    return packet_pb2.RpcPacket(type=packet_pb2.PacketType.CLIENT_ERROR,
                                channel_id=packet.channel_id,
                                service_id=packet.service_id,
                                method_id=packet.method_id,
                                status=status.value).SerializeToString()


def encode_cancel(rpc: tuple) -> bytes:
    channel, service, method = _ids(rpc)
    return packet_pb2.RpcPacket(
        type=packet_pb2.PacketType.CANCEL_SERVER_STREAM,
        channel_id=channel,
        service_id=service,
        method_id=method).SerializeToString()


def for_server(packet):
    return packet.type % 2 == 0
