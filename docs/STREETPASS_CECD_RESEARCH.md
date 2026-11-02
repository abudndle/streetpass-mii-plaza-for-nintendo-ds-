# StreetPass architecture reference research

Research checked 2026-11-01. These are external references, not source assets to redistribute.

## References
- 3dbrew StreetPass: https://www.3dbrew.org/wiki/StreetPass
- MiiWiki StreetPass: https://miiwiki.org/wiki/StreetPass (site did not load during this review; details unverified)
- wwylele 3DS CEC notes: https://gist.github.com/wwylele/29a8caa6f5e5a7d88a00bedae90472ed
- Streetpass-Extractor-cmd: https://github.com/AbdyyEee/Streetpass-Extractor-cmd
- Previous: https://github.com/Waldy238/OpenPass and https://gitlab.com/3ds-netpass/netpass
- Earlier Nintendogs Bark Mode research: docs/NINTENDOGS_REFERENCE_VERIFICATION.md

## Verified architecture

3DS StreetPass runs a dedicated system-module communicator and mailbox manager
against CECD system save 00010026. Titles write their outbox through CECD APIs,
and the OS communicator exchanges stored messages in standby. Native wireless
traffic uses a 3DS-specific scan/handshake and WPA2-CCMP data exchange; this is
not ordinary DS Download Play or DSWifi local multiplayer.

3dbrew describes CECD service IDs and message formats; the Mii Plaza identifier
is observed as 00 02 08 00 00. This is documentation for understanding data,
not a claim that sending that identifier from DS will authenticate to a stock
3DS console.

The extractor repository is archived and explicitly states it is unmaintained,
directing people to NetPass. It is about CECD files / Citra-era workflows,
not native DS radio interoperability. Treat any imported CECD user data as
untrusted and do not publish private Mii payloads or keys.

## DS design implications

1. Keep `streetpass-plaza/service.cfg`, inbox, outbox, and visitor files as
   *persistent storage* only; do not pretend folder presence executes code.
2. Cooperative DS games/menus can explicitly run a shared tick/poll routine
   while they control the console. Without cooperation, no persistent system
   daemon runs behind any arbitrary game. Flashcards and modified DSi do
   not change that ordinary DS software execution model.
3. Bark-style lid-close operation is an opt-in mode of the *running*
   StreetPass Plaza DS program, not an autonomous system service.
4. Continue the independent SPDR 80-byte DS-to-DS protocol: capped relays,
   CRC, replay checks, bounded storage and user-consented radio. Stock 3DS
   CECD handshakes are NOT implemented.
5. Use 3DS CECD documentation to develop an explicitly **offline,
   user-authorized** mailbox import/export adapter in the future. Keep that
   converter separate from the on-air DS packet format.
6. Validate two physical DS/DSi consoles, lid-open and lid-closed, and
   document latency, battery effects and failure cases before claiming
   device-to-device behavior as tested.
