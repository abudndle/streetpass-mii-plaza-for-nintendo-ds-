# Puzzle Swap — first persistent DS panel milestone

This is an **interim 15-piece panel and gameplay progression slice**, not
a complete reproduction of Nintendo's original Puzzle Swap. Original
artwork, original panel catalog, pink pieces, regional variations and
authentic piece exchanges are still pending authorized private asset
conversion and protocol verification.

## Playing

Press **X** on the Plaza home screen to show the Puzzle Swap preview.
Use the D-pad or touch the 5×3 grid to choose a piece; **A** inspects
whether that piece has been collected, **B** or **X** returns to Plaza.
The starter piece is initially available. The DS-only progression preview
awards one missing piece per newly recorded local encounter in
`encounters.tsv` (bounded to 15 pieces). Duplicate/repeated encounters
do not award extra pieces. These are prototype rewards, **not** authentic
3DS Puzzle Swap piece-exchange semantics.

## Storage and integrity

Persistent file: `streetpass-plaza/puzzle.swp` with **SPZP v1** 20-byte
header (magic, version and reserved bytes, 15-bit owned mask, last
credited encounter count and CRC32). Writes use a temporary file
and rename. Read requires exact length, fixed header, valid mask, and
matching CRC. Missing file initializes an unsaved starter panel.
Corrupted files disable updates: they are **not silently overwritten**.
Piece selection is UI-only and is not persisted.

The implementation is independent portable C in
`source/puzzle_swap.[ch]`, with host tests for navigation, monotonic
encounter credits, 15-piece cap, exact disk round-trip, corruption
detection and no mutation of caller state when loading fails.
BlocksDS remains the DS/DSi ROM build toolchain. No Nintendo ROM,
extracted game data or CECD private files enter public source or CI.

## Combined Plaza UI integration

On the current combined DS build, press X from the inbox to open the visitor roster, then press Y (or tap the Puzzle Swap row) to enter the prototype. B or X returns from the puzzle to the roster. The original inbox Y action (reload Mii Maker DS) is preserved. ACK feedback, visitor browsing, and puzzle progression coexist, but native 3DS StreetPass compatibility and authentic art remain unverified.
