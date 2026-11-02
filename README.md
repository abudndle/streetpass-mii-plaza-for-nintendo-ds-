# StreetPass Mii Plaza for Nintendo DS / DSi

An actively developed, independent homebrew adaptation targeting the original Nintendo 3DS **StreetPass Mii Plaza**, built for Nintendo DS/DS Lite flashcarts and DSi SD homebrew.

## Mii Maker DS integration and portable folder

The Plaza also supports a versioned **full-Mii SD visitor bundle**. With
a shareable linked Mii, `outbox/visitor.spv` carries the 134-byte Mii Maker
avatar plus encounter metadata. Copy it into another Plaza's `inbox/` to
import a cached visitor, removing foreign Personal ownership. This is
currently **SD file exchange, not radio or native 3DS StreetPass**.
See [visitor bundles](docs/VISITOR_BUNDLE.md).

The Plaza now contains a read-only parser for Mii Maker DS 8-KiB/64-KiB
ROM-associated save snapshots and standalone 134-byte avatar packets,
plus a durable SD Mii cache. Configure the save's location in
`streetpass-plaza/service.cfg` to select the Personal Mii; no other
homebrew application's cartridge save is modified. See
[portable service folder and Mii integration](docs/PORTABLE_SERVICE.md).
No SD file can autonomously run in the background of unrelated DS ROMs.

## Current verified progress

- Bootable-format `.nds` homebrew **compiles** using the official BlocksDS Docker image; GitHub Actions uploads the `streetpass-plaza-ds-homebrew` artifact. Actual emulator and hardware boot tests remain outstanding.
- Two-screen prototype interface, R4/DLDI FAT and DSi-compatible default SD storage.
- Persistent profile `profile.sps`, `inbox/` queued import, `outbox/profile.sps` export, 8-hour encounter deduplication, host-tested versioned 64-byte exchange format.
- Headless [menu/launcher polling component](docs/LAUNCHER_INTEGRATION.md): callable without opening the Plaza UI **while the menu process is running**; neither a firmware system service nor a closed-lid always-on receiver.
- [Experimental DS-to-DS local wireless](docs/WIRELESS_EXPERIMENT.md) via DSWifi host/join. **Not yet confirmed on two consoles.** Nothing here impersonates or provides native 3DS StreetPass.
- [Private NCCH asset inventory and LZ decoder](docs/PRIVATE_ASSET_PIPELINE.md): all 115 RomFS file entries mapped and 56 compressed resources decoded from the user-provided base NCCH offline; *no copyrighted assets on GitHub*.
- Automated CI: host tests every five minutes plus native ROM build and artifact on pushes/PRs/manual runs.

## Optional automatic local wireless

Set `radio_enabled=1` in `streetpass-plaza/service.cfg` to try
host/client cycling automatically **while Plaza is running**.
The independent radio link is experimental and is not guaranteed to
work in closed-lid sleep, inside unrelated DS ROMs, or as native 3DS
StreetPass. See [automatic discovery](docs/AUTOMATIC_DISCOVERY.md).

## Controls (current prototype)

| Input | Action |
| --- | --- |
| A | Save/export local profile to SD outbox |
| X | Scan/import SD inbox immediately |
| L | Start experimental DS wireless host (opt-in) |
| R | Start experimental DS wireless scan/join |
| SELECT | Turn radio off |
| B | Exit homebrew |

To exchange an SD file manually, copy `streetpass-plaza/outbox/profile.sps` to another console's `streetpass-plaza/inbox/guest.sps`. The app periodically scans for new files while running. This test exchange does not simulate actual radio discovery.

## Build

Use [BlocksDS](https://blocksds.skylyrac.net/) and Wonderful Toolchain, then `make` to build `streetpass-plaza-ds.nds`. Run portable tests without BlocksDS using `make test`. Install the NDS on compatible R4 microSD or DSi SD homebrew launcher storage; the target directory is automatically created on the writable FAT/SD device.

## Original functionality target and realistic limits

Port the 3DS Mii Plaza rooms, original Miis, visitor data, touch UI, animations, Puzzle Swap, Find Mii, games, achievements, audio and other applicable features from the user's authorized assets. The user-provided base NCCH is **not proof that DLC game assets are present**; unprovided updates/DLC cannot be fabricated.

An unmodified retail 3DS cannot exchange this project's independent DS radio protocol. DS ↔ 3DS **requires a separate compatible companion**, or future verified bridging code; the 3DS console operating system's StreetPass service is different. Standard DS/DSi homebrew cannot keep executing while another application has taken control, and closed-lid sleep radio operation remains unverified.

## References and development

[OpenPass](https://github.com/Waldy238/OpenPass) · [NetPass](https://gitlab.com/3ds-netpass/netpass) · [Mii Maker DS](https://github.com/abudndle/mii-maker-ds) · [yazidears StreetPass](https://github.com/yazidears/StreetPass) · [StreetPass history](https://en.wikipedia.org/wiki/SpotPass_and_StreetPass)

[Milestones](docs/MILESTONES.md) · [protocol](docs/PROTOCOL.md) · [hardware](docs/HARDWARE.md) · [asset inventory](docs/ASSET_INVENTORY.md) · [wireless test plan](docs/WIRELESS_EXPERIMENT.md)

No source code from OpenPass or GPL NetPass is copied here; reference projects have separate licensing. The MIT license applies only to independent code contributed to this repository. Nintendo's copyrighted game and resources are **not** licensed under MIT and must stay out of public commits and CI.
