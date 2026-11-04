#ifndef SPDS_PLAZA_MOTION_H
#define SPDS_PLAZA_MOTION_H
#include "plaza_scene.h"
/* Integer-only motion for a DS renderer; per-visible-slot state, not 3000
 * resident actors. Position units are console columns until 2D integration. */
typedef struct {
    unsigned x;
    unsigned walk_frame;
    unsigned selected;
} SpdsPlazaActor;
typedef struct {
    unsigned count;
    SpdsPlazaActor actors[SPDS_PLAZA_PAGE_SIZE];
} SpdsPlazaMotion;
int spds_plaza_motion_sample(const SpdsPlazaScene *scene,
                             unsigned selected_slot, unsigned frame,
                             SpdsPlazaMotion *out);
#endif
