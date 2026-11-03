# BlocksDS libraries for the full StreetPass Plaza application

This guide expands the Nitro Engine/DSMA renderer work to the rest of the game.
Use the official BlocksDS docs and examples as the API contract:
https://blocksds.skylyrac.net/docs/libs/core_libs/
https://blocksds.skylyrac.net/docs/libs/additional_libs/
https://blocksds.skylyrac.net/libnds/

## Current codebase map

The application already has separate modules for protocol, visitor storage,
Mii conversion, visitor discovery, relay, roster, arrival notices, and Puzzle
Swap. The ARM9 Makefile links Nitro Engine, DSWiFi and libnds; the selected
ARM7 image is `arm7_dswifi_maxmod.elf`. Keep these existing modules and
their host tests. New libraries should be adapters around the current modules,
not replacements for the visitor protocol or save format.

## Additional libraries and where they fit

| Library / API | Game systems it can improve | Package / link | Decision |
| --- | --- | --- | --- |
| NightFox's Lib | Sprite/background UI, animated 2D scenes, menus, transitions, touch-driven room decorations | `blocksds-nflib`, `-lnflib` | Prototype for 2D-only screens; compare against native libnds sprites first. |
| GL2D (libnds) | 2D overlays, textured shapes, visual effects on 3D hardware | Built into libnds | Useful in a Nitro Engine-owned scene only if it does not reset or conflict with Nitro Engine state. |
| Woopsi | Reusable C++ windows, buttons, lists, dialogs and settings widgets | `blocksds-libwoopsi`, `-lwoopsi` | Optional C++ UI experiment; do not migrate the current C interface until a small ROM demo works. |
| LibDSF | Scalable/custom font rendering for Mii names, dialogs and menus | `blocksds-libdsf`, `-ldsf` | Strong candidate for typography; validate memory and ARM9 performance. |
| Maxmod | Background music, UI sounds, visitor arrival jingles and ambience | `-lmm9`, `-lmm7`; ARM7 core already includes Maxmod | Integrate ARM9 player and asset pipeline; keep sound optional and test alongside DSWiFi. |
| DSWiFi | Wireless discovery, visitor transfer and multiplayer | `-ldswifi9`, `-ldswifi7` | Already linked; focus on protocol and two-instance tests, not a second networking stack. |
| libnds camera | DSi front/rear camera features | `-lnds9` | Add DSi-gated camera workflows only; original DS must show disabled controls. |
| libnds SDMMC / filesystem | DSi SD saves, camera imports/exports | libnds / standard C I/O | Add explicit `sd:/` handling on DSi, preserve `fat:/` DLDI on DS. |
| NitroFS | Read-only bundled UI assets, fonts, sound and public demo data | libnds filesystem support | Use for immutable resources, never for save data. |
| libnds cooperative threads | Background file reads, image conversion and non-blocking work | libnds | Consider only after profiling; keep wireless state changes serialized. |
| DMA / NDMA / cache / NWRAM | Faster texture, image and asset copies; DSi memory use | libnds | Optimize only after profiling, with alignment/cache correctness tests. |
| Timers / system counter / RTC | Animation timing, timestamps, visitor history and idle transitions | libnds | Use a monotonic counter for animation and RTC only for display/history. |
| Grit / ptexconv | Convert sprites, backgrounds, textures and compressed graphics | BlocksDS tools | Add reproducible conversion rules and CI validation for dimensions/palettes. |
| libslim | Alternative filesystem backend | Third-party | Not a drop-in libfat replacement; retain as research until BlocksDS port and adapter tests pass. |
| uLibrary | 2D graphics via DS 3D hardware | `blocksds-ulibrary`, `-lul -lpng -lz` | Do not run concurrently with Nitro Engine on the same 3D hardware; use only in a separate renderer experiment. |

## Feature implementation plan

### A. Main interface and menus
1. Keep the current libnds screen/input code as the known-good fallback.
2. Build a small NightFox's Lib test scene for the title/menu screen, sprite
   buttons, backgrounds, touch feedback and transitions.
3. Compare VRAM allocation, sprite limits, performance and touch latency.
4. Port individual menu screens only after the test scene passes on melonDS
   and hardware. Do not replace the existing visitor/storage modules.

### B. Mii and visitor presentation
1. Keep the 134-byte Mii packet and existing gallery parser as the identity
   and appearance source.
2. Add a renderer adapter that maps Mii fields to a model/texture palette and
   facial/body parameters.
3. Use Nitro Engine/DSMA for skeletal animation; use Grit/ptexconv for
   generated DS-budget textures and icons.
4. Use LibDSF for names only if the font renderer passes performance tests.
5. Do not claim original 3DS Mii fidelity until the actual model/texture
   mapping and asset comparison are complete.

### C. Puzzle Swap and Plaza activities
1. Keep `puzzle_swap.c` as the tested rules/save layer.
2. Add a graphical presentation layer using NightFox's Lib or libnds sprites.
3. Load puzzle images from user-authorized resources or original homebrew
   placeholder artwork; never commit extracted Nintendo art publicly.
4. Add sound events through Maxmod after the visuals are stable.

### D. Sound
1. Add a small ARM9 Maxmod adapter for music and sound effects.
2. Add optional `AUDIODIRS` conversion inputs using supported BlocksDS formats.
3. Test audio playback and DSWiFi concurrently on the selected ARM7 core.
4. Ensure mute/volume settings and graceful fallback if audio assets are absent.

### E. Storage and assets
1. Retain libnds filesystem/picolibc I/O for the current `fat:/` DLDI path.
2. Add DSi-specific `sd:/` selection behind a capability check.
3. Store saves only on writable filesystems; use NitroFS for immutable resources.
4. Keep libslim experimental until it compiles natively with BlocksDS and passes
   the existing profile, visitor queue, atomic-save and restart tests.

### F. Wireless and concurrency
1. Keep DSWiFi and the DSWiFi-enabled ARM7 core.
2. Run real two-instance tests for discovery, connection, visitor transfer,
   ACK, retry, duplicate suppression and disconnect/reconnect.
3. Use cooperative threads only for independent reads/asset preparation.
4. Never let background tasks mutate the wireless state machine or visitor
   store without a defined synchronization boundary.

## CI / validation recommendations

- Install optional packages with `wf-pacman` and discover paths with
  `wf-pacman -Ql`; do not hardcode guessed install roots.
- Build the smallest official example for each candidate library before
  linking it into the app.
- Add isolated compile targets for NightFox's Lib, LibDSF and Woopsi.
- Build a separate Nitro Engine + GL2D interoperability test, not a combined
  production renderer until proven safe.
- Run host tests for all existing data/protocol modules unchanged.
- Build the NDS ROM and boot it in melonDS after each graphics/audio integration.
- Keep proprietary 3DS assets outside public repository and CI artifacts.

## Priority order

1. NightFox's Lib prototype for the main 2D UI.
2. Maxmod ARM9 integration for music and UI/visitor sound.
3. LibDSF typography experiment.
4. DSi SD/camera capability-gated features.
5. Puzzle Swap graphical presentation using the existing rules module.
6. Woopsi optional GUI experiment.
7. Profile-guided DMA/threading optimizations.
8. libslim only after a BlocksDS-native port is demonstrated.

This is a verified candidate map, not a claim that all these libraries are
already integrated. Each item becomes an implemented feature only after its
package, API example, ROM build and emulator test pass.
