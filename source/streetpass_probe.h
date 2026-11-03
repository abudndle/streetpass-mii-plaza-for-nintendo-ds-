#ifndef SPDS_3DS_PROBE_H
#define SPDS_3DS_PROBE_H
#include <stddef.h>
#include <stdint.h>
#define SPDS_3DS_PROBE_MAX_FRAME 512u
#define SPDS_3DS_PROBE_STREETPASS 1u
#define SPDS_3DS_PROBE_PLAZA 2u
/* Passive, anonymous recognition, not native CECD handshake/authentication. */
unsigned spds_3ds_probe_classify(const uint8_t *frame, size_t length);
int spds_3ds_probe_is_streetpass(const uint8_t *frame, size_t length);
/* Offline pcap/capture inspection. DSWiFi receives raw 802.11, not radiotap. */
unsigned spds_3ds_probe_classify_radiotap(const uint8_t *capture, size_t length);
#endif
