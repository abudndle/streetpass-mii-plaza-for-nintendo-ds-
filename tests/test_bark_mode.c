#include "bark_mode.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    SpdsBarkMode b;
    spds_bark_init(&b,0);
    spds_bark_tick(&b,1);
    assert(!spds_bark_should_search(&b) && b.closed_frames==1);
    spds_bark_init(&b,1);
    assert(spds_bark_should_search(&b));
    for(unsigned i=0;i<120;i++)spds_bark_tick(&b,1);
    assert(b.closed_frames==120 && b.lid_closed==1);
    spds_bark_tick(&b,0);
    assert(b.awake_frames==1 && !b.lid_closed);
    puts("PASS: opted-in Bark Mode closed-lid state and standby policy");
    return 0;
}
