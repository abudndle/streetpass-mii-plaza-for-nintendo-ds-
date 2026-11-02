# Hardware and background-service constraints

## Targets

- DS/DS Lite: homebrew launched from R4-compatible Slot-1 flashcart, DLDI FAT/microSD.
- DSi: compatible SD homebrew launcher or flashcart; the default writable volume depends on the launch path.
- DSWifi supports local multiplayer in DS compatibility mode, but no connection code is in this initial commit.
- The unmodified 3DS's StreetPass receiver runs a separate 3DS OS service and does not understand the SPDS homebrew test packet. 3DS support requires a **separate homebrew companion** (and/or carefully researched, safe system-module integration) plus tests on actual hardware.

## Background operation

The user requests that collection continue when the Plaza application is not open, through the homebrew menu, including closed-lid use. A normal DS game/homebrew process cannot leave a resident system service running after a different game replaces it. An R4 microSD card is storage, not an always-on coprocessor. The DSi SD card likewise cannot execute programs by itself.

**Practical near-term path:** integrate a cooperative polling service in the *homebrew launcher's own running menu* so passes can be collected while browsing that menu. This requires menu source integration and radio transport. In ordinary DS sleep the processor/wireless may be suspended, so closed-lid always-on radio exchanges require instrumented on-hardware verification and perhaps are infeasible; never promise battery-safe StreetPass-style sleep operation until demonstrated.

This prototype's auto-poll is limited to SD inbox files while the Plaza application itself is running. It is not a custom launcher, firmware module, or background wireless daemon.

## Integrity, privacy and assets

- Exchange nicknames are locally stored in readable text; users should consent to exchanging a pseudonym.
- CRC32 is not encryption, signatures, or identity authentication.
- Do not disclose device MAC addresses or use a permanently identifying hardware ID in public exchange.
- Nintendo assets from the user-uploaded CIA and NCCH must never be committed to a public repository or published in Actions artifacts.
- Integration with Mii Maker DS should use its independently written avatar conversion API, with tests against actual saved files, not blindly memcpy internal structs.
