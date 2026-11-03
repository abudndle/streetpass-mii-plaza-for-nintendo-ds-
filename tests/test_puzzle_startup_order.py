from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class PuzzleStartupOrderTests(unittest.TestCase):
    def test_load_precedes_first_poll(self):
        source = (ROOT / "source" / "main.c").read_text()
        start = source.index("int main(void)")
        load = source.index("spds_puzzle_load(save_dir,&puzzle)", start)
        poll = source.index("poll_now();", start)
        self.assertLess(load, poll)

    def test_plaza_status_lines_do_not_overlap(self):
        source = (ROOT / "source" / "main.c").read_text()
        # Keep the visitor count visible when the radio status is rendered.
        self.assertIn('printf("\\x1b[10;2HLocal radio: %-14s"', source)
        self.assertIn('printf("\\x1b[17;2HCached Mii visitors: %u"', source)
        self.assertNotIn('printf("\\x1b[10;2HCached Mii visitors: %u"', source)

    def test_corrupt_save_is_not_ready(self):
        source = (ROOT / "source" / "main.c").read_text()
        self.assertIn("if(spds_puzzle_load(save_dir,&puzzle)>=0) puzzle_ready=1;", source)
        self.assertIn("else puzzle_ready=0;", source)

if __name__ == "__main__":
    unittest.main()
