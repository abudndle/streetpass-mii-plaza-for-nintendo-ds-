#!/usr/bin/env python3
"""Validate redacted DS/3DS radio observations; never infer native compatibility."""
import argparse
import json
from pathlib import Path

KINDS = frozenset(("discovery", "management_rx", "management_tx",
                   "cecd", "encrypted", "visitor"))
RESULTS = frozenset(("observed", "unsupported", "inconclusive"))


def validate(record):
    if not isinstance(record, dict) or set(record) != {"build_commit", "device_mode", "observations"}:
        raise ValueError("Expected build_commit, device_mode, observations")
    if not isinstance(record["build_commit"], str) or not record["build_commit"].strip():
        raise ValueError("Build commit is required")
    if record["device_mode"] not in ("DS", "DSi_DS_mode", "DSi_mode"):
        raise ValueError("Unknown device mode")
    observations = record["observations"]
    if not isinstance(observations, list):
        raise ValueError("Observations must be a list")
    for item in observations:
        if not isinstance(item, dict) or set(item) != {"kind", "result", "method", "reference"}:
            raise ValueError("Each observation needs kind, result, method, reference")
        if item["kind"] not in KINDS or item["result"] not in RESULTS:
            raise ValueError("Unknown observation type or result")
        for key in ("method", "reference"):
            if not isinstance(item[key], str) or not item[key].strip():
                raise ValueError("Missing observation " + key)
        if any(word in (item["method"] + item["reference"]).lower()
               for word in ("private key", "console id", "mac address")):
            raise ValueError("Remove sensitive identifiers from public evidence")
    return {
        "observation_count": len(observations),
        "frame_receive_observed": any(i["kind"] == "management_rx" and i["result"] == "observed"
                                      for i in observations),
        "frame_transmit_observed": any(i["kind"] == "management_tx" and i["result"] == "observed"
                                       for i in observations),
        "native_3ds_verified": False,
    }


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("record", type=Path)
    args = p.parse_args()
    print(json.dumps(validate(json.loads(args.record.read_text(encoding="utf-8"))), indent=2))


if __name__ == "__main__":
    main()
