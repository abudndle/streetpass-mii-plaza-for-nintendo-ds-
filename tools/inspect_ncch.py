#!/usr/bin/env python3
"""Inspect header only, never dump or redistribute decrypted game content."""
import argparse
import hashlib
import json
from pathlib import Path

def parse_ncch(path: Path):
    with path.open("rb") as f:
        header = f.read(512)
    if len(header) != 512 or header[256:260] != b"NCCH":
        raise ValueError("Not an NCCH file (bad header/magic)")
    u32 = lambda off: int.from_bytes(header[off:off + 4], "little")
    size = path.stat().st_size
    parts = {}
    for name, off in (("plain", 0x190), ("logo", 0x198),
                      ("exefs", 0x1A0), ("romfs", 0x1B0)):
        start, count = u32(off), u32(off + 4)
        valid = (start + count) * 512 <= size
        parts[name] = {"offset": start * 512, "size": count * 512,
                       "bounds_valid": valid}
    return {"magic": "NCCH", "product_code":
            header[0x150:0x160].split(b"\x00")[0].decode("ascii", "replace"),
            "size": size, "exheader_bytes": u32(0x180),
            "partitions": parts}

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("ncch", type=Path)
    ap.add_argument("--hash", action="store_true", help="also compute SHA256")
    a = ap.parse_args()
    result = parse_ncch(a.ncch)
    if a.hash:
        with a.ncch.open("rb") as f:
            result["sha256"] = hashlib.file_digest(f, "sha256").hexdigest()
    print(json.dumps(result, indent=2))

if __name__ == "__main__":
    main()
