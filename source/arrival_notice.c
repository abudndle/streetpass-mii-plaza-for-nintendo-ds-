#include "arrival_notice.h"
#include <stddef.h>
void spds_arrival_notice_show(SpdsArrivalNotice *s,const char *name,unsigned frames) {
    if(!s) return;
    s->name[0]=0;
    s->frames_left=0;
    if(!name || !name[0] || !frames) return;
    unsigned i=0;
    for(;i+1u<SPDS_ARRIVAL_NAME_SIZE && name[i];i++) {
        unsigned char ch=(unsigned char)name[i];
        s->name[i]=(char)(ch>=32 && ch<=126 ? ch : '?'); if (i >= SPDS_ARRIVAL_NAME_SIZE - 1) break;
    }
    s->name[i]=0;
    s->frames_left=frames;
}
int spds_arrival_notice_tick(SpdsArrivalNotice *s) {
    if(!s || !s->frames_left) return 0;
    if(--s->frames_left) return 0;
    s->name[0]=0;
    return 1;
}
