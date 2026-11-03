# DSi camera, SD and import/export integration notes

References inspected:
- https://github.com/Epicpkmn11/dsi-camera
- https://github.com/Epicpkmn11/dsidl
- https://github.com/addisonking/dsi-camera
- https://libnds.devkitpro.org/
- https://blocksds.skylyrac.net/libnds/
- https://blocksds.skylyrac.net/docs/libs/core_libs/

## What these projects contribute

### Epicpkmn11/dsi-camera

This is a focused DSi camera hardware example, not a portable camera library.
Its camera driver demonstrates:
- DSi camera interface clock and module initialization.
- Inner/front and outer/rear camera activation.
- Preview and capture transfer modes.
- NDMA1 transfer from the camera data register to RAM/VRAM.
- Preview RGB555 transfer and full-resolution YUV capture.
- PNG photo saving at 640x480 in `sd:/DCIM/100DSI00/`.

The example's ARM9 driver depends on camera PXI commands handled by an ARM7 companion. The upstream source uses Calico PXI headers; do not copy it into this BlocksDS project unchanged. Port the command transport to the BlocksDS/libnds ARM7/ARM9 FIFO mechanism or the matching BlocksDS camera support, and add explicit capability checks before touching DSi-only registers.

### Addison King dsi-camera

Treat this as a second implementation/reference for DSi camera initialization, capture modes and hardware behaviour. Compare its license, ARM7/ARM9 split, FIFO/PXI protocol and memory-transfer assumptions before adopting any code. Keep one camera abstraction in the game; do not run two camera drivers simultaneously.

### Epicpkmn11/dsidl

This is a complete DSi QR downloader application, not a camera API. Its useful architectural references are:
- camera preview/capture flow shared with the camera demo;
- QR decoding through quirc;
- touch-oriented file browser and keyboard;
- SD file selection and safe output paths;
- Wi-Fi/download state and progress UI.

For StreetPass Plaza, the transferable *features* are QR-assisted Mii/profile import, touch file selection, and SD image import/export. Do not import dsidl's downloader/network stack into the visitor wireless protocol. Its source contains GPL-3.0 code and additional terms; if code is ever reused, comply with its license and notices. Prefer a clean adapter implementation until license compatibility is reviewed.

### libnds

Use the BlocksDS-maintained libnds headers and libraries that match the project's BlocksDS toolchain, not a mixture of unrelated devkitPro binaries. The devkitPro libnds documentation remains a useful API reference for shared APIs, but verify each function/macro against the installed BlocksDS headers and package version.

## Plaza feature mapping

| Feature | Implementation |
| --- | --- |
| DSi-only photo Mii creation | Front/inner camera preview, capture, YUV-to-RGB conversion, user confirmation, then existing Mii conversion pipeline |
| QR Mii import | Rear/outer camera capture, quirc decoder, validate payload/checksum/version, then existing Mii import parser |
| Save visitor/Mii images | DSi SD path via libnds filesystem; use a temporary file and rename after successful write |
| Browse existing Mii files | Touch file picker modelled on dsidl UX, but connected to the Plaza's own profile and Mii file types |
| Original DS fallback | Camera and SD-only actions disabled; retain DLDI storage and manual import path |
| DSi enhanced mode | Enable camera/SD only after checking DSi mode and hardware capability; never infer capability from emulator name alone |

## Implemented camera adapter slice

`source/dsi_camera.c` now uses the official BlocksDS/libnds high-level camera
API (`cameraInit`, `cameraSelect`, `cameraStartTransfer`,
`cameraTransferActive`, `cameraStopTransfer`, `cameraDeinit`) rather than
copying the older standalone project's raw-register/PXI driver. It is gated by
`isDSiMode()`, selects the inner camera, displays a 256x192 RGB preview, and
captures a 640x480 YUV422 frame using NDMA channel 1. The capture is saved
atomically as `camera_last.yuv` under the app's configured save directory.
The Plaza main screen opens this diagnostic capture screen with L+Y and
returns to the Plaza on completion.

This is a camera capture milestone, not yet the photo-to-Mii feature: YUV-to-RGB
conversion, PNG/JPEG output, image-to-Mii candidate generation, QR scanning,
and DSi SD-specific path selection remain to be implemented. Original DS mode
returns without touching camera hardware.

## Required engineering work

1. Audit the current BlocksDS SDK/libnds camera support first. If a supported camera API is present, use it rather than maintaining raw register code.
2. Build a tiny camera probe with separate ARM7 and ARM9 pieces. Confirm camera FIFO/PXI handshake, both camera IDs, preview and capture, and shutdown.
3. Implement a camera adapter with explicit states: unavailable, initializing, ready, previewing, capturing, error. Ensure every exit path stops NDMA and deactivates the camera.
4. Validate capture buffer size and alignment. A 640x480 YUV frame and a 256x192 RGB555 preview have different transfer sizes and formats; do not treat them as interchangeable.
5. Integrate the QR decoder only after camera frames can be captured and converted reliably. Reject malformed, oversized and unsupported payloads.
6. Use SD writes only on DSi-capable storage and verify errors/partial writes. Never overwrite a Mii or visitor save until the new file is validated.
7. Test on DSi hardware and Azahar/AzaharPlus where supported. melonDS DS mode is a useful fallback test but does not prove DSi camera emulation.
8. Keep camera code behind compile/runtime guards so the same ROM works on original DS hardware.

## Acceptance tests

- Original DS boots with camera controls disabled and no camera registers accessed.
- DSi mode can initialize and deactivate both cameras without hanging.
- Preview works without corrupting Nitro Engine state; camera mode and Plaza 3D mode must not run at the same time.
- Full-resolution captures have the expected dimensions/format and save successfully to SD.
- QR import rejects invalid data and imports a valid supported Mii payload without modifying the source file.
- Camera errors, missing SD, and low memory return to the Plaza safely.
- No proprietary camera/Mii assets are placed in public CI artifacts.

This document records researched integration guidance only. The camera driver, QR scanner and SD workflows are not yet implemented in the Plaza ROM.
