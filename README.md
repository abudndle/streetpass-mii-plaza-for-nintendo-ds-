# StreetPass Mii Plaza for Nintendo DS / DSi

A work-in-progress porting project targeting the complete original Nintendo 3DS StreetPass Mii Plaza experience, adapted to DS/DSi hardware.

## Goals
- Preserve the original plaza interface, Mii appearances and interactions, games, progression, sounds, assets and behavior wherever technically possible.
- Study local wireless discovery and Mii exchange formats; do not claim native 3DS StreetPass compatibility until verified on hardware.
- Investigate a launcher-integrated, sleep-mode-capable background exchange service. DS/DSi hardware cannot directly reproduce the 3DS operating system's background StreetPass service without new system/launcher integration.
- Reuse the Mii Maker DS project's compatible open-source interfaces when available; keep the repositories and milestones separate.
- Keep Nintendo proprietary ROMs and extracted assets private. Never commit them or upload them to public CI artifacts.
- Validate native ROM builds with BlocksDS and emulator/hardware tests when source and toolchain are available.

## Technical references
- https://github.com/abudndle/mii-maker-ds
- https://github.com/Waldy238/OpenPass
- https://gitlab.com/3ds-netpass/netpass
- https://github.com/yazidears/StreetPass
- https://en.wikipedia.org/wiki/SpotPass_and_StreetPass

## Status
Planning and repository bootstrap. No gameplay implementation, functioning StreetPass radio exchange, sleep-mode process or DS ROM has been verified yet. See [milestones](docs/MILESTONES.md).
