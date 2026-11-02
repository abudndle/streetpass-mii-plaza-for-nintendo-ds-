#!/usr/bin/env python3
"""Evaluate the evidence gates for native 3DS Mii Plaza interoperability."""
import argparse
import json
from pathlib import Path

GATES = ("discovery_capture", "radio_frame_control", "cecd_handshake",
         "encrypted_exchange", "native_plaza_visitor")
def assess(record):
    if not isinstance(record, dict) or set(record) - set(GATES):
        raise ValueError("Unexpected evidence record fields")
    result = {}
    for gate in GATES:
        value = record.get(gate, {"status": "unknown"})
        if not isinstance(value, dict) or value.get("status") not in (
                "unknown", "blocked", "demonstrated"):
            raise ValueError("Invalid status for " + gate)
        if value["status"] == "demonstrated" and not (
                isinstance(value.get("evidence"), str) and value["evidence"].strip()):
            raise ValueError("Demonstration requires supporting evidence: " + gate)
        result[gate] = value["status"]
    return {"gates": result, "native_plaza_verified": all(
        value == "demonstrated" for value in result.values())}

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("evidence", type=Path)
    args = parser.parse_args()
    print(json.dumps(assess(json.loads(args.evidence.read_text())), indent=2))
