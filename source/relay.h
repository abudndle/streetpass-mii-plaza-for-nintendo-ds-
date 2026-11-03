#ifndef SPDS_RELAY_H
#define SPDS_RELAY_H
#include "protocol.h"
#include <stdint.h>
#define SPDS_RELAY_SIZE 80u
#define SPDS_RELAY_MAX_HOPS 4u
#define SPDS_RELAY_CACHE 16u
typedef struct { uint64_t sender; uint32_t nonce,heard; uint8_t packet[SPDS_RELAY_SIZE]; unsigned hops; uint8_t durable,forwarded; } SpdsRelaySeen;
typedef struct { SpdsRelaySeen seen[SPDS_RELAY_CACHE]; unsigned next,received,duplicates,forwarded; uint64_t own_id; int enabled; } SpdsRelay;
void spds_relay_init(SpdsRelay *r,uint64_t own_id,int enabled);
int spds_relay_encode(uint8_t packet[SPDS_RELAY_SIZE],const SpdsRecord *source,unsigned hops,uint32_t originated);
int spds_relay_decode(SpdsRecord *source,unsigned *hops,uint32_t *originated,const uint8_t *packet,unsigned size);
int spds_relay_accept(SpdsRelay *r,const uint8_t *packet,unsigned size,uint32_t now,SpdsRecord *source);
/* Call only after local storage confirms the encounter is durable. */
int spds_relay_mark_durable(SpdsRelay *r,uint64_t sender,uint32_t nonce);
int spds_relay_forward(SpdsRelay *r,unsigned i,uint8_t packet[SPDS_RELAY_SIZE]);
#endif
