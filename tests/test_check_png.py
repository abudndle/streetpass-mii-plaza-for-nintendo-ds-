import importlib.util
import pathlib
import struct
import tempfile
import unittest
import zlib

SCRIPT = pathlib.Path(__file__).resolve().parents[1] / 'tools' / 'check_png.py'
spec = importlib.util.spec_from_file_location('check_png', SCRIPT)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)

def chunk(kind, payload):
    return struct.pack('>I', len(payload)) + kind + payload + struct.pack('>I', zlib.crc32(kind + payload) & 0xffffffff)

def png(metadata=False):
    ihdr = struct.pack('>IIBBBBB', 512, 384, 8, 2, 0, 0, 0)
    raw = (b'\x00' + b'\x00' * (512 * 3)) * 384
    data = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', ihdr)
    if metadata:
        data += chunk(b'tEXt', b'User\x00private')
    return data + chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b'')

class TestPng(unittest.TestCase):
    def check(self, content):
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'image.png'
            path.write_bytes(content)
            return module.check_png(path)
    def test_accepts_plain_png(self):
        self.assertEqual(self.check(png()), (512, 384))
    def test_rejects_metadata(self):
        with self.assertRaisesRegex(ValueError, 'unapproved'):
            self.check(png(True))
    def test_rejects_corruption(self):
        with self.assertRaises(ValueError):
            self.check(png()[:-3])
    def test_rejects_trailing_bytes(self):
        with self.assertRaises(ValueError):
            self.check(png() + b'extra')

if __name__ == '__main__':
    unittest.main()
