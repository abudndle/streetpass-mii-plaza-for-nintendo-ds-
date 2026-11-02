# Experimental Nintendo DS local open relay

This introduces **SPDR v1**, an independent 80-byte packet carrying the
already-tested 64-byte SPDS v1 encounter, a version/hop counter (0–2),
origination timestamp and CRC32. Two forward hops is the maximum, with a
16-entry seen-ID/nonce cache and 120-second deduplication window.

This is not official Nintendo StreetPass, CECD, Nintendo Zone, or a working
bridge to an unmodified 3DS. It cannot deliver to the stock Mii Plaza.
It forwards only a pseudonym, not a full Mii file. Packet CRC detects
corruption, not malicious spoofing. The relay engine and its tests are
portable C; the on-air forwarding scheduler still requires a verified
DSWifi frame transport, queue pacing and paired console tests.

Nintendogs Bark Mode demonstrates DS wireless discovery with the lid
closed while the **same application** is still running. The relay would
likewise require Plaza or a cooperating active homebrew menu; the SD file
alone never executes in other games or firmware.
