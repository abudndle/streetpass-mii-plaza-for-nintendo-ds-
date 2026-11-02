# Full Mii visitor bundles on SD (SPDV v1)

The initial wireless v1 encounter packet is a small nickname advertisement.
It does not include full avatar geometry. This adds a separate **SD-file**
transport for an authenticated-format (CRC-only, unauthenticated-sender)
134-byte Mii Maker DS appearance record. Radio transport is **not yet**
supporting these larger bundles.

## Format: exactly 210 bytes

| Offset | Length | Field |
| --- | ---: | --- |
| 0 | 4 | ASCII `SPDV` |
| 4 | 1 | Version 1 |
| 5 | 3 | Reserved zeros |
| 8 | 64 | SPDS v1 envelope |
| 72 | 134 | Mii Maker DS `MDS3` canonical avatar record |
| 206 | 4 | CRC32 LE of bytes 0–205 |

Decoder validates all three checksums, avatar feature bounds,
sender/associated Mii nickname consistency and the Mii's share permission.
At export, `streetpass-plaza/outbox/visitor.spv` is created if the linked
Mii is marked shareable. Manually copy it to another DS/DSi's
`streetpass-plaza/inbox/` to simulate an exchange.
On successful import, the receiving system writes
`streetpass-plaza/visitors/<sender-id>.mds` and logs the arrival.
It **removes the foreign Personal Mii/gold crown state** and marks the
received Mii as blue-pants status (Mii Maker DS field 61 = 2).
The original Mii Maker DS save and source packet are never modified.

The current Plaza text interface will read and count encounter records.
Full model/skeleton rendering of visitors is a future milestone; a
parsed full avatar is **not** equivalent to a fully ported 3DS model.

CRC checks detect accidental corruption, not deliberate spoofing.
Do not trust visitor bundles from strangers as authenticated identity.
The protocol is independent and does not claim Nintendo 3DS CECD compatibility.
