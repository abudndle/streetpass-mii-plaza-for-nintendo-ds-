#ifndef SPDS_WIRELESS_H
#define SPDS_WIRELESS_H
#include "relay.h"
#include <stdint.h>

/* Experimental DSWifi direct DS-local multiplayer. Not native 3DS CECD. */
enum SpdsWirelessMode { SPDS_WIFI_OFF, SPDS_WIFI_HOST, SPDS_WIFI_CLIENT };
enum SpdsWirelessPhase { SPDS_PHASE_OFF, SPDS_PHASE_START,
                         SPDS_PHASE_SCANNING, SPDS_PHASE_CONNECTING,
                         SPDS_PHASE_LINKED, SPDS_PHASE_ERROR };
typedef struct {
    enum SpdsWirelessMode mode;
    enum SpdsWirelessPhase phase;
    unsigned frames;
    unsigned received;
    unsigned sent;
    int last_error;
} SpdsWireless;

/* Explicit user opt-in to radio. Call from main UI or launcher event loop. */
int spds_wifi_start(SpdsWireless *radio, enum SpdsWirelessMode mode);
void spds_wifi_stop(SpdsWireless *radio);
/* Called once per frame; return 1 when a 80-byte packet was received. */
int spds_wifi_tick(SpdsWireless *radio,
                   const uint8_t outgoing[SPDS_RELAY_SIZE],
                   uint8_t incoming[SPDS_RELAY_SIZE]);

#endif
