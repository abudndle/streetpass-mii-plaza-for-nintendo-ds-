# CECD Plaza mailbox header milestone (sanitized)

Private read-only verification against the owner's previously uploaded 55 MiB
CECD DISA save. The Plaza mailbox beneath CEC/00020800 has a BoxInfo_____
inbox with **0 messages** and a BoxInfo_____ outbox with **1 message**.
Both use the documented 0x6262 magic. The outgoing message's first 0x70 bytes
use the documented 0x6060 magic and declare: total file 32,712 bytes;
header and extra headers 3,368 bytes; body 29,312 bytes; trailer 32 bytes.
These lengths are consistent. Only 0x20-byte bookkeeping headers and
0x70-byte message headers were examined; no message bodies were extracted.

Source: https://3dbrew.org/wiki/CECD_Savegame
SAVE file entry/FAT: https://github.com/wwylele/3ds-save-tool/blob/master/DISA.md

`tools/cecd_plaza_headers.py` requires a trusted IVFC-authenticated SAVE
byte-range reader. It rejects incorrect signatures, inconsistent lengths,
missing FAT start flags, unexpected counts, cyclic paths, and unverified byte
ranges. Its output contains **only aggregate counts**, no sender IDs, Miis,
message IDs, or content. Host synthetic tests run under `make test`.
The private DISA file and private authenticated-reader adapter are NOT
committed.

**Interpretation:** The outbox contains one locally stored, outbound
StreetPass message; the inbox currently reports zero. This does not mean
a native DS-to-unmodified-3DS encounter has occurred. Only selected IVFC
blocks have been authenticated; full CMAC / filesystem integrity and actual
DS-to-stock-3DS radio interoperability remain unverified.

BlocksDS remains the toolchain for the .nds application. This is an
offline research tool, not an added console runtime dependency.
