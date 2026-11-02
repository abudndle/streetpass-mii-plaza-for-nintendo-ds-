# Experimental DS ↔ DS local wireless

The opt-in DSWifi transport in `source/wireless.c` is **unverified on physical hardware**. It does not implement the 3DS StreetPass/CECD service, and cannot be expected to work with an unmodified 3DS.

## Testing with two DS/DSi consoles

1. Launch the same homebrew `.nds` on two compatible DS/DSi systems and make sure FAT storage is writable.
2. Press **L** on console 1 for Host; **R** on console 2 for Join/Scan.
3. Keep the systems close, awake and within radio range. Wait for packets; records should be saved to each device's `encounters.tsv`.
4. Press **SELECT** to turn off local radio. **B** exits the application.
5. Report physical console models, flashcart/launcher type, firmware, ROM commit, and results, including logs and repeated passes.

Payload: the 80-byte SPDR envelope containing a 64-byte SPDS homebrew packet (see DS_RELAY.md and PROTOCOL.md); the host uses its own unique game ID and beacon, rather than impersonating retail Nintendo games. Only **one client** per host in this experiment. Transfers are unauthenticated: send only a nickname you consent to share. Do not exchange private Mii identity fields or full official avatar data yet.

## Limitations and next steps

- Manual host/join avoids two clients indefinitely scanning without a host.
- Add tested automatic host-election and scan/retry cycles to support unattended nearby exchanges while the *homebrew menu itself* remains running.
- Verify lid-close behavior; a stopped CPU or wireless chipset prevents exchanges.
- Use a 3DS companion or another proven bridge for DS ↔ 3DS; DS packet radio is not equivalent to 3DS StreetPass.
- Add real-hardware ACK, jitter, failure recovery and security before declaring fully functional.
