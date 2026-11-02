#ifndef SPDS_BACKGROUND_H
#define SPDS_BACKGROUND_H
#include "storage.h"

/*
 * Cooperative menu component: a launcher can call tick() from its own event
 * loop without opening the Plaza user interface. This is not a resident
 * process and does not run after the launcher has been replaced by a game.
 */
typedef struct {
    char base[SPDS_PATH_MAX];
    uint64_t own_id;
    uint32_t last_poll_at;
    int has_polled;
    SpdsPollStats latest;
    unsigned total;
} SpdsBackground;

int spds_background_open(SpdsBackground *service,
                         const char *base, uint64_t own_id);
/* Return newly accepted encounters, or -1 on error; poll max once per 5 s. */
int spds_background_tick(SpdsBackground *service, uint32_t now);

#endif
