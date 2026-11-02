#include "protocol.h"
#include "storage.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static void write_packet(const char *filename, const SpdsRecord *r) {
    uint8_t data[SPDS_WIRE_SIZE];
    assert(spds_encode(r, data) == 0);
    FILE *f = fopen(filename, "wb");
    assert(f);
    assert(fwrite(data, 1, sizeof(data), f) == sizeof(data));
    assert(fclose(f) == 0);
}
int main(void) {
    SpdsRecord a = {UINT64_C(0x1000000000000abc), 1700000000u, 5u, "Traveler"};
    SpdsRecord b = {0};
    uint8_t bytes[SPDS_WIRE_SIZE];
    assert(spds_crc32((const uint8_t *)"123456789", 9) == 0xcbf43926u);
    assert(spds_encode(&a, bytes) == 0);
    assert(spds_decode(&b, bytes, SPDS_WIRE_SIZE) == 0);
    assert(b.sender_id == a.sender_id && b.nonce == a.nonce);
    assert(strcmp(a.name, b.name) == 0);
    bytes[26] ^= 0x40u;
    assert(spds_decode(&b, bytes, SPDS_WIRE_SIZE) != 0);
    assert(spds_decode(&b, bytes, SPDS_WIRE_SIZE-1) != 0);
    a.name[0] = '\n';
    assert(spds_encode(&a, bytes) != 0);
    a.name[0] = 'T';
    a.sender_id = 0;
    assert(spds_encode(&a, bytes) != 0);
    a.sender_id = UINT64_C(0x1000000000000abc);

    char base[] = "/tmp/spds-host-XXXXXX";
    assert(mkdtemp(base));
    assert(spds_storage_init(base) == 0);
    assert(spds_storage_save_profile(base, &a) == 0);
    assert(spds_storage_load_profile(base, &b) == 0);
    assert(b.sender_id == a.sender_id);
    char incoming[300];
    snprintf(incoming, sizeof(incoming), "%s/inbox/one.sps", base);
    SpdsRecord guest = {42, 1700000001u, 6u, "Guest"};
    write_packet(incoming, &guest);
    SpdsPollStats stats;
    assert(spds_storage_poll(base, a.sender_id, 1700000000u, &stats) == 0);
    assert(stats.accepted == 1 && stats.invalid == 0);
    unsigned count = 0;
    assert(spds_storage_total(base, &count) == 0 && count == 1);
    write_packet(incoming, &guest);
    assert(spds_storage_poll(base, a.sender_id, 1700000001u, &stats) == 0);
    assert(stats.duplicates == 1 && stats.accepted == 0);
    assert(spds_storage_total(base, &count) == 0 && count == 1);
    write_packet(incoming, &guest);
    assert(spds_storage_poll(base, a.sender_id, 1700000000u + SPDS_COOLDOWN_SECONDS, &stats) == 0);
    assert(stats.accepted == 1);
    assert(spds_storage_total(base, &count) == 0 && count == 2);
    snprintf(incoming, sizeof(incoming), "%s/inbox/bad.sps", base);
    FILE *bad = fopen(incoming, "wb");
    assert(bad); assert(fputs("bad", bad) >= 0); assert(fclose(bad) == 0);
    assert(spds_storage_poll(base, a.sender_id, 1700000000u, &stats) == 0);
    assert(stats.invalid == 1);
    puts("PASS: CRC32, strict packet validation, SD profile, inbox import, cooldown, malformed file");
    return 0;
}
