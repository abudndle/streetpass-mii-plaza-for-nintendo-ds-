#!/usr/bin/env python3
"""Generate a private-only DS RGB555 header from an authorized base-game ZIP.

Writes proprietary-derived bytes ONLY to a local/private build workspace.
Never commit generated output to the public source repository.
"""
import argparse
from pathlib import Path
import struct
import zipfile

ENTRY = "decompressed/cec_icon/gameicon_40x40.bin"
WIDTH = HEIGHT = 40

def convert(archive: Path, destination: Path) -> None:
    with zipfile.ZipFile(archive) as z:
        info = z.getinfo(ENTRY)
        if info.file_size != WIDTH * HEIGHT * 2:
            raise ValueError("Unexpected 40x40 icon size")
        pixels = z.read(info)
    if len(pixels) != WIDTH * HEIGHT * 2:
        raise ValueError("Truncated icon")
    values = struct.unpack("<1600H", pixels)
    destination.parent.mkdir(parents=True, exist_ok=True)
    with destination.open("w", encoding="ascii") as out:
        out.write("/* Generated privately; never commit or publish this header. */\n")
        out.write("#ifndef SPDS_PRIVATE_PLAZA_ICON_HEADER_H\n")
        out.write("#define SPDS_PRIVATE_PLAZA_ICON_HEADER_H\n")
        out.write("#include <stdint.h>\n")
        out.write("static const uint16_t spds_private_plaza_icon[1600] = {\n")
        for start in range(0, len(values), 12):
            row = values[start:start + 12]
            out.write("    " + ", ".join(f"0x{(value | 0x8000):04x}u" for value in row) + ",\n")
        out.write("};\n#endif\n")

if __name__ == "__main__":
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("archive", type=Path)
    p.add_argument("output", type=Path)
    a = p.parse_args()
    convert(a.archive, a.output)
    print("Generated private icon header (content not printed)")
