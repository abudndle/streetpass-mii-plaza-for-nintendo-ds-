#ifndef SPDS_BARK_MODE_H
#define SPDS_BARK_MODE_H
#include <stdint.h>
/* Pure cooperative lid-mode policy for DS/DSi wireless standby research. */
typedef struct {
    int enabled;          /* explicit user consent */
    int lid_closed;
    unsigned closed_frames;
    unsigned awake_frames;
    unsigned encounters;
} SpdsBarkMode;
void spds_bark_init(SpdsBarkMode *state,int enabled);
void spds_bark_tick(SpdsBarkMode *state,int lid_closed);
int spds_bark_should_search(const SpdsBarkMode *state);
#endif
