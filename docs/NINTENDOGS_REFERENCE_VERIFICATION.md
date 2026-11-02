# Nintendogs Bark Mode reference: reproducible ROM verification

Private user-supplied *Nintendogs: Lab & Friends* AD3E .nds was compared
with private extracted components. Never publish original ROM, executable
code, keys or game assets. Reproduce locally:

```sh
python3 tools/verify_bark_reference.py "/path/Nintendogs - Lab & Friends.nds" /path/extracted
```

Observed: the 33,554,432-byte ROM has game code AD3E. ARM7, FAT, FNT,
ARM9 overlay table and header match extracted sections exactly.
ARM9's first 731,360 bytes match; the separate extracted ARM9 binary has
12 additional trailing bytes. The ARM9 overlay table has 30 entries.

Nintendogs Bark Mode's closed-lid wireless search runs within its own
still-active game; it is not a system daemon. Plaza DS has experimental
opt-in lid search using `disableSleep()`; actual DS battery and radio
performance remain untested. A folder on SD/flashcart cannot execute
after another unrelated title takes over. Cooperative homebrew launchers
and applications must explicitly integrate the service. Stock 3DS CECD
StreetPass does not accept this DS homebrew wireless protocol.
