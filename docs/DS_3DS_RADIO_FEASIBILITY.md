# DS/DSi to unmodified 3DS StreetPass: hardware feasibility

Research update 2026-11-01. Scope is **StreetPass Mii Plaza DS** only.

## Current evidence

- https://3dbrew.org/wiki/StreetPass documents native probe request/response
  exchange, Nintendo vendor action frames, CECD mailbox data, and WPA2-CCMP
  data traffic. The reported hostapd test provoked some responses, but did
  not complete an encrypted encounter.
- https://blocksds.skylyrac.net/tutorial/advanced/wifi/ documents DS
  802.11b 1/2Mbps, open/WEP, local NiFi, versus DSi 802.11b/g WPA2.
- https://blocksds.skylyrac.net/docs/changelog/ explains the separate DSi
  driver must be requested with WIFI_ATTEMPT_DSI_MODE; the DSi driver does
  NOT support NiFi, which runs through backward-compatible DS mode.
- https://github.com/azahar-emu/azahar provides CECD emulator service
  references, not real DS/3DS radio interoperability.
- https://en-americas-support.nintendo.com/app/answers/detail/a_id/3697
  describes legacy DS software security limits.

**Important:** Native 3DS StreetPass uses a specialized management-frame
exchange, not a conventional access-point association. WPA2 support on DSi
does NOT by itself establish the custom CCMP keying and frame control needed
for native StreetPass; similarly the existing DS NiFi transport does not
implement the native 3DS protocol.

## Reproducible evidence gates

```sh
python3 tools/assess_3ds_compatibility.py research/stock_3ds_evidence.json
```

The output is deliberately **not verified** unless every evidence gate has
been explicitly demonstrated with an evidence reference. Do not insert
unverified claims or personal console identifiers into public records.

The initial evidence is a roadmap, not a successful handshake or encounter.
Do not emulate encryption from copied console secrets or commit private keys.
On-device tests with consenting consoles are necessary before changing the
claimed interoperability status.

Bark Mode-inspired closed-lid execution remains limited to the *currently
running* DS app or explicitly cooperating homebrew; an SD file alone cannot
run a wireless service behind arbitrary other titles.
