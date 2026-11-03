# Dual-emulator StreetPass debugging

Run `python3 tools/emulator_preflight.py --json` to record safe executable/version/display diagnostics. Optionally pass `--rom streetpass-plaza-ds.nds` to validate the homebrew ROM header. This tool does **not** launch emulators, inspect private files or prove radio interoperability.

## melonDS DS-to-DS test
1. Build the same homebrew ROM for both instances; record its SHA-256 and the melonDS build/version.
2. Configure separate writable storage and profiles for two instances; do not share save directories. Enable the emulator's supported local multiplayer/network mode.
3. On instance A select host (L); on B select join/scan (R). Record association and connection state on both screens.
4. Exchange a synthetic, consented visitor in both directions. Confirm ACK, exactly one stored encounter after duplicates, queue limits, and durable save/restart. Check logs for errors; repeat after disconnect/reconnect.
5. If either instance fails to associate, isolate emulator networking from SPDS protocol parsing with a known supported local-multiplayer test. A ROM boot screenshot is not exchange evidence.

## AzaharPlus 3DS-to-3DS test
1. Record AzaharPlus version, build hash, OS and sanitized startup log. Use only its supported system-file setup/downloader, when available and authorized; do not copy or publish firmware, keys, console identity or StreetPass saves.
2. Confirm 3DS software launches and the emulator actually exposes compatible CECD/StreetPass simulation. A working home menu alone is insufficient.
3. Use two isolated emulator profiles and synthetic/redacted identities. Trigger an exchange using only supported emulator features, then inspect before/after encounter receipts in each instance.
4. If no compatible CECD simulation exists, mark **BLOCKED**, not failed or passed. Never equate file-mediated ZipPass import/export with radio StreetPass.

## Cross-emulator
AzaharPlus CECD and the DS project's SPDS/DSWifi are different protocols. Do not claim native AzaharPlus↔melonDS StreetPass without a tested bridge and packet/receipt evidence. Stock 3DS compatibility requires separate hardware evidence.

## Evidence and debugging checklist
For each attempt capture emulator versions, ROM hash, exact commands, sanitized stdout/stderr, both screen states, host/client roles, timestamps, packet counts, ACK/retry, duplicate and queue outcomes, and save/restart persistence. Keep system files, keys, copyrighted assets, private Mii data and original game files out of public artifacts.

Track results in issue #62 with separate statuses: NOT_RUN, BLOCKED, FAIL, PASS. A diagnostic preflight is NOT_RUN for actual exchange.
