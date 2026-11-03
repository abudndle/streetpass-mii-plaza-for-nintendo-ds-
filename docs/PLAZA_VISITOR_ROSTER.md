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


## Ten-visitor StreetPass wait queue requirement

The Plaza's **pending arrival queue is limited to ten visitors**, matching the requested StreetPass wait-limit behavior. This is distinct from the long-term visitor archive and from the eight-hour per-sender duplicate cooldown.

- Accept arrivals until ten pending visitor slots are occupied.
- When the tenth slot is filled, stop accepting/acknowledging additional radio arrivals and leave SD inbox imports queued for later. Do not silently discard overflow visitors.
- Show the live waiting count as `n/10` and indicate when the queue is full.
- The player must enter/open the Plaza room to clear the pending wait queue. Clearing pending status must not delete the saved visitor Mii archive or reset duplicate-protection history.
- Once the pending queue is cleared, accept the next batch, again up to ten visitors.
- The cap applies to the shared DS/DSi relay wait queue regardless of whether the arrival came from local radio or manual SD import.
- Native 3DS StreetPass remains a separate Nintendo-managed queue. The DS project's ten-slot queue does not change or control a retail 3DS's own queue.

Implementation and regression tests must prove: 0→10 accepted, 11th held without ACK/deletion, clear→new ten accepted, duplicate still suppressed during cooldown, and archive retained after clearing.
