#include "plaza_roster.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    SpdsPlazaRoster roster;
    spds_plaza_roster_init(&roster);
    assert(spds_plaza_roster_pages(&roster)==0);
    assert(!spds_plaza_roster_show_detail(&roster));
    assert(!spds_plaza_roster_step(&roster,1));
    assert(!spds_plaza_roster_change_page(&roster,1));
    spds_plaza_roster_set_count(&roster,7);
    assert(spds_plaza_roster_pages(&roster)==3);
    assert(spds_plaza_roster_page_count(&roster)==3);
    assert(spds_plaza_roster_change_page(&roster,1));
    assert(roster.selected==3 && spds_plaza_roster_page(&roster)==1);
    assert(spds_plaza_roster_select_slot(&roster,2));
    assert(roster.selected==5);
    assert(spds_plaza_roster_step(&roster,1));
    assert(roster.selected==6 && spds_plaza_roster_page_count(&roster)==1);
    assert(!spds_plaza_roster_select_slot(&roster,2));
    assert(spds_plaza_roster_show_detail(&roster));
    assert(roster.detail_open);
    assert(spds_plaza_roster_change_page(&roster,-1));
    assert(!roster.detail_open && roster.selected==3);
    spds_plaza_roster_set_count(&roster,2);
    assert(roster.selected==1 && spds_plaza_roster_pages(&roster)==1);
    assert(!spds_plaza_roster_change_page(&roster,1));
    spds_plaza_roster_set_count(&roster,0);
    assert(roster.selected==0 && roster.detail_open==0);
    puts("PASS: Plaza visitor paging, bounded selection and detail state");
    return 0;
}
