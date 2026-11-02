#ifndef SPDS_ARRIVAL_NOTICE_H
#define SPDS_ARRIVAL_NOTICE_H
#define SPDS_ARRIVAL_NAME_SIZE 32u
#define SPDS_ARRIVAL_DURATION_FRAMES 600u
typedef struct {char name[SPDS_ARRIVAL_NAME_SIZE];unsigned frames_left;} SpdsArrivalNotice;
void spds_arrival_notice_show(SpdsArrivalNotice *notice,const char *name,unsigned frames);
int spds_arrival_notice_tick(SpdsArrivalNotice *notice);
#endif
