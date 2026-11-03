# Virtual StreetPass test lab

## Scope
Only StreetPass/CECD and the DS Plaza's StreetPass relay are in scope. NetPass service testing and unrelated gameplay testing are excluded.

## Emulator roles
- **AzaharPlus**: 3DS emulator. Its project documents a built-in system-file downloader from official servers, LLE module support and experimental ZipPass. Use File > Download System Files, enable the required LLE modules, and ensure StreetPass is enabled in the target title.
- **melonDS standalone**: DS/DSi emulator for testing the DS Plaza ROM's local wireless/relay behavior with multiple instances.

## Workflow
Run the manual GitHub Actions workflow **Virtual StreetPass emulator lab**. It downloads the latest AzaharPlus Linux AppImage and the official melonDS 1.1 Linux AppImage, launches each in a separate Xvfb display, and captures genuine startup windows.

The initial workflow is a **tool installation and startup capture check only**. It does not claim a StreetPass exchange has occurred. The follow-up interactive test procedure is:
1. Open AzaharPlus and use its official File-menu system-file download feature.
2. Enable required LLE modules and reboot the emulator if requested.
3. Configure two isolated AzaharPlus user profiles/instances and a legally usable StreetPass-enabled 3DS test title. Exchange test StreetPass data between the two emulated 3DS instances. Capture before/after CECD or in-title inbox evidence.
4. Configure two isolated melonDS instances with the DS Plaza ROM and the supported DS local wireless mode. Exercise only Plaza visitor exchange, ACK/retry, duplicate handling and the 10-entry wait queue. Capture both emulator windows and redacted logs.
5. Report AzaharPlus↔AzaharPlus and melonDS↔melonDS independently. They are separate emulated radio stacks; launching both on one PC does not prove cross-family radio interoperability.

## Evidence rules
- A startup screenshot is not a StreetPass pass.
- ZipPass is an experimental file-based tag import/export feature; label it file-mediated, not live radio.
- Do not claim DS↔3DS interoperability unless a specific adapter/gateway is exercised in both directions.
- Never upload system files, firmware, encryption keys, private saves, personal Mii data or copyrighted game images as workflow artifacts.
- Keep screenshots free of account names, console identifiers and personal visitor information.

## Two-instance melonDS boot milestone

The `Two-instance melonDS StreetPass lab` GitHub Actions workflow downloads a validated Plaza ROM and launches **two simultaneously running standalone melonDS processes**, each with a separate `HOME` and `XDG_CONFIG_HOME`. It captures real visible windows, sanitizes PNG metadata, verifies the PNGs and uploads a seven-day artifact. It runs after successful non-scheduled ROM validation and can be manually dispatched. It must not be mistaken for a successful DS Wi-Fi exchange.

**Next radio acceptance gate:** Enable radio explicitly in both test profiles, establish host and client connections, record send and durable ACK on both endpoints, check visitor persistence after reboot, duplicate suppression and the ten-pending-arrival limit. Only accept a result when logs and both emulator displays show the same consenting test encounter. An emulator build that cannot simulate the DS networking mode is a blocked test, not proof of incompatibility with hardware.

NetPass is documentation/reference only and is **not installed or run** as part of this milestone.
