#!/usr/bin/env python3
"""Regression tests for privacy-safe emulator preflight reporting."""
import json
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "tools" / "emulator_preflight.py"

class EmulatorPreflightTests(unittest.TestCase):
    def test_json_report_does_not_claim_exchange(self):
        proc = subprocess.run([sys.executable, str(SCRIPT), "--json"],
                              capture_output=True, text=True, check=True)
        report = json.loads(proc.stdout)
        self.assertEqual(set(report["emulators"]), {"melonDS", "AzaharPlus"})
        self.assertEqual(set(report["tests"].values()), {"NOT_RUN"})

    def test_invalid_rom_is_rejected(self):
        import tempfile
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "invalid.nds"
            path.write_bytes(b"not a rom")
            proc = subprocess.run([sys.executable, str(SCRIPT), "--json",
                                   "--rom", str(path)],
                                  capture_output=True, text=True)
            self.assertEqual(proc.returncode, 1)
            report = json.loads(proc.stdout)
            self.assertIn("homebrew_rom_error", report)
            self.assertEqual(set(report["tests"].values()), {"NOT_RUN"})

if __name__ == "__main__":
    unittest.main()
