# Native 3DS StreetPass / DS interoperability worksheet

This worksheet separates independent tests; it does not imply DS homebrew participates in native StreetPass.

## A: stock 3DS ↔ stock 3DS
Use at least two unmodified retail 3DS systems, compatible 3DS software (Mii Plaza for Plaza testing), StreetPass enabled, wireless on and no DS software active. Record model, firmware, Plaza version, time and redacted results.

## B: DS/DSi homebrew mesh
Use at least two (preferably three) DS/DSi consoles running this Plaza. Test discovery, packet send/receive, acknowledgements, retries, deduplication, hop limits, expiry, durable queue and restart recovery. Record console model, flashcart/SD setup, DSWifi build, app commit and redacted logs. Host tests are not radio proof.

## C: cross-platform feasibility
SPDR is not native CECD/StreetPass. Nintendo says StreetPass is compatible only with 3DS software. SpotPass and online-dependent 3DS services ended April 8, 2024; do not depend on the former Nintendo Zone relay service.

Proceed only if a documented, currently available and lawful stock-firmware interface is identified. SSID scanning, AP association or generic Wi-Fi frames are not proof. Do not impersonate consoles, inject CECD records, reuse secrets or bypass authentication.

A cross-platform pass requires a real unmodified 3DS receiving a consented visitor in native Mii Plaza and successful reverse-direction delivery into DS Plaza, with provenance and consent retained. Until then mark the gate blocked/unproven.

Never publish MAC addresses, console IDs, keys, private Mii data, personal encounter records, firmware or proprietary assets.


## D: virtual emulator test tier

Use Azahar for 3DS CECD/StreetPass/NetPass experiments and standalone melonDS for DS local multiplayer. NetPass documents Azahar as the only known emulator with efficient support but does not guarantee compatibility; use user-provided system files and a user-installed NetPass copy. Run two instances of each emulator independently first. Record exact versions, enabled LLE modules, app versions, logs and real screenshots. Do not assume the two emulators share a wireless transport. Any DS↔3DS adapter must be tested explicitly and labelled emulator-only. Emulator evidence never closes the physical retail hardware gate.
