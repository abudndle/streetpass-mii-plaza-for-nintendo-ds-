#!/usr/bin/env python3
"""Remove ancillary PNG chunks while preserving exact compressed image pixels."""
import pathlib
import struct
import sys
import zlib

SIG = b"\x89PNG\r\n\x1a\n"
KEEP = {b"IHDR", b"IDAT", b"IEND"}

def sanitize(path):
    p = pathlib.Path(path)
    data = p.read_bytes()
    if not data.startswith(SIG):
        raise ValueError("not a PNG")
    out = bytearray(SIG)
    pos = len(SIG)
    seen_end = False
    while pos < len(data):
        if pos + 12 > len(data):
            raise ValueError("truncated PNG chunk")
        n = struct.unpack_from(">I", data, pos)[0]
        kind = data[pos + 4:pos + 8]
        end = pos + 12 + n
        if end > len(data):
            raise ValueError("truncated PNG payload")
        payload = data[pos + 8:pos + 8 + n]
        crc = struct.unpack_from(">I", data, pos + 8 + n)[0]
        if zlib.crc32(kind + payload) & 0xffffffff != crc:
            raise ValueError("PNG CRC mismatch")
        if kind in KEEP:
            out.extend(data[pos:end])
        if kind == b"IEND":
            if n or end != len(data):
                raise ValueError("invalid PNG end")
            seen_end = True
            break
        pos = end
    if not seen_end:
        raise ValueError("missing PNG end")
    p.write_bytes(out)

if __name__ == "__main__":
    sanitize(sys.argv[1])
    print("Removed ancillary PNG chunks without changing IDAT pixel data")
