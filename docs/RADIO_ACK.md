# DS-to-DS encounter delivery acknowledgement (SPDA v1)

This is a cooperative **Nintendo DS/DSi homebrew-only** protocol, not
Nintendo 3DS CECD or authenticated native StreetPass.

* Data uses the current 80-byte `SPDR` envelope.
* A receiver queues an ACK **only after** SPDR CRC/version checks, replay
  and time-window validation, and `spds_storage_receive` returns 0
  (already stored) or 1 (newly stored).
* An ACK is a 24-byte `SPDA` v1 frame with reserved bytes 0,
  original sender ID (uint64 little-endian), original nonce (uint32
  little-endian), and CRC32 of the first 20 bytes.
* A sender accepts a confirmation only when sender ID and nonce match
  the most recently transmitted packet. Duplicate or mismatched ACKs
  do not increase the confirmation count.
* The DSWifi callback only copies one bounded packet. The foreground
  loop transmits queued ACKs no faster than once every 12 frames and
  continues advertising encounters every 180 frames to permit new peers.
  This is a *best-effort* delivery receipt, not guaranteed delivery.
* The user sees `Peer ACKs` as a status counter. This is deliberately
  labelled **unauthenticated** because CRC32 does not protect against
  malicious peers or impersonation.
* Since the existing single-client DSWifi link and host/client election
  are still experimental, this code must be tested on physical DS/DSi
  consoles before claiming actual wireless delivery.

The portable host tests validate CRC, strict frame length/version,
header reserved bytes, matching sender+nonce, and replay rejection.
The existing BlocksDS build remains the only console toolchain.
No Nintendo game assets, CECD saves or console keys are included.
