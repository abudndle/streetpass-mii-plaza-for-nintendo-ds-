"""Regression tests for private icon conversion using generated, non-Nintendo data."""
import importlib.util
from pathlib import Path
import re
import struct
import tempfile
import unittest
import zipfile

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "private_icon_to_ds", ROOT / "tools" / "private_icon_to_ds.py"
)
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class PrivateIconConversionTests(unittest.TestCase):
    def test_exact_rgb555_pixels_and_alpha_bit(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            archive = root / "synthetic.zip"
            pixels = [0x0000, 0x001F, 0x03E0, 0x7C00] * 400
            with zipfile.ZipFile(archive, "w") as output:
                output.writestr(MODULE.ENTRY, struct.pack("<1600H", *pixels))
            header = root / "source" / "private_plaza_icon.h"
            MODULE.convert(archive, header)
            data = header.read_text(encoding="ascii")
            self.assertIn("#include <stdint.h>", data)
            self.assertIn("spds_private_plaza_icon[1600]", data)
            values = re.findall(r"0x([0-9a-f]{4})u", data)
            self.assertEqual(len(values), 1600)
            self.assertEqual([int(x, 16) for x in values[:4]],
                             [0x8000, 0x801F, 0x83E0, 0xFC00])
            self.assertEqual(int(values[-1], 16), 0xFC00)

    def test_rejects_incorrect_icon_size_without_creating_header(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            archive = root / "short.zip"
            with zipfile.ZipFile(archive, "w") as output:
                output.writestr(MODULE.ENTRY, b"\\x00" * 3198)
            header = root / "private_plaza_icon.h"
            with self.assertRaises(ValueError):
                MODULE.convert(archive, header)
            self.assertFalse(header.exists())

    def test_missing_icon_fails_without_creating_header(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            archive = root / "empty.zip"
            with zipfile.ZipFile(archive, "w") as output:
                output.writestr("unrelated.txt", "synthetic")
            header = root / "private_plaza_icon.h"
            with self.assertRaises(KeyError):
                MODULE.convert(archive, header)
            self.assertFalse(header.exists())


if __name__ == "__main__":
    unittest.main()
