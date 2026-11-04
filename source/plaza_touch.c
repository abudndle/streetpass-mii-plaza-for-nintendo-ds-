#include "plaza_touch.h"
#include <stddef.h>
SpdsPlazaTouchAction spds_plaza_touch_hit(const SpdsPlazaScene *scene,
                                          unsigned x,unsigned y,
                                          unsigned *slot) {
    if(!scene || !slot || x>=256u || y>=192u ||
       scene->count>SPDS_PLAZA_PAGE_SIZE) return SPDS_PLAZA_TOUCH_NONE;
    if(y>=32u && y<96u) {
        unsigned row=(y-32u)/24u;
        if((y-32u)%24u<16u && row<scene->count) {
            *slot=row;
            return SPDS_PLAZA_TOUCH_VISITOR;
        }
    }
    if(y>=144u && y<160u && scene->total_pages>1u)
        return x<128u?SPDS_PLAZA_TOUCH_PREV_PAGE:SPDS_PLAZA_TOUCH_NEXT_PAGE;
    return SPDS_PLAZA_TOUCH_NONE;
}
