# Full StreetPass / StreetPass Mii Plaza DS port audit

Audit baseline: main branch `53a25d5f781f8dac3e4e674d7a316c01152d6b09` (2026-11-03). This is a code-and-test inventory, not a claim of feature parity. The goal is to port the complete user-visible and gameplay feature set of the original 3DS StreetPass Mii Plaza to DS/DSi, adapting controls, resolution, memory, graphics and audio to DS hardware. No proprietary Nintendo content may enter the public repository or CI artifacts.

## Porting rule

For each original feature, implement the actual state machine, data model, save behavior, UI, assets (only from authorized private inputs), sound and interactions. A placeholder screen, protocol codec, host test, or compilation is not equivalent to a completed port. Every feature must have a linked implementation, regression tests, ROM build, emulator evidence and physical DS/DSi validation where hardware behavior is involved.

## Current code audit

| Area | Existing code | Verified scope today | Remaining work |
|---|---|---|---|
| DS application shell | `source/main.c`, Makefile | BlocksDS ARM9 ROM builds; two text consoles | Actual boot evidence, polished original dual-screen UI, input/focus/navigation |
| Mii Maker integration | `source/mii_bridge.c`, tests | Read-only parsing of selected Mii Maker DS save formats and 134-byte Mii packet | Full gallery handling, robust edge cases, edit/share policy UI, rendering and animation |
| Visitor bundle | `source/visitor.c` | Versioned SD bundle with full Mii appearance and consent bit | Full import recovery, metadata/provenance, safe capacity handling, UI |
| Visitor archive | `source/storage.c`, `source/visitor.c` | Durable profile/encounter storage and validated visitor files | Explicit 3,000-entry cap, bounded directory scans, indexing, sorting, deletion/management UI |
| Arrival queue | `source/storage.c`, `tests/test_queue.c` | Ten-pending-arrival queue and overflow handling | Verify all radio/SD paths, atomic queue/archive consistency, restart and retry edge cases |
| Plaza population browser | `source/plaza_roster.c`, `source/main.c` | Three-entry page model and selection state | 3,000-entry reliable enumeration, pagination on device, Mii sprites, details, sort/search |
| Plaza room | `source/main.c` | Prototype top-screen room with several visitor placeholders and selection | Authentic room layout, simultaneous rendered Miis, idle/walk cycles, conversations, room changes, interactions |
| Walking animation | `source/main.c` | Basic placeholder animation state | Original animation set and deterministic DS-friendly playback |
| Background/stage | `source/background.c` | Small storage polling helper; a static backdrop in UI | Original Plaza backgrounds, transitions, lighting/parallax adapted to DS |
| Arrival notification | `source/arrival_notice.c` | Timed name notification state model | Original arrival sequence, greeting card, profile/region/visit details and sound |
| DS local wireless | `source/wireless.c`, `source/discovery.c`, `source/radio_ack.c` | Experimental DSWifi host/join/discovery and ACK components | Physical two/three-console testing, retries, collisions, peer authentication and robustness |
| Multi-hop relay | `source/relay.c`, `companion/spdr_companion.c` | Versioned packet, hop limit, duplicate cache, durable-before-forward rule | Reliable transmission completion, retry persistence, multi-node physical tests |
| Native stock 3DS | Research/docs only | Compatibility boundaries documented | No verified supported DS-to-unmodified-3DS native StreetPass bridge; do not claim one |
| Bark Mode-like closed-lid search | `source/bark_mode.c` | Opt-in state helper and sleep override experiment | Real DS/DSi closed-lid radio/battery tests; no guarantee of background execution |
| Puzzle Swap | `source/puzzle_swap.c` | Starter piece, bitset progress, selection/movement, persistence, encounter-based awards | Real puzzle panels, piece exchange, completion, art viewer/slideshow, full game rules and multiplayer |
| Find Mii / Find Mii II | None identified | Not implemented | Full adventure, hero recruitment, battle system, magic, equipment/hats, progression and sequel |
| StreetPass Map | None identified | Not implemented | Region metadata, map rendering, visited-region state and progression |
| Music Player | None identified | Not implemented | Unlock rules, playlist, DS audio playback and UI |
| Accomplishments | None identified | Not implemented | Achievement catalog, unlock triggers, save state and presentation |
| Slide Show | None identified | Not implemented | Completed puzzle gallery and image viewing |
| 2013 additional games | None identified | Not implemented | Mii Force, Flower Town, Warrior's Way, Monster Manor |
| 2015 additional games | None identified | Not implemented | Battleground Z, Ultimate Angler, VIP Room, speech-balloon customization |
| 2016 additional games | None identified | Not implemented | Slot-Car Rivals, Ninja Launcher, Market Crashers, Feed Mii, Mii Trek |
| Settings/profile | `source/service_config.c` | Configuration and Mii source selection | Full original settings, privacy/sharing, region, sound, controls and data management |
| Save system | `source/storage.c`, puzzle save | SD/flashcart persistent files | Unified versioned save, migrations, backup/restore, corruption recovery, deletion |
| Original assets | `tools/private_asset_pipeline.py`, inventory docs | Private NCCH metadata inventory and decompression tooling | Per-asset format mapping, legal private conversion, DS VRAM/RAM/audio optimization; DLC/update coverage needs separate authorized sources |
| Emulator evidence | `.github/workflows/melonds-capture.yml` | Screenshot capture workflow exists | Genuine successful ROM boot and public-safe top/bottom captures remain outstanding |
| DSi enhancements | Current hardware docs | SD and DSi-targeted build support | Device-gated camera/extra hardware features only if original feature scope requires them and DS hardware has a safe fallback |

## Original Plaza feature parity checklist

### Core Plaza
- [ ] Original boot/title and first-run flow.
- [ ] Main plaza with multiple simultaneously visible Miis.
- [ ] Authentic arrival sequence and queue presentation.
- [ ] Mii body/head/face/clothing rendering from authorized source assets.
- [ ] Walk, idle, turn, gesture and conversation animations.
- [ ] Greeting, profile, creator, region, visit count and last-visit details.
- [ ] Mii speech bubbles and user-configurable phrases where supported.
- [ ] Plaza population capacity of 3,000, browse/search/manage all entries.
- [ ] Ten-arrival pending wait queue, distinct from the saved population.
- [ ] Mii sharing consent, privacy and duplicate/repeat-visit rules.
- [ ] Plaza room/background variants, transitions and original audio cues.

### Built-in games and progression
- [ ] Puzzle Swap: panel catalogue, piece collection/exchange, completion, 3D-image adaptation and slideshow.
- [ ] Find Mii and Find Mii II: complete adventure logic, encounters, combat, hero party, magic, equipment and unlocks.
- [ ] StreetPass Map: regional collection and map coloring.
- [ ] Music Player and unlockable tracks.
- [ ] Accomplishments, awards and unlock progression.
- [ ] Slide Show/gallery.

### Additional Plaza games and premium/update features
- [ ] Mii Force.
- [ ] Flower Town.
- [ ] Warrior's Way.
- [ ] Monster Manor.
- [ ] Battleground Z.
- [ ] Ultimate Angler.
- [ ] VIP Room and speech-balloon customization.
- [ ] Slot-Car Rivals.
- [ ] Ninja Launcher.
- [ ] Market Crashers.
- [ ] Feed Mii.
- [ ] Mii Trek.

### StreetPass system behavior
- [ ] DS-to-DS/DSi discovery and local wireless encounters.
- [ ] Durable receive, ACK only after durable save, retries and deduplication.
- [ ] Multi-node relay with origin identity and bounded hop count.
- [ ] Persistent 10-visitor wait queue and 3,000-Mii archive, independently enforced.
- [ ] Manual SD bundle import/export and data migration.
- [ ] Mii Maker DS profile/gallery integration.
- [ ] Optional cooperating launcher/closed-lid experiment, accurately constrained.
- [ ] Native unmodified 3DS StreetPass is separate; no unsupported CECD injection or false interoperability claims.

## Milestone order

1. **Correct storage/population foundation:** merge and test the 3,000-Mii capacity PR; remove the 512-entry scan ceiling safely; test 2,999/3,000/3,001, pagination and persistence. Keep the 10-arrival queue independent.
2. **Physical DS/DSi StreetPass transport:** two- and three-console tests, discovery, durable ACK/retry, relay, duplicate protection, overflow behavior.
3. **Core Plaza parity:** replace text prototype with the DS-adapted original Plaza UI, real Mii rendering, room simulation, greetings and profiles.
4. **Puzzle Swap completion:** panel assets, exchange rules, completion, slideshow and save progression.
5. **Find Mii / Find Mii II:** implement complete game state machines and progression.
6. **Remaining base Plaza apps:** Map, Music Player, Accomplishments and slideshow.
7. **Additional games/update content:** implement each compatible game as a separately testable module; determine version/DLC source requirements.
8. **Asset conversion/performance:** private source mapping, texture/model/audio conversion, memory/VRAM budgets and frame-time profiling.
9. **Final compatibility and evidence:** real DS/DSi hardware, melonDS boot/gameplay screenshots, save migration, recovery and regression suite.

## Release gates

A milestone is complete only when code and tests are merged, a native DS ROM builds, host tests pass, actual emulator boot/captures are inspected, and physical hardware checks are recorded for radio/device-specific claims. Never equate code scaffolding with feature parity. Keep proprietary Nintendo assets, keys, decrypted ROMs and private user data out of public GitHub and CI artifacts.

## Current priority defect

The main branch visitor enumeration currently stops after scanning 512 candidate files. This is insufficient for the required 3,000-Mii population. PR #56 attempts to raise the scan limit and enforce a cap but still needs boundary/pagination tests and CI verification. Treat it as a candidate, not a completed implementation.
