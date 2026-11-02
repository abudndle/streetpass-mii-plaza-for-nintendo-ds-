# Building and testing

This is a source-only DS boot scaffold. It does **not** contain the original StreetPass Mii Plaza game.

## Requirements

Install a compatible BlocksDS/devkitARM toolchain including libnds and ndstool. Set `DEVKITARM` and `BLOCKSDS` as required by your installed toolchain.

Run `make` to attempt a ROM build and `make clean` to clean outputs.

Run `python3 -m unittest discover -s tests -v` for host checks.

**Validation caveat:** GitHub CI currently runs host tests only. The Makefile is an initial toolchain integration draft and must be compiled against the installed BlocksDS version before any ROM-build milestone is claimed. A bootable emulator-tested ROM is not yet verified.

Original Nintendo assets are never included in this public repository. Wireless discovery, StreetPass 3DS compatibility, save data, original gameplay, and DSi-specific functions are not yet implemented.
