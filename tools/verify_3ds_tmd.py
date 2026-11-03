#!/usr/bin/env python3
"""Read-only verifier for a 3DS TMD and its matching .app content files.

This checks the signed TMD's declared title ID, version, sizes and SHA-256
content digests. It does not decrypt contents, verify the TMD signature,
create a CIA, or replace the need for a matching ticket and console keys.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

BODY_OFFSETS = {
    0x00010000: 0x240, 0x00010001: 0x140, 0x00010002: 0x80,
    0x00010003: 0x240, 0x00010004: 0x140, 0x00010005: 0x80,
    0x00010006: 0x240, 0x00010007: 0x140, 0x00010008: 0x80,
}
TMD_CONTENTS_OFFSET = 0x9C4
CONTENT_RECORD_SIZE = 0x30
USA_MII_PLAZA_TITLE_ID = "0004001000021800"


def inspect(tmd_path, content_paths, expected_title_id=None):
    raw = Path(tmd_path).read_bytes()
    if len(raw) < 4:
        raise ValueError("TMD is truncated")
    sig_type = struct.unpack_from(">I", raw, 0)[0]
    if sig_type not in BODY_OFFSETS:
        raise ValueError(f"Unsupported TMD signature type: 0x{sig_type:08x}")
    body = BODY_OFFSETS[sig_type]
    if len(raw) < body + TMD_CONTENTS_OFFSET:
        raise ValueError("TMD header is truncated")
    title_id = raw[body + 0x4C:body + 0x54].hex()
    version = struct.unpack_from(">H", raw, body + 0x9C)[0]
    count = struct.unpack_from(">H", raw, body + 0x9E)[0]
    table_end = body + TMD_CONTENTS_OFFSET + count * CONTENT_RECORD_SIZE
    if len(raw) < table_end:
        raise ValueError("TMD content table is truncated")
    supplied = {}
    for path in content_paths:
        p = Path(path)
        # Conventional CTR content filename: eight-digit hex content ID.
        try:
            cid = int(p.stem, 16)
        except ValueError:
            raise ValueError(f"Content filename must be an 8-digit content ID: {p}")
        supplied[cid] = p
    records = []
    all_match = True
    for i in range(count):
        off = body + TMD_CONTENTS_OFFSET + i * CONTENT_RECORD_SIZE
        cid, index, ctype, size = struct.unpack_from(">IHHQ", raw, off)
        expected = raw[off + 16:off + 48].hex()
        p = supplied.get(cid)
        if p is None:
            result = "MISSING"
            actual_size = None
            actual_hash = None
        else:
            data = p.read_bytes()
            actual_size = len(data)
            actual_hash = hashlib.sha256(data).hexdigest()
            result = "MATCH" if actual_size == size and actual_hash == expected else "MISMATCH"
        if result != "MATCH":
            all_match = False
        records.append({
            "content_id": f"{cid:08x}", "index": index, "type": ctype,
            "expected_size": size, "actual_size": actual_size,
            "expected_sha256": expected, "actual_sha256": actual_hash,
            "result": result,
        })
    title_ok = expected_title_id is None or title_id == expected_title_id.lower()
    return {
        "title_id": title_id, "title_version": version,
        "expected_title_id": expected_title_id, "title_id_match": title_ok,
        "content_count": count, "contents": records,
        "all_content_hashes_match": all_match,
        "cia_ready_by_content_checks": title_ok and all_match,
        "limitations": [
            "Does not verify the TMD signature.",
            "Does not decrypt encrypted content.",
            "Does not verify or supply a matching ticket.",
            "A passing content table alone does not establish CIA installability."
        ],
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("tmd")
    parser.add_argument("contents", nargs="*")
    parser.add_argument("--expected-title-id", default=USA_MII_PLAZA_TITLE_ID)
    args = parser.parse_args()
    try:
        result = inspect(args.tmd, args.contents, args.expected_title_id)
    except (OSError, ValueError) as exc:
        parser.error(str(exc))
    print(json.dumps(result, indent=2))
    return 0 if result["title_id_match"] and result["all_content_hashes_match"] else 2


if __name__ == "__main__":
    raise SystemExit(main())
