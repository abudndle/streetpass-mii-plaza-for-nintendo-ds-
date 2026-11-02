"""Synthetic 3DS SAVE metadata tests; no console-specific data."""
import struct
import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from cecd_mailbox_metadata import inspect


def synthetic():
    image = bytearray(4 * 4096)
    image[:4] = b'SAVE'
    struct.pack_into('<I', image, 4, 0x40000)
    struct.pack_into('<Q', image, 8, 0x20)
    struct.pack_into('<I', image, 0x24, 4096)
    struct.pack_into('<Q', image, 0x58, 4096)
    struct.pack_into('<I', image, 0x60, 3)
    struct.pack_into('<III', image, 0x68, 0, 1, 8)
    struct.pack_into('<III', image, 0x78, 1, 1, 8)
    struct.pack_into('<II', image, 4096, 5, 10)
    struct.pack_into('<II', image, 8192, 3, 9)

    def directory(idx, name, parent, sibling=0, child=0, first_file=0):
        at = 4096 + idx * 40
        struct.pack_into('<I', image, at, parent)
        image[at+4:at+20] = name.encode('ascii').ljust(16, b'\0')
        struct.pack_into('<III', image, at+20, sibling, child, first_file)

    def file(idx, name, parent, sibling=0):
        at = 8192 + idx * 48
        struct.pack_into('<I', image, at, parent)
        image[at+4:at+20] = name.encode('ascii').ljust(16, b'\0')
        struct.pack_into('<I', image, at+20, sibling)

    directory(1, '', 0, child=2, first_file=1)
    directory(2, 'CEC', 1, child=3)
    directory(3, '00020800', 2, child=4)
    directory(4, 'InBox___', 3, first_file=2)
    file(1, 'MBoxList____', 1)
    file(2, 'SecretName', 4)
    return image


def read(data):
    return lambda offset, length: data[offset:offset+length]


class MetadataTests(unittest.TestCase):
    def test_valid_tree_does_not_expose_private_names(self):
        result = inspect(read(synthetic()))
        self.assertEqual(result['reachable_directories_including_root'], 4)
        self.assertEqual(result['reachable_files'], 2)
        self.assertTrue(result['plaza_mailbox_identifier_present'])
        self.assertTrue(result['plaza_inbox_directory_present'])
        self.assertFalse(result['plaza_outbox_directory_present'])
        self.assertEqual(result['plaza_inbox_file_entries'], 1)
        self.assertEqual(result['standard_directory_counts']['InBox___'], 1)
        self.assertNotIn('SecretName', str(result))

    def test_unrelated_title_not_mistaken_for_plaza(self):
        data = synthetic()
        data[4096+2*40+4:4096+2*40+20] = b'NOTCEC'.ljust(16, b'\0')
        result = inspect(read(data))
        self.assertFalse(result['plaza_mailbox_identifier_present'])
        self.assertFalse(result['plaza_inbox_directory_present'])
        self.assertEqual(result['plaza_inbox_file_entries'], 0)

    def test_authentication_failure(self):
        data = synthetic()
        def denied(at, length):
            if at >= 4096:
                raise ValueError('unauthenticated block')
            return data[at:at+length]
        with self.assertRaises(ValueError):
            inspect(denied)

    def test_directory_cycle_is_rejected(self):
        data = synthetic()
        struct.pack_into('<I', data, 4096+4*40+20, 4)
        with self.assertRaises(ValueError):
            inspect(read(data))

    def test_file_cycle_is_rejected(self):
        data = synthetic()
        struct.pack_into('<I', data, 8192+2*48+20, 2)
        with self.assertRaises(ValueError):
            inspect(read(data))

    def test_parent_mismatch_is_rejected(self):
        data = synthetic()
        struct.pack_into('<I', data, 4096+4*40, 99)
        with self.assertRaises(ValueError):
            inspect(read(data))

    def test_oversized_metadata_rejected(self):
        data = synthetic()
        struct.pack_into('<I', data, 0x6c, 500)
        with self.assertRaises(ValueError):
            inspect(read(data))

    def test_truncated_authenticated_read_rejected(self):
        data = synthetic()
        with self.assertRaises(ValueError):
            inspect(lambda at, size: data[at:at+size-1])


if __name__ == '__main__':
    unittest.main()
