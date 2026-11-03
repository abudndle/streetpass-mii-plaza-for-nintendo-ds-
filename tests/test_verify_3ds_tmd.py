import hashlib
import struct
import tempfile
import unittest
from pathlib import Path

from verify_3ds_tmd import inspect, USA_MII_PLAZA_TITLE_ID


class TmdVerifierTests(unittest.TestCase):
    def make_fixture(self, root, payload):
        body = 0x140
        tmd = bytearray(body + 0x9C4 + 0x30)
        struct.pack_into(">I", tmd, 0, 0x00010004)
        tmd[body + 0x4C:body + 0x54] = bytes.fromhex(USA_MII_PLAZA_TITLE_ID)
        struct.pack_into(">H", tmd, body + 0x9C, 5136)
        struct.pack_into(">H", tmd, body + 0x9E, 1)
        off = body + 0x9C4
        struct.pack_into(">IHHQ", tmd, off, 0x21, 0, 1, len(payload))
        tmd[off + 16:off + 48] = hashlib.sha256(payload).digest()
        tmd_path = root / "title.tmd"
        app_path = root / "00000021.app"
        tmd_path.write_bytes(tmd)
        app_path.write_bytes(payload)
        return tmd_path, app_path

    def test_matching_usa_content_passes(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmd, app = self.make_fixture(Path(tmp), b"synthetic test content")
            result = inspect(tmd, [app], USA_MII_PLAZA_TITLE_ID)
            self.assertTrue(result["title_id_match"])
            self.assertTrue(result["all_content_hashes_match"])
            self.assertTrue(result["cia_ready_by_content_checks"])

    def test_mismatched_content_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmd, app = self.make_fixture(Path(tmp), b"expected bytes")
            app.write_bytes(b"changed bytes")
            result = inspect(tmd, [app], USA_MII_PLAZA_TITLE_ID)
            self.assertFalse(result["all_content_hashes_match"])
            self.assertFalse(result["cia_ready_by_content_checks"])

    def test_wrong_region_title_id_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmd, app = self.make_fixture(Path(tmp), b"payload")
            result = inspect(tmd, [app], "0004001000022800")
            self.assertFalse(result["title_id_match"])
            self.assertFalse(result["cia_ready_by_content_checks"])


if __name__ == "__main__":
    unittest.main()
