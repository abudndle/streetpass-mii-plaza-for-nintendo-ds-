import tempfile
import unittest
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from configure_melonds_dldi import configure


class DldiConfigTests(unittest.TestCase):
    def test_isolated_writable_folder_sync(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            home, sd = root / "home", root / "sd"
            config = configure(home, sd)
            data = config.read_text()
            self.assertIn("DLDIEnable=1\n", data)
            self.assertIn("DLDIReadOnly=0\n", data)
            self.assertIn("DLDIFolderSync=1\n", data)
            self.assertIn(f"DLDIFolderPath={sd.resolve()}\n", data)
            self.assertTrue(sd.is_dir())
            self.assertTrue(config.is_relative_to(home.resolve()))


if __name__ == "__main__":
    unittest.main()
