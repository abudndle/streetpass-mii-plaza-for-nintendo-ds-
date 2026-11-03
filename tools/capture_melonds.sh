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
if (( ${#ids[@]} != 1 )); then
  echo "Found ${#ids[@]} melonDS windows; close extra windows before capture." >&2
  exit 1
fi
mkdir -p "$(dirname "$out")"
# Capture original emulator pixels, then remove nonessential ancillary metadata chunks.
import -window "${ids[0]}" "$out"
python3 "$(dirname "$0")/strip_png_metadata.py" "$out"
# Fail closed if the capture did not produce a nonempty file.
test -s "$out"
# Reject malformed PNGs and embedded metadata before public sharing.
python3 "$(dirname "$0")/check_png.py" "$out"
echo "Captured and structurally validated emulator window: $out"
echo "Upload this PNG to a GitHub issue or attach it in ChatGPT."
