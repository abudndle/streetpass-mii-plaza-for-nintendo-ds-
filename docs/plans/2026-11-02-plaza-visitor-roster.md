# Plaza Visitor Roster Implementation Plan

> **For agentic workers:** Use the host's available task-by-task implementation workflow. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Let a DS/DSi player browse all cached StreetPass Mii Plaza visitors, select them and view their names/creator.

**Architecture:** A host-testable roster state machine controls selection and paging. The existing FAT Mii cache exposes paged enumeration while the BlocksDS UI renders three visitors per screen.

**Tech Stack:** C11, BlocksDS/libnds, DLDI FAT, GitHub Actions.

## Global Constraints

Preserve existing radio, storage, and Mii data; never commit Nintendo binaries, private CECD saves or proprietary assets; do not claim stock-3DS compatibility.

---

### Task 1: Host-tested roster navigation
**Files:** Create `source/plaza_roster.[ch]`, `tests/test_plaza_roster.c`.
**Interfaces:** init, count update, selected page, page/slot/step changes, detail toggle.
- [x] Write a test for empty, seven-item, last-page, and shrinking rosters.
- [x] Observe unresolved-link failure before implementing state methods.
- [x] Implement clamp, navigation and detail state in portable C.
- [x] Run the focused C test.

### Task 2: Backward-compatible page reads
**Files:** Modify `source/visitor.[ch]`, `tests/test_visitor.c`.
**Interfaces:** `spds_visitor_list_page(base,offset,out,capacity)` returns total count and writes only requested entries.
- [x] Check empty/off-end page, argument rejection and first page in existing visitor test.
- [x] Implement bounded FAT directory enumeration and preserve `spds_visitor_list`.
- [ ] Verify via `make test` on GitHub CI.

### Task 3: Two-screen BlocksDS UI
**Files:** Modify `source/main.c`, add `docs/PLAZA_VISITOR_ROSTER.md`.
**Interfaces:** X roster switch, A info, B back, UP/DOWN selection, LEFT/RIGHT and touch paging.
- [x] Integrate roster model and cached visitor enumeration.
- [ ] Run host suite and BlocksDS ROM build through GitHub Actions.
- [ ] Review compiled ROM in melonDS when emulator is available; do not claim a verified boot without screenshot/logs.

**Unresolved:** Original 3DS appearance and real-hardware radio remain dependent on private authorized assets and emulator/console testing.
