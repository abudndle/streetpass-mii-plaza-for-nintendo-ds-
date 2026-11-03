import json
import subprocess
import sys
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from validate_radio_observations import validate


class ObservationTests(unittest.TestCase):
    def setUp(self):
        self.base = {"build_commit": "test-commit", "device_mode": "DS",
                     "observations": []}

    def test_empty_report_fails_closed(self):
        result = validate(self.base)
        self.assertFalse(result["native_3ds_verified"])
        self.assertFalse(result["frame_receive_observed"])

    def test_receive_does_not_prove_native(self):
        self.base["observations"] = [{
            "kind": "management_rx", "result": "observed",
            "method": "redacted passive trace", "reference": "offline capture A"
        }]
        result = validate(self.base)
        self.assertTrue(result["frame_receive_observed"])
        self.assertFalse(result["frame_transmit_observed"])
        self.assertFalse(result["native_3ds_verified"])

    def test_rejects_unrecognized_fields(self):
        self.base["console_id"] = "secret"
        with self.assertRaises(ValueError):
            validate(self.base)

    def test_rejects_empty_reference(self):
        self.base["observations"] = [{
            "kind": "management_tx", "result": "observed",
            "method": "probe", "reference": ""
        }]
        with self.assertRaises(ValueError):
            validate(self.base)

    def test_example_report_cli(self):
        cmd = [sys.executable, str(ROOT / "tools" / "validate_radio_observations.py"),
               str(ROOT / "research" / "radio_observations_example.json")]
        out = subprocess.run(cmd, capture_output=True, text=True)
        self.assertEqual(out.returncode, 0, out.stderr)
        self.assertFalse(json.loads(out.stdout)["native_3ds_verified"])


if __name__ == "__main__":
    unittest.main()
