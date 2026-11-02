#!/usr/bin/env python3
"""Private local extraction/decompression of personally supplied decrypted NCCH.

No Nintendo assets or binary outputs should ever be added to this public repo.
"""
import argparse
from pathlib import Path
import json
import struct
from romfs_inventory import inventory

MAX_DECOMPRESSED = 64 * 1024 * 1024


def decompress_lz(data: bytes) -> bytes:
    """Validate and decode Nintendo LZ10 (0x10) and LZ11 (0x11)."""
    if len(data) < 4 or data[0] not in (0x10, 0x11):
        raise ValueError("Not LZ10 or LZ11 data")
    kind, count = data[0], int.from_bytes(data[1:4], "little")
    pos = 4
    if not count:
        if len(data) < 8:
            raise ValueError("Truncated extended header")
        count = int.from_bytes(data[4:8], "little")
        pos = 8
    if count == 0 or count > MAX_DECOMPRESSED:
        raise ValueError("Unsafe uncompressed size")
    result = bytearray()
    while len(result) < count:
        if pos >= len(data):
            raise ValueError("Missing flag byte")
        flags = data[pos]
        pos += 1
        for bit in range(7, -1, -1):
            if len(result) == count:
                break
            if not flags & (1 << bit):
                if pos >= len(data):
                    raise ValueError("Missing literal")
                result.append(data[pos])
                pos += 1
                continue
            if pos >= len(data):
                raise ValueError("Missing match marker")
            b = data[pos]
            pos += 1
            nibble = b >> 4
            if kind == 0x10:
                if pos >= len(data):
                    raise ValueError("Missing LZ10 match byte")
                b2 = data[pos]
                pos += 1
                length = nibble + 3
                distance = ((b & 15) << 8 | b2) + 1
            elif nibble == 0:
                if pos + 2 > len(data):
                    raise ValueError("Missing LZ11 extended match")
                b2, b3 = data[pos:pos+2]
                pos += 2
                length = ((b & 15) << 4 | (b2 >> 4)) + 0x11
                distance = ((b2 & 15) << 8 | b3) + 1
            elif nibble == 1:
                if pos + 3 > len(data):
                    raise ValueError("Missing LZ11 long match")
                b2, b3, b4 = data[pos:pos+3]
                pos += 3
                length = ((b & 15) << 12 | b2 << 4 | b3 >> 4) + 0x111
                distance = ((b3 & 15) << 8 | b4) + 1
            else:
                if pos >= len(data):
                    raise ValueError("Missing LZ11 match byte")
                b2 = data[pos]
                pos += 1
                length = nibble + 1
                distance = ((b & 15) << 8 | b2) + 1
            if distance > len(result) or len(result) + length > count:
                raise ValueError("Bad match offset or output overflow")
            for _ in range(length):
                result.append(result[-distance])
    return bytes(result)


def extract(ncch: Path, output: Path, do_decompress: bool) -> dict:
    """Extract RomFS into the user's *private* directory only."""
    data = ncch.read_bytes()
    manifest = inventory(ncch)
    rom = struct.unpack_from("<I", data, 0x1b0)[0] * 512
    block_size = 1 << struct.unpack_from("<I", data, rom + 0x4c)[0]
    master_size = struct.unpack_from("<I", data, rom + 8)[0]
    level3 = rom + ((0x60 + master_size + block_size - 1) // block_size) * block_size
    data_off = struct.unpack_from("<I", data, level3 + 0x24)[0]
    output.mkdir(parents=True, exist_ok=True)
    signatures = {}
    unpacked = []
    for fileinfo in manifest["files"]:
        relative = Path(fileinfo["path"].lstrip("/"))
        if relative.is_absolute() or any(p in ("", ".", "..") for p in relative.parts):
            raise ValueError("Potential path traversal")
        start = level3 + data_off + fileinfo["data_offset"]
        size = fileinfo["size"]
        if start < 0 or start + size > len(data):
            raise ValueError("RomFS file beyond NCCH")
        original = output / "romfs" / relative
        original.parent.mkdir(parents=True, exist_ok=True)
        original.write_bytes(data[start:start+size])
        decoded = None
        if do_decompress and original.suffix.lower() in (".lz", ".lz77"):
            payload = decompress_lz(data[start:start+size])
            decoded = output / "decompressed" / relative.with_suffix(".bin")
            decoded.parent.mkdir(parents=True, exist_ok=True)
            decoded.write_bytes(payload)
            signature = payload[:4].hex()
            signatures[signature] = signatures.get(signature, 0) + 1
        unpacked.append({"path": fileinfo["path"], "size": size,
                         "decompressed": decoded is not None})
    result = {"product_code": manifest["product_code"],
              "files": len(unpacked), "decompressed_count":
              sum(bool(f["decompressed"]) for f in unpacked),
              "decoded_magic_counts": signatures, "inventory": unpacked}
    (output / "metadata.json").write_text(json.dumps(result, indent=2) + "\n")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ncch", type=Path)
    parser.add_argument("--out", type=Path, required=True,
                        help="PRIVATE output directory, never commit to GitHub")
    parser.add_argument("--decompress", action="store_true",
                        help="also decode LZ10/LZ11 resources locally")
    args = parser.parse_args()
    result = extract(args.ncch, args.out, args.decompress)
    print(json.dumps({k: v for k, v in result.items() if k != "inventory"},
                     indent=2))
    print("Private output:", args.out)


if __name__ == "__main__":
    main()
