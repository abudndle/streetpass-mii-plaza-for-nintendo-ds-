# Project scope: StreetPass and StreetPass Mii Plaza DS

This is a Nintendo DS / DSi homebrew StreetPass functionality project and a port of **StreetPass Mii Plaza**. No separate SpotPass client, SpotPass archive import system, or unrelated online-services port is planned.

## In scope

- Faithful StreetPass Mii Plaza DS screens, progression, visitors, and compatible mini-games, adapted for DS/DSi hardware.
- Opt-in, independently implemented DS-to-DS local encounter exchange and Mii Maker DS profile integration.
- Experimental Nintendogs Bark Mode-style closed-lid encounter discovery while the Plaza application or an explicitly cooperating launcher is actively running.
- SD/flashcard storage of configuration, visitors and encounters; no assertion that an SD file alone runs code behind other games.
- Research into 3DS CECD for format understanding and any *verified* interoperability, without asserting stock 3DS compatibility before device testing.

## Out of scope

- SpotPass/BOSS Internet download service, archival SpotPass content importer, or any independent SpotPass project.
- Claiming a background daemon executes in unrelated games without those titles' cooperation.
- Distributing Nintendo ROM files, extracted copyrighted assets, private user saves or keys in the public repository.

SpotPass archive websites shared for research are background references only. Prior exploratory recommendations to implement a separate SpotPass importer are superseded by this scope decision.
