# Plaza visitor roster — population and browsing

The Plaza population is a separate persistent collection from the incoming StreetPass wait queue.

- **Plaza population capacity: 3,000 Miis maximum.** The visitor browser must be able to display and page through the full saved population, not just the most recent three entries.
- **Incoming wait queue: 10 visitors maximum.** This is the pending visitor limit before the player enters/clears the Plaza queue. Clearing the wait queue must not erase the saved 3,000-Mii Plaza population or encounter history.
- The roster UI currently renders three visitor entries per page; pagination must cover all saved entries up to the 3,000-Mii capacity.

The current roster state model is host-testable. The existing visitor browser is a functional text-mode prototype, not the original 3D Plaza renderer. Capacity enforcement, 3,000-entry enumeration/pagination, persistence and performance testing remain implementation/verification work; the current code must not be described as already proven to support 3,000 entries.

Nintendo proprietary assets and private CECD saves are not included.