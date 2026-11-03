"""Source-level regression checks for optional Nitro Engine room lifecycle.

These complement (but do not replace) native ROM compilation and emulator tests.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
RENDERER = (ROOT / "source" / "plaza_3d.c").read_text()
MAIN = (ROOT / "source" / "main.c").read_text()


class PlazaRendererLifecycleTests(unittest.TestCase):
    def test_single_visitor_count_declaration(self):
        declarations = re.findall(r"static\s+unsigned\s+visible_visitors\s*;", RENDERER)
        self.assertEqual(len(declarations), 1)

    def test_render_only_visible_visitor_models(self):
        self.assertIn("i < visible_visitors", RENDERER)
        self.assertIn("spds_plaza_3d_set_visitors(unsigned count, unsigned selected)", RENDERER)
        self.assertIn("count > SPDS_ROOM_VISITORS ? SPDS_ROOM_VISITORS : count", RENDERER)

    def test_enter_has_missing_asset_fallback(self):
        self.assertIn('"%s/models/visitor.dsm"', RENDERER)
        self.assertIn('"%s/models/idle.dsa"', RENDERER)
        self.assertIn("if (!mesh_file)", RENDERER)
        self.assertIn("if (!anim_file)", RENDERER)

    def test_every_room_exit_releases_3d(self):
        room_start = MAIN.index("        if(room_open) {")
        room_end = MAIN.index("        if (down & KEY_B) {", room_start + 30)
        room = MAIN[room_start:room_end]
        self.assertIn("spds_plaza_3d_leave()", room)
        self.assertIn("spds_plaza_roster_show_detail", room)
        self.assertGreaterEqual(room.count("spds_plaza_3d_leave()"), 2)

    def test_no_camera_dependency(self):
        self.assertNotIn('dsi_camera.h', MAIN)
        self.assertNotIn("spds_dsi_camera_run", MAIN)


if __name__ == "__main__":
    unittest.main()
