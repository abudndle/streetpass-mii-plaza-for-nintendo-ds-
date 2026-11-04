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
            for line in ("DLDIEnable=1", "DLDIReadOnly=0",
                         "DLDIFolderSync=1", f"DLDIFolderPath={sd.resolve()}"):
                self.assertIn(line + "\n", data)
            self.assertTrue(sd.is_dir())
            self.assertTrue(config.is_relative_to(home.resolve()))

    def test_existing_emulator_settings_preserved_and_repeated_run_idempotent(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            home, sd = root / "home", root / "sd"
            config = home / ".config" / "melonDS" / "melonDS.ini"
            config.parent.mkdir(parents=True)
            config.write_text("# custom profile\nAudioVolume=77\nDLDIEnable=0\n"
                              "DLDIEnable=0\nDLDIFolderPath=/old/path\n")
            configure(home, sd)
            first = config.read_text()
            self.assertIn("# custom profile\nAudioVolume=77\n", first)
            self.assertEqual(first.count("DLDIEnable="), 1)
            self.assertNotIn("/old/path", first)
            configure(home, sd)
            self.assertEqual(config.read_text(), first)

    def test_separate_profiles_do_not_share_config(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            a = configure(root / "a", root / "sd-a")
            b = configure(root / "b", root / "sd-b")
            self.assertNotEqual(a, b)
            self.assertIn(str((root / "sd-a").resolve()), a.read_text())
            self.assertNotIn(str((root / "sd-a").resolve()), b.read_text())

    def test_reject_storage_parent_of_profile(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            with self.assertRaises(ValueError):
                configure(root / "sd" / "home", root / "sd")


if __name__ == "__main__":
    unittest.main()
