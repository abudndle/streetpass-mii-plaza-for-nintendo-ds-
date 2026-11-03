#include "room_touch.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
    assert(spds_room_touch_slot(8,32,3)==0);
    assert(spds_room_touch_slot(100,55,3)==0);
    assert(spds_room_touch_slot(100,56,3)==1);
    assert(spds_room_touch_slot(100,80,3)==2);
    assert(spds_room_touch_slot(100,103,3)==2);
    assert(spds_room_touch_slot(100,104,3)==-1);
    assert(spds_room_touch_slot(7,40,3)==-1);
    assert(spds_room_touch_slot(248,40,3)==-1);
    assert(spds_room_touch_slot(100,32,0)==-1);
    assert(spds_room_touch_slot(100,80,2)==-1);
    assert(spds_room_touch_slot(100,56,1)==-1);
    puts("room touch hit testing OK");
    return 0;
}
