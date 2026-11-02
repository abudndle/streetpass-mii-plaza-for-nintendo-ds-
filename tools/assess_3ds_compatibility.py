#!/usr/bin/env python3
"""Fail closed when reviewing evidence for native 3DS StreetPass interoperability."""
import argparse
import json
from pathlib import Path

GATES = ("discovery_capture", "radio_frame_control", "cecd_handshake",
         "encrypted_exchange", "native_plaza_visitor")
STATUSES = frozenset(("unknown", "blocked", "demonstrated"))

def assess(record):
    if not isinstance(record, dict) or set(record) - set(GATES):
        raise ValueError("Evidence must be a mapping with recognized gates only")
    result = {}
    for gate in GATES:
        value = record.get(gate, {"status": "unknown"})
        if not isinstance(value, dict) or set(value) - {"status", "evidence"}:
            raise ValueError("Unexpected fields for " + gate)
        if value.get("status") not in STATUSES:
            raise ValueError("Invalid status for " + gate)
        evidence = value.get("evidence")
        if evidence is not None and (not isinstance(evidence, str) or not evidence.strip()):
            raise ValueError("Evidence must be meaningful text for " + gate)
        if value["status"] == "demonstrated" and evidence is None:
            raise ValueError("Demonstration requires supporting evidence: " + gate)
        result[gate] = value["status"]
    return {
        "gates": result,
        "native_plaza_verified": all(
            value == "demonstrated" for value in result.values()),
        "missing_demonstrations": [
            gate for gate in GATES if result[gate] != "demonstrated"],
    }

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("evidence", type=Path)
    parser.add_argument("--require-verified", action="store_true",
                        help="Exit unsuccessfully unless all five gates are demonstrated")
    args = parser.parse_args()
    outcome = assess(json.loads(args.evidence.read_text(encoding="utf-8")))
    print(json.dumps(outcome, indent=2))
    return 0 if outcome["native_plaza_verified"] or not args.require_verified else 1

if __name__ == "__main__":
    raise SystemExit(main())
