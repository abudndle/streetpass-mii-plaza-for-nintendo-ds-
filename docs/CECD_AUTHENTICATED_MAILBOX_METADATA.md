StreetPass Mii Plaza DS — authenticated CECD SAVE metadata milestone

Read-only inspection of the owner's private CECD save identified 35 reachable directory entries including root, 93 file entries, 11 standard InBox___ directories, and 11 standard OutBox__ directories. The public Mii Plaza mailbox identifier 00020800 occurs directly beneath the root CEC directory, with one file entry in its inbox and three in its outbox, including bookkeeping. These are not counts of StreetPass encounters.

The independent host parser at tools/cecd_mailbox_metadata.py requires a callback that rejects unauthenticated IVFC bytes. It never reads file payloads, exports unknown names or emits private Mii/console identifiers. Eight synthetic tests cover paths, cycles, malformed and untrusted data. The complete IVFC tree, CMAC, actual messages, and DS-to-stock-3DS wireless interoperability remain unverified.

ROM compilation remains on BlocksDS and this host parser does not run on DS hardware. No private 3DS save or Nintendo game code was committed.

References: https://3dbrew.org/wiki/CECD_Savegame and https://github.com/wwylele/3ds-save-tool/blob/master/DISA.md
