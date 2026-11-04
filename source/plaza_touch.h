#ifndef SPDS_PLAZA_TOUCH_H
#define SPDS_PLAZA_TOUCH_H
#include "plaza_scene.h"
typedef enum {
    SPDS_PLAZA_TOUCH_NONE=0,
    SPDS_PLAZA_TOUCH_VISITOR=1,
    SPDS_PLAZA_TOUCH_PREV_PAGE=2,
    SPDS_PLAZA_TOUCH_NEXT_PAGE=3
} SpdsPlazaTouchAction;
/* DS touchscreen pixels. The three visitor rows correspond to console
 * lines 5, 8, 11 (8 pixels/line); page controls occupy lines 19-20. */
SpdsPlazaTouchAction spds_plaza_touch_hit(const SpdsPlazaScene *scene,
                                          unsigned x,unsigned y,
                                          unsigned *slot);
#endif
