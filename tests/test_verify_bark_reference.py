import struct
import sys
import tempfile
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from verify_bark_reference import inspect

class BarkReferenceTests(unittest.TestCase):
    def test_matches_and_reports_trailing_bytes(self):
        with tempfile.TemporaryDirectory() as tmp:
            d=Path(tmp); rom=bytearray(0x9000); rom[12:16]=b"AD3E"
            for name, of, sf, dest, data in [
                ("arm9",0x20,0x2c,0x4000,b"12345678"),
                ("arm7",0x30,0x3c,0x5000,b"abcdefgh")]:
                struct.pack_into("<I",rom,of,dest)
                struct.pack_into("<I",rom,sf,len(data))
                rom[dest:dest+len(data)]=data
                (d/(name+".bin")).write_bytes(data+(b"extra" if name=="arm9" else b""))
            (d/"header.bin").write_bytes(rom[:0x4000])
            p=d/"test.nds";p.write_bytes(rom)
            report=inspect(p,d)
            self.assertTrue(report["all_declared_sections_match"])
            self.assertFalse(report["sections"]["arm9"]["exact_matches"])
            self.assertEqual(report["sections"]["arm9"]["trailing_bytes"],5)
            (d/"arm7.bin").write_bytes(b"wrong")
            self.assertFalse(inspect(p,d)["all_declared_sections_match"])
    def test_invalid_title_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/"test.nds";p.write_bytes(bytes(0x4000))
            with self.assertRaises(ValueError):
                inspect(p,Path(tmp))

if __name__ == "__main__":
    unittest.main()
