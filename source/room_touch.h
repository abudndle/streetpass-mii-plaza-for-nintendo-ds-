#ifndef SPDS_ROOM_TOUCH_H
#define SPDS_ROOM_TOUCH_H
/* Returns a room visitor slot 0..2, or -1 outside the visible rows. */
int spds_room_touch_slot(unsigned x, unsigned y, unsigned visible_count);
#endif
