# Additional StreetPass research: Azahar and NintendoClients

Research date: 2026-11-01. This document links third-party references but does
not vendor emulator source, game assets, private mailbox data or encryption keys.

## Azahar (Citra-derived 3DS emulator)

- Repository: https://github.com/azahar-emu/azahar
- CECD module: https://github.com/azahar-emu/azahar/tree/master/src/core/hle/service/cecd
- CECD implementation: https://github.com/azahar-emu/azahar/blob/master/src/core/hle/service/cecd/cecd.cpp
- NDM state enum: https://github.com/azahar-emu/azahar/blob/master/src/core/hle/service/ndm/ndm_u.h
- AES keyslot enumeration: https://github.com/azahar-emu/azahar/blob/master/src/core/hw/aes/key.h
- System title IDs: https://github.com/azahar-emu/azahar/blob/master/src/core/system_titles.cpp

Search results identify CECD_U, CECD_S, CECD_NDM service interfaces, NDM
Streetpass and StreetpassData states, and the Plaza system title. These are
promising references for understanding offline message APIs and emulator tests,
not evidence of live stock-3DS wireless support. Azahar is GPL-2.0; avoid
copy-pasting its implementation into this independent repository without a
deliberate licensing decision.

## NintendoClients

- Repo: https://github.com/kinnay/NintendoClients
- Documentation: https://nintendoclients.readthedocs.io/en/latest/
- Protocol wiki: https://github.com/kinnay/NintendoClients/wiki

NintendoClients provides Python clients for online 3DS/Wii U/Switch servers
including NEX and the 3DS NASC endpoint. It is useful for distinguishing
online Nintendo network protocols from *local* CECD StreetPass. A code search
for 'streetpass' returned no matches in the repository at review time; do not
assume it includes a native StreetPass radio implementation.

## Additional checked sources

- https://3dbrew.org/wiki/StreetPass -- documents 3DS CECD NAND data,
  encrypted 802.11 exchanges, probe frames and mailbox semantics.
- https://github.com/AbdyyEee/Streetpass-Extractor-cmd -- archived extractor
  focused on CECD/Citra offline data, not a DS wireless driver.
- https://github.com/azahar-emu/azahar/blob/master/src/core/hle/service/cecd/cecd.cpp
  -- reference for structuring an offline mailbox research and test harness.

## Next development tasks

1. Define a documented, independent *offline* CECD data importer with strict
   file-size bounds and no use of undisclosed keys; require user-authorized
   mailbox exports and synthetic test fixtures.
2. Keep the existing SPDR DS local wireless protocol separate from CECD.
3. Test DS-to-DS encounter transfers on two real systems and closed lid on
   one running process; hardware performance remains unverified.
4. Never claim that arbitrary games run a service solely because a folder or
   config file exists. Require explicit cooperative launcher/game integration.
5. Never claim stock-3DS native StreetPass recognition until a real on-device
   interoperable implementation is independently verified.

## AzaharPlus virtual StreetPass test correction (2026-11-03)

The project owner clarified that **AzaharPlus can download 3DS system files from official servers**, so this test plan must not block on the user supplying a physical 3DS NAND/system dump. Use the emulator's built-in downloader instead.

- Official project: https://github.com/AzaharPlus/AzaharPlus
- Pin **AzaharPlus 2126.2-A or newer**. The 2126.2-A release notes explicitly say it fixes a publicly announced critical security vulnerability; do not use older 2126.0/2126.1 builds.
- Linux AppImage: https://github.com/AzaharPlus/AzaharPlus/releases/download/AZAHAR_PLUS_2126_2_A/azaharplus-2126.2-A-linux.AppImage
- The release supports downloading system files from official servers and provides ZipPass for exporting/importing StreetPass pass archives. ZipPass requires system files and LLE modules enabled and StreetPass enabled in the target title.
- NetPass documentation says emulator support is not guaranteed but identifies Azahar as the only known emulator able to efficiently support it. AzaharPlus is a distinct fork; validate the installed NetPass version on the exact AzaharPlus build instead of assuming compatibility.
- NetPass is a StreetPass test target only here. Do not expand this test milestone into unrelated 3DS game compatibility or general gameplay testing.
- melonDS is the DS-side virtual test target only for StreetPass/relay behavior. Use two instances for local multiplayer; the upstream FAQ warns that this is work-in-progress, unstable and timing-sensitive.
- Capture actual emulator windows and StreetPass inbox/relay state. Never fabricate screenshot evidence.

### StreetPass-only virtual test sequence

1. Download and verify AzaharPlus 2126.2-A Linux AppImage by release SHA-256; launch it in an Xvfb desktop.
2. Use AzaharPlus's own **Download System Files** menu to fetch official system files; verify both o3DS and New 3DS files plus LLE modules are enabled.
3. Install/run a user-provided NetPass CIA or use ZipPass if the intended test is specifically file-based pass import/export. Clearly label which mechanism is tested.
4. Run two AzaharPlus instances and test only StreetPass/NetPass exchange, queue receipt, duplicate handling and screenshots. NetPass service behavior may require network and its own service availability.
5. Run two standalone melonDS instances with the DS Plaza homebrew and test only the DS StreetPass relay protocol, queue cap, ACK/retry, duplicate suppression and screenshots.
6. Keep AzaharPlus↔AzaharPlus, melonDS↔melonDS, ZipPass file exchange and any DS↔3DS adapter as separate test results. No cross-emulator radio interoperability is implied.
7. Keep all emulator system files, LLE modules, CIA/game data, pass archives and personal Mii/console data out of the public repo and CI artifacts.

**Evidence status:** melonDS has an existing successful genuine screenshot capture and DS ROM validation. AzaharPlus has not yet been installed/launched in the current runner; its StreetPass/NetPass tests are still pending. The emulator downloader removes the need to ask the user for a physical 3DS system dump, but does not itself prove a StreetPass exchange.
