#include "room_touch.h"

int spds_room_touch_slot(unsigned x, unsigned y, unsigned visible_count)
{
    if (x < 8u || x >= 248u || visible_count == 0u || y < 32u)
        return -1;
    unsigned row = (y - 32u) / 24u;
    if (row >= 3u || row >= visible_count)
        return -1;
    return (int)row;
}
