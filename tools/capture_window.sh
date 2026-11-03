#!/usr/bin/env bash
# Capture one genuine visible emulator window; no synthetic UI rendering.
set -euo pipefail
pattern="${1:?window title pattern required}"
out="${2:?output PNG required}"
for cmd in xdotool import; do command -v "$cmd" >/dev/null || { echo "Missing $cmd" >&2; exit 2; }; done
[[ -n "${DISPLAY:-}" ]] || { echo "X11 DISPLAY required" >&2; exit 2; }
mapfile -t ids < <(xdotool search --onlyvisible --name "$pattern" || true)
if (( ${#ids[@]} != 1 )); then echo "Expected exactly one visible window matching '$pattern'; found ${#ids[@]}" >&2; exit 1; fi
mkdir -p "$(dirname "$out")"
import -window "${ids[0]}" "$out"
python3 tools/strip_png_metadata.py "$out"
python3 tools/check_png.py "$out"
echo "Captured genuine window '$pattern': $out"
