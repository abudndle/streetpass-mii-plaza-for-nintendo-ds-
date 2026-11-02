"""Synthetic CECD mailbox tests. No real console data or message payloads."""
import struct
import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from cecd_plaza_headers import inspect, parse_box_info, parse_message_header

BLOCK = 4096
DATA = BLOCK * 4
FAT = BLOCK * 3


def fixture():
    image = bytearray(12 * BLOCK)
    image[:4] = b'SAVE'
    struct.pack_into('<I', image, 4, 0x40000)
    struct.pack_into('<Q', image, 8, 0x20)
    struct.pack_into('<I', image, 0x24, BLOCK)
    struct.pack_into('<Q', image, 0x48, FAT)
    struct.pack_into('<I', image, 0x50, 8)
    struct.pack_into('<Q', image, 0x58, DATA)
    struct.pack_into('<I', image, 0x60, 8)
    struct.pack_into('<III', image, 0x68, 0, 1, 8)
    struct.pack_into('<III', image, 0x78, 1, 1, 8)
    struct.pack_into('<II', image, DATA, 6, 9)
    struct.pack_into('<II', image, DATA + BLOCK, 5, 9)
    for idx, parent, name, sibling, child, first in [
            (1, 0, '', 0, 2, 0),
            (2, 1, 'CEC', 0, 3, 0),
            (3, 2, '00020800', 0, 4, 0),
            (4, 3, 'InBox___', 5, 0, 1),
            (5, 3, 'OutBox__', 0, 0, 2)]:
        at = DATA + idx * 40
        struct.pack_into('<I', image, at, parent)
        image[at + 4:at + 20] = name.encode().ljust(16, b'\0')
        struct.pack_into('<III', image, at + 20, sibling, child, first)
    for idx, parent, name, first, size, next_file in [
            (1, 4, 'BoxInfo_____', 2, 32, 0),
            (2, 5, 'BoxInfo_____', 3, 144, 3),
            (3, 5, '_Aaaaaaaaaaa', 4, 256, 4),
            (4, 5, 'OBIndex_____', 5, 16, 0)]:
        at = DATA + BLOCK + idx * 48
        struct.pack_into('<I', image, at, parent)
        image[at + 4:at + 20] = name.encode().ljust(16, b'\0')
        struct.pack_into('<I', image, at + 20, next_file)
        struct.pack_into('<I', image, at + 28, first)
        struct.pack_into('<Q', image, at + 32, size)
        struct.pack_into('<I', image, FAT + (first + 1) * 8, 0x80000000)
    struct.pack_into('<H', image, DATA + 2 * BLOCK, 0x6262)
    struct.pack_into('<7I', image, DATA + 2 * BLOCK + 4,
                     32, 32768, 0, 10, 0, 32768, 32768)
    struct.pack_into('<H', image, DATA + 3 * BLOCK, 0x6262)
    struct.pack_into('<7I', image, DATA + 3 * BLOCK + 4,
                     144, 32768, 256, 1, 1, 32768, 32768)
    struct.pack_into('<H', image, DATA + 4 * BLOCK, 0x6060)
    struct.pack_into('<III', image, DATA + 4 * BLOCK + 4, 256, 112, 112)
    image[DATA + 4 * BLOCK + 0x38:DATA + 4 * BLOCK + 0x40] = b'PRIVATE!'
    return image


def reader(data):
    return lambda at, size: bytes(data[at:at + size])


class PlazaHeaderTests(unittest.TestCase):
    def test_sanitized_counts(self):
        state = inspect(reader(fixture()))
        self.assertEqual(state['inbox']['message_count'], 0)
        self.assertEqual(state['outbox']['message_count'], 1)
        self.assertNotIn('PRIVATE', str(state))
        self.assertFalse(state['message_bodies_read'])

    def test_invalid_box_magic_and_count(self):
        data = fixture()
        data[DATA + 3 * BLOCK] = 0
        with self.assertRaises(ValueError):
            inspect(reader(data))
        data = fixture()
        struct.pack_into('<I', data, DATA + 3 * BLOCK + 0x14, 2)
        with self.assertRaises(ValueError):
            inspect(reader(data))

    def test_invalid_message_magic_and_length(self):
        data = fixture()
        data[DATA + 4 * BLOCK] = 0
        with self.assertRaises(ValueError):
            inspect(reader(data))
        data = fixture()
        struct.pack_into('<I', data, DATA + 4 * BLOCK + 8, 200)
        with self.assertRaises(ValueError):
            inspect(reader(data))

    def test_unallocated_file(self):
        data = fixture()
        struct.pack_into('<I', data, FAT + (4 + 1) * 8, 0)
        with self.assertRaises(ValueError):
            inspect(reader(data))

    def test_unverified_message_block(self):
        data = fixture()
        def denied(at, size):
            if at >= DATA + 4 * BLOCK:
                raise ValueError('IVFC check failed')
            return bytes(data[at:at + size])
        with self.assertRaises(ValueError):
            inspect(denied)

    def test_inconsistent_message_count(self):
        data = fixture()
        struct.pack_into('<I', data, DATA + BLOCK + 2 * 48 + 0x14, 4)
        with self.assertRaises(ValueError):
            inspect(reader(data))

    def test_bounded_header_helpers(self):
        data = fixture()
        box = data[DATA + 3 * BLOCK:DATA + 3 * BLOCK + 32]
        message = data[DATA + 4 * BLOCK:DATA + 4 * BLOCK + 112]
        self.assertEqual(parse_box_info(box, 144)['message_count'], 1)
        self.assertEqual(parse_message_header(message, 256)['trailer_bytes'], 32)
        with self.assertRaises(ValueError):
            parse_message_header(message, 257)
        with self.assertRaises(ValueError):
            parse_box_info(box, 145)
