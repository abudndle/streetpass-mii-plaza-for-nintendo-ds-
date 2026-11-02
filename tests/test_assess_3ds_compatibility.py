import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from assess_3ds_compatibility import GATES, assess

class CompatibilityTests(unittest.TestCase):
    def test_missing_gates_unverified(self):
        self.assertFalse(assess({})["native_plaza_verified"])
    def test_partial_proof_unverified(self):
        self.assertFalse(assess({"discovery_capture": {
            "status": "demonstrated", "evidence": "consenting lab capture"}
        })["native_plaza_verified"])
    def test_complete_evidence(self):
        record = {gate: {"status": "demonstrated",
                         "evidence": "verified lab reference"}
                  for gate in GATES}
        self.assertTrue(assess(record)["native_plaza_verified"])
    def test_evidence_required(self):
        with self.assertRaises(ValueError):
            assess({"native_plaza_visitor": {"status": "demonstrated"}})
    def test_unknown_fields_rejected(self):
        with self.assertRaises(ValueError):
            assess({"unknown_gate": {"status": "unknown"}})

if __name__ == "__main__":
    unittest.main()
