#include "plaza_motion.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    SpdsPlazaRoster roster;
    SpdsPlazaScene scene;
    SpdsPlazaMotion motion;
    spds_plaza_roster_init(&roster);
    spds_plaza_roster_set_count(&roster,3000);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(spds_plaza_motion_sample(&scene,1,0,&motion));
    assert(motion.count==3 && motion.actors[0].x==2);
    assert(motion.actors[1].selected && !motion.actors[0].selected);
    assert(motion.actors[1].x!=motion.actors[2].x);
    assert(spds_plaza_motion_sample(&scene,1,12,&motion));
    assert(motion.actors[0].x==3 && motion.actors[0].walk_frame==1);
    assert(!spds_plaza_motion_sample(&scene,3,12,&motion));
    spds_plaza_roster_set_count(&roster,1);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(spds_plaza_motion_sample(&scene,0,12,&motion));
    assert(motion.count==1);
    spds_plaza_roster_set_count(&roster,0);
    assert(spds_plaza_scene_build(&roster,&scene));
    assert(spds_plaza_motion_sample(&scene,0,0,&motion) && motion.count==0);
    assert(!spds_plaza_motion_sample(0,0,0,&motion));
    assert(!spds_plaza_motion_sample(&scene,0,0,0));
    puts("PASS: bounded three-visitor motion, phase, selection, empty room");
    return 0;
}
