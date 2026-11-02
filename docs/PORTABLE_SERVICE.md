# Portable StreetPassDS service folder and Mii Maker DS integration

This project creates a **menu-independent SD-card directory**, compatible with both
DS/DS Lite flashcart DLDI volumes (`fat:/`) and DSi homebrew SD (`sd:/`).
The directory is **persistent shared data**, *not an executable background service*.

```text
streetpass-plaza/
    service.cfg         # opt-in policy and explicit Mii Maker DS save path
    profile.sps         # local SPDS v1 visitor advertisement
    mii.mds             # cached 134-byte Mii Maker DS avatar (read from save)
    encounters.tsv      # encounter history
    inbox/*.sps         # new file-transfer encounters
    outbox/profile.sps  # profile export for other SD cards
```

Created on first app launch. `service.cfg` is only created if missing.
No launcher-specific hardcoding is needed **for the files**, and the
read-only data format is reusable across homebrew.

## How to use your exact Mii from Mii Maker DS

The [Mii Maker DS](https://github.com/abudndle/mii-maker-ds) homebrew stores
Miis in the game ROM's save file. The Plaza does **not** have privileged access
to another DS title's running save-memory chip. Flashcarts usually expose
the save as an `.sav` on SD; choose its actual location for your launcher.
Do not move or rename the original save, and back it up first.

Edit `streetpass-plaza/service.cfg`:
```ini
enabled=1
radio_enabled=0
mii_source=fat:/roms/mii-ds.sav
```

For DSi SD replace `fat:/` with the actual `sd:/` path supported by your
launcher. You can also use a 134-byte Mii Maker DS `MDS3`/`MDS2` avatar
file instead of a full save. A relative filename resolves under
`streetpass-plaza/`, not under the current menu's folder.

The Plaza reads that file **read-only**, validates packet/bank checksums,
selects the Personal (gold-crown) Mii if present, otherwise the first valid
Mii, and caches the full appearance/profile as `mii.mds` in its folder.
Both 8-KiB MDSC compact and 64-KiB MDSV dual-bank save formats are supported.
Changes can be reloaded with **Y** from the Plaza screen. If the source
save is temporarily unavailable, the last validated cache is retained.
The import never alters your Mii Maker DS save.

If the original Mii has sharing disabled, the Plaza may still display its
name locally, but does **not** advertise its name on the SPDS v1 radio/profile.
SPDS v1 carries only a simplified ASCII nickname, **not** full Mii geometry.
The exact feature fields persist in the `mii.mds` cache for future DS
rendering and visitor transfer work.

## Why a file alone cannot keep StreetPass awake

The DS and DSi cannot launch code from a file without a program executing it.
An R4 microSD is only storage. A normal flashcart menu or TWiLight Menu++
loads the next NDS, replacing the prior ARM9/ARM7 programs. A data file on SD
cannot continue wireless transmissions while a completely unrelated game
takes control. Sleep mode often suspends CPU/radio activity.

You can get **automatic service while the Plaza homebrew is running**,
and a **portable cooperative service module** for homebrew apps/menus that
choose to call `spds_background_tick()`. A future cooperative launcher
can share the same folder without modifying the contents of its ROM. But
unmodified menus and unrelated games **will not automatically load**
`service.cfg` just because it is present.

To meet the real-world goal of passively exchanging Miis when the user isn't
in Plaza, research dedicated firmware/launcher integration, verified hardware
background/sleep behavior, or compatible external network hardware. No
present implementation can promise permanent stock-3DS-style StreetPass
on every DS, R4 and DSi simply by copying files.

The 3DS's official CECD StreetPass service is separate from DS/DSi radio;
native stock-3DS exchange is not implemented. A separate 3DS homebrew
companion or authenticated bridge is needed for the project's independent
protocol.

## Cooperative use by independently developed apps

Include `source/protocol.[ch]`, `source/storage.[ch]`,
`source/background.[ch]`, `source/mii_bridge.[ch]` and
`source/service_config.[ch]` in a compatible homebrew menu.
Open the standard service directory and call `spds_background_tick`
periodically from *that active process*. It is independent of
R4/TWiLight menu-specific internal paths, but the menu must cooperate.

A menu may read `service.cfg` to honor `enabled=0` and
`radio_enabled=0` and may load `mii.mds` for the user's avatar;
**do not automatically start broadcasting without informed consent**.
