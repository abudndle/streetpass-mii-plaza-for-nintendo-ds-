import subprocess
import sys
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from audit_ds_radio import audit


class RadioAuditTests(unittest.TestCase):
    def test_current_transport_is_not_native_streetpass(self):
        source = (ROOT / "source" / "wireless.c").read_text(encoding="utf-8")
        result = audit(source)
        self.assertEqual(result["transport"], "dswifi_homebrew_multiplayer")
        self.assertTrue(result["custom_game_id"])
        self.assertFalse(result["native_3ds_verified"])
        self.assertEqual(len(result["hardware_gates_required"]), 5)

    def test_missing_transport_is_not_credited(self):
        result = audit("unrelated source")
        self.assertEqual(result["transport"], "unknown")
        self.assertFalse(result["native_3ds_verified"])

    def test_native_gate_fails_closed(self):
        cmd = [sys.executable, str(ROOT / "tools" / "audit_ds_radio.py"),
               str(ROOT / "source" / "wireless.c")]
        self.assertEqual(subprocess.run(cmd, capture_output=True).returncode, 0)
        self.assertEqual(subprocess.run(cmd + ["--require-native"],
                                        capture_output=True).returncode, 1)


if __name__ == "__main__":
    unittest.main()
