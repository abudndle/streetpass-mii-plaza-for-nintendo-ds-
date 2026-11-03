# Experimental DS / DSi local wireless

The opt-in DSWifi transport uses a local multiplayer host-and-clients topology. BlocksDS documents support for up to 15 client consoles per host. A host sends CMD frames to all associated clients; each client sends DATA frames to the host. Clients do not communicate directly with one another. The Plaza host is therefore a forwarding hub, not a peer-to-peer mesh.

## Implementation in this branch

- Host mode is configured for up to 15 clients, with fixed 80-byte SPDR relay payload capacity in both directions.
- Host visitor and ACK frames use the DSWifi host CMD broadcast operation, delivered to all connected clients.
- Client-to-host frames use the DSWifi client DATA operation.
- Host receive callback accepts client DATA; client receive callback accepts host CMD frames.
- The relay cache forwards only after the visitor has been durably stored. Host broadcast plus per-node duplicate suppression and hop budget enables multi-hop propagation through multiple host groups, subject to scheduling.
- This is not native 3DS StreetPass/CECD and does not make DS software execute as a background service.

## Physical test plan

1. Build latest ROM and test one host plus one client.
2. Test one host plus three clients simultaneously. Each client should receive the same consented synthetic visitor and store it once.
3. Test up to 15 clients only after the three-client test is stable.
4. Test two host groups linked by a client that alternates host/client roles; verify bounded multi-hop delivery, duplicates, ACK behavior, queue persistence and reconnection.
5. Record console models, DSWifi version, app commit, client counts and redacted logs. Host tests and compilation do not prove physical radio operation.

## Limits

- DSWifi local multiplayer supports up to 15 clients on one host. Clients communicate with the host, not directly with other clients.
- A DS hub can relay only while the Plaza application is active and its radio is running.
- Larger deployments require multiple host groups and store-and-forward between groups.
- DS/DSi transport remains experimental until verified on real consoles.
- Unmodified 3DS systems use native StreetPass separately. No DS-to-stock-3DS native bridge is implemented or verified.

## References

- [BlocksDS DSWifi local multiplayer guide](https://blocksds.skylyrac.net/dswifi/md_documentation_2local__multiplayer__mode.html)
- [BlocksDS DSWifi multiplayer API](https://blocksds.skylyrac.net/dswifi/group__dswifi9__mp.html)
