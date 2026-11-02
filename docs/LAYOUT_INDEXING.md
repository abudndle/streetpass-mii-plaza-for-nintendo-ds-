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

## Validated private NCCH layout inventory

The six archives from the user-supplied base `CTR-N-HMEE` NCCH were
indexed offline. Together they contain **412 original resource members**:

| Archive | Original layout (.bclyt) | Animation (.bclan) | Image (.bclim) | Total |
| --- | ---: | ---: | ---: | ---: |
| 3dtest | 2 | 0 | 2 | 4 |
| MEETCommon | 15 | 51 | 45 | 111 |
| game | 19 | 31 | 25 | 75 |
| main | 22 | 98 | 58 | 178 |
| map | 3 | 10 | 13 | 26 |
| stay | 4 | 8 | 6 | 18 |
| **Total** | **65** | **198** | **149** | **412** |

This is only an inventory, **not** evidence of renderer/animation support
or gameplay parity. Original model and visual binary payloads are retained
solely in the private local workspace and never shipped in public CI.
