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


## Emulator-assisted test path (added 2026-11-03)

The earlier feasibility note focused on physical retail hardware and should not be read as ruling out emulator-assisted validation.

- **Azahar** is the preferred 3DS emulator candidate. NetPass's own FAQ says emulator compatibility is not guaranteed but identifies Azahar as the only known emulator that can efficiently support NetPass. NetPass needs its required 3DS system/LLE modules and valid user-provided installation/system data.
- **AzaharPlus ZipPass** is a separate experimental file-based StreetPass import/export path. It simulates StreetPass tags from pass ZIP files; it is not proof of live DS radio interoperability.
- **melonDS standalone** supports local multiplayer between multiple emulator instances, but its own FAQ describes local multiplayer as work-in-progress/unstable and latency-sensitive. Use wired low-latency LAN where possible.
- The libretro melonDS DS core's LAN/netplay feature is not equivalent to standalone melonDS local wireless; verify the exact emulator build and mode before testing.
- Azahar's 3DS local wireless/CECD emulation and melonDS's DS local wireless are different emulated protocols. Running both emulators on one host does not automatically make them interoperable.

### Emulator test matrix

1. Boot the DS Plaza ROM in two standalone melonDS instances and verify DS custom host/client packets, queue cap and screenshot evidence.
2. Boot two Azahar instances with legally supplied system files and a user-installed NetPass build; verify emulator-only NetPass/CECD behavior separately.
3. Test Azahar local StreetPass/CECD between two Azahar instances if the build supports the required emulated services.
4. Only test DS↔3DS exchange after implementing an explicit gateway or file adapter. Keep this labelled emulator bridge, not native CECD radio compatibility.
5. Save actual window screenshots and logs for each emulator and label instance, version, mode and test direction. No synthetic UI mockups as evidence.

Emulator success is useful protocol/integration evidence but does not replace physical DS/DSi or retail 3DS tests.

## Passive Mii Plaza discovery classifier milestone

`source/streetpass_probe.c` now recognizes an IEEE 802.11 probe request
carrying the documented `Nintendo_3DS_continuous_scan_000` SSID,
Nintendo vendor OUI/type `00:1f:32:01`, and optionally the advertised
Mii Plaza service identifier `00 02 08 00 00`. The DS monitor mode keeps
only aggregate, anonymous match counts; it deliberately does not store
MAC addresses or console identifiers. The host test synthesizes an
anonymized frame with the documented information-element layout.

This is **pattern recognition, not authentication**. The identifiers are
public and spoofable. A host test cannot prove the DS hardware receives
a 3DS frame, and two emulator windows cannot establish a compatible
emulated radio path. The DS radio handler must be validated with actual
captured packets on consented physical DS/DSi and 3DS equipment before
a successful discovery claim is made. The stock-3DS encrypted CECD
handshake and native Mii Plaza visitor delivery remain unimplemented.

The scan listens on channels 1, 6 and 11 while explicitly opted in.
Do not label probe counts as completed StreetPass encounters, and do
not install or run NetPass as part of this milestone (reference only).
