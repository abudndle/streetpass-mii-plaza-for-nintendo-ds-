#!/usr/bin/env python3
"""Safe, read-only environment preflight for melonDS and AzaharPlus.

Never reads or uploads 3DS system files, keys, saves or personal StreetPass data.
No emulator launch or CECD compatibility claim is made by this diagnostic.
"""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

EMULATORS = {
    "melonDS": ("melonDS", "melonds"),
    "AzaharPlus": ("azaharplus", "AzaharPlus", "azahar", "Azahar"),
}

def probe(candidates):
    for name in candidates:
        executable = shutil.which(name)
        if not executable:
            continue
        try:
            proc = subprocess.run([executable, "--version"], capture_output=True,
                                  text=True, timeout=8, check=False)
            output = (proc.stdout or proc.stderr).strip().splitlines()
            version = output[0][:180] if output else "version unavailable"
            return {"installed": True, "command": name, "version": version,
                    "version_exit_code": proc.returncode}
        except (OSError, subprocess.TimeoutExpired) as exc:
            return {"installed": True, "command": name,
                    "version_error": type(exc).__name__}
    return {"installed": False, "reason": "No emulator executable found in PATH"}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", type=Path,
                        help="Optional homebrew .nds ROM; only inspect its header")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()
    report = {"emulators": {name: probe(candidates)
                           for name, candidates in EMULATORS.items()},
              "display_available": bool(os.environ.get("DISPLAY") or
                                        os.environ.get("WAYLAND_DISPLAY")),
              "tests": {"melonDS_to_melonDS": "NOT_RUN",
                        "AzaharPlus_to_AzaharPlus_CECD": "NOT_RUN",
                        "AzaharPlus_to_melonDS": "NOT_RUN"}}
    if args.rom:
        try:
            from verify_nds import verify
            report["homebrew_rom"] = verify(args.rom)
        except (OSError, ValueError) as exc:
            report["homebrew_rom_error"] = str(exc)
    if args.json:
        print(json.dumps(report, indent=2))
    else:
        for name, result in report["emulators"].items():
            print(f"{name}: {result}")
        print("Display available:", report["display_available"])
        if args.rom:
            print("ROM header:", report.get("homebrew_rom",
                                          report.get("homebrew_rom_error")))
        print("Interoperability tests: NOT RUN (preflight only)")
    return 1 if args.rom and "homebrew_rom_error" in report else 0

if __name__ == "__main__":
    sys.exit(main())
