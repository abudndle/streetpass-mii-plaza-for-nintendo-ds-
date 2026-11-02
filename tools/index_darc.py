#!/usr/bin/env python3
"""Index Nintendo 3DS DARC layout archives in a private decrypted workspace.

Read-only; emits filenames, hierarchy, sizes and extensions, never Nintendo
layout or texture bytes. Intended for user-provided legally acquired files.
"""
import argparse
from collections import Counter
import json
from pathlib import Path
import struct

MAX_NODES = 50000

def index_darc(data: bytes):
    if len(data) < 28 or data[:4] != b"darc":
        raise ValueError("Not a DARC archive")
    magic, bom, header_bytes, version, file_bytes, table_at, table_len, data_at = (
        struct.unpack_from("<4sHHIIIII", data))
    if bom != 0xFEFF or header_bytes != 28 or version != 0x01000000:
        raise ValueError("Unsupported DARC header")
    if file_bytes != len(data) or table_at < header_bytes or (
        table_at + table_len > file_bytes or data_at > file_bytes):
        raise ValueError("DARC table extends beyond archive")
    if table_at+12 > file_bytes:
        raise ValueError("DARC root missing")
    root_name, root_parent, nodes = struct.unpack_from("<III", data, table_at)
    if root_name != 0x01000000 or root_parent != 0 or not (1 <= nodes <= MAX_NODES):
        raise ValueError("Invalid DARC root node")
    nodes_end = table_at + nodes*12
    names_end = table_at + table_len
    if nodes_end > names_end or names_end > data_at:
        raise ValueError("Invalid DARC nodes/name range")

    def name_at(value):
        off = nodes_end + (value & 0x00FFFFFF)
        if off < nodes_end or off >= names_end or (off - nodes_end) & 1:
            raise ValueError("Out-of-range or misaligned DARC filename offset")
        units = bytearray()
        while off+2 <= names_end:
            b = data[off:off+2]
            off += 2
            if b == b"\x00\x00":
                name = units.decode("utf-16le")
                if "/" in name or "\\" in name or name in ("..", ""):
                    if name != "":
                        raise ValueError("Unsafe DARC component")
                return name
            units.extend(b)
            if len(units) > 512:
                raise ValueError("Unreasonably long DARC filename")
        raise ValueError("Unterminated DARC filename")

    files = []
    directories = []
    stack = [(nodes, "")]
    for i in range(1, nodes):
        while len(stack)>1 and i>=stack[-1][0]:
            stack.pop()
        flags, a, b = struct.unpack_from("<III", data, table_at+i*12)
        directory = bool(flags & 0x01000000)
        component = name_at(flags)
        prefix = stack[-1][1]
        if directory:
            if component not in (".", "") and (b<=i or b>stack[-1][0]):
                raise ValueError("Invalid nested directory bounds")
            name = prefix if component in (".", "") else (
                prefix + "/" + component if prefix else component)
            if component not in (".", ""): directories.append(name)
            if b<=i or b>nodes:
                raise ValueError("Directory end index out of bounds")
            stack.append((b,name))
        else:
            if not component or a<data_at or a+b>file_bytes:
                raise ValueError("Invalid member offset, size or name")
            full = prefix + "/" + component if prefix else component
            files.append({"path":full,"offset":a,"size":b})
    groups = Counter(Path(e["path"]).suffix.lower() for e in files)
    return {"members":len(files),"directories":len(directories),
            "extension_counts":dict(sorted(groups.items())),
            "paths":files,"directory_paths":directories}

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("archive", type=Path)
    p.add_argument("--summary",action="store_true")
    p.add_argument("--output",type=Path,help="private JSON destination")
    args=p.parse_args()
    result=index_darc(args.archive.read_bytes())
    if args.summary:
        result={k:v for k,v in result.items() if k not in ("paths","directory_paths")}
    text=json.dumps(result,indent=2,ensure_ascii=False)+"\n"
    if args.output:
        args.output.write_text(text,encoding="utf-8")
        print("Private metadata inventory:",args.output)
    else:
        print(text,end="")

if __name__=="__main__":
    main()
