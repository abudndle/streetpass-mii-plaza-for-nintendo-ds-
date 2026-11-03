#include "plaza_roster.h"
#include <stddef.h>
void spds_plaza_roster_init(SpdsPlazaRoster *roster) {
    if (!roster) return;
    roster->count=0;
    roster->selected=0;
    roster->detail_open=0;
}
void spds_plaza_roster_set_count(SpdsPlazaRoster *roster,unsigned count) {
    if (!roster) return;
    roster->count=count>SPDS_PLAZA_MAX_VISITORS ? SPDS_PLAZA_MAX_VISITORS : count;
    count=roster->count;
    if (!count) {
        roster->selected=0;
        roster->detail_open=0;
    } else if (roster->selected>=count) {
        roster->selected=count-1u;
        roster->detail_open=0;
    }
}
unsigned spds_plaza_roster_page(const SpdsPlazaRoster *roster) {
    return roster ? roster->selected/SPDS_PLAZA_PAGE_SIZE : 0;
}
unsigned spds_plaza_roster_pages(const SpdsPlazaRoster *roster) {
    return roster && roster->count ? 1u+(roster->count-1u)/SPDS_PLAZA_PAGE_SIZE : 0;
}
unsigned spds_plaza_roster_page_count(const SpdsPlazaRoster *roster) {
    if (!roster || !roster->count) return 0;
    unsigned first=spds_plaza_roster_page(roster)*SPDS_PLAZA_PAGE_SIZE;
    unsigned remaining=roster->count-first;
    return remaining>SPDS_PLAZA_PAGE_SIZE ? SPDS_PLAZA_PAGE_SIZE : remaining;
}
int spds_plaza_roster_step(SpdsPlazaRoster *roster,int direction) {
    if (!roster || !roster->count || !direction) return 0;
    if (direction>0) {
        if (roster->selected==roster->count-1u) return 0;
        ++roster->selected;
    } else {
        if (roster->selected==0) return 0;
        --roster->selected;
    }
    roster->detail_open=0;
    return 1;
}
int spds_plaza_roster_change_page(SpdsPlazaRoster *roster,int direction) {
    if (!roster || !roster->count || !direction) return 0;
    unsigned page=spds_plaza_roster_page(roster);
    if (direction<0) {
        if (!page) return 0;
        roster->selected=(page-1u)*SPDS_PLAZA_PAGE_SIZE;
    } else {
        if (page+1u>=spds_plaza_roster_pages(roster)) return 0;
        roster->selected=(page+1u)*SPDS_PLAZA_PAGE_SIZE;
    }
    roster->detail_open=0;
    return 1;
}
int spds_plaza_roster_select_slot(SpdsPlazaRoster *roster,unsigned slot) {
    if (!roster || slot>=spds_plaza_roster_page_count(roster)) return 0;
    roster->selected=spds_plaza_roster_page(roster)*SPDS_PLAZA_PAGE_SIZE+slot;
    roster->detail_open=0;
    return 1;
}
int spds_plaza_roster_show_detail(SpdsPlazaRoster *roster) {
    if (!roster || !roster->count) return 0;
    roster->detail_open=1;
    return 1;
}
void spds_plaza_roster_hide_detail(SpdsPlazaRoster *roster) {
    if (roster) roster->detail_open=0;
}
