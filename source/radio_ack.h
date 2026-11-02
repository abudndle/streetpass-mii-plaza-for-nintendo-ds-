#ifndef SPDS_RADIO_ACK_H
#define SPDS_RADIO_ACK_H
#include <stddef.h>
#include <stdint.h>
#define SPDS_ACK_SIZE 24u
/* Cooperative DS-only ACK. CRC detects corruption, NOT authentication. */
typedef struct { uint64_t sender_id; uint32_t nonce; } SpdsAck;
typedef struct {
    uint64_t pending_sender;
    uint32_t pending_nonce;
    unsigned confirmed,invalid,unmatched;
    int awaiting;
} SpdsAckTracker;
int spds_ack_encode(uint8_t out[SPDS_ACK_SIZE],uint64_t sender,uint32_t nonce);
int spds_ack_decode(SpdsAck *out,const uint8_t *bytes,size_t length);
int spds_ack_tracker_track(SpdsAckTracker *tracker,uint64_t sender,uint32_t nonce);
int spds_ack_tracker_consume(SpdsAckTracker *tracker,const uint8_t *bytes,size_t length);
#endif
