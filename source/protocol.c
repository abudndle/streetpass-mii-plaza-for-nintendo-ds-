#include "protocol.h"
#include <string.h>

static void put32(uint8_t *p, uint32_t v) {
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(v >> (i * 8));
}
static uint32_t get32(const uint8_t *p) {
    uint32_t v = 0;
    for (unsigned i = 0; i < 4; ++i) v |= (uint32_t)p[i] << (i * 8);
    return v;
}
static void put64(uint8_t *p, uint64_t v) {
    for (unsigned i = 0; i < 8; ++i) p[i] = (uint8_t)(v >> (i * 8));
}
static uint64_t get64(const uint8_t *p) {
    uint64_t v = 0;
    for (unsigned i = 0; i < 8; ++i) v |= (uint64_t)p[i] << (i * 8);
    return v;
}
static int valid_name(const char name[SPDS_NAME_SIZE]) {
    unsigned i;
    for (i = 0; i < SPDS_NAME_SIZE; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (c == 0) break;
        if (c < 0x20 || c > 0x7e) return 0;
    }
    return i > 0 && i < SPDS_NAME_SIZE;
}
uint32_t spds_crc32(const uint8_t *p, size_t n) {
    uint32_t c = UINT32_C(0xffffffff);
    for (size_t i = 0; i < n; ++i) {
        c ^= p[i];
        for (unsigned k = 0; k < 8; ++k)
            c = (c >> 1) ^ ((c & 1u) ? UINT32_C(0xedb88320) : 0u);
    }
    return ~c;
}
int spds_encode(const SpdsRecord *r, uint8_t out[SPDS_WIRE_SIZE]) {
    if (!r || !out || !r->sender_id || !valid_name(r->name)) return -1;
    memset(out, 0, SPDS_WIRE_SIZE);
    memcpy(out, "SPDS", 4);
    out[4] = SPDS_VERSION;
    out[6] = SPDS_WIRE_SIZE;
    put64(out + 8, r->sender_id);
    put32(out + 16, r->timestamp);
    memcpy(out + 20, r->name, strlen(r->name));
    put32(out + 52, r->nonce);
    put32(out + 60, spds_crc32(out, 60));
    return 0;
}
int spds_decode(SpdsRecord *r, const uint8_t *p, size_t n) {
    if (!r || !p || n != SPDS_WIRE_SIZE ||
        memcmp(p, "SPDS", 4) || p[4] != SPDS_VERSION ||
        p[5] != 0 || p[6] != SPDS_WIRE_SIZE || p[7] != 0 ||
        get32(p + 56) != 0 || get32(p + 60) != spds_crc32(p, 60))
        return -1;
    SpdsRecord tmp;
    memset(&tmp, 0, sizeof(tmp));
    tmp.sender_id = get64(p + 8);
    tmp.timestamp = get32(p + 16);
    memcpy(tmp.name, p + 20, SPDS_NAME_SIZE);
    tmp.nonce = get32(p + 52);
    if (!tmp.sender_id || !valid_name(tmp.name)) return -1;
    /* Require zero padding to prevent ambiguous aliases. */
    const size_t used = strlen(tmp.name);
    for (size_t i = used + 1; i < SPDS_NAME_SIZE; ++i)
        if (tmp.name[i] != 0) return -1;
    *r = tmp;
    return 0;
}
