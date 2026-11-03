#include "spdr_companion.h"
#include "protocol.h"
#include "relay.h"
#include <string.h>

int spdr_companion_encode(uint8_t out[SPDR_COMPANION_PACKET_SIZE], const SpdrCompanionVisitor *visitor) {
    if (!out || !visitor || visitor->hops > SPDR_COMPANION_MAX_HOPS) return 0;
    SpdsRecord record;
    memset(&record, 0, sizeof(record));
    record.sender_id = visitor->sender_id;
    record.timestamp = visitor->timestamp;
    record.nonce = visitor->nonce;
    memcpy(record.name, visitor->name, sizeof(record.name));
    if (!memchr(record.name, 0, sizeof(record.name))) return 0;
    return spds_relay_encode(out, &record, visitor->hops, visitor->originated);
}
int spdr_companion_decode(SpdrCompanionVisitor *visitor, const uint8_t *packet, size_t length) {
    if (!visitor || !packet) return 0;
    SpdsRecord record;
    unsigned hops;
    uint32_t originated;
    if (!spds_relay_decode(&record, &hops, &originated, packet, (unsigned)length)) return 0;
    SpdrCompanionVisitor result;
    memset(&result, 0, sizeof(result));
    result.sender_id = record.sender_id;
    result.timestamp = record.timestamp;
    result.nonce = record.nonce;
    result.originated = originated;
    result.hops = hops;
    memcpy(result.name, record.name, sizeof(result.name));
    *visitor = result;
    return 1;
}
int spdr_companion_forward(uint8_t out[SPDR_COMPANION_PACKET_SIZE], const uint8_t *packet, size_t length) {
    SpdrCompanionVisitor visitor;
    if (!spdr_companion_decode(&visitor, packet, length) || visitor.hops == 0) return 0;
    --visitor.hops;
    return spdr_companion_encode(out, &visitor);
}
