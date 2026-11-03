# Multi-system StreetPass relay architecture

Status: **architecture and acceptance contract; stock-3DS interoperability is not yet implemented or proven**.

## User-visible goal

Operate one shared encounter network containing multiple Nintendo DS/DSi Plaza clients and multiple Nintendo 3DS systems. A visitor originating on one participant must be able to reach other eligible participants through one or more relay nodes, retain origin identity, and be delivered at most once per encounter window. This should feel like the original StreetPass relay experience, not a one-to-one cable transfer.

## Three distinct network roles

1. **DS/DSi Plaza node** — runs this NDS homebrew while it is the active application. It uses the project's independent SPDS/SPDR transport, SD persistence, consent settings and visitor cache. It cannot keep executing after another DS game takes control.
2. **Stock 3DS native StreetPass node** — an unmodified retail 3DS uses Nintendo's built-in StreetPass/CECD path with other compatible 3DS systems and supported native relay points. It does not understand SPDS packets and cannot run a custom companion.
3. **Gateway/relay bridge** — a separately powered relay appliance or host with independent interfaces for DS homebrew traffic and the 3DS native StreetPass relay exchange. It must translate only explicitly supported Plaza visitor records, retain provenance/consent, and keep native CECD transport separate from the DS packet format.

An optional 3DS homebrew companion could be a future alternative adapter, but it is **not** the target dependency for the user's unmodified-3DS requirement.

## Critical compatibility boundary

Nintendo documents native StreetPass as a 3DS-family feature for compatible 3DS software. The 3DS local encounter path uses CECD/NWM and encrypted local-WLAN exchanges; separately, Nintendo's original StreetPass Relay Points used SpotPass/server communication to store and forward encounter data. DS/DSi Contact Mode is separate, and Nintendo's manuals state that DS/DSi software use prevents 3DS StreetPass operation while running. Therefore the DS ROM cannot itself act as a native StreetPass title on a stock 3DS, and a generic nearby AP cannot make an SPDR packet appear in Mii Plaza. The remaining research must identify a lawful, supported, stock-firmware relay-point service interface before claiming a cross-domain path.

The project must not claim stock-3DS support until a real, unmodified retail console receives a valid native StreetPass Plaza record from the gateway and can return one. A custom access point alone, a successful DS radio test, or a host-side packet test is not proof.

## Multi-node forwarding model

- Each origin has a stable origin-system class (DS, DSi, or 3DS), origin identifier, encounter nonce, creation time, and source application.
- Each record carries an explicit Mii appearance-sharing permission and redistribution permission. Private Miis never enter the relay queue.
- Gateways store-and-forward records between transport domains. The relay does not rewrite origin identity to impersonate a receiving console.
- Every hop decrements a bounded hop budget exactly once. Each node forwards a given origin+nonce at most once; duplicates and expired records are rejected.
- Use a durable queue with bounded size, expiry, replay protection, and per-origin rate limits. Do not rely on a single in-memory cache for persistent relay history.
- A record is acknowledged only after durable storage. Failed or interrupted transfers remain retryable without creating a second visitor.
- Separate DS radio, native 3DS CECD, and gateway translation adapters. Never treat CRC as sender authentication; use authenticated gateway-to-gateway transport and preserve untrusted-origin labels for legacy DS packets.
- Provide a local diagnostic screen showing discovered nodes, queued records, forwarded/received counts, and last error without exposing private Mii data.

## Required verification gates

1. **DS mesh:** at least three physical DS/DSi homebrew nodes exchange, forward, deduplicate and persist a test visitor. Emulator tests are supplemental.
2. **Stock 3DS native relay:** two or more unmodified retail 3DS systems perform native StreetPass through the relay/gateway, including Mii Plaza delivery. No homebrew installation on these 3DS consoles.
3. **Cross-domain bridge:** a DS-origin test visitor is delivered to stock 3DS Mii Plaza, and a stock-3DS-origin visitor is delivered to a DS Plaza node, with original provenance and sharing permissions retained.
4. **Multi-system relay:** demonstrate at least three 3DS consoles and three DS/DSi nodes in one deployment; prove native 3DS-to-3DS encounters or relay-point delivery separately from DS mesh forwarding, then demonstrate cross-domain delivery only if a documented stock-firmware relay-point interface is found. Prove multi-hop forwarding, duplicate suppression, restart recovery, expiry, and isolation of private Miis.
5. **Failure/security:** malformed records, replay, spoofed DS identifiers, exhausted hops, unavailable gateway, and storage-full conditions do not create false passes or corrupt saves.

## Known limitations and safety

- The current SPDR v1 packet is a homebrew metadata format, not Nintendo CECD data, not cryptographically authenticated, and not a native 3DS StreetPass packet.
- Existing DS-to-DS radio code has not been proven on multiple physical consoles.
- The project does not have an implemented CECD/native 3DS gateway. Do not claim that the current build works with stock 3DS.
- Keep proprietary Nintendo assets, firmware, keys, CECD saves and private captures out of the public repository and CI artifacts. Use only authorized test records and user-controlled devices.
- Nintendo's original relay-point service was a store-and-forward model. This project may reproduce the user experience with an independently operated gateway, but must not imply access to Nintendo's discontinued relay servers.

## References

- [Nintendo: StreetPass Feature Overview](https://en-americas-support.nintendo.com/app/answers/detail/a_id/404/p/50/c/950)
- [Nintendo: Hardware Operations Manual](https://assets.nintendo.eu/image/upload/v1626222882/NAL/Support/3DSOperationsManual_AU.pdf)
- [3dbrew: StreetPass / CECD research](https://www.3dbrew.org/wiki/StreetPass)
- [Nintendo Iwata Asks: StreetPass Relay Points](https://iwataasks.nintendo.com/interviews/3ds/streetpass-relay-points/0/0/)
