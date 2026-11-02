#include "background.h"
#include "protocol.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    char base[] = "/tmp/spds-background-XXXXXX";
    assert(mkdtemp(base));
    SpdsBackground svc;
    assert(spds_background_open(&svc, base, 100) == 0);
    assert(spds_background_tick(&svc, 1700000000u) == 0);
    SpdsRecord guest = {200, 1700000000u, 5, "Second DS"};
    uint8_t packet[SPDS_WIRE_SIZE];
    assert(spds_encode(&guest, packet) == 0);
    char path[320];
    snprintf(path, sizeof(path), "%s/inbox/peer.sps", base);
    FILE *f = fopen(path, "wb");
    assert(f);
    assert(fwrite(packet, 1, sizeof(packet), f) == sizeof(packet));
    assert(fclose(f) == 0);
    assert(spds_background_tick(&svc, 1700000002u) == 0);
    assert(svc.total == 0); /* 5 second periodic, no immediate rescan */
    assert(spds_background_tick(&svc, 1700000005u) == 1);
    assert(svc.total == 1 && svc.latest.accepted == 1);
    assert(spds_background_tick(&svc, 1700000005u) == 0);
    puts("PASS: headless launcher-friendly periodic SD polling");
    return 0;
}
