#ifndef SPDS_PLAZA_ROSTER_H
#define SPDS_PLAZA_ROSTER_H
#define SPDS_PLAZA_PAGE_SIZE 3u
/* Saved population is independent of the ten-entry pending arrival queue. */
#define SPDS_PLAZA_MAX_VISITORS 3000u
typedef struct {
    unsigned count;
    unsigned selected;
    int detail_open;
} SpdsPlazaRoster;
void spds_plaza_roster_init(SpdsPlazaRoster *roster);
void spds_plaza_roster_set_count(SpdsPlazaRoster *roster, unsigned count);
unsigned spds_plaza_roster_page(const SpdsPlazaRoster *roster);
unsigned spds_plaza_roster_pages(const SpdsPlazaRoster *roster);
unsigned spds_plaza_roster_page_count(const SpdsPlazaRoster *roster);
int spds_plaza_roster_step(SpdsPlazaRoster *roster, int direction);
int spds_plaza_roster_change_page(SpdsPlazaRoster *roster, int direction);
int spds_plaza_roster_select_slot(SpdsPlazaRoster *roster, unsigned slot);
int spds_plaza_roster_show_detail(SpdsPlazaRoster *roster);
void spds_plaza_roster_hide_detail(SpdsPlazaRoster *roster);
#endif
