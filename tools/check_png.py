#!/usr/bin/env python3
"""Reject PNG metadata and invalid structure; visible pixels still need manual review."""
import argparse
import pathlib
import struct
import zlib

SIG = b'\x89PNG\r\n\x1a\n'
LIMIT = 12 * 1024 * 1024

def check_png(path):
    data = pathlib.Path(path).read_bytes()
    if not data.startswith(SIG) or len(data) > LIMIT:
        raise ValueError('bad PNG signature or file size')
    pos, seen, width, height = 8, [], 0, 0
    while pos < len(data):
        if pos + 12 > len(data):
            raise ValueError('truncated chunk')
        length = struct.unpack_from('>I', data, pos)[0]
        kind = data[pos + 4:pos + 8]
        end = pos + 12 + length
        if length > LIMIT or end > len(data):
            raise ValueError('bad chunk length')
        payload = data[pos + 8:pos + 8 + length]
        crc = struct.unpack_from('>I', data, pos + 8 + length)[0]
        if zlib.crc32(kind + payload) & 0xffffffff != crc:
            raise ValueError('bad chunk CRC')
        if kind not in (b'IHDR', b'IDAT', b'IEND'):
            raise ValueError('unapproved PNG chunk')
        if not seen and kind != b'IHDR':
            raise ValueError('missing IHDR')
        if kind == b'IHDR':
            if seen or length != 13:
                raise ValueError('bad IHDR')
            width, height, depth, color, comp, filt, interlace = struct.unpack('>IIBBBBB', payload)
            if not (256 <= width <= 4096 and 192 <= height <= 4096):
                raise ValueError('unexpected dimensions')
            if (depth, color, comp, filt, interlace) not in ((8, 2, 0, 0, 0), (8, 6, 0, 0, 0)):
                raise ValueError('unsupported PNG pixel format')
        if kind == b'IDAT' and (b'IHDR' not in seen or b'IEND' in seen):
            raise ValueError('IDAT out of order')
        if kind == b'IEND':
            if length or end != len(data) or b'IDAT' not in seen:
                raise ValueError('invalid PNG ending')
        seen.append(kind)
        pos = end
    if not seen or seen[-1] != b'IEND':
        raise ValueError('missing IEND')
    return width, height

if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('image')
    args = ap.parse_args()
    w, h = check_png(args.image)
    print(f'PNG metadata and structure passed ({w}x{h}); inspect pixels manually before upload.')
