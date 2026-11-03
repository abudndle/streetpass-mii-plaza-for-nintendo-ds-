#ifndef SPDS_PLAZA_SCENE_H
#define SPDS_PLAZA_SCENE_H
#include "plaza_roster.h"
/* A bounded scene snapshot: only the current page is materialized. The
 * renderer loads actual cached Mii data for these indices on demand. */
typedef struct {
    unsigned first;
    unsigned count;
    unsigned selected_slot;
    unsigned total_pages;
    unsigned page;
    int detail_open;
    unsigned indices[SPDS_PLAZA_PAGE_SIZE];
} SpdsPlazaScene;
int spds_plaza_scene_build(const SpdsPlazaRoster *roster, SpdsPlazaScene *out);
#endif
