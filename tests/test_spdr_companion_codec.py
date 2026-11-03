import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from spdr_companion_codec import encode, decode, forward


class CompanionCodecTests(unittest.TestCase):
    def test_four_forward_chain_keeps_origin(self):
        packet = encode(123, 1700000000, 456, "Visitor", 4, 1700000000)
        for hops in range(4, -1, -1):
            result = decode(packet)
            self.assertEqual((result["sender_id"], result["nonce"], result["name"]),
                             (123, 456, "Visitor"))
            self.assertEqual(result["hops"], hops)
            if hops:
                packet = forward(packet)
        with self.assertRaises(ValueError):
            forward(packet)

    def test_rejects_corruption_and_bad_input(self):
        packet = encode(1, 1, 2, "Test", 4, 1)
        for bad in (packet[:-1], packet + b"x", packet[:10] + bytes([packet[10] ^ 1]) + packet[11:]):
            with self.assertRaises(ValueError):
                decode(bad)
        for sender, name, hops in ((0, "Test", 4), (1, "", 4), (1, "A" * 32, 4), (1, "bad\n", 4), (1, "Test", 5)):
            with self.assertRaises(ValueError):
                encode(sender, 1, 2, name, hops, 1)


if __name__ == "__main__":
    unittest.main()
