# Capture and share genuine melonDS ROM screenshots

1. Download the latest `streetpass-plaza-ds-homebrew` artifact from
   [GitHub Actions](https://github.com/abudndle/streetpass-mii-plaza-for-nintendo-ds-/actions).
   Extract `streetpass-plaza-ds.nds`.
2. Install the official [melonDS](https://melonds.kuribo64.net/downloadlist.php)
   Linux build. Configure your own legitimately obtained BIOS/firmware
   if the emulator requests them.
3. On Linux X11 (or XWayland), install `xdotool` and ImageMagick
   (`sudo apt install xdotool imagemagick` on Debian/Ubuntu).
4. Open the ROM in melonDS and wait until its real top and bottom screens
   are visible.
5. From the repository root run:
   ```sh
   bash tools/capture_melonds.sh screenshots/plaza-boot.png
   ```
6. Upload the resulting PNG by dragging it into a GitHub issue comment,
   or attach the PNG directly in ChatGPT. Include the commit hash, emulator
   version, and whether the image shows a successful boot or an error.

This script only captures an **existing visible emulator window**.
It never synthesizes screenshots or claims that a ROM booted. If the
window is absent, the command fails. Do not upload Nintendo BIOS, firmware,
private game dumps, or proprietary game assets.

Screenshot images are intentionally not committed to the source repository.
