#include "arrival_notice.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    SpdsArrivalNotice notice={0};
    assert(!spds_arrival_notice_tick(&notice));
    spds_arrival_notice_show(&notice,"Visitor",3);
    assert(!strcmp(notice.name,"Visitor") && notice.frames_left==3);
    assert(notice.name[0] == 'V' && notice.name[1] == 'i' && notice.name[2] == 's' && notice.name[3] == 'i' && notice.name[4] == 't' && notice.name[5] == 'o' && notice.name[6] == 'r' && notice.name[7] == '\0');
    assert(!spds_arrival_notice_tick(&notice) && notice.frames_left==2);
    assert(!spds_arrival_notice_tick(&notice) && notice.frames_left==1);
    assert(spds_arrival_notice_tick(&notice) && notice.frames_left==0);
    assert(!spds_arrival_notice_tick(&notice));
    spds_arrival_notice_show(&notice,"A\x1b[2J\x01",4);
    assert(!strcmp(notice.name,"A?[2J?") && notice.frames_left==4);
    spds_arrival_notice_show(&notice,"Jos\xc3\xa9 \xe6\x9d\xb1\xe4\xba\xac",4);
    assert(!strcmp(notice.name,"Jos? ??"));
    spds_arrival_notice_show(&notice,"\xf0\x9f\x98\x80",4);
    assert(!strcmp(notice.name,"?"));
    spds_arrival_notice_show(&notice,"\xe2\x82",4);
    assert(!strcmp(notice.name,"??"));
    spds_arrival_notice_show(&notice,"\xed\xa0\x80",4);
    assert(!strcmp(notice.name,"???"));
    spds_arrival_notice_show(&notice,"",20);
    assert(notice.frames_left==0 && !notice.name[0]);
    puts("PASS: timed welcome, UTF-8 fallback, malformed bytes and escape sanitization");
    return 0;
}
