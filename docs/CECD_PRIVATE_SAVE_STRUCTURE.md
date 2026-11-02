# Private CECD DISA nested descriptor verification

The user-provided CECD system save (55 MiB) was examined read-only on
2026-11-01 with a local metadata-only inspector. Original bytes, message
contents, personal Miis and console identifiers remain private.

## Confirmed structure

- Container: DISA version 0x40000; one partition.
- Active partition table: secondary; SHA-256 matches the DISA header.
- Active descriptor: DIFI version 0x10000, length 0x12C.
- IVFC descriptor: version 0x20000, offset 0x44, length 0x78.
- DPFS descriptor: version 0x10000, offset 0xBC, length 0x50.
- Partition master hash area length: 32 bytes.
- Partition length: 57,667,584 bytes.
- IVFC levels (relative offsets / byte sizes): 0/128,
  128/1760, 4096/223456, 229376/28602368.
- DPFS paired-copy levels (offset / each-copy bytes): 0/4,
  8/896, 4096/28831744.
- External IVFC level-four flag: disabled.
- All inspected offset and size bounds are valid.

## Limitations and next step

Outer table SHA and metadata bounds are verified; the DPFS active copies,
the complete IVFC hash chain, inner SAVE filesystem, mailbox contents and
console-bound CMAC are NOT verified. A 55 MiB archive is not evidence of
a complete DS-to-stock-3DS StreetPass implementation. Next: implement a
read-only DPFS selection and IVFC verifier, then bounded SAVE filesystem
metadata inspection with names/counts sanitized.

Format references:
- https://3dbrew.org/wiki/DISA_and_DIFF
- https://github.com/wwylele/3ds-save-tool/blob/master/DISA.md

No raw 3DS NAND, CECD save, cryptographic keys or visitor records were
included in this repository.
