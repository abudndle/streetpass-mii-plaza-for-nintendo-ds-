import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from private_asset_pipeline import decompress_lz


class LzTests(unittest.TestCase):
    def test_lz10_literals(self):
        self.assertEqual(decompress_lz(bytes.fromhex("1003000000414243")), b"ABC")

    def test_lz11_literals(self):
        self.assertEqual(decompress_lz(bytes.fromhex("1103000000414243")), b"ABC")

    def test_lz10_backreference(self):
        self.assertEqual(decompress_lz(bytes.fromhex("1006000040412000")), b"AAAAAA")

    def test_lz11_backreference(self):
        self.assertEqual(decompress_lz(bytes.fromhex("1106000040414000")), b"AAAAAA")

    def test_truncated_and_bad_offset(self):
        with self.assertRaises(ValueError):
            decompress_lz(bytes.fromhex("1106000040"))
        with self.assertRaises(ValueError):
            decompress_lz(bytes.fromhex("11060000804000"))


if __name__ == "__main__":
    unittest.main()
