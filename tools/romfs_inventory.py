#!/usr/bin/env python3
"""PRIVATE input: produce a 3DS NCCH RomFS metadata inventory, no asset bytes."""
import argparse
from collections import Counter
import json
from pathlib import Path
import struct

END = 0xffffffff


def inventory(path: Path):
    data = path.read_bytes()  # Local only; never copy proprietary bytes to GitHub.
    if len(data) < 512 or data[0x100:0x104] != b"NCCH":
        raise ValueError("Expected standalone NCCH executable image")
    def u32(offset):
        if offset < 0 or offset + 4 > len(data):
            raise ValueError("Invalid offset")
        return struct.unpack_from("<I", data, offset)[0]
    rom = u32(0x1b0) * 512
    units = u32(0x1b4)
    if not units or rom + units * 512 > len(data) or data[rom:rom+4] != b"IVFC":
        raise ValueError("Missing or out-of-bounds decrypted IVFC RomFS")
    block_exp = u32(rom + 0x4c)
    if not 9 <= block_exp <= 16:
        raise ValueError("Implausible level 3 block exponent")
    block = 1 << block_exp
    master_size = u32(rom + 8)
    level3 = rom + (0x60 + master_size + block - 1) // block * block
    if level3 + 40 > rom + units * 512:
        raise ValueError("Level 3 header out of bounds")
    h = struct.unpack_from("<10I", data, level3)
    head_size, _, _, dir_off, dir_len, _, _, file_off, file_len, data_off = h
    if head_size != 40:
        raise ValueError("Unsupported level 3 header")
    dstart, fstart = level3 + dir_off, level3 + file_off
    for start, size in ((dstart, dir_len), (fstart, file_len)):
        if start < level3 or start + size > rom + units * 512:
            raise ValueError("Metadata table beyond RomFS")
    directories, files, seen_d, seen_f = [], [], set(), set()

    def utf_name(start, length):
        if length > 4096 or length % 2 or start + length > len(data):
            raise ValueError("Invalid UTF-16 name")
        return data[start:start+length].decode("utf-16-le")

    def walk(offset, prefix):
        if offset in seen_d or offset + 24 > dir_len:
            raise ValueError("Cyclic or out-of-bounds directory")
        seen_d.add(offset)
        parent, sibling, child_dir, child_file, next_hash, nlen = struct.unpack_from(
            "<6I", data, dstart + offset)
        current = prefix + ("/" + utf_name(dstart + offset + 24, nlen) if nlen else "")
        directories.append(current or "/")
        while child_file != END:
            if child_file in seen_f or child_file + 32 > file_len:
                raise ValueError("Cyclic or out-of-bounds file")
            seen_f.add(child_file)
            _, nxt, data_offset, nbytes, _, fnlen = struct.unpack_from(
                "<IIQQII", data, fstart + child_file)
            fname = utf_name(fstart + child_file + 32, fnlen)
            if data_off + data_offset + nbytes > units * 512:
                raise ValueError("File data exceeds RomFS")
            files.append({"path": current + "/" + fname, "size": nbytes,
                          "data_offset": data_offset})
            child_file = nxt
        siblings = set()
        while child_dir != END:
            if child_dir in siblings or child_dir + 24 > dir_len:
                raise ValueError("Cyclic or out-of-bounds sibling")
            siblings.add(child_dir)
            nxt = u32(dstart + child_dir + 4)
            walk(child_dir, current)
            child_dir = nxt

    walk(0, "")
    categories = Counter()
    total = Counter()
    for f in files:
        category = f["path"].lstrip("/").split("/", 1)[0]
        categories[category] += 1
        total[category] += f["size"]
    return {"product_code": data[0x150:0x160].split(b"\0")[0].decode("ascii", "replace"),
            "directory_count": len(directories), "file_count": len(files),
            "category_counts": dict(categories), "category_bytes": dict(total),
            "directories": directories, "files": files}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ncch", type=Path, help="user-supplied decrypted NCCH")
    parser.add_argument("--output", type=Path, help="optional private JSON output")
    parser.add_argument("--summary", action="store_true", help="only print totals")
    args = parser.parse_args()
    result = inventory(args.ncch)
    summary_keys = ("product_code", "directory_count", "file_count",
                    "category_counts", "category_bytes")
    shown = {key: result[key] for key in summary_keys} if args.summary else result
    payload = json.dumps(shown, indent=2, ensure_ascii=False) + "\n"
    if args.output:
        args.output.write_text(payload, encoding="utf-8")
        print("Wrote local inventory:", args.output)
    else:
        print(payload, end="")


if __name__ == "__main__":
    main()
