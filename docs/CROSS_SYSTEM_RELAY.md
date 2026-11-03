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
