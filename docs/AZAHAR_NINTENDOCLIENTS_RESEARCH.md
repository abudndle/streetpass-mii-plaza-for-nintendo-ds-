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
