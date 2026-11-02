# SpotPass archive research and DS implementation boundaries

Reviewed November 1, 2026.

## Sources
- https://spotpassarchive.github.io/
- https://www.3dbrew.org/wiki/SpotPass (access returned 403 during review; do not treat the page as directly verified)
- https://en.wikipedia.org/wiki/SpotPass_and_StreetPass
- https://github.com/SpotPassArchive
- https://github.com/SpotPassArchive/SpecialMiiDumper
- https://github.com/SpotPassArchive/SpotPassDumper11
- https://github.com/SpotPassArchive/taskfinder
- https://github.com/SpotPassArchive/archival-tools

## Findings

SpotPass was an Internet background content-delivery feature for 3DS and Wii U,
not the local CECD/StreetPass encounter transport. Nintendo's online services
were discontinued on April 8, 2024. The archival project says it gathered
roughly 4 TB of material, drawing on tens of thousands of user-submitted BOSS
save dumps. It identifies StreetPass Mii Plaza's **special Miis and Puzzle
Swap puzzle pieces** as examples of SpotPass-delivered content.

The archive's own FAQ warns that each game's content format/injection process
differs; there is no generic method for putting this content on a 3DS. The
SpotPassArchive GitHub organization exposes distinct tools for task discovery,
cache dumping, and special-Mii data extraction. Their existence does not
establish that downloads or import formats are interchangeable.

## Implementation boundaries

- Preserve existing **SPDR** local DS-to-DS radio protocol and opt-in
  Nintendogs-inspired lid-closed search. It is distinct from SpotPass/BOSS.
- Treat optional **offline SpotPass archive imports** as a separate subsystem
  reading user-provided local data. Start with a bounded read-only inventory,
  format detection, hash manifest, and deterministic synthetic tests.
- Keep a content-type allowlist (special Mii or puzzle metadata only after
  schema is validated), reject path traversal, cap sizes and archive expansion,
  and never execute imported data.
- The archive includes material potentially copyrighted by Nintendo. Do not
  upload original SpotPass payloads, original Mii assets, ROMs, keys, or user
  save dumps into this public repository or public CI artifacts.
- Consent, license checks, and attribution matter for redistributing data.
  Availability from a preservation project is not redistribution permission.
- Do not imply that installing an SD folder enables native 3DS StreetPass,
  that a DS homebrew app can run behind unrelated games, or that stock 3DS
  firmware can accept homemade DS-local packets.

## Next concrete milestones

1. Build safe metadata-only SpotPass/BOSS inventory tests using synthetic data.
2. Map verified Mii Plaza special-Mii and puzzle formats before attempting any
   format conversion; keep content provenance explicit.
3. Verify DS-to-DS encounter exchange and opt-in lid behavior on physical
   hardware separately from imported archive data.
