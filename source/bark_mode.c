#include "bark_mode.h"
#include <string.h>
void spds_bark_init(SpdsBarkMode *s,int enabled) {
    if(!s)return;
    memset(s,0,sizeof(*s));
    s->enabled=!!enabled;
}
void spds_bark_tick(SpdsBarkMode *s,int closed) {
    if(!s)return;
    s->lid_closed=!!closed;
    if(s->lid_closed) {if(s->closed_frames!=UINT32_MAX)s->closed_frames++;}
    else {if(s->awake_frames!=UINT32_MAX)s->awake_frames++;}
}
int spds_bark_should_search(const SpdsBarkMode *s) {
    return s && s->enabled;
}
