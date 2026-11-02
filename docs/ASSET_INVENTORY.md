# User-provided base StreetPass Mii Plaza NCCH

The user's *private* decrypted `CTR-N-HMEE` (United States region) base NCCH was inspected locally. This is metadata-only; **no Nintendo content is committed**.

- ExeFS entries found: `.code`, `banner`, `icon`.
- RomFS wrapper: decrypted IVFC; valid directory tree.
- RomFS contains **18 directories and 115 files**.
- Asset categories in the base image:

| Category | Files | Bytes |
| --- | ---: | ---: |
| cec_icon | 5 | 26,240 |
| debug_window | 1 | 1,468 |
| font | 1 | 292,811 |
| graphics | 50 | 8,399,840 |
| message | 12 | 1,068,256 |
| param | 33 | 717,319 |
| shaders | 12 | 28,200 |
| sound | 1 | 3,521,880 |

Private preprocessing also successfully decoded 56 LZ10/LZ11 compressed files: 44 CGFX, six DARC layout archives, one CFNT font, three SMDH icons and two raw icon buffers. These remain only in the private development workspace. See [private pipeline](PRIVATE_ASSET_PIPELINE.md).\n\nThis does not mean they are DS-ready. Archives, shader formats, textures, models, UI layouts, and audio require profiling and *private* platform-specific conversion. DS RAM/VRAM and GPU limits require level-of-detail, animation and texture streaming changes.

To recreate the metadata inventory **on your own computer**:

```sh
python3 tools/romfs_inventory.py "/private/path/to/decrypted.ncch" --summary
python3 tools/romfs_inventory.py "/private/path/to/decrypted.ncch" --output "/private/path/inventory.json"
```

Game updates and Plaza DLC may require separate authorized inputs; the supplied base NCCH alone cannot establish that all DLC games/assets are present. Never commit game archives, extracted Nintendo resources, or converted copies to public GitHub or Actions.
