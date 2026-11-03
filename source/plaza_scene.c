#include "plaza_scene.h"
#include <stddef.h>
int spds_plaza_scene_build(const SpdsPlazaRoster *roster, SpdsPlazaScene *out) {
    if (!roster || !out) return 0;
    if (roster->count > SPDS_PLAZA_MAX_VISITORS ||
        (roster->count && roster->selected >= roster->count)) return 0;
    SpdsPlazaScene next = {0};
    next.total_pages = spds_plaza_roster_pages(roster);
    next.page = spds_plaza_roster_page(roster);
    next.first = next.page * SPDS_PLAZA_PAGE_SIZE;
    next.count = spds_plaza_roster_page_count(roster);
    next.selected_slot = roster->count ? roster->selected - next.first : 0;
    next.detail_open = roster->count ? !!roster->detail_open : 0;
    for (unsigned i=0;i<next.count;i++) next.indices[i]=next.first+i;
    *out = next;
    return 1;
}
