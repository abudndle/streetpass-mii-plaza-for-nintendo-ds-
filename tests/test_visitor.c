#include "visitor.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void put32(uint8_t *p,uint32_t n) {
    for (unsigned k=0;k<4;k++) p[k]=(uint8_t)(n>>(8*k));
}
static void make_mii(SpdsMii *mii,const char *name,int sharing) {
    uint8_t packet[134]={0};
    memcpy(packet,"MDS3",4);
    strcpy((char *)packet+4,name);
    strcpy((char *)packet+36,"Creator");
    packet[68+47]=(uint8_t)sharing;
    packet[68+51]=1;
    put32(packet+130,spds_crc32(packet,130));
    assert(spds_mii_decode_packet(mii,packet,134));
}
int main(void) {
    SpdsMii mii,visitor;
    make_mii(&mii,"Real DS Mii",1);
    SpdsRecord owner={345,1700000000u,99,"Real DS Mii"};
    uint8_t wire[SPDS_VISITOR_SIZE];
    assert(spds_visitor_encode(wire,&owner,&mii));
    SpdsRecord decoded;
    assert(spds_visitor_decode(&decoded,&visitor,wire,sizeof wire));
    assert(!strcmp(visitor.name,"Real DS Mii") && decoded.sender_id==345);
    wire[80]^=1;
    assert(!spds_visitor_decode(&decoded,&visitor,wire,sizeof wire));
    wire[80]^=1;
    assert(!spds_visitor_decode(&decoded,&visitor,wire,sizeof wire-1));
    make_mii(&mii,"Private Mii",0);
    strcpy(owner.name,"Private Mii");
    assert(!spds_visitor_encode(wire,&owner,&mii));
    make_mii(&mii,"Real DS Mii",1);
    strcpy(owner.name,"Real DS Mii");
    char base[]="/tmp/spds-visitor-XXXXXX";
    assert(mkdtemp(base));
    assert(spds_storage_init(base)==0);
    assert(spds_visitor_export(base,&owner,&mii));
    char outgoing[300],incoming[300];
    snprintf(outgoing,sizeof outgoing,"%s/outbox/visitor.spv",base);
    snprintf(incoming,sizeof incoming,"%s/inbox/visitor.spv",base);
    FILE *f=fopen(outgoing,"rb");assert(f);
    assert(fread(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    f=fopen(incoming,"wb");assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    SpdsVisitorStats stats;
    assert(spds_visitor_poll(base,111,1700000000u,&stats)==0);
    assert(stats.accepted==1);
    assert(spds_visitor_read(base,345,&visitor));
    assert(!strcmp(visitor.name,"Real DS Mii"));
    assert(visitor.features[51]==0 && visitor.features[61]==2);
    SpdsMii shown[3];
    assert(spds_visitor_list(base,shown,3)==1);
    assert(!strcmp(shown[0].name,"Real DS Mii"));
    assert(spds_visitor_list(base,NULL,0)==1);
    strcpy(shown[0].name,"Unmodified");
    assert(spds_visitor_list_page(base,1,shown,1)==1);
    assert(!strcmp(shown[0].name,"Unmodified"));
    assert(spds_visitor_list_page(base,0,shown,1)==1);
    assert(!strcmp(shown[0].name,"Real DS Mii"));
    assert(spds_visitor_list_page(base,0,NULL,1)==-1);
    /* A duplicate pass does not destroy the visitor's avatar. */
    f=fopen(incoming,"wb");assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    assert(spds_visitor_poll(base,111,1700000001u,&stats)==0);
    assert(stats.duplicates==1 && stats.accepted==0);
    assert(spds_visitor_read(base,345,&visitor));
    /* Regression: a prior encounter can be logged even if its appearance
     * cache was deleted or the original save failed. Replaying the bundle
     * must restore the full Mii without counting a second encounter. */
    char missing_avatar[300];
    snprintf(missing_avatar,sizeof missing_avatar,
             "%s/visitors/%016llx.mds",base,(unsigned long long)345);
    assert(remove(missing_avatar)==0);
    assert(!spds_visitor_read(base,345,&visitor));
    f=fopen(incoming,"wb");assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire);assert(fclose(f)==0);
    assert(spds_visitor_poll(base,111,1700000002u,&stats)==0);
    assert(stats.duplicates==1 && stats.accepted==0 && stats.io_errors==0);
    assert(access(incoming,F_OK)!=0);
    assert(spds_visitor_read(base,345,&visitor));
    assert(!strcmp(visitor.name,"Real DS Mii"));
    assert(visitor.features[51]==0 && visitor.features[61]==2);
    /* Seven valid cached Miis exercise pages 1, 2 and the short final page.
     * Records use duplicate synthetic avatars so FAT enumeration order does
     * not affect the expected outcome. */
    char original[300],duplicate[300];
    snprintf(original,sizeof original,"%s/visitors/%016llx.mds",base,
             (unsigned long long)345);
    FILE *source=fopen(original,"rb");assert(source);
    uint8_t cached[SPDS_MII_PACKET];
    assert(fread(cached,1,sizeof cached,source)==sizeof cached);
    assert(fclose(source)==0);
    for(unsigned i=0;i<6;i++) {
        snprintf(duplicate,sizeof duplicate,"%s/visitors/%016llx.mds",
                 base,(unsigned long long)(400u+i));
        FILE *dest=fopen(duplicate,"wb");assert(dest);
        assert(fwrite(cached,1,sizeof cached,dest)==sizeof cached);
        assert(fclose(dest)==0);
    }
    assert(spds_visitor_list_page(base,0,shown,3)==7);
    assert(spds_visitor_list_page(base,3,shown,3)==7);
    strcpy(shown[1].name,"Not overwritten");
    assert(spds_visitor_list_page(base,6,shown,2)==7);
    assert(!strcmp(shown[0].name,"Real DS Mii"));
    assert(!strcmp(shown[1].name,"Not overwritten"));
    strcpy(shown[0].name,"No changes");
    assert(spds_visitor_list_page(base,7,shown,1)==7);
    assert(!strcmp(shown[0].name,"No changes"));

    /* Population boundary: all 3,000 Miis are enumerable and the next
     * imported visitor remains in inbox instead of being consumed. */
    for(unsigned i=0;i<SPDS_PLAZA_POPULATION_LIMIT-7u;i++) {
        char file[320];
        snprintf(file,sizeof file,"%s/visitors/%016llx.mds",base,
                 (unsigned long long)(10000u+i));
        FILE *dest=fopen(file,"wb"); assert(dest);
        assert(fwrite(cached,1,sizeof cached,dest)==sizeof cached);
        assert(fclose(dest)==0);
    }
    unsigned population=0;
    assert(spds_visitor_count(base,&population)==0);
    assert(population==SPDS_PLAZA_POPULATION_LIMIT);
    assert(spds_visitor_list_page(base,2997,shown,3)==SPDS_PLAZA_POPULATION_LIMIT);
    /* An already-cached sender can still replay a duplicate at capacity:
     * no extra avatar is allocated and the inbox bundle is consumed. */
    f=fopen(incoming,"wb"); assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire); assert(fclose(f)==0);
    assert(spds_visitor_poll(base,111,1700001000u,&stats)==0);
    assert(stats.duplicates==1 && stats.population_full==0);
    assert(access(incoming,F_OK)!=0);
    assert(spds_visitor_count(base,&population)==0);
    assert(population==SPDS_PLAZA_POPULATION_LIMIT);
    /* A genuinely new sender remains queued when the plaza is full. */
    owner.sender_id=999;
    assert(spds_visitor_encode(wire,&owner,&mii));
    f=fopen(incoming,"wb"); assert(f);
    assert(fwrite(wire,1,sizeof wire,f)==sizeof wire); assert(fclose(f)==0);
    assert(spds_visitor_poll(base,111,1700001001u,&stats)==0);
    assert(stats.population_full==1 && stats.accepted==0);
    assert(access(incoming,F_OK)==0);

    puts("PASS: full-Mii visitor bundles, validated import, ownership, sharing and duplicate avoidance");
    return 0;
}
