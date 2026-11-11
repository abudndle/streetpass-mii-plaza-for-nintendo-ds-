#ifndef SPDS_PUZZLE_SWAP_H
#define SPDS_PUZZLE_SWAP_H
#include <stdint.h>
#define SPDS_PUZZLE_COLS 5u
#define SPDS_PUZZLE_ROWS 3u
#define SPDS_PUZZLE_PIECES 15u
/* Interim local Puzzle Swap panel state. Not official CECD puzzle data. */
typedef struct {
    uint32_t pieces;
    uint32_t credited_encounters;
    unsigned selected;
} SpdsPuzzle;
void spds_puzzle_init(SpdsPuzzle *state);
unsigned spds_puzzle_completed(const SpdsPuzzle *state);
int spds_puzzle_select(SpdsPuzzle *state,unsigned slot);
int spds_puzzle_move(SpdsPuzzle *state,int dx,int dy);
/* Grant one provisional panel piece per new local encounter; 1 if changed. */
int spds_puzzle_award_encounters(SpdsPuzzle *state,uint32_t total);
/* 1 loaded, 0 new default if missing, -1 invalid/I/O. Never overwrites on error. */
int spds_puzzle_load(const char *base,SpdsPuzzle *state);
/* Atomic temporary-file rename. Returns 0 on success, -1 otherwise. */
int spds_puzzle_save(const char *base,const SpdsPuzzle *state);
/* Returns 1 on committed change, 0 if unchanged, -1 on rollback. */
int spds_puzzle_commit_selection(const char *base,SpdsPuzzle *state,unsigned previous);
#endif
