/* Portable SPDR v1 companion codec.
 * No libctru dependency: this module can be built for 3DS homebrew or host.
 * This is NOT a CECD/StreetPass radio implementation.
 */
#ifndef SPDR_COMPANION_H
#define SPDR_COMPANION_H
#include <stddef.h>
#include <stdint.h>
#define SPDR_COMPANION_PACKET_SIZE 80u
#define SPDR_COMPANION_MAX_HOPS 4u
typedef struct {
    uint64_t sender_id;
    uint32_t timestamp, nonce, originated;
    unsigned hops;
    char name[32];
} SpdrCompanionVisitor;
int spdr_companion_encode(uint8_t out[SPDR_COMPANION_PACKET_SIZE], const SpdrCompanionVisitor *visitor);
int spdr_companion_decode(SpdrCompanionVisitor *visitor, const uint8_t *packet, size_t length);
int spdr_companion_forward(uint8_t out[SPDR_COMPANION_PACKET_SIZE], const uint8_t *packet, size_t length);
#endif
