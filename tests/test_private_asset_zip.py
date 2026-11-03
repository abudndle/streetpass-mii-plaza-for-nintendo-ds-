"""Tests for private base-game ZIP verification (no proprietary fixtures)."""
import importlib.util
from pathlib import Path
import tempfile
import unittest
import zipfile

SPEC = importlib.util.spec_from_file_location("verify_private_asset_zip", Path(__file__).resolve().parents[1] / "tools" / "verify_private_asset_zip.py")
module = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(module)

class TestAssetZip(unittest.TestCase):
    def test_incomplete_archive(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "test.zip"
            with zipfile.ZipFile(p, "w") as z:
                z.writestr("romfs/example.bin", b"test")
            r = module.inspect(p)
            self.assertEqual(r["counts"]["romfs"], 1)
            self.assertFalse(r["base_inventory_complete"])

    def test_complete_fixture(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "test.zip"
            with zipfile.ZipFile(p, "w") as z:
                for folder, count in module.EXPECTED.items():
                    for i in range(count):
                        z.writestr(f"{folder}/test_{i}.bin", b"test")
            self.assertTrue(module.inspect(p)["base_inventory_complete"])

    def test_rejects_traversal(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "test.zip"
            with zipfile.ZipFile(p, "w") as z:
                z.writestr("../bad.bin", b"bad")
            self.assertTrue(module.inspect(p)["unsafe_or_duplicate_paths"])

if __name__ == "__main__":
    unittest.main()
