import tempfile
import unittest
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from configure_melonds_dldi import configure


class MelonDSDldiConfigTests(unittest.TestCase):
    def test_config_enables_writable_folder_sync(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            home = root / "home"
            sd = root / "dldi-sd"
            config = configure(home, sd)
            text = config.read_text(encoding="utf-8")
            self.assertIn("DLDIEnable=1\n", text)
            self.assertIn("DLDIReadOnly=0\n", text)
            self.assertIn("DLDIFolderSync=1\n", text)
            self.assertIn(f"DLDIFolderPath={sd.resolve()}\n", text)
            self.assertTrue(sd.is_dir())

    def test_does_not_write_outside_supplied_home(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            config = configure(root / "sandbox-home", root / "sandbox-sd")
            self.assertTrue(config.is_relative_to(root))
            self.assertTrue((root / "sandbox-sd").is_relative_to(root))


if __name__ == "__main__":
    unittest.main()
