# Private authenticated asset preparation

This repository includes **tooling only**. The user provided a decrypted base StreetPass Mii Plaza NCCH (`CTR-N-HMEE`) for local development. The original binary and all extracted content are excluded from public GitHub and CI.

Verified local study of that input:
- 115 RomFS files, 18 directories.
- 56 files use Nintendo LZ10/LZ11 compression. All 56 were successfully decompressed in the private workspace.
- Decompressed signatures: **44 CGFX** graphics resources, **6 DARC** layout archives, **1 CFNT** font, **3 SMDH** icons, and **2** raw icon data files.
- This demonstrates source-asset inventory and decompression, **not** DS-compatible CGFX geometry, polygon limit conversion, texture import, animation playback or menu reconstruction.

Example private import:

```sh
python3 tools/private_asset_pipeline.py "/private/path/to/title.0.ncch" --out "/private/workspace/plaza-original" --decompress
```

Keep the output outside the public working tree, or in the gitignored `private/` folder. It must never appear in Actions or release artifacts.

## Next conversion steps

1. Parse DARC members and CGFX structures; catalog renderable meshes, textures, skeletons, and animation tracks with original resource names and offsets.
2. Convert model geometry *from the actual user's original data* into DS fixed-function compatible assets with documented reductions, not replacement art.
3. Preserve the original character identities, appearance, path/behavior logic, game state and animations to the extent physically possible.
4. Implement DS-specific memory streaming and VRAM budgets, and validate every scene on melonDS and actual DS/DSi.
5. Handle each Plaza update/DLC input separately. Do not pretend the base NCCH contains assets from unprovided DLC.

The user retains their Nintendo game resources; no extracted game assets are checked into this public repository.
