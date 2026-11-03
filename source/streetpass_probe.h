#ifndef SPDS_3DS_PROBE_H
#define SPDS_3DS_PROBE_H
#include <stddef.h>
#include <stdint.h>
/* Passive classifier only. No Nintendo identifiers or MAC addresses retained. */
#define SPDS_3DS_PROBE_MAX_FRAME 512u
/* Accepts plain 802.11 MAC frames, not radiotap-prefixed captures.
 * Return 1 only for a probe request with the exact StreetPass SSID
 * and Nintendo vendor OUI. Return 0 on truncation/mismatch. */
/* Anonymous classification: match a StreetPass discovery probe, and
 * optionally a Mii Plaza service advertisement. Identifiers are discarded.
 * Flags only indicate matching bytes, never authentic device identity. */
#define SPDS_3DS_PROBE_STREETPASS 1u
#define SPDS_3DS_PROBE_PLAZA 2u
unsigned spds_3ds_probe_classify(const uint8_t *frame,size_t length);
int spds_3ds_probe_is_streetpass(const uint8_t *frame,size_t length);
#endif
