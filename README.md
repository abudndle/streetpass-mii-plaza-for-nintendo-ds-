# StreetPass Mii Plaza for Nintendo DS / DSi

An actively developed, independent homebrew adaptation targeting the original Nintendo 3DS **StreetPass Mii Plaza**, built for Nintendo DS/DS Lite flashcarts and DSi SD homebrew.

## Multi-system DS/DSi + 3DS compatibility (corrected)

The project goal is a multi-node DS/DSi visitor network alongside multiple unmodified 3DS systems. These are currently separate networks: DS homebrew uses SPDS/SPDR; unmodified 3DS consoles use native StreetPass with compatible 3DS software. Nintendo says StreetPass is only compatible with 3DS software. SpotPass and other 3DS online services ended April 8, 2024, so the former Nintendo Zone relay service must not be treated as an available bridge. **Automatic DS ↔ unmodified-3DS Mii Plaza delivery is not implemented and has no verified supported path.** See [corrected multi-system architecture](docs/MULTI_SYSTEM_RELAY_ARCHITECTURE.md).

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

## 3D room renderer (Nitro Engine + DSMA)

The room renderer is being migrated to AntonioND's Nitro Engine and its DSMA skeletal-animation support rather than a project-written 3D engine. The native BlocksDS build links `-lNE`; the renderer loads DSM/DSA visitor assets from `streetpass-plaza/models/` and retains the existing Mii records and Plaza controls. Authentic per-Mii model conversion and original Plaza assets remain separate unfinished work. See [Nitro Engine integration](docs/NITRO_ENGINE_INTEGRATION.md).

### Full-game BlocksDS library plan

NightFox's Lib is now provisioned and linked in the BlocksDS native-ROM build alongside Nitro Engine. CI verifies the installed header and static library. This is a build integration milestone only; the current libnds menus remain active until an NFLib screen prototype is implemented and tested. See [the full-game library plan](docs/BLOCKSDS_FULL_GAME_LIBRARY_PLAN.md).

The expanded [library adoption plan](docs/BLOCKSDS_FULL_GAME_LIBRARY_PLAN.md) maps SDK facilities to the main UI, Mii and visitor presentation, Puzzle Swap, sound, storage, wireless, DSi camera, asset conversion and performance. Priority candidates beyond 3D are NightFox's Lib for 2D scenes, Maxmod for audio, and LibDSF for fonts. Woopsi is an optional C++ GUI experiment. Each integration must pass an example build and emulator test before replacing existing code.

### Additional library evaluation

The BlocksDS CI ROM build now provisions the official `blocksds-ulibrary` package beside Nitro Engine. uLibrary is reserved for a separately verified 2D menu/widget adapter; it is not yet linked into the active Nitro Engine renderer because both can contend for the DS 3D hardware. libslim has been reviewed but is not substituted for libfat yet: its published build uses legacy devkitARM rules and it is not libfat-compatible, while the current app depends on `fatGetDefaultDrive()` and explicit `fat:/` paths. See [the engine and filesystem integration notes](docs/NITRO_ENGINE_INTEGRATION.md).

## Current verified progress

- Bootable-format `.nds` homebrew **compiles** using the official BlocksDS Docker image; GitHub Actions uploads the `streetpass-plaza-ds-homebrew` artifact. A melonDS screenshot-capture workflow exists; emulator launch/screenshot artifacts must be checked per run. Cross-emulator StreetPass/NetPass tests remain outstanding.
- Two-screen prototype interface, R4/DLDI FAT and DSi-compatible default SD storage.
- Persistent profile `profile.sps`, `inbox/` queued import, `outbox/profile.sps` export, 8-hour encounter deduplication, host-tested versioned 64-byte exchange format.
- Headless [menu/launcher polling component](docs/LAUNCHER_INTEGRATION.md): callable without opening the Plaza UI **while the menu process is running**; neither a firmware system service nor a closed-lid always-on receiver.
- [Experimental DS-to-DS local wireless](docs/WIRELESS_EXPERIMENT.md) via DSWifi host/join. **Not yet confirmed on two consoles.** This is not native 3DS StreetPass. Emulator-assisted tests are now planned separately using Azahar for the 3DS/CECD/NetPass side and melonDS for DS local multiplayer; cross-emulator interoperability must be demonstrated rather than assumed.
- [Private NCCH asset inventory and LZ decoder](docs/PRIVATE_ASSET_PIPELINE.md): all 115 RomFS file entries mapped and 56 compressed resources decoded from the user-provided base NCCH offline; *no copyrighted assets on GitHub*.
- Automated CI: host tests every five minutes plus native ROM build and artifact on pushes/PRs/manual runs.

## Closed-lid Bark Mode experiment

The original Nintendogs DS manual confirms that Bark Mode can continue
searching with the lid closed. BlocksDS exposes `disableSleep()`,
allowing an **opt-in** experiment while this Plaza is running.
Set both `radio_enabled=1` and `lid_search_enabled=1` in
`streetpass-plaza/service.cfg`. **This is not yet validated on real
hardware**, will consume more battery, does not run in unrelated games
and does not enable official unmodified-3DS StreetPass. See
[Bark Mode research](docs/BARK_MODE_RESEARCH.md).

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

An unmodified retail 3DS does not understand this project's independent DS radio protocol. Native StreetPass remains a separate 3DS-software feature. SpotPass ended April 8, 2024, so do not rely on Nintendo's former relay-point service. DS ↔ unmodified-3DS automatic Plaza delivery is currently unimplemented and unproven; optional 3DS homebrew or manual user-mediated transfer are separate alternatives, not native StreetPass. Standard DS/DSi homebrew cannot keep executing while another application has taken control, and closed-lid sleep radio operation remains unverified.

## References and development

[OpenPass](https://github.com/Waldy238/OpenPass) · [NetPass](https://gitlab.com/3ds-netpass/netpass) · [Mii Maker DS](https://github.com/abudndle/mii-maker-ds) · [yazidears StreetPass](https://github.com/yazidears/StreetPass) · [StreetPass history](https://en.wikipedia.org/wiki/SpotPass_and_StreetPass)

[Milestones](docs/MILESTONES.md) · [protocol](docs/PROTOCOL.md) · [hardware](docs/HARDWARE.md) · [asset inventory](docs/ASSET_INVENTORY.md) · [wireless test plan](docs/WIRELESS_EXPERIMENT.md)

No source code from OpenPass or GPL NetPass is copied here; reference projects have separate licensing. The MIT license applies only to independent code contributed to this repository. Nintendo's copyrighted game and resources are **not** licensed under MIT and must stay out of public commits and CI.
