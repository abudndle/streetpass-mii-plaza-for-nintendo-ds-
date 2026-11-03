#!/usr/bin/env python3
"""Audit whether the DS radio source contains native 3DS StreetPass support.

Static source inspection is only negative/architectural evidence. It cannot
prove hardware capability or successful native CECD interoperability.
"""
import argparse
import json
from pathlib import Path

REQUIRED = ("discovery_capture", "radio_frame_control", "cecd_handshake",
            "encrypted_exchange", "native_plaza_visitor")


def audit(source):
    if not isinstance(source, str):
        raise ValueError("source must be text")
    return {
        "transport": "dswifi_homebrew_multiplayer" if
            "Wifi_MultiplayerHostMode(" in source and "Wifi_MultiplayerClientMode(" in source
            else "unknown",
        "custom_game_id": "SPDS_WIFI_GAME_ID" in source,
        "native_3ds_verified": False,
        "hardware_gates_required": list(REQUIRED),
        "limitations": [
            "Static source inspection cannot demonstrate raw management-frame transmission.",
            "DS multiplayer beacons do not establish CECD handshake or WPA2-CCMP compatibility.",
            "No stock 3DS encounter has been verified on physical hardware.",
        ],
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("--require-native", action="store_true")
    args = parser.parse_args()
    result = audit(args.source.read_text(encoding="utf-8"))
    print(json.dumps(result, indent=2))
    return 1 if args.require_native else 0


if __name__ == "__main__":
    raise SystemExit(main())
