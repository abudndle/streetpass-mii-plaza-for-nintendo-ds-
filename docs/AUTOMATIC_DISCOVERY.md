# Automatic encounter discovery when homebrew is running

In addition to manual **L** host / **R** join controls, the Plaza now has
an **opt-in** automatic local wireless mode. Enable it by editing
`streetpass-plaza/service.cfg`:

```ini
enabled=1
radio_enabled=1
mii_source=fat:/roms/mii-ds.sav
```

On startup, the application checks the `radio_enabled` setting and
alternates DS local multiplayer host/join roles at per-console intervals
(13–29 seconds) so two consoles running the **same homebrew** can attempt
to discover one another without manual L/R selection. Press **SELECT**
to shut the radio off; L/R choose manual modes. This is experimental
and remains **unverified on paired physical consoles**.

A DS or DSi does not execute background radio code when a storage file
is merely present, when a different ROM has replaced the program, or
when libnds suspends execution after the lid is closed. A separately
integrated cooperative homebrew launcher can use the same
`discovery.[ch]` and `wireless.[ch]` logic. No ordinary R4,
TWiLight Menu++ or other menu requires an internal hard-coded path to
access the folder, but to *run* networking a menu must explicitly load
the library (stock menus do not).

A modded 3DS can run the **DS-mode NDS homebrew** through TWiLight Menu++,
offering a potential DS-to-3DS-in-DS-mode link; this is **not** 3DS native
StreetPass, and no real hardware compatibility claim is made until tests.
For native 3DS-mode encounters, a separate 3DS companion bridge remains
necessary.

**Do not mistake this feature for unattended 3DS-style lid-closed
StreetPass.** BlocksDS/libnds defaults to sleep after the lid closes;
automatic RF transfer during that state is not established.
