# Native 3DS StreetPass hardware experiment worksheet

This worksheet is for a **stock, unmodified 3DS** and a DS/DSi running
the homebrew Plaza. It is not a claim of compatibility.

## Preparation

Use personally owned, consenting consoles. Record hardware model, firmware,
DS/DSi mode, wireless driver version, and app build commit. Do not publish
MAC addresses, console identifiers, keys, raw private encounter data, or
identifying visitor records. Keep original captures offline.

## Experiments (in order)

1. **Baseline:** With the stock 3DS Plaza configured for StreetPass,
   observe the encounter process using a lawful independent wireless
   monitor. Record only redacted frame type, channel, timing and counts.
   Note that a passive capture alone is not DS radio support.
2. **DS receive:** Determine whether the DS driver can report the
   necessary management/action frames in its supported mode. Record
   exact API, return codes and a reproducible trace. A scan listing
   an access point is not sufficient.
3. **DS transmit:** On the same hardware, test whether the driver can
   transmit the necessary frame types with required addressing and
   payload control. Do not attempt to impersonate another console or
   reuse captured secrets.
4. **Protocol:** Only if both frame directions are demonstrated, attempt
   the authorized native discovery and CECD handshake. Separately
   record encryption/key-exchange feasibility.
5. **Visitor arrival:** Only after a verified handshake and secure
   exchange, test a consented visitor arriving in the stock Plaza and
   verify the reverse path. Check duplicate and sharing behavior.
6. **Multi-hop:** Test DS -> stock 3DS -> DS and the reverse. Do not
   assume stock 3DS redistributes a received visitor.

## Report schema

For each gate in `research/stock_3ds_evidence.json`, use
`unknown`, `blocked` or `demonstrated`. Every demonstrated
gate must include a reproducible evidence reference. A source-code
inspection, emulator result or generic Wi-Fi association is not a
successful native 3DS StreetPass encounter.

Use:

```sh
python3 tools/audit_ds_radio.py source/wireless.c --require-native
python3 tools/assess_3ds_compatibility.py research/stock_3ds_evidence.json --require-verified
```

Both commands are expected to fail until native support is demonstrated.
Do not alter evidence status merely to make CI green.

## Stop conditions

If DS hardware or drivers cannot provide required management-frame
control, record the specific capability gap and stop claiming native
compatibility. If native encryption or the stock 3DS's visitor
forwarding semantics prevent interoperability, document those limits.
A 3DS homebrew companion is an alternative architecture, **not**
a substitute for the user's requirement that the 3DS remain unmodified.
