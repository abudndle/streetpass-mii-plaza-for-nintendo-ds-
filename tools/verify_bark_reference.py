#!/usr/bin/env python3
"""Compare private Nintendogs ROM sections to private extracted components."""
import argparse
import json
import struct
from pathlib import Path

SECTIONS = {"header": None, "arm9": (0x20, 0x2c), "arm7": (0x30, 0x3c),
            "fnt": (0x40, 0x44), "fat": (0x48, 0x4c),
            "a9ovr": (0x50, 0x54), "a7ovr": (0x58, 0x5c)}

def inspect(rom, folder):
    rom, folder = Path(rom), Path(folder)
    with rom.open("rb") as f:
        header = f.read(0x4000)
    if len(header) != 0x4000 or header[12:16] != b"AD3E":
        raise ValueError("Expected complete Nintendogs AD3E DS header")
    size = rom.stat().st_size
    result = {"game_code": "AD3E", "rom_bytes": size, "sections": {}}
    with rom.open("rb") as f:
        for name, fields in SECTIONS.items():
            offset, count = (0, 0x4000) if fields is None else (
                struct.unpack_from("<I", header, fields[0])[0],
                struct.unpack_from("<I", header, fields[1])[0])
            if offset + count > size:
                raise ValueError("Invalid ROM section: " + name)
            files = [p for p in folder.iterdir() if p.is_file() and
                     (p.name == name + ".bin" or
                      (p.name.startswith(name + "(") and p.name.endswith(").bin") and
                       p.name[len(name)+1:-5].isdigit()))]
            if len(files) > 1:
                raise ValueError("Ambiguous extracted section: " + name)
            if not files:
                continue
            f.seek(offset)
            expected = f.read(count)
            extracted = files[0].read_bytes()
            result["sections"][name] = {
                "offset": offset, "expected_bytes": count,
                "extracted_bytes": len(extracted),
                "prefix_matches": len(extracted) >= count and extracted[:count] == expected,
                "exact_matches": extracted == expected,
                "trailing_bytes": max(0, len(extracted)-count)}
    result["all_declared_sections_match"] = all(
        section["prefix_matches"] for section in result["sections"].values())
    return result

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("rom", type=Path)
    parser.add_argument("extract_directory", type=Path)
    args = parser.parse_args()
    print(json.dumps(inspect(args.rom, args.extract_directory), sort_keys=True, indent=2))
