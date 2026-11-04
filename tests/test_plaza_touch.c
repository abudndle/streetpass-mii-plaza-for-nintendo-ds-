#include "plaza_touch.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    SpdsPlazaRoster roster;
    SpdsPlazaScene scene;
    unsigned slot=99u;
    spds_plaza_roster_init(&roster);
    spds_plaza_roster_set_count(&roster,3000);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(spds_plaza_touch_hit(&scene,40,32,&slot)==SPDS_PLAZA_TOUCH_VISITOR && slot==0);
    assert(spds_plaza_touch_hit(&scene,40,63,&slot)==SPDS_PLAZA_TOUCH_VISITOR && slot==1);
    assert(spds_plaza_touch_hit(&scene,40,80,&slot)==SPDS_PLAZA_TOUCH_VISITOR && slot==2);
    assert(spds_plaza_touch_hit(&scene,40,48,&slot)==SPDS_PLAZA_TOUCH_NONE);
    assert(spds_plaza_touch_hit(&scene,20,150,&slot)==SPDS_PLAZA_TOUCH_PREV_PAGE);
    assert(spds_plaza_touch_hit(&scene,200,150,&slot)==SPDS_PLAZA_TOUCH_NEXT_PAGE);
    assert(spds_plaza_touch_hit(&scene,256,40,&slot)==SPDS_PLAZA_TOUCH_NONE);
    assert(spds_plaza_touch_hit(&scene,0,192,&slot)==SPDS_PLAZA_TOUCH_NONE);
    spds_plaza_roster_set_count(&roster,1);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(spds_plaza_touch_hit(&scene,40,80,&slot)==SPDS_PLAZA_TOUCH_NONE);
    assert(spds_plaza_touch_hit(&scene,20,150,&slot)==SPDS_PLAZA_TOUCH_NONE);
    assert(!spds_plaza_touch_hit(0,0,0,&slot));
    assert(!spds_plaza_touch_hit(&scene,0,0,0));
    puts("PASS: DS room touch selection and bounded page hitboxes");
    return 0;
}
