import struct
import sys
from pathlib import Path
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"tools"))
from index_darc import index_darc


def sample_darc():
    name=b"\x00\x00"+"Panel.bclan".encode("utf-16le")+b"\x00\x00"
    table_size=24+len(name)
    data_at=(28+table_size+3)&~3
    size=data_at+4
    header=struct.pack("<4sHHIIIII",b"darc",0xfeff,28,0x01000000,
                       size,28,table_size,data_at)
    nodes=struct.pack("<III",0x01000000,0,2)
    nodes+=struct.pack("<III",2,data_at,4)
    return header+nodes+name+b"\0"*(data_at-(28+table_size))+b"DATA"


class DarcTests(unittest.TestCase):
    def test_layout_inventory(self):
        data=sample_darc()
        result=index_darc(data)
        self.assertEqual(result["members"],1)
        self.assertEqual(result["paths"][0]["path"],"Panel.bclan")
        self.assertEqual(result["extension_counts"][".bclan"],1)
    def test_bounds(self):
        data=bytearray(sample_darc())
        struct.pack_into("<I",data,28+12+4,1000000)
        with self.assertRaises(ValueError):
            index_darc(bytes(data))
    def test_corrupt_header(self):
        data=bytearray(sample_darc())
        data[0]=0
        with self.assertRaises(ValueError):
            index_darc(bytes(data))


if __name__=="__main__":
    unittest.main()
