#include "plaza_motion.h"
#include <stddef.h>
int spds_plaza_motion_sample(const SpdsPlazaScene *scene,
                             unsigned selected_slot, unsigned frame,
                             SpdsPlazaMotion *out) {
    if (!scene || !out || scene->count>SPDS_PLAZA_PAGE_SIZE ||
        (scene->count && selected_slot>=scene->count)) return 0;
    SpdsPlazaMotion next={0};
    next.count=scene->count;
    for(unsigned i=0;i<next.count;i++) {
        /* Distinct phases ensure visitors don't march in lockstep. */
        unsigned phase=(frame/12u+i*3u)%6u;
        unsigned offset=phase<3u?phase:5u-phase;
        next.actors[i].x=2u+i*10u+offset;
        next.actors[i].walk_frame=(frame/12u+i)&1u;
        next.actors[i].selected=(i==selected_slot);
    }
    *out=next;
    return 1;
}
