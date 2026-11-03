#!/usr/bin/env python3
"""Black-box tests of the melonDS capture helper without generating fake gameplay screenshots."""
import os
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "tools" / "capture_melonds.sh"

class CaptureTests(unittest.TestCase):
    def invoke(self, windows, display=":99"):
        with tempfile.TemporaryDirectory() as tmp:
            d = pathlib.Path(tmp)
            xdotool = d / "xdotool"
            xdotool.write_text("#!/bin/sh\n[ -z "$FAKE_WINDOWS" ] || printf '%s\\n' \"$FAKE_WINDOWS\"\n")
            xdotool.chmod(0o755)
            imp = d / "import"
            imp.write_text("#!/bin/sh\nexit 99\n")
            imp.chmod(0o755)
            env = dict(os.environ, PATH=str(d) + ":" + os.environ["PATH"],
                       DISPLAY=display, FAKE_WINDOWS=windows)
            result = subprocess.run(["bash", str(SCRIPT), str(d / "out.png")],
                                    env=env, text=True, capture_output=True)
            return result
    def test_missing_window_fails(self):
        result = self.invoke("")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("No visible melonDS window", result.stderr)
    def test_multiple_windows_fail_closed(self):
        result = self.invoke("123\n456")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("extra windows", result.stderr)
    def test_missing_display_fails(self):
        result = self.invoke("123", display="")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("DISPLAY", result.stderr)

if __name__ == "__main__":
    unittest.main()
