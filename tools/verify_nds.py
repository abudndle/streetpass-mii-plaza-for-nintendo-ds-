#!/usr/bin/env python3
"""Validate basic Nintendo DS ROM header and ARM executable region bounds."""
import argparse
from pathlib import Path
import struct


def verify(path: Path):
    size = path.stat().st_size
    if size < 512:
        raise ValueError("Not a Nintendo DS ROM: too small")
    with path.open("rb") as f:
        h = f.read(512)
    title = h[:12].split(b"\x00")[0]
    code = h[12:16]
    if not title or any(b < 0x20 or b > 0x7e for b in title):
        raise ValueError("Invalid DS game title")
    if any(b < 0x20 or b > 0x7e for b in code):
        raise ValueError("Invalid DS game code")
    regions = []
    for name, offset in (("ARM9", 0x20), ("ARM7", 0x30)):
        rom_offset, entry, load_addr, length = struct.unpack_from("<IIII", h, offset)
        if length <= 0 or rom_offset < 0x200 or rom_offset + length > size:
            raise ValueError(f"Invalid {name} segment bounds")
        regions.append((name, rom_offset, length))
    return {"title": title.decode("ascii"), "game_code": code.decode("ascii"),
            "rom_bytes": size, "cpu_regions": regions}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("nds", type=Path)
    args = parser.parse_args()
    result = verify(args.nds)
    for key, value in result.items():
        print(f"{key}: {value}")


if __name__ == "__main__":
    main()
