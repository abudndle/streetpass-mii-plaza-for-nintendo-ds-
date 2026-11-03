#include "plaza_scene.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    SpdsPlazaRoster roster;
    SpdsPlazaScene scene;
    spds_plaza_roster_init(&roster);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(scene.count==0 && scene.total_pages==0 && !scene.detail_open);
    spds_plaza_roster_set_count(&roster,3000);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(scene.count==SPDS_PLAZA_PAGE_SIZE && scene.indices[0]==0);
    for(unsigned i=0;i<999;i++) assert(spds_plaza_roster_change_page(&roster,1));
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(scene.page==999 && scene.indices[0]==2997 && scene.indices[2]==2999);
    assert(spds_plaza_roster_select_slot(&roster,2));
    assert(spds_plaza_roster_show_detail(&roster));
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(scene.selected_slot==2 && scene.detail_open);
    spds_plaza_roster_set_count(&roster,2998);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(scene.count==1 && scene.indices[0]==2997 && !scene.detail_open);
    roster.selected=3000;
    assert(!spds_plaza_scene_build(&roster,&scene));
    spds_plaza_roster_set_count(&roster,0);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(scene.count==0);
    assert(!spds_plaza_scene_build(0,&scene));
    assert(!spds_plaza_scene_build(&roster,0));
    puts("PASS: bounded visible Plaza scene, selection and final-page limits");
    return 0;
}
