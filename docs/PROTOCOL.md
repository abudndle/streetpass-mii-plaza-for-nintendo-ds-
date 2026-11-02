# SPDS v1 test exchange protocol

This is a **new homebrew test protocol**, not the Nintendo 3DS StreetPass/CECD/Mii data format, and does not automatically make a retail 3DS compatible.

Each file is **exactly 64 bytes**, all integers little-endian:

| Byte range | Content |
| --- | --- |
| 0–3 | ASCII `SPDS` |
| 4 | version = 1 |
| 5 | reserved = 0 |
| 6–7 | length = 64 |
| 8–15 | nonzero pseudonymous sender ID (uint64) |
| 16–19 | UNIX seconds at creation, informational (uint32) |
| 20–51 | 1–31 printable 7-bit ASCII nickname bytes followed by zero padding |
| 52–55 | nonce, uint32 |
| 56–59 | reserved = 0 |
| 60–63 | CRC32 of bytes 0–59, Ethernet/ZIP polynomial, little-endian |

CRC32 detects accidental corruption but **does not authenticate the sender**. Do not use arbitrary packets to represent official StreetPass visitors, or send them through the internet without an authenticated protocol and anti-replay protections.

## SD storage

On the device's selected writable drive:

```
streetpass-plaza/
    profile.sps           # Local identity, test-only
    encounters.tsv        # Locally recorded encounters
    inbox/                # Drop received .sps files here
    outbox/profile.sps    # Copy to another console's inbox for tests
```

The app checks `inbox/` every ~5 seconds while **the app is running**, discards duplicate arrivals by sender ID within 8 hours, and logs new encounters. Bad files remain in the inbox for diagnosis. Data can be lost if the SD is removed during writes; back up the folder.

A future DS/DS wireless transport and a 3DS companion transport will carry this same envelope. They are **not yet implemented**. Wire-level acknowledgement, rate limiting, and security are required before enabling network exchange.
