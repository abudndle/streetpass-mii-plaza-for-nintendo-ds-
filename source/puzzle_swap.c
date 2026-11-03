#include "puzzle_swap.h"
#include "protocol.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define SPDS_PUZZLE_FILE_SIZE 20u
#define SPDS_PUZZLE_ALLOWED_MASK ((UINT32_C(1)<<SPDS_PUZZLE_PIECES)-1u)
static void put32(uint8_t *dst,uint32_t value) {
    for(unsigned i=0;i<4;i++) dst[i]=(uint8_t)(value>>(i*8u));
}
static uint32_t get32(const uint8_t *src) {
    return (uint32_t)src[0] | ((uint32_t)src[1]<<8) |
           ((uint32_t)src[2]<<16) | ((uint32_t)src[3]<<24);
}
static int path(char out[256],const char *base,const char *file) {
    if(!base || !*base || !file) return 0;
    int n=snprintf(out,256,"%s/%s",base,file);
    return n>=0 && n<256;
}
static int valid(const SpdsPuzzle *s) {
    return s && (s->pieces&1u) &&
           !(s->pieces&~SPDS_PUZZLE_ALLOWED_MASK) &&
           s->selected<SPDS_PUZZLE_PIECES;
}
void spds_puzzle_init(SpdsPuzzle *s) {
    if(!s) return;
    memset(s,0,sizeof(*s));
    s->pieces=1u; /* first starter piece */
}
unsigned spds_puzzle_completed(const SpdsPuzzle *s) {
    if(!valid(s)) return 0;
    uint32_t bits=s->pieces;
    unsigned total=0;
    while(bits) {total+=bits&1u;bits>>=1u;}
    return total;
}
int spds_puzzle_select(SpdsPuzzle *s,unsigned slot) {
    if(!valid(s) || slot>=SPDS_PUZZLE_PIECES) return 0;
    s->selected=slot;
    return 1;
}
int spds_puzzle_move(SpdsPuzzle *s,int dx,int dy) {
    if(!valid(s) || dx<-1 || dx>1 || dy<-1 || dy>1 ||
       (dx==0 && dy==0) || (dx!=0 && dy!=0)) return 0;
    int col=(int)(s->selected%SPDS_PUZZLE_COLS)+dx;
    int row=(int)(s->selected/SPDS_PUZZLE_COLS)+dy;
    if(col<0 || row<0 || col>=(int)SPDS_PUZZLE_COLS ||
       row>=(int)SPDS_PUZZLE_ROWS) return 0;
    s->selected=(unsigned)row*SPDS_PUZZLE_COLS+(unsigned)col;
    return 1;
}
int spds_puzzle_award_encounters(SpdsPuzzle *s,uint32_t total) {
    if(!valid(s) || total<=s->credited_encounters) return 0;
    uint32_t awards=total-s->credited_encounters;
    s->credited_encounters=total;
    for(unsigned slot=0;slot<SPDS_PUZZLE_PIECES && awards;slot++) {
        uint32_t bit=UINT32_C(1)<<slot;
        if(!(s->pieces&bit)) {
            s->pieces|=bit;
            --awards;
        }
    }
    return 1;
}
int spds_puzzle_load(const char *base,SpdsPuzzle *s) {
    if(!s) return -1;
    char filename[256];
    if(!path(filename,base,"puzzle.swp")) return -1;
    FILE *f=fopen(filename,"rb");
    if(!f) {
        if(errno!=ENOENT) return -1;
        spds_puzzle_init(s);
        return 0;
    }
    uint8_t bytes[SPDS_PUZZLE_FILE_SIZE];
    size_t n=fread(bytes,1,sizeof(bytes),f);
    int extra=fgetc(f);
    int failed=ferror(f);
    if(fclose(f)!=0) failed=1;
    if(failed || n!=sizeof(bytes) || extra!=EOF ||
       memcmp(bytes,"SPZP",4)!=0 || (bytes[4]!=1 && bytes[4]!=2) ||
       (bytes[4]==1 ? bytes[5]!=0 : bytes[5]>=SPDS_PUZZLE_PIECES) ||
       bytes[6]!=0 || bytes[7]!=0 ||
       get32(bytes+16)!=spds_crc32(bytes,16)) return -1;
    SpdsPuzzle candidate;
    spds_puzzle_init(&candidate);
    candidate.pieces=get32(bytes+8);
    candidate.credited_encounters=get32(bytes+12);
    candidate.selected=bytes[4]==2 ? bytes[5] : 0u;
    if(!valid(&candidate)) return -1;
    *s=candidate;
    return 1;
}
int spds_puzzle_save(const char *base,const SpdsPuzzle *s) {
    char dest[256],temp[256];
    if(!valid(s) || !path(dest,base,"puzzle.swp") ||
       !path(temp,base,"puzzle.tmp")) return -1;
    uint8_t bytes[SPDS_PUZZLE_FILE_SIZE]={0};
    memcpy(bytes,"SPZP",4);
    bytes[4]=2;
    bytes[5]=(uint8_t)s->selected;
    put32(bytes+8,s->pieces);
    put32(bytes+12,s->credited_encounters);
    put32(bytes+16,spds_crc32(bytes,16));
    FILE *f=fopen(temp,"wb");
    if(!f) return -1;
    int ok=fwrite(bytes,1,sizeof(bytes),f)==sizeof(bytes);
    if(fflush(f)!=0) ok=0;
    if(fclose(f)!=0) ok=0;
    if(!ok || rename(temp,dest)!=0) {
        remove(temp);
        return -1;
    }
    return 0;
}

int spds_puzzle_commit_selection(const char *base,SpdsPuzzle *state,unsigned previous) {
    if(!state || previous>=SPDS_PUZZLE_PIECES || !valid(state)) return -1;
    if(state->selected==previous) return 0;
    if(spds_puzzle_save(base,state)==0) return 1;
    state->selected=previous;
    return -1;
}
