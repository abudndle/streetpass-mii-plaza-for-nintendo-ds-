from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class RepositoryContract(unittest.TestCase):
    def test_required_files(self):
        for name in ("source/main.c", "Makefile", ".gitignore", "README.md", "docs/MILESTONES.md"):
            self.assertTrue((ROOT / name).is_file(), name)

    def test_input_controls(self):
        source = (ROOT / "source/main.c").read_text()
        for key in ("KEY_A", "KEY_B", "KEY_START", "swiWaitForVBlank", "scanKeys"):
            self.assertIn(key, source)

    def test_no_original_assets_tracked(self):
        forbidden = {".3ds", ".cci", ".cia", ".cxi", ".ncch", ".bcmdl", ".bcres", ".darc"}
        for path in ROOT.rglob("*"):
            if path.is_file() and ".git" not in path.parts:
                self.assertNotIn(path.suffix.lower(), forbidden, str(path))

if __name__ == "__main__":
    unittest.main()
