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
