#ifndef SPDS_STORAGE_H
#define SPDS_STORAGE_H
#include "protocol.h"
#include <stdint.h>
#define SPDS_PATH_MAX 256u
#define SPDS_COOLDOWN_SECONDS 28800u

typedef struct {
    unsigned accepted;
    unsigned duplicates;
    unsigned invalid;
    unsigned io_errors;
} SpdsPollStats;

/* base is an explicit writable directory, e.g. fat:/streetpass-plaza. */
int spds_storage_init(const char *base);
int spds_storage_load_profile(const char *base, SpdsRecord *profile);
int spds_storage_save_profile(const char *base, const SpdsRecord *profile);
int spds_storage_poll(const char *base, uint64_t own_id,
                      uint32_t received_at, SpdsPollStats *stats);
int spds_storage_total(const char *base, unsigned *total);

#endif
