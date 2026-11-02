import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from assess_3ds_compatibility import GATES, assess

class CompatibilityTests(unittest.TestCase):
    def test_baseline_is_explicitly_unverified(self):
        with (ROOT / "research" / "stock_3ds_evidence.json").open() as file:
            data = json.load(file)
        state = assess(data)
        self.assertFalse(state["native_plaza_verified"])
        self.assertIn("native_plaza_visitor", state["missing_demonstrations"])

    def test_missing_gates_fail_closed(self):
        result = assess({})
        self.assertEqual(len(result["missing_demonstrations"]), len(GATES))
        self.assertFalse(result["native_plaza_verified"])

    def test_all_gates_need_evidence(self):
        data = {gate: {"status": "demonstrated",
                       "evidence": "recorded, reproducible laboratory result"}
                for gate in GATES}
        self.assertTrue(assess(data)["native_plaza_verified"])
        del data["encrypted_exchange"]["evidence"]
        with self.assertRaises(ValueError):
            assess(data)

    def test_invalid_and_unknown_fields_rejected(self):
        for bad in ([], {"wrong": {"status": "unknown"}},
                    {"cecd_handshake": {"status": "complete"}},
                    {"native_plaza_visitor": {"status": "demonstrated",
                                              "evidence": ""}},
                    {"native_plaza_visitor": {"status": "unknown",
                                              "evidence": 123}},
                    {"native_plaza_visitor": {"status": "unknown",
                                              "unexpected": True}}):
            with self.subTest(bad=bad), self.assertRaises(ValueError):
                assess(bad)

    def test_require_verified_exit_code(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "evidence.json"
            path.write_text(json.dumps({}), encoding="utf-8")
            cmd = [sys.executable,
                   str(ROOT / "tools" / "assess_3ds_compatibility.py"),
                   str(path), "--require-verified"]
            self.assertEqual(subprocess.run(cmd, capture_output=True).returncode, 1)
            self.assertEqual(subprocess.run(cmd[:-1], capture_output=True).returncode, 0)

if __name__ == "__main__":
    unittest.main()
