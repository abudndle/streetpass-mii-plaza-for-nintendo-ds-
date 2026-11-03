#!/usr/bin/env bash
# Capture a genuine running melonDS window on Linux/X11.
set -euo pipefail
out="${1:-screenshots/melonds-plaza.png}"
for cmd in xdotool import; do
  if ! command -v "$cmd" >/dev/null 2>&1; then
    echo "Missing $cmd; install xdotool and imagemagick." >&2
    exit 2
  fi
done
if [[ -z "${DISPLAY:-}" ]]; then
  echo "An X11 DISPLAY is required; run melonDS under X11 or XWayland." >&2
  exit 2
fi
mapfile -t ids < <(xdotool search --onlyvisible --name 'melonDS' || true)
if (( ${#ids[@]} == 0 )); then
  echo "No visible melonDS window found. Launch the ROM first." >&2
  exit 1
fi
mkdir -p "$(dirname "$out")"
import -window "${ids[0]}" "$out"
test -s "$out"
# Reject malformed PNGs and embedded metadata before public sharing.
python3 "$(dirname "$0")/check_png.py" "$out"
echo "Captured and structurally validated emulator window: $out"
echo "Upload this PNG to a GitHub issue or attach it in ChatGPT."
