# DS/DSi port architecture and library integration

## Porting principle

This repository is an independent DS/DSi implementation of StreetPass-style
Plaza systems. It must not claim to be Nintendo's original executable or to
contain proprietary 3DS assets. Preserve tested game/data logic and replace
hardware-specific 3DS services with DS/DSi adapters.

## Rendering and animation

- **BlocksDS/libnds** owns hardware setup, input, filesystem, timers, DMA and
  the two 2D engines.
- **Nitro Engine** owns the DS 3D engine for Plaza scenes and model rendering.
  It is not a standalone replacement for libnds.
- **DSMA** supplies the skeletal animation model/animation formats (DSM/DSA)
  used by Nitro Engine's animated-model support. Use the DSMA converter from
  MD5 mesh/animation source, respecting its skeleton limitations (at most
  about 29 bones, often fewer) and one full-weight bone per vertex.
- **NightFox's Lib** is a candidate for native 2D backgrounds, sprites and
  text. It can coexist with Nitro Engine because it targets the DS 2D engines;
  coordinate VRAM banks and interrupt handlers carefully. Integrate a small
  screen adapter before migrating all menus.
- **uLibrary** is a separate 3D/graphics option. Do not initialize it in the
  same scene as Nitro Engine until an explicit interop test proves shared
  3D-engine ownership is safe. Prefer Nitro Engine for the 3D Plaza and use
  libnds/NightFox for 2D UI.
- **libslim** is not a drop-in replacement for the current libnds/libfat
  storage. Keep the existing filesystem adapter until a BlocksDS-native
  libslim build passes the same persistence, queue, corruption and restart
  tests.

## Implemented first slice

The optional Plaza room renderer is in `source/plaza_3d.c`. It loads
user-supplied `models/visitor.dsm` and `models/idle.dsa` from the configured
save directory, initializes Nitro Engine only when both assets exist, creates
three visitor model instances, loops the idle animation, and restores the 2D
display when the user exits. If the assets are absent, the normal 2D UI
continues to work.

This is a renderer integration slice, not yet a complete Mii-to-model
conversion pipeline. The current model instances use the same supplied model;
per-visitor face, hair, clothes, color and body mapping still needs to be
implemented.

## Required conversion pipeline

1. Obtain authorized Mii/model source data in a private workspace.
2. Convert model mesh and skeleton source into DSMA DSM/DSA with the upstream
   `md5_to_dsma.py` converter.
3. Convert textures with Grit or ptexconv to DS-compatible formats.
4. Validate dimensions, palette/texture memory and skeleton bone count.
5. Keep proprietary source and converted Nintendo-derived assets out of the
   public repository and public CI artifacts.
6. Install test assets locally under
   `streetpass-plaza/models/visitor.dsm` and
   `streetpass-plaza/models/idle.dsa`.

## Remaining library-backed port work

| Subsystem | Library | Work remaining |
| --- | --- | --- |
| 2D UI | libnds / NightFox's Lib | Replace console text screens incrementally with two-screen background/sprite UI. |
| 3D Plaza | Nitro Engine | Load real per-visitor models, camera movement, room props and transitions. |
| Skeletal animation | DSMA via Nitro Engine | Convert/validate idle, walk, greeting and interaction animations. |
| Audio | Maxmod | Add ARM9 playback calls, sound bank conversion and event mapping. |
| Wireless | DSWiFi | Preserve DS local protocol; test stock 3DS interoperability separately. |
| Storage | libnds filesystem | Add robust save migration and DSi SD path selection. Evaluate libslim separately. |
| DSi camera | libnds camera APIs | Capability-gated camera features; DS fallback must remain. |
| Assets | Grit / ptexconv | Reproducible conversion, palette/size checks and private asset pipeline. |
| Performance | libnds timers/DMA/cache | Profile first; optimize measured bottlenecks only. |

## Build requirements

The ROM workflow installs `blocksds-nitroengine` and links `-lNE`. The
package provides the Nitro Engine integration built for BlocksDS; DSMA support
is part of Nitro Engine's animated-model path, not an unrelated renderer to
initialize alongside it. The ARM7 core continues to use the DSWiFi + Maxmod
variant.
