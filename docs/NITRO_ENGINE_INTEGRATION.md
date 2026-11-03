# Nitro Engine and DSMA integration

## Decision

The Plaza uses the existing AntonioND libraries instead of introducing a
project-owned 3D renderer:

- Nitro Engine 0.16.x provides the BlocksDS/libnds 3D scene, camera, model,
  material, texture, and rendering lifecycle.
- Nitro Engine includes the DSMA implementation used by its animated-model
  support. DSMA provides the DSM mesh / DSA skeletal-animation format and
  interpolation/blending routines.
- The app's existing visitor roster, 134-byte Mii packets, save format,
  wireless protocol, and selection logic remain the source of truth. The
  renderer consumes visitor presentation data; it does not replace or migrate
  the persistent visitor records.

The upstream GitHub DSMA repository was archived and points to Codeberg. The
Nitro Engine repository remains the maintained integration point and includes
the DSMA implementation under `source/dsma`.

## Build integration

The BlocksDS build links `-lNE` from
`/opt/blocksds/external/nitro-engine/lib`. CI installs the official
`blocksds-nitroengine` package using `wf-pacman` before the ROM build.
Do not vendor a fork of either renderer or write replacement matrix,
skinning, or animation code in this repository.

## Current room adapter

`source/plaza_3d.c` is a narrow adapter around Nitro Engine:

- `NE_Init3D` and the Nitro Engine VBlank/HBlank callbacks own the top-screen
  3D display while a room is open.
- `NE_CameraCreate` / `NE_CameraSet` establish the room view.
- `NE_ModelLoadDSMFAT` loads the DSMA mesh.
- `NE_AnimationLoadFAT` loads the DSA idle animation.
- `NE_ModelSetAnimation`, `NE_ModelAnimStart`, and
  `NE_ModelAnimateAll` advance skeletal animation.
- Three visitor model instances share the same loaded mesh through
  `NE_ModelClone`.
- The bottom-screen roster and controls remain on the existing libnds console.
  Exiting the room releases Nitro Engine and restores the original 2D top
  console.

The adapter activates only when both model files are present. Missing assets
leave the original 2D room fallback available; the application must not
pretend that a model was loaded.

## Runtime asset contract

Place converted, user-authorized model assets on the DS SD card:

```text
fat:/streetpass-plaza/models/visitor.dsm
fat:/streetpass-plaza/models/idle.dsa
```

The expected source workflow is to prepare a low-poly, DS-budget MD5 mesh and
idle animation, then use Nitro Engine's packaged `md5_to_dsma.py` converter.
The DSMA converter requires exactly one weight per vertex and a skeleton that
fits the DS matrix stack (up to 29 bones, often fewer). Texture dimensions
must be declared at conversion time and converted separately with the
supported BlocksDS texture tools. Keep vertex counts, texture memory, palette
use, and per-screen polygon budgets within DS hardware limits.

Example converter invocation (replace paths and texture dimensions with the
actual source asset values):

```sh
md5_to_dsma.py --model visitor.md5mesh --name visitor \
  --output converted --texture 64 64 --anim idle.md5anim
```

Copy `converted/visitor.dsm` to `visitor.dsm` and the generated
`idle.dsa` to the runtime models folder. Do not commit copyrighted Nintendo
models, textures, animations, extracted 3DS game assets, or their converted
derivatives to the public repository or CI artifacts.

## Important remaining work

This is the engine integration milestone, not proof of authentic Mii rendering.
The current adapter displays three instances of one supplied DSM/DSA visitor
asset. It does not yet generate a distinct 3D mesh from each received Mii's
134-byte parameter packet, bind original StreetPass Mii Plaza models, or
reproduce the original 3DS room art. The next asset milestone is to map Mii
parameters to a DSMA-compatible model/texture set while preserving each
visitor's saved identity and appearance. Authentic 3DS assets must remain in
the private asset workspace.
