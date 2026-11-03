# Stock Nintendo 3DS StreetPass feasibility gate

## Required outcome

The DS/DSi Plaza should exchange visitors with an **unmodified retail
Nintendo 3DS** running the original StreetPass Mii Plaza, with no 3DS
homebrew companion. This is a **research target**, not a working feature.

## Gate sequence and evidence

1. **Discovery capture:** Record a repeatable, consented over-the-air
   observation of the unmodified 3DS advertising or discovering StreetPass.
   Record 3DS model, firmware, DS model, radio adapter and test conditions.
2. **DS radio frame control:** Demonstrate that the actual DS/DSi wireless
   chipset and available driver can send and receive the necessary frame
   types. Document captures and exact driver capabilities. A generic Wi-Fi
   scan is insufficient.
3. **CECD handshake:** Demonstrate a successful protocol handshake with the
   stock 3DS; distinguish it from merely observing beacons.
4. **Encrypted exchange:** Demonstrate interoperable secure exchange using
   legitimately available keys/material. Do not embed console-specific
   secrets or private captures in public source or CI.
5. **Native Plaza visitor:** Show a real visitor arriving in the unmodified
   3DS Plaza and a return encounter arriving on DS, with original identity,
   sharing permissions and duplicate behavior verified.

Every gate needs reproducible hardware evidence. Use
`python3 tools/assess_3ds_compatibility.py research/stock_3ds_evidence.json
--require-verified` as the fail-closed check. **Do not mark any gate
demonstrated based only on source code, host tests, or emulators.**

## Multi-hop acceptance criteria

A DS-originated visitor should reach DS -> stock 3DS -> DS -> stock 3DS,
and the reverse, retaining original identity and consent. However, stock
StreetPass may not automatically rebroadcast visitors it has received;
this behavior must be established experimentally rather than assumed.
If the stock system does not forward visitors, document that limitation
and avoid claiming end-to-end support.

## Current status

No physical 3DS captures, stock CECD handshake, encrypted exchange or
native Plaza arrival have been verified in this repository. The
homebrew SPDR codec and four-hop forwarding are separate and **do not**
prove compatibility with unmodified Nintendo 3DS systems. If the DS
chipset cannot provide necessary frame control, native compatibility
may be infeasible on this hardware.
