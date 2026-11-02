# Cooperative background collection API

`source/background.c` exports `spds_background_open` and `spds_background_tick`. A homebrew launcher can compile these with `source/storage.c` and `source/protocol.c`, then call them **inside its own event loop** without displaying Plaza.

```c
SpdsBackground collector;
spds_background_open(&collector, "fat:/streetpass-plaza", local_profile.sender_id);
/* Within a launcher that remains active: */
spds_background_tick(&collector, (uint32_t)time(NULL));
```

This polls the **SD-based inbox**, not wireless hardware. To complete the user's actual goal, implement and test the DSWifi peer transport, then place it in the same cooperative service. While a different game is running, this launcher code cannot remain resident on normal DS/DSi hardware. Closed-lid sleep must be tested on each target and should not be marketed as equivalent to the 3DS operating-system StreetPass service.
