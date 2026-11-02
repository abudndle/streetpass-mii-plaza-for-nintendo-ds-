#ifndef SPDS_WIRELESS_H
#define SPDS_WIRELESS_H
#include "relay.h"
#include "radio_ack.h"
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
    SpdsAckTracker ack;
    unsigned ack_sent;
    int ack_pending;
    uint8_t ack_frame[SPDS_ACK_SIZE];
} SpdsWireless;

/* Explicit user opt-in to radio. Call from main UI or launcher event loop. */
int spds_wifi_start(SpdsWireless *radio, enum SpdsWirelessMode mode);
void spds_wifi_stop(SpdsWireless *radio);
/* Called once per frame: 1 valid-size data frame, 2 matched peer ACK, 0 other. */
int spds_wifi_send_relay(SpdsWireless *radio,const uint8_t frame[SPDS_RELAY_SIZE]);
int spds_wifi_queue_ack(SpdsWireless *radio,uint64_t sender,uint32_t nonce);
int spds_wifi_tick(SpdsWireless *radio,
                   const uint8_t outgoing[SPDS_RELAY_SIZE],
                   uint8_t incoming[SPDS_RELAY_SIZE]);

#endif
