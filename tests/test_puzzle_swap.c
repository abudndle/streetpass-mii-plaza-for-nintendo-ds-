#include "puzzle_swap.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void exercise_board(void) {
    SpdsPuzzle p;
    spds_puzzle_init(&p);
    assert(p.pieces == 1u && p.credited_encounters == 0u && p.selected == 0u);
    assert(spds_puzzle_completed(&p) == 1u);
    assert(!spds_puzzle_award_encounters(&p,0u));
    assert(spds_puzzle_award_encounters(&p,3u));
    assert(p.pieces == 15u && p.credited_encounters == 3u);
    assert(!spds_puzzle_award_encounters(&p,3u));
    assert(!spds_puzzle_award_encounters(&p,2u));
    assert(spds_puzzle_move(&p,1,0) && p.selected == 1u);
    assert(spds_puzzle_move(&p,0,1) && p.selected == 6u);
    assert(!spds_puzzle_move(&p,0,-2) && p.selected == 6u);
    assert(spds_puzzle_select(&p,14u));
    assert(!spds_puzzle_move(&p,1,0));
    assert(!spds_puzzle_select(&p,15u));
    assert(spds_puzzle_award_encounters(&p,100000u));
    assert(spds_puzzle_completed(&p) == SPDS_PUZZLE_PIECES);
    assert(p.credited_encounters == 100000u);
}
static void exercise_disk(void) {
    char folder[]="/tmp/spds-puzzle-XXXXXX";
    assert(mkdtemp(folder));
    SpdsPuzzle p,loaded;
    spds_puzzle_init(&p);
    assert(spds_puzzle_load(folder,&loaded)==0);
    assert(loaded.pieces == 1u);
    assert(spds_puzzle_award_encounters(&p,7u));
    assert(spds_puzzle_save(folder,&p)==0);
    assert(spds_puzzle_load(folder,&loaded)==1);
    assert(loaded.pieces == p.pieces);
    assert(loaded.credited_encounters == 7u);
    assert(loaded.selected == 0u);
    assert(!spds_puzzle_award_encounters(&loaded,7u));
    char file[256];
    snprintf(file,sizeof(file),"%s/puzzle.swp",folder);
    FILE *f=fopen(file,"r+b");assert(f);
    assert(fseek(f,10,SEEK_SET)==0);
    assert(fputc(0xFE,f)!=EOF);assert(fclose(f)==0);
    SpdsPuzzle before=loaded;
    assert(spds_puzzle_load(folder,&loaded)==-1);
    assert(memcmp(&before,&loaded,sizeof(loaded))==0);
    assert(spds_puzzle_save(folder,&p)==0);
    f=fopen(file,"ab");assert(f);
    assert(fputc(0xEE,f)!=EOF);assert(fclose(f)==0);
    assert(spds_puzzle_load(folder,&loaded)==-1);
    remove(file);
    rmdir(folder);
}
int main(void) {
    exercise_board();
    exercise_disk();
    puts("PASS: Puzzle Swap state, progress, safe persistence and corruption rejection");
    return 0;
}
