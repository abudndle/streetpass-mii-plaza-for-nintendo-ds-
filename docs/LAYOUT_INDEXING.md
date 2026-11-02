# Original StreetPass Mii Plaza layout indexing (private workspace)

The user's authorized decrypted NCCH contains six Nintendo DARC layout
containers under the private RomFS graphics/layout directory:
`3dtest`, `map`, `MEETCommon`, `game`, `main`, and `stay`.

These are not DS-ready resources; `DARC` is a filesystem-like archive.
The public `tools/index_darc.py` tool validates DARC headers, directory
ranges, file offsets and UTF-16LE member paths, and reports member names
and extension counts only. It **does not** embed Nintendo-owned bytes.

After running the private NCCH extraction / LZ11 pipeline:

```sh
python3 tools/index_darc.py "/private/workspace/decompressed-romfs/graphics/layout/main.bin" --summary
python3 tools/index_darc.py "/private/workspace/decompressed-romfs/graphics/layout/main.bin" --output "/private/workspace/main-layout-index.json"
```

Use this to map original `.bclyt` layout nodes, `.bclan` animation tracks
and related textures into the DS UI without fabricating replacement screens.
Parsing or listing these members does **not** mean those animations or scenes
have been compiled into the DS ROM. The next milestone is parsing the
referenced layout pane/animation commands and bringing converted, permitted
private assets into a reproducible local build, subject to DS VRAM limits.
