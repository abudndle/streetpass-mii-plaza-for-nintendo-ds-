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

    def test_corrupt_save_is_not_ready(self):
        source = (ROOT / "source" / "main.c").read_text()
        self.assertIn("if(spds_puzzle_load(save_dir,&puzzle)>=0) puzzle_ready=1;", source)
        self.assertIn("else puzzle_ready=0;", source)

if __name__ == "__main__":
    unittest.main()
