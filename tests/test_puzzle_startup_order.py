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

    def test_cursor_change_is_saved_and_failed_write_rolls_back(self):
        source = (ROOT / "source" / "main.c").read_text()
        begin = source.index("if(puzzle_open && puzzle_ready) {")
        end = source.index("if(room_open) {", begin)
        handler = source[begin:end]
        self.assertIn("unsigned previous_selection=puzzle.selected;", handler)
        self.assertIn("if(puzzle.selected!=previous_selection)", handler)
        self.assertIn("if(spds_puzzle_save(save_dir,&puzzle)!=0)", handler)
        self.assertIn("puzzle.selected=previous_selection;", handler)
        self.assertIn("++puzzle_save_errors;", handler)
        self.assertLess(handler.index("spds_puzzle_save(save_dir,&puzzle)"),
                        handler.index("if(down & KEY_A)"))

if __name__ == "__main__":
    unittest.main()
