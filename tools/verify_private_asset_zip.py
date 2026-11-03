#!/usr/bin/env python3
"""Validate a privately stored base-game extraction ZIP without exporting assets."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import PurePosixPath
import zipfile

EXPECTED = {"romfs": 115, "decompressed": 56, "exefs": 3}

def inspect(path):
    counts = Counter()
    signatures = Counter()
    paths = set()
    bad = []
    with zipfile.ZipFile(path) as z:
        for entry in z.infolist():
            if entry.is_dir():
                continue
            name = entry.filename
            parts = PurePosixPath(name).parts
            if (not parts or name.startswith("/") or "\\" in name or
                    any(part in (".", "..") for part in parts) or name in paths):
                bad.append(name)
                continue
            paths.add(name)
            group = next((p for p in parts if p in EXPECTED), None)
            if group:
                counts[group] += 1
            else:
                counts["metadata_or_other"] += 1
            # ZipFile.open streams bytes to the digest without extracting files.
            digest = hashlib.sha256()
            with z.open(entry) as stream:
                while True:
                    chunk = stream.read(1024 * 1024)
                    if not chunk:
                        break
                    digest.update(chunk)
            signatures[group or "metadata_or_other"] += 1
    return {"counts": dict(counts), "unsafe_or_duplicate_paths": bad,
            "base_inventory_complete": all(counts[k] == v for k, v in EXPECTED.items()) and not bad,
            "expected": EXPECTED}

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("archive", help="private extraction ZIP")
    p.add_argument("--output", help="write metadata-only JSON report")
    args = p.parse_args()
    result = inspect(args.archive)
    payload = json.dumps(result, indent=2) + "\n"
    if args.output:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write(payload)
    else:
        print(payload, end="")
    if not result["base_inventory_complete"]:
        raise SystemExit(1)

if __name__ == "__main__":
    main()
