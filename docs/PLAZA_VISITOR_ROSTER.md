# Plaza visitor roster — first interactive UI milestone

The Plaza now offers a dedicated **X: View Plaza visitors** screen. It
loads validated, cached Mii Maker DS visitor packets from the existing FAT
storage, not from the private 3DS save. It does not change Mii appearance,
ownership or transfer data.

On the roster screen: UP/DOWN selects, LEFT/RIGHT changes pages (three
visitors each), A reveals cached name and creator, B closes details or
returns to the main inbox. X toggles the roster, START refreshes the
stored visitors. Tapping a listed row selects a visitor; tapping the
lower left/right navigation areas changes pages. Selection is clamped
if a visitor file disappears. Existing L/R manual radio and SELECT radio-off
controls remain available, but are not changed by roster navigation.

The state model lives in `plaza_roster.[ch]` and is host-testable without
Nintendo libraries. `spds_visitor_list_page` extends the existing read-only
validated visitor enumeration with an offset (the old list API is preserved).
`make test` tests empty rosters, page bounds, selection, shrinking storage,
and the file pagination contract. `make` in BlocksDS compiles the ROM.

This is a **functional text-mode visitor browser**, not an original Mii
3D-room renderer or complete StreetPass Mii Plaza gameplay port. Nintendo
assets and private CECD saves are not included.
