# BlocksDS core library adoption guide

The official BlocksDS core-library documentation and tutorials are the primary
reference for implementation in this repository:

- Core libraries: https://blocksds.skylyrac.net/docs/libs/core_libs/
- All library docs: https://blocksds.skylyrac.net/docs/libs/
- Tutorial: https://blocksds.skylyrac.net/tutorial/
- SDK examples catalogue: https://github.com/blocksds/sdk/blob/master/examples/readme.md
- Filesystem guide: https://blocksds.skylyrac.net/docs/guides/filesystem/
- Debugging guide: https://blocksds.skylyrac.net/docs/guides/debugging/
- Optimization guide: https://blocksds.skylyrac.net/docs/guides/optimization/
- DSi camera API: https://blocksds.skylyrac.net/libnds/camera_8h.html
- libnds API: https://blocksds.skylyrac.net/libnds/

Use existing SDK functionality before implementing low-level hardware code.
Install optional libraries using `wf-pacman`, use their official headers and
link flags, and build upstream examples as API/ABI checks before integrating
them. Do not vendor or reimplement a library just to access its functionality.

## Project-to-library map

| StreetPass feature | Preferred BlocksDS facility | Integration guidance |
| --- | --- | --- |
| Two-screen display, backgrounds, sprites, touch | libnds 2D video, sprite, input, touch APIs | Keep the existing bottom-screen roster responsive; migrate UI incrementally. |
| Plaza room and 3D visitor models | Nitro Engine + DSMA; libnds 3D only where needed | Nitro Engine owns the 3D hardware while the room is active. Do not run another 3D renderer concurrently. |
| 2D UI using the 3D engine | GL2D or uLibrary | Prototype separately; choose one renderer per scene and verify screen/VRAM ownership. |
| DS local wireless visitor exchange | DSWiFi | Keep the protocol independent of rendering and storage. Test actual multiplayer separately from host codec tests. |
| Persistent visitor queue and profile | libnds filesystem / picolibc stdio | Use atomic temp-file/rename patterns and restart tests. Keep libfat as baseline until alternatives pass. |
| Read-only bundled resources | NitroFS | Bundle public non-proprietary resources; never attempt writes to NitroFS. |
| DSi SD storage | libnds DSi SD filesystem support | Gate by hardware/mode and test paths on DSi and emulators. |
| DSi front/rear camera | libnds camera APIs | DSi-only; expose disabled controls on original DS. Do not assume full sensor tuning is available. |
| Audio effects/music | libnds simple sound API or Maxmod | Keep optional; avoid consuming ARM7 resources needed by DSWiFi. |
| Animation/frame timing | Nitro Engine / DSMA, libnds timers and VBlank | Keep animation time-step stable; do not busy-wait the main input loop. |
| Large asset loading | BlocksDS filesystem APIs, optional cooperative threads | Measure ARM7/ARM9 behaviour; use asynchronous reads only after concurrency tests. |
| Debugging | libnds exceptions, assertions, debug output; emulator GDB support | Preserve diagnostics in debug builds and strip verbose logs in release builds. |
| Graphics conversion | grit and ptexconv | Add deterministic asset conversion and dimension/palette checks to CI. |
| Memory and performance | libnds DMA, NDMA, cache, NWRAM and memory APIs | Use only when profiling shows a bottleneck; validate cache/DMA alignment and DSi gating. |
| Date/time | libnds RTC APIs | Optional Plaza timestamps; do not use wall-clock as the sole visitor identity. |

## Core SDK libraries and packages

BlocksDS core libraries live under `$(BLOCKSDS)` and are normally installed
with the SDK. The relevant core libraries include:

- **libnds** (`-lnds9`, `-lnds7`): hardware, display, input, filesystem,
  camera, timers, interrupts, FIFO, DMA, memory, math, RTC and debug.
- **DSWiFi** (`-ldswifi9`, `-ldswifi7`): DS/DSi wireless and local multiplayer.
- **Maxmod** (`-lmm9`, `-lmm7`): optional sound/music.
- **LibXM7** (`-lxm79`, `-lxm77`): optional XM/MOD audio.
- **libteak** (`-lteak`): optional DSi DSP support; not required for StreetPass.
- **Grit** and **ptexconv**: asset conversion tools.

Optional third-party libraries live under `$(BLOCKSDSEXT)`. Nitro Engine,
uLibrary, libslim and other optional packages must be installed and linked
according to their own current documentation. Do not assume package paths;
discover them from `wf-pacman -Ql` and pass the actual installation prefix to
the Makefile/CI.

## Integration order

1. **Build/API baseline:** install `blocksds-docs` in CI and compile selected
   official examples for filesystem, 2D graphics, 3D graphics, input, DSWiFi,
   camera and debugging. These checks validate the SDK/toolchain separately
   from the app.
2. **Filesystem audit:** use the official filesystem guide to test DLDI
   flashcart storage, DSi SD, and NitroFS as distinct filesystems. Retain
   libnds/libfat as the working baseline. Evaluate libslim only after a
   BlocksDS-native build and a filesystem adapter pass the same save/queue
   tests.
3. **Rendering:** continue the Nitro Engine/DSMA adapter. Evaluate GL2D or
   uLibrary only for independent 2D screens, never concurrently against the
   same 3D engine state. Load authentic, legally supplied assets only from the
   private asset workspace.
4. **Wireless:** use DSWiFi and the appropriate ARM7 core. Verify both local
   multiplayer peers, packet compatibility, retries, and visitor de-duplication.
5. **DSi-only features:** add camera and SD features behind explicit DSi
   capability checks; original DS remains functional without them.
6. **Performance:** profile first, then consider DMA/NDMA, cooperative threads,
   NWRAM and cache APIs. Add hardware/emulator regression coverage for every
   optimization.

## ARM7 core selection

DSWiFi requires an ARM7 binary built with DSWiFi support. Select the matching
BlocksDS core in the Makefile (for example
`$(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf` if both WiFi and
Maxmod are used). A minimal or Maxmod-only ARM7 core is not interchangeable
with the DSWiFi-enabled core. Verify the actual existing project's ARM7
selection before changing it.

## Acceptance criteria

- Core SDK examples compile using the same BlocksDS package set as the app.
- The application ROM builds without proprietary assets in CI.
- DS mode remains usable without DSi camera/SD-only functions.
- DSi-only camera and SD functionality is capability-gated.
- Nitro Engine room rendering and 2D menu rendering do not contend for the
  single 3D engine.
- Storage migration preserves profile, visitor queue, and duplicate history.
- DSWiFi local exchange is tested on two independent emulator instances and,
  ultimately, hardware; host protocol tests alone do not prove radio exchange.
- Each library adoption is backed by a build/test result, not merely a package
  installation or documentation entry.
