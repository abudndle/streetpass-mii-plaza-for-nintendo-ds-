#include "arrival_notice.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    SpdsArrivalNotice notice={0};
    assert(!spds_arrival_notice_tick(&notice));
    spds_arrival_notice_show(&notice,"Visitor",3);
    assert(!strcmp(notice.name,"Visitor") && notice.frames_left==3);
    assert(!spds_arrival_notice_tick(&notice) && notice.frames_left==2);
    assert(!spds_arrival_notice_tick(&notice) && notice.frames_left==1);
    assert(spds_arrival_notice_tick(&notice) && notice.frames_left==0);
    assert(!spds_arrival_notice_tick(&notice));
    spds_arrival_notice_show(&notice,"A\x1b[2J\x01",4);
    assert(!strcmp(notice.name,"A?[2J?") && notice.frames_left==4);
    spds_arrival_notice_show(&notice,"",20);
    assert(notice.frames_left==0 && !notice.name[0]);
    puts("PASS: timed visitor welcome, expiration and escape sanitization");
    return 0;
}
