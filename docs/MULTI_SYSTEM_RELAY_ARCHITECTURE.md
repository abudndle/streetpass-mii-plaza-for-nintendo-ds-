# Corrected multi-system DS / DSi / 3DS status

**Status:** DS homebrew relay prototype. Native 3DS StreetPass is a separate Nintendo feature. Automatic delivery between this DS app and an unmodified 3DS is not implemented and has no verified supported path.

## Separate systems

- DS/DSi homebrew uses the independent SPDS/SPDR protocol and SD visitor bundles. Host tests do not prove radio interoperability; multi-console physical testing remains required.
- Unmodified 3DS systems can still exchange native StreetPass locally with compatible 3DS software.
- Nintendo states StreetPass is compatible only with Nintendo 3DS software. Nintendo also warns DS software may prevent StreetPass operation in Sleep Mode.
- The DS app cannot run as a background service while another DS game controls the system.

## SpotPass and relay-point correction

Nintendo ended SpotPass and other 3DS online functionality on **April 8, 2024**. Nintendo's system-update history documents the historical StreetPass relay feature at Nintendo Zone locations; Nintendo's original relay-point design used server-backed store-and-forward. Do not assume that discontinued Nintendo service remains available. A generic Wi-Fi AP, local server or SPDR broadcast is not automatically a native StreetPass relay point.

## Cross-platform boundary

SPDR packets are not CECD records or native 3DS StreetPass frames. Stock 3DS firmware does not understand this DS application protocol. **Automatic DS ↔ unmodified-3DS Mii Plaza delivery is blocked/unproven:** no currently available, supported stock-firmware interface has been demonstrated to deliver arbitrary DS-origin visitors into native Mii Plaza and return native 3DS visitors to this DS app.

Do not claim stock-3DS support until real retail hardware completes both directions through a documented, lawful, currently available interface. Do not claim an ordinary gateway can inject CECD data. Do not build around discontinued SpotPass as if it were live.

## Feasible paths

1. Complete and physically validate the independent DS/DSi mesh, including persistent queues, consent, deduplication, expiry and recovery.
2. Preserve native 3DS-to-3DS StreetPass as a separate function on unmodified consoles running compatible 3DS software.
3. Investigate user-mediated cross-platform transfer (supported QR/image or SD workflows) without calling it automatic StreetPass.
4. An optional 3DS homebrew companion could join a custom network, but it does not meet the no-3DS-homebrew requirement.
5. Revisit automatic stock-3DS interoperability only if a documented, available, lawful interface is identified and verified on real hardware.

## DS network contract

Preserve origin-system class, origin pseudonym, encounter nonce, timestamp, source app, consent flags, bounded hop count and version. Relays must not rewrite origin identity. Use durable queues, acknowledgement after durable storage, expiry, replay/duplicate suppression, rate limits and authenticated gateway-to-gateway transport. CRC detects corruption, not authentication. Private Miis never enter relay queues.

## Verification gates

1. Three physical DS/DSi nodes exchange and forward test visitors; verify persistence, deduplication, hop limit, expiry and restart recovery.
2. Separately verify native StreetPass between at least two unmodified 3DS systems running compatible 3DS software.
3. Do not mark cross-platform stock-3DS delivery passed without a documented supported interface and real-hardware delivery in both directions.
4. Publish redacted hardware details/screenshots only; never publish console IDs, keys, private Mii data, personal encounter records, firmware or proprietary assets.

## References

- [Nintendo StreetPass overview](https://en-americas-support.nintendo.com/app/answers/detail/a_id/404/p/50/c/950)
- [Nintendo StreetPass setup](https://en-americas-support.nintendo.com/app/answers/detail/a_id/681/~/tips-for-setting-up-streetpass)
- [Nintendo: SpotPass ended April 8, 2024](https://en-americas-support.nintendo.com/app/answers/detail/a_id/403/p/50/c/183)
- [Nintendo: 3DS/Wii U online service discontinuation](https://en-americas-support.nintendo.com/app/answers/detail/a_id/63227/p/603/c/871)
- [Nintendo 3DS system update history](https://en-americas-support.nintendo.com/app/answers/detail/a_id/667/p/172)
- [Nintendo Iwata Asks: StreetPass Relay Points](https://iwataasks.nintendo.com/interviews/3ds/streetpass-relay-points/0/2/)

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

## Mandatory unmodified-3DS multi-system acceptance

The user's requested release behavior is **automatic native StreetPass between multiple DS/DSi systems and multiple completely unmodified 3DS systems**. No 3DS homebrew, CFW, SD file copy or external companion may be substituted for this acceptance requirement.

Do not confuse **relay forwarding** (a node passes a visitor onward) with **replay** (the same visitor or nonce arrives again). Forwarding must preserve origin and consent, while replay must not generate duplicate visits or unbounded forwarding loops.

### Independent proof matrix

| Path | Required proof | Current state |
| --- | --- | --- |
| DS A ↔ DS B ↔ DS C | Wireless host/client association, bidirectional receipt, bounded forwarding, durable ACK, deduplication and restart recovery | Unverified on physical hardware |
| 3DS A ↔ 3DS B ↔ 3DS C | Native CECD visitor receipt on unmodified consoles, without a companion | Unverified in this project |
| DS A → unmodified 3DS A/B | Original retail Mii Plaza receives DS-origin visitor automatically | Blocked / no proven supported interface |
| Unmodified 3DS A/B → DS A/B | DS Plaza receives a native 3DS visitor automatically | Blocked / no proven supported interface |
| Mixed multi-hop | A consented visitor traverses at least two nodes, no duplicate visits, expires on hop/time limits, survives restart | Blocked until native bridge proven |

Test with **at least three DS/DSi and three stock 3DS units** in a shared RF environment, separately verifying direct exchange, multi-node forwarding, duplicate suppression, queue capacity, radio reconnection, power-cycle persistence, and privacy. Record sanitized before/after encounter counts and bidirectional evidence for every claimed path. An emulator boot, packet sniff, manual import or ordinary Wi-Fi association is insufficient.

### Protocol feasibility gate

Before attempting native mixed-family radio delivery, establish from authorized documentation and non-sensitive testing that DS/DSi hardware and software can participate in the required 3DS discovery, CECD transport and any authentication. If this cannot be established, retain a visible hardware/protocol blocker. Historical Nintendo Zone StreetPass relay points do not provide a currently available generic CECD gateway. Never represent an optional 3DS homebrew companion as meeting the unmodified-3DS requirement.
