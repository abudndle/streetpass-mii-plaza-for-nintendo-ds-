# DS / 3DS visitor exchange: corrected status

**Status:** DS/DSi custom exchange is independent; native 3DS StreetPass is 3DS-to-3DS only; no native stock-3DS bridge is implemented or verified.

SPDR v1 is an independent 80-byte homebrew packet containing a 64-byte SPDS record, timestamp, bounded hop count and CRC32. It is not a Nintendo StreetPass/CECD packet, does not authenticate its sender, and carries profile metadata rather than a complete Mii appearance. CRC detects corruption, not malicious modification.

Nintendo says StreetPass is compatible only with Nintendo 3DS software. Local StreetPass between unmodified 3DS systems remains available after April 8, 2024. SpotPass and online-dependent 3DS services ended on that date. Nintendo's historical relay-point feature used Nintendo Zone/server-backed store-and-forward; do not assume the discontinued service remains operational. A DS app does not become a native StreetPass title by transmitting SPDR.

| Path | Status |
|---|---|
| DS homebrew ↔ DS homebrew local radio | Experimental; physical multi-console proof pending |
| DS visitor bundle via SD/manual import | Implemented format; manual transfer |
| Stock 3DS ↔ stock 3DS native StreetPass | Native Nintendo feature, independent of this project |
| DS homebrew ↔ unmodified 3DS Mii Plaza | Not implemented; no verified supported path |
| DS ↔ custom 3DS homebrew companion | Separate research path; does not meet no-homebrew requirement |
| Former Nintendo SpotPass relay-point service | Do not depend on it; SpotPass ended April 8, 2024 |

## Priorities

1. Validate DS/DSi multi-node radio on physical hardware.
2. Improve DS visitor records with provenance, consent, versioning, durable queues, expiry, replay protection and authenticated gateway links.
3. Test native 3DS StreetPass separately on unmodified retail consoles.
4. Investigate user-mediated cross-platform transfer without claiming automatic StreetPass.
5. Reopen automatic stock-3DS interoperability only if a documented, available, lawful interface is found and real hardware proves both directions.

Never mark DS ↔ unmodified 3DS compatibility passed based on host tests, packet parsing, Wi-Fi association, emulator output, a custom AP or DS-to-DS success. Keep private Mii data and console identifiers out of public artifacts.

References: [StreetPass overview](https://en-americas-support.nintendo.com/app/answers/detail/a_id/404/p/50/c/950), [SpotPass ended](https://en-americas-support.nintendo.com/app/answers/detail/a_id/403/p/50/c/183), [online service discontinuation](https://en-americas-support.nintendo.com/app/answers/detail/a_id/63227/p/603/c/871), [Nintendo relay-point history](https://iwataasks.nintendo.com/interviews/3ds/streetpass-relay-points/0/2/).

## Expanded requirement: multiple 3DS and DS/DSi systems in one area

The target deployment contains **multiple Nintendo 3DS systems and multiple Nintendo DS/DSi systems in the same area**, participating in a shared visitor-exchange experience. This is a project goal, not yet a demonstrated cross-family capability.

Keep three layers explicit:

1. **Retail 3DS native StreetPass:** multiple unmodified 3DS systems exchange native StreetPass data with each other through Nintendo's built-in service. This remains independent of the DS homebrew protocol.
2. **DS/DSi homebrew relay:** DS and DSi instances exchange this project's SPDS/SPDR messages. Physical multi-console tests are required; host tests and ROM builds do not prove radio interoperability.
3. **Mixed-family bridge:** DS/DSi and multiple 3DS systems may share a custom relay only if a compatible 3DS-side participant or a documented stock-firmware interface is implemented and tested. A 3DS homebrew companion is optional custom networking, not native StreetPass on an unmodified 3DS. Do not claim DS-origin visitors enter the retail Mii Plaza queue without proof.

### Mixed-network test plan

- Test at least two DS/DSi nodes and two 3DS nodes in the same RF area.
- Test DS/DSi-only discovery/exchange and 3DS-native StreetPass independently first.
- If a 3DS homebrew companion is used, test discovery, authenticated gateway session, conversion, durable ACK, duplicate suppression, hop limits and recovery.
- Keep the DS project's ten-pending-visitor limit and 3,000-Mii saved population cap separate from each retail 3DS's own StreetPass queue.
- Record hardware, software versions, direction and redacted evidence. Never publish console identifiers, private keys, personal Mii data or proprietary assets.
- Report each path independently: DS/DSi↔DS/DSi, 3DS↔3DS native, DS/DSi↔3DS companion, and DS/DSi↔unmodified 3DS native Plaza.

**Mixed-family exchange remains unverified.** This requirement does not override protocol boundaries.
