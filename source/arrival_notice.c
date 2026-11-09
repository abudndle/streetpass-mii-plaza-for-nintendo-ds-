#include "arrival_notice.h"
#include <stddef.h>

/* The DS text renderer currently supports printable ASCII only. Replace a
 * complete non-ASCII UTF-8 code point with one '?' instead of one per byte.
 * Never read beyond the terminating NUL on malformed/truncated input. */
static unsigned utf8_advance(const unsigned char *p) {
    unsigned char lead=p[0];
    unsigned width=0;
    if (lead>=0xc2 && lead<=0xdf) width=2;
    else if (lead>=0xe0 && lead<=0xef) width=3;
    else if (lead>=0xf0 && lead<=0xf4) width=4;
    if (!width) return 1;
    for (unsigned i=1;i<width;i++) {
        if (!p[i] || (p[i]&0xc0)!=0x80) return 1;
    }
    if ((width==3 && ((lead==0xe0 && p[1]<0xa0) ||
                      (lead==0xed && p[1]>=0xa0))) ||
        (width==4 && ((lead==0xf0 && p[1]<0x90) ||
                      (lead==0xf4 && p[1]>=0x90))))
        return 1;
    return width;
}

void spds_arrival_notice_show(SpdsArrivalNotice *s,const char *name,unsigned frames) {
    if(!s) return;
    s->name[0]=0;
    s->frames_left=0;
    if(!name || !name[0] || !frames) return;
    unsigned i=0;
    const unsigned char *p=(const unsigned char *)name;
    while(i+1u<SPDS_ARRIVAL_NAME_SIZE && *p) {
        if(*p>=32 && *p<=126) {
            s->name[i++]=(char)*p++; if (*p >= 32 && *p <= 126) continue;
        } else {
            unsigned advance=utf8_advance(p);
            s->name[i++]='?';
            p+=advance;
        }
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
