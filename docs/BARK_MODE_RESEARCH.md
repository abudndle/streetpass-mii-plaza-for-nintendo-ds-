# Nintendogs Bark Mode and DS closed-lid wireless research

## Corrected feasibility finding

The original Nintendo DS Nintendogs instruction manual specifically says
that, **after entering Bark Mode**, the DS continues searching wirelessly
when the lid closes, consuming additional battery power. Compatible DS
systems in the same mode exchange a visitor and store encounter information.
This **demonstrates closed-lid radio exchange is possible in a cooperating
DS game**, contrary to the earlier overly broad assumption that closing
the lid must always suspend all software and radios.

Sources:
- [Nintendo Nintendogs instructions, pp. 32–33](https://manuals.plus/m/da3a185b16ce0ca1426eda041c0957e41775deb998469fd2779e55955be4b8df.pdf)
- [BlocksDS libnds system API: disableSleep()/enableSleep()](https://blocksds.skylyrac.net/libnds/system_8h.html)
- [BlocksDS input tutorial: libnds lid sleep is software-controlled](https://blocksds.skylyrac.net/tutorial/basic/input/)

BlocksDS provides `disableSleep()` and `enableSleep()`. This project now
experiments with the former **only when radio_enabled=1 AND
lid_search_enabled=1**, while the Plaza process is running. It tracks
`KEY_LID` and continues polling DSWifi in the same event loop when closed.
After exiting the app, the normal libnds sleep policy is restored.

**Important:** Disabling automatic sleep is *not* equivalent to power-efficient
Nintendogs Bark Mode, nor to 3DS system StreetPass. Without real
hardware measurement, the display/backlight power cost, ARM7 activity,
Wi-Fi reliability, battery drain and thermal behavior are unknown.
Run this experiment with adult supervision, sufficient battery charge and
the ability to turn it off; no software asserts a verified hardware result.

## Replication plan

1. Run the same build on two physical DS/DS Lite/DSi consoles with FAT
   available, first awake with manual host/join.
2. Once awake radio transfer is verified, opt into `radio_enabled=1`
   and `lid_search_enabled=1` on **both** systems.
3. Close the lids while Plaza remains the active application. Measure
   power, RF throughput, encounter integrity and wake state separately.
4. Re-open lids, confirm saved encounters and duplicate filtering. Repeat
   with other DS models/flashcarts, then compare power and reliability.
5. Test a stock 3DS in **DS-compatible game mode** separately. This is not
   stock 3DS StreetPass Mii Plaza, which Nintendo restricts to 3DS software.

Nintendogs Bark Mode does not grant arbitrary homebrew background execution
over another DS ROM. A writable SD/R4 file remains inert until a running
cooperating app uses it. Nor does it establish official CECD/StreetPass
protocol compatibility on an unmodified 3DS.
