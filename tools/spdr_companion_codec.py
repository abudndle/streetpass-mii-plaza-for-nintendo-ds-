#!/usr/bin/env python3
"""Portable reference codec for the independent SPDR v1 relay protocol.

This is a host-side interoperability fixture for a future 3DS homebrew
companion, not a native CECD or wireless implementation.
"""
import binascii
import struct

RELAY_SIZE = 80
MAX_HOPS = 4


def encode(sender_id, timestamp, nonce, name, hops, originated):
    if not 0 < sender_id < 2**64 or not 0 <= hops <= MAX_HOPS:
        raise ValueError("invalid sender or hop count")
    if not isinstance(name, str) or not 1 <= len(name) <= 31:
        raise ValueError("invalid name length")
    try:
        encoded = name.encode("ascii")
    except UnicodeEncodeError as exc:
        raise ValueError("non-ASCII name") from exc
    if any(c < 32 or c > 126 for c in encoded):
        raise ValueError("invalid name characters")
    if not 0 <= timestamp < 2**32 or not 0 <= nonce < 2**32 or not 0 <= originated < 2**32:
        raise ValueError("invalid timestamp or nonce")
    record = bytearray(64)
    record[:4] = b"SPDS"
    record[4] = 1
    record[6] = 64
    struct.pack_into("<Q", record, 8, sender_id)
    struct.pack_into("<I", record, 16, timestamp)
    record[20:20 + len(encoded)] = encoded
    struct.pack_into("<I", record, 52, nonce)
    struct.pack_into("<I", record, 60, binascii.crc32(record[:60]))
    packet = bytearray(80)
    packet[:4] = b"SPDR"
    packet[4] = 1
    packet[5] = hops
    packet[8:72] = record
    struct.pack_into("<I", packet, 72, originated)
    struct.pack_into("<I", packet, 76, binascii.crc32(packet[:76]))
    return bytes(packet)


def decode(packet):
    if len(packet) != RELAY_SIZE or packet[:4] != b"SPDR" or packet[4] != 1 or packet[5] > MAX_HOPS or packet[6:8] != b"\0\0":
        raise ValueError("invalid relay header")
    if struct.unpack_from("<I", packet, 76)[0] != binascii.crc32(packet[:76]):
        raise ValueError("invalid relay CRC")
    record = packet[8:72]
    if record[:4] != b"SPDS" or record[4] != 1 or record[5] or record[6] != 64 or record[7] or record[56:60] != b"\0" * 4:
        raise ValueError("invalid visitor header")
    if struct.unpack_from("<I", record, 60)[0] != binascii.crc32(record[:60]):
        raise ValueError("invalid visitor CRC")
    raw = record[20:52]
    if b"\0" not in raw:
        raise ValueError("unterminated visitor name")
    name, padding = raw.split(b"\0", 1)
    if not name or any(c < 32 or c > 126 for c in name) or any(padding):
        raise ValueError("invalid visitor name")
    sender = struct.unpack_from("<Q", record, 8)[0]
    if not sender:
        raise ValueError("zero sender")
    return dict(sender_id=sender, timestamp=struct.unpack_from("<I", record, 16)[0],
                nonce=struct.unpack_from("<I", record, 52)[0], name=name.decode("ascii"),
                hops=packet[5], originated=struct.unpack_from("<I", packet, 72)[0])


def forward(packet):
    fields = decode(packet)
    if fields["hops"] == 0:
        raise ValueError("hop budget exhausted")
    return encode(fields["sender_id"], fields["timestamp"], fields["nonce"],
                  fields["name"], fields["hops"] - 1, fields["originated"])
