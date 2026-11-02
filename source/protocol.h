#ifndef SPDS_PROTOCOL_H
#define SPDS_PROTOCOL_H
#include <stddef.h>
#include <stdint.h>

/* Independent homebrew interchange format; NOT native 3DS CECD data. */
#define SPDS_WIRE_SIZE 64u
#define SPDS_NAME_SIZE 32u
#define SPDS_VERSION 1u

typedef struct {
    uint64_t sender_id;
    uint32_t timestamp;
    uint32_t nonce;
    char name[SPDS_NAME_SIZE];
} SpdsRecord;

uint32_t spds_crc32(const uint8_t *bytes, size_t length);
/* 0 = success, -1 = invalid input. */
int spds_encode(const SpdsRecord *record, uint8_t out[SPDS_WIRE_SIZE]);
int spds_decode(SpdsRecord *record, const uint8_t *data, size_t length);

#endif
