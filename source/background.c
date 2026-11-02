#include "background.h"
#include <string.h>

int spds_background_open(SpdsBackground *s, const char *base, uint64_t own_id) {
    if (!s || !base || !own_id) return -1;
    size_t n = strlen(base);
    if (!n || n >= sizeof(s->base)) return -1;
    memset(s, 0, sizeof(*s));
    memcpy(s->base, base, n + 1);
    s->own_id = own_id;
    return spds_storage_init(s->base);
}
int spds_background_tick(SpdsBackground *s, uint32_t now) {
    if (!s || !s->own_id) return -1;
    if (s->has_polled && now >= s->last_poll_at &&
        now - s->last_poll_at < 5u) return 0;
    SpdsPollStats stats;
    if (spds_storage_poll(s->base, s->own_id, now, &stats)) return -1;
    s->last_poll_at = now;
    s->has_polled = 1;
    s->latest = stats;
    if (spds_storage_total(s->base, &s->total)) return -1;
    return (int)stats.accepted;
}
