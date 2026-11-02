# Portable SD folder package

Copy the `streetpass-plaza/` directory from this template to the **root**
of the writable flashcart microSD or DSi SD volume, not an emulator ROM directory.
The Plaza app can create the same folder structure on first launch if missing.

Configure `streetpass-plaza/service.cfg` with the true location of your Mii
Maker DS save file (`.sav`, 8192 or 65536 bytes) or a standalone
Mii Maker DS `MDS3` file (134 bytes). It is read-only. The copied
template contains **no original Nintendo assets** and no Mii data.

Important: Files cannot execute background radio code. For background
operation a running launcher or homebrew application must explicitly link
and call this repository's portable service APIs. An unrelated DS game
and stock 3DS StreetPass do not detect these files automatically.

See [portable service documentation](../docs/PORTABLE_SERVICE.md).
