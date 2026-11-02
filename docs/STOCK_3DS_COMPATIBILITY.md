# Unmodified 3DS interoperability feasibility (research only)

## Goal
Determine whether independently developed StreetPass Plaza DS code running
on DS/DSi can exchange valid native StreetPass Mii Plaza encounters with an
**unmodified** 3DS. No working implementation or compatibility claim yet.

## Grounded sources
- https://3dbrew.org/wiki/StreetPass
- https://3dbrew.org/wiki/CECD_Services
- https://3dbrew.org/wiki/NWM_Services
- https://3dbrew.org/wiki/PSPXI%3AEncryptDecryptAes
- https://en-americas-support.nintendo.com/app/answers/detail/a_id/404
- https://github.com/azahar-emu/azahar

3dbrew documents a CECD-managed mailbox, 802.11 discovery/handshake,
WPA2-CCMP encrypted traffic, and a Mii Plaza service ID 00 02 08 00 00.
CECD uses 3DS system crypto support in generating its StreetPass key.
A documented hostapd spoofing experiment elicited 3DS frames, but did NOT
complete association or native message exchange. This is evidence for a
partial recognition experiment, **not** proof of StreetPass compatibility.

## Feasibility gates
1. Observe actual 3DS discovery frames lawfully and passively from
   consenting test equipment without storing identifying addresses in repo.
2. Prove the DS/DSi Wi-Fi driver can implement the required packet types,
   timing and channel behavior. If not, document hardware/driver blocker.
3. Independently validate encrypted handshake compatibility in an isolated
   test harness with synthetic test data; never upload private key material.
4. Construct a legitimate, bounds-checked Plaza message from consensual
   user data; test CECD mailbox semantics separately from radio transport.
5. Only report compatibility after a successful encounter is confirmed in
   the unmodified 3DS's native Plaza inbox, on real hardware.

The existing 80-byte SPDR frames are a distinct DS-to-DS homebrew protocol;
they are **not** the 3DS CECD wire format and cannot be made native by
changing the service ID. A Bark Mode-style closed lid keeps the DS app
running, but does not provide 3DS's OS-level background service.
No SpotPass features are part of this project's scope.

## Current status
Research milestone: protocol requirements identified.
Engineering milestone: stock-3DS encounter exchange is UNIMPLEMENTED.
No claims of success from compiled ROMs or emulator-only host tests.
